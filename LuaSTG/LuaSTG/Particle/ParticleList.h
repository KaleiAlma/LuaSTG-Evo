#pragma once
#include <cstdint>
#include <iterator>
#include <functional>
#include <cstdlib>
#include <vector>


namespace LuaSTGPlus::Particle
{
    using Index = uint16_t;
    template<typename P>
    class ParticleList
    {
    private:
        static constexpr Index INVALID_INDEX = (uint16_t)-1;
    public:
        void insert(P val)
        {
            if (free == INVALID_INDEX)
            {
                if (maxn >= arr.size())
                {
                    Index temp = arr[back].next;
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
                Index temp = arr[free].next;
                arr[free] = Node(std::move(val));
                if (front != INVALID_INDEX)
                    arr[front].next = free;
                front = free;
                if (back == INVALID_INDEX)
                    back = front;
                free = temp;
            }
        }

        void foreach(std::function<bool(P* const)> fn)
        {
            if (back == INVALID_INDEX)
                return;

            Index current = back;
            Index last = INVALID_INDEX;
            while (current != INVALID_INDEX)
            {
                Index temp = arr[current].next;
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
                else
                    last = current;

                current = temp;
            }
        }

        void clear()
        {
            maxn = 0;
            front = INVALID_INDEX;
            back = INVALID_INDEX;
            free = INVALID_INDEX;
            for (Node& n : arr)
                n.next = INVALID_INDEX;
        }
    public:
        ParticleList(Index size) : arr(size)
        {
            //spdlog::debug("[particle] sizeof Node: {}", sizeof(Node));
            //spdlog::debug("[particle] Address of arr: {}", fmt::ptr(arr));
        }
        ~ParticleList() = default;
    public:
        Index GetSize() { return arr.size(); }
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
            Iterator(Index idx, ParticleList* pl) : node_idx(idx), pl(pl) {}
            reference operator*() const { return *pl->arr[node_idx]; }
            pointer operator->() { return pl->arr[node_idx]; }
            Iterator& operator++() { node_idx = pl->arr[node_idx].next; return *this; }
            Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

            friend bool operator== (const Iterator& a, const Iterator& b)
            { return a.pl == b.pl && a.node_idx == b.node_idx; }
        private:
            Index node_idx;
            ParticleList* pl;
        };
        friend class Iterator;

        Iterator begin() { return Iterator(back, this); }
        Iterator end() { return Iterator(INVALID_INDEX, this); }
    private:
        struct Node {
            Node() : val(), next(INVALID_INDEX) {}
            Node(P v) : val(v), next(INVALID_INDEX) {}
            P& operator*() { return val; }
            P* operator->() { return &val; }
            P* operator&() { return &val; }
            P val;
            Index next;
        };
    private:
        std::vector<Node> arr;
        Index maxn = 0;
        Index front = INVALID_INDEX;
        Index back = INVALID_INDEX;
        Index free = INVALID_INDEX;
    };
}
