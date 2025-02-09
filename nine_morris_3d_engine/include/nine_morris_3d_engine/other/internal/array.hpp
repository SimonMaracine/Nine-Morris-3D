#pragma once

#include <utility>
#include <iterator>
#include <cstddef>

namespace sm::internal {
    // Fixed-capacity, dynamic-sized array
    template<typename T, typename C, C Capacity>
    class Array {
    public:
        void push_back(const T& item) {
            m_data[m_size++] = item;
        }

        void push_back(T&& item) {
            m_data[m_size++] = std::move(item);
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            auto address {&m_data[m_size++]};
            new (address) T(std::forward<Args>(args)...);
        }

        void pop_back() {
            m_size--;
        }

        const T& operator[](C index) const {
            return m_data[index];
        }

        T& operator[](C index) {
            return m_data[index];
        }

        const T& back() const {
            return m_data[m_size - 1];
        }

        T& back() {
            return m_data[m_size - 1];
        }

        C size() const {
            return m_size;
        }

        bool empty() const {
            return m_size == 0;
        }

        void clear() {
            m_size = 0;
        }

        void resize(C size) {
            m_size = size;
        }

        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = const T;
            using pointer = const T*;
            using reference = const T&;
            using iterator_category = std::forward_iterator_tag;

            explicit const_iterator(C index, pointer data)
                : m_index(index), m_data(data) {}

            const_iterator& operator++() {
                m_index++;
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator self {*this};
                ++(*this);
                return self;
            }

            bool operator==(const_iterator other) const {
                return m_index == other.m_index;
            }

            bool operator!=(const_iterator other) const {
                return !(*this == other);
            }

            reference operator*() const {
                return m_data[m_index];
            }
        private:
            C m_index;
            pointer m_data;
        };

        class iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::forward_iterator_tag;

            explicit iterator(C index, pointer data)
                : m_index(index), m_data(data) {}

            iterator& operator++() {
                m_index++;
                return *this;
            }

            iterator operator++(int) {
                iterator self {*this};
                ++(*this);
                return self;
            }

            bool operator==(iterator other) const {
                return m_index == other.m_index;
            }

            bool operator!=(iterator other) const {
                return !(*this == other);
            }

            reference operator*() {
                return m_data[m_index];
            }
        private:
            C m_index;
            pointer m_data;
        };

        const_iterator begin() const {
            return const_iterator(0, m_data);
        }

        const_iterator end() const {
            return const_iterator(m_size, m_data);
        }

        iterator begin() {
            return iterator(0, m_data);
        }

        iterator end() {
            return iterator(m_size, m_data);
        }
    private:
        T m_data[Capacity];
        C m_size {};
    };
};
