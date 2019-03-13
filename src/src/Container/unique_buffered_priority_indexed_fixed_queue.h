#pragma once
#include <optional>
#include <array>
#include <cassert>
#include <iterator>

namespace ska {
    template <typename T, std::size_t ArraySize>
    class unique_buffered_priority_indexed_fixed_queue_iterator {
        using Container = std::array<std::optional<T>, ArraySize>;
        Container& buf;
        std::size_t off;
        
        void next_element() {
            do { ++off;} while(!buf[off].has_value() && off < ArraySize);
            keep_safe();
        }

        void previous_element() {
            do { --off;} while(!buf[off].has_value() && off > 0);
            keep_safe();
        }

        void keep_safe() {
            if(!buf[off].has_value()) {
                off = ArraySize;
            }
        }

    public:
        unique_buffered_priority_indexed_fixed_queue_iterator(Container& buf, std::size_t offset)
            : buf(buf), off(offset) {
            keep_safe();
        }
        bool operator==(const unique_buffered_priority_indexed_fixed_queue_iterator &i) {
            return &i.buf == &buf && i.off == off;
        }
        bool operator!=(const unique_buffered_priority_indexed_fixed_queue_iterator &i) {
            return !(*this == i);
        }
        unique_buffered_priority_indexed_fixed_queue_iterator & operator++()    { next_element(); return *this; }
        unique_buffered_priority_indexed_fixed_queue_iterator operator++(int) { auto t = *this; next_element(); return t; }
        unique_buffered_priority_indexed_fixed_queue_iterator & operator--()    { previous_element(); return *this; }
        unique_buffered_priority_indexed_fixed_queue_iterator operator--(int) { auto t = *this; previous_element(); return t; }
        bool operator<(unique_buffered_priority_indexed_fixed_queue_iterator const&sibling) const { return off < sibling.off;}
        bool operator<=(unique_buffered_priority_indexed_fixed_queue_iterator const&sibling) const { return off <= sibling.off; }
        bool operator>(unique_buffered_priority_indexed_fixed_queue_iterator const&sibling) const { return off > sibling.off; }
        bool operator>=(unique_buffered_priority_indexed_fixed_queue_iterator const&sibling) const { return off >= sibling.off; }
        T& operator*() const { return buf[off].value(); }
    };

    template <class T, std::size_t ArraySize, class Compare = std::less<T> >
    class unique_buffered_priority_indexed_fixed_queue {
    using BufferArrayContainer = std::array<std::vector<T>, ArraySize>;
    using ArrayContainer = std::array<std::optional<T>, ArraySize>;
    protected:
        ArrayContainer front_container;
        BufferArrayContainer buffer;
        Compare comp;
        std::size_t counter = 0;
    public:
        using iterator = unique_buffered_priority_indexed_fixed_queue_iterator<T, ArraySize>;
        using const_iterator = unique_buffered_priority_indexed_fixed_queue_iterator<const T, ArraySize>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        explicit unique_buffered_priority_indexed_fixed_queue() = default;
        
        bool empty()       const { return size() == 0; }
        std::size_t size() const { return counter; }
        const T& top()     const { assert(counter > 0); return front_container[counter - 1].value(); }
        
        void push(T x) {
            assert(x.priority < ArraySize && x.priority >= 0);
            auto& cell = front_container[x.priority];
            if(cell.has_value()) {
                auto newValue = std::move(cell.value());
                cell = std::move(x);
                buffer[x.priority].push_back(std::move(newValue));
            } else {
                cell = std::move(x);
                counter++;
            }
        }

        void pop() {
            if(remove_from_index(counter - 1)) {
                counter--;
            }
        }

        void remove(const T& x) {
            remove_from_index(x.priority);
        }
        
        void erase(const decltype(front_container.begin())& it) {
            remove(*it);
        }

        iterator begin() { return { buf, 0 }; }
        iterator end() { return {buf, counter}; }
        const_iterator begin() const { return {buf, 0}; }
        const_iterator end() const { return {buf, counter}; }
        const_iterator cbegin() const { return begin(); }
        const_iterator cend() const { return end(); }

        reverse_iterator rbegin() { return reverse_iterator{end()}; }
        reverse_iterator rend() { return reverse_iterator{begin()}; }
        const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
        const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
        const_reverse_iterator crbegin() const { return rbegin(); }
        const_reverse_iterator crend() const { return rend(); }

        private:
        bool remove_from_index(std::size_t index) {
            assert(index < ArraySize && index >= 0);
            auto& bufferPool = buffer[index];
            if(bufferPool.empty()) {
                front_container[index] = std::move(std::optional<T>{});
                return true;
            } 
            front_container[index] = std::move(bufferPool.back());
            bufferPool.pop_back();
            return false;
        }
    };
}

namespace std {
    template<typename T, std::size_t ArraySize>
    class iterator_traits<ska::unique_buffered_priority_indexed_fixed_queue_iterator<T, ArraySize>> {
    public:
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;
    };
}
