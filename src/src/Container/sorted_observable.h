#pragma once
#include <Utils/Observable.h>
#include <Utils/SubObserver.h>
#include "iterable_priority_queue.h"

namespace ska {
    struct ObserverComparatorPriorityLess {
        template<class Observer>
        bool operator()(const Observer& left, const Observer& right) const {
            if(left->priority == right->priority) {
                return 0;
            }
            return left->priority < right->priority;
        }
    };

    template <class T>
    class PriorityObserver : public Observer<T> {
        friend struct ObserverComparatorPriorityLess;
        template <class T, std::size_t ArraySize, class Compare>
        friend class unique_buffered_priority_indexed_fixed_queue;
        
    protected:
        template <class ...Args>
        PriorityObserver(int p, Args&& ... args) : Observer<T>::Observer(std::forward<Args>(args)...), priority(p) {}
        
    private:
        int priority;
    };

    template <class T>
    using priority_queue_for_observer = iterable_priority_queue<T, std::vector<T>, ObserverComparatorPriorityLess>;

    template <class T>
    using observable_priority_queue = Observable<T, priority_queue_for_observer, PriorityObserver>;

    template <class T>
    using subobserver_priority_queue = SubObserver<T, priority_queue_for_observer, PriorityObserver>;
}