#pragma once
#include <Utils/Observable.h>

namespace ska {
    struct CompilationPriorityLess {
        template<class Observer>
        bool operator()(const T& left, const T& right) const {
            if(left.priority == right.priority) {
                return 0;
            }
            return left.priority < right.priority;
        }
    };

    template <class T>
    struct CompilationMetaWrapper {
        CompilationMetaWrapper() = default;
        CompilationMetaWrapper(T&& d) : data {data} {}

        //TODO specify priority for each listener type (statement parser, symbol table, symbol table type updater, semantic type checker)
        int priority = 0;
        T data;
    };
    
    template <class T>
    struct CompilationPriorityQueue : 
        public std::priority_queue<CompilationMetaWrapper<T>, std::vector<CompilationMetaWrapper<T>>, CompilationPriorityLess> {

    };
    
    template <class T>
	using observable_priority_queue = Observable<T, CompilationPriorityQueue>;

    template <class T>
    using subobserver_priority_queue = SubObserver<T, CompilationPriorityQueue>;
}