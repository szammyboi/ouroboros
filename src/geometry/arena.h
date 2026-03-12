/*
    Derived from: https://medium.com/@sgn00/high-performance-memory-management-arena-allocators-c685c81ee338
*/

#pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>

class Arena {
public:
    Arena(std::size_t bytes)
        : m_Buffer(new std::byte[bytes]), m_Capacity(bytes), m_Offset(0) {}

    ~Arena()
    {
        destruct();
        delete[] m_Buffer;
    }

    Arena(const Arena&) = delete;
    Arena(Arena&&) = delete;

    template<typename T, typename... Args>
    T* create(Args&&... args)
    {
        auto [ptr, new_offset] = allocate(m_Offset, sizeof(T), alignof(T));

        T* obj = std::construct_at(static_cast<T*>(ptr), std::forward<Args>(args)...);

        if constexpr (std::is_trivially_destructible_v<T>) {
            
            m_Offset = new_offset;
            return obj;
        } 
        
        auto [wrapper_ptr, final_offset] = allocate(new_offset, sizeof(DestructWrapper), alignof(DestructWrapper));

        auto call_destruct = [](void* p) {
            static_cast<T*>(p)->~T();
        };

        DestructWrapper* wrapper = std::construct_at(static_cast<DestructWrapper*>(wrapper_ptr), call_destruct, m_Tail, obj);
        m_Tail = wrapper;

        m_Offset = final_offset;
        return obj;
    }

    void clear()
    {
        destruct();
        m_Tail = nullptr;
        m_Offset = 0;
    }
private:
    std::pair<void*, size_t> allocate(size_t current_offset, size_t size, size_t alignment)
    {
        std::byte* current_ptr = m_Buffer + current_offset;
        size_t space = m_Capacity - current_offset;
        void* aligned_ptr = current_ptr;

        if (std::align(alignment, size, aligned_ptr, space) == nullptr) {
            throw std::bad_alloc();
        }

        auto new_offset = static_cast<std::byte*>(aligned_ptr) - m_Buffer + size;
        return {aligned_ptr, new_offset};
    }

    void destruct() {
        while (m_Tail) {
            m_Tail->func(m_Tail->obj);
            m_Tail = m_Tail->prev;
        }
    }
private:
    struct DestructWrapper {
        void (*func)(void*);
        DestructWrapper* prev;
        void* obj;
    };

    std::byte* m_Buffer;
    std::size_t m_Capacity;
    std::size_t m_Offset;
    DestructWrapper* m_Tail = nullptr;
};