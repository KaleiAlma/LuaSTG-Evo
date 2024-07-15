#pragma once
#include <iterator>
#include <functional>
#include <cstdlib>


namespace LuaSTGPlus::Particle
{
    template<typename P>
    class ParticleList
    {
    private:
        static constexpr int32_t INVALID_INDEX = -1;
    public:
        void insert(P val) {
            if (free == INVALID_INDEX)
            {
                if (maxn >= size)
                {
                    int32_t temp = arr[back].next;
                    arr[back] = Node(std::move(val));
                    if (front != INVALID_INDEX)
                        arr[front].next = back;
                    front = back;
                    back = temp;
                }
                else
                {
                    arr[maxn] = Node(std::move(val));
                    //arr[maxn].next = front;
                    if (front != INVALID_INDEX)
                        arr[front].next = maxn;
                    front = maxn;
                    if (back == INVALID_INDEX)
                        back = front;
                    maxn++;
                }
            }
            else
            {
                int32_t temp = arr[free].next;
                arr[free] = Node(std::move(val));
                if (front != INVALID_INDEX)
                    arr[front].next = free;
                front = free;
                if (back == INVALID_INDEX)
                    back = front;
                free = temp;
            }
        }
    public:
        void foreach(std::function<bool(P* const)> fn)
        {
            if (back == INVALID_INDEX)
                return;

            int32_t current = back;
            int32_t last = INVALID_INDEX;
            while (current != INVALID_INDEX)
            {
                int32_t temp = arr[current].next;
                if (fn(&arr[current].val))
                {
                    if (current == back)
                    {
                        if (back == front)
                            front = arr[current].next;
                        back = arr[current].next;
                    }
                    else
                        arr[last].next = arr[current].next;

                    arr[current].next = free;
                    free = current;
                }

                last = current;
                current = temp;
            }
        }
    public:
        ParticleList(int32_t size) : arr(new Node[size]), size(size) {
            //spdlog::debug("[particle] sizeof Node: {}", sizeof(Node));
            //spdlog::debug("[particle] Address of arr: {}", fmt::ptr(arr));
        }
        ~ParticleList()
        {
            delete [] arr;
        }
    public:
        int32_t GetSize() { return size; }
        P* GetFront() { return &arr[front].val; }
    public:
        struct Iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using diffrence_type = std::ptrdiff_t;
            using value_type = P;
            using pointer = P*;
            using reference = P&;
        public:
            Iterator(int32_t idx, ParticleList* pl) : node_idx(idx), pl(pl) {}
            reference operator*() const { return *pl->arr[node_idx]; }
            pointer operator->() { return pl->arr[node_idx]; }
            Iterator& operator++() { node_idx = pl->arr[node_idx].next; return *this; }
            Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

            friend bool operator== (const Iterator& a, const Iterator& b) { return a.node_idx == b.node_idx; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.node_idx != b.node_idx; };
        private:
            int32_t node_idx;
            ParticleList* pl;
        };
        friend class Iterator;

        Iterator begin() { return Iterator(back, this); }
        Iterator end() { return Iterator(INVALID_INDEX, this); }
    private:
        class Node {
        public:
            Node() : val(), next(INVALID_INDEX) {}
            Node(P v) : val(v), next(INVALID_INDEX) {}
            P& operator*() { return val; }
            P* operator->() { return &val; }
            P* operator&() { return &val; }
        private:
            P val;
            int32_t next;
        private:
            friend class ParticleList;
            friend class Iterator;
        };
    private:
        Node* arr = nullptr;
        int32_t size;
        int32_t maxn = 0;
        int32_t front = INVALID_INDEX;
        int32_t back = INVALID_INDEX;
        int32_t free = INVALID_INDEX;
    };
}
