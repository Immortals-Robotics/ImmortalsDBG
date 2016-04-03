#pragma once

#include "../bx/allocator.h"

#include <cassert>

#if !defined(alignof)
#define alignof(x) __alignof(x)
#endif

namespace bx
{
    template <typename Ty>
    class FixedPool
    {
    public:
        FixedPool();
        ~FixedPool();

        bool create(int _bucketSize, AllocatorI* _alloc);
        void destroy();

        Ty* newInstance();
        void deleteInstance(Ty* _inst);
        void clear();

    private:
        AllocatorI* m_alloc;
        int m_maxItems;
        Ty* m_buffer;
        Ty** m_ptrs;
        int m_index;
    };

    template <typename Ty>
    class Pool 
    {
    public:
        Pool();
        ~Pool();
        
        bool create(int _bucketSize, AllocatorI* _alloc);
        void destroy();

        Ty* newInstance();
        void deleteInstance(Ty* _inst);
        void clear();
        int getLeakCount() const;

    private:
        struct Bucket
        {
            Bucket* prev;
            Bucket* next;
            uint8_t* buffer;
            Ty** ptrs;
            int iter;
        };

    private:
        Bucket* createBucket();
        void destroyBucket(Bucket* bucket);

    private:
        AllocatorI* m_alloc;
        int m_maxItemsPerBucket;
        int m_numBuckets;
        Bucket* m_firstBucket;
    };

    // Implementation
    template <typename Ty>
    bx::FixedPool<Ty>::FixedPool()
    {
        m_alloc = nullptr;
        m_maxItems = 0;
        m_buffer = nullptr;
        m_ptrs = nullptr;
        m_index = 0;
    }

    template <typename Ty>
    bx::FixedPool<Ty>::~FixedPool()
    {
        assert(m_buffer == nullptr);
        assert(m_ptrs == nullptr);
    }

    template <typename Ty>
    bool bx::FixedPool<Ty>::create(int _bucketSize, AllocatorI* _alloc)
    {
        m_alloc = _alloc;
        m_maxItems = _bucketSize;

        size_t total_sz =
            sizeof(Ty)*m_maxItems +
            sizeof(Ty*)*m_maxItems;
        uint8_t* buff = (uint8_t*)BX_ALLOC(m_alloc, total_sz);
        if (!buff)
            return false;
        memset(buff, 0x00, total_sz);

        m_buffer = (Ty*)buff;
        buff += sizeof(Ty)*m_maxItems;
        m_ptrs = (Ty**)buff;

        // Assign pointer references
        for (int i = 0, c = m_maxItems; i < c; i++)
            m_ptrs[c - i - 1] = &m_buffer[i];
        m_index = m_maxItems;

        return true;
    }

    template <typename Ty>
    void bx::FixedPool<Ty>::destroy()
    {
        if (m_buffer) {
            BX_FREE(m_alloc, m_buffer);
            m_buffer = nullptr;
            m_ptrs = nullptr;
        }
        m_maxItems = 0;
        m_index = 0;
        m_alloc = nullptr;
    }

    template <typename Ty>
    Ty* bx::FixedPool<Ty>::newInstance()
    {
        if (m_index > 0)
            return new(m_ptrs[--m_index]) Ty;
        else
            return nullptr;
    }

    template <typename Ty>
    void bx::FixedPool<Ty>::deleteInstance(Ty* _inst)
    {
        assert(m_index != m_maxItems);
        m_ptrs[m_index++] = _inst;
        return;
    }

    template <typename Ty>
    void bx::FixedPool<Ty>::clear()
    {
        int bucketSize = m_maxItems;
        for (int i = 0; i < bucketSize; i++)
            b->ptrs[bucketSize - i - 1] = &b->buffer[i];
        b->iter = bucketSize;
    }

    template <typename Ty>
    Pool<Ty>::Pool()
    {
        m_firstBucket = nullptr;
        m_numBuckets = 0;
        m_alloc = nullptr;
        m_maxItemsPerBucket = 0;
    }

    template <typename Ty>
    Pool<Ty>::~Pool()
    {
        assert(m_firstBucket == nullptr);
        assert(m_numBuckets == 0);
    }

    template <typename Ty>
    bool Pool<Ty>::create(int _bucketSize, AllocatorI* _alloc)
    {
        assert(_alloc);

        m_maxItemsPerBucket = _bucketSize;
        m_alloc = _alloc;

        if (!createBucket())
            return false;
        return true;
    }

    template <typename Ty>
    void Pool<Ty>::destroy()
    {
        Bucket* b = m_firstBucket;
        while (b)   {
            Bucket* next = b->next;
            destroyBucket(b);
            b = next;
        }
    }

    template <typename Ty>
    Ty* Pool<Ty>::newInstance()
    {
        Bucket* b = m_firstBucket;
        while (b)   {
            if (b->iter > 0)
                return new(b->ptrs[--b->iter]) Ty;
            b = b->next;
        }

        b = createBucket();
        if (!b)
            return nullptr;

        return new(b->ptrs[--b->iter]) Ty;
    }

    template <typename Ty>
    void Pool<Ty>::deleteInstance(Ty* _inst)
    {
        _inst->~Ty();

        Bucket* b = m_firstBucket;
        size_t buffer_sz = sizeof(Ty)*m_maxItemsPerBucket;
        uint8_t* u8ptr = (uint8_t*)_inst;

        while (b)   {
            if (u8ptr >= b->buffer && u8ptr < (b->buffer + buffer_sz))  {
                assert(b->iter != m_maxItemsPerBucket);
                b->ptrs[b->iter++] = _inst;
                return;
            }

            b = b->next;
        }

        assert(false);  // pointer does not belong to this pool !
    }

    template <typename Ty>
    void Pool<Ty>::clear()
    {
        int bucket_size = m_maxItemsPerBucket;

        Bucket* b = m_firstBucket;
        while (b)    {
            // Re-assign pointer references
            for (int i = 0; i < bucket_size; i++)
                b->ptrs[bucket_size-i-1] = (Ty*)b->buffer + i;
            b->iter = bucket_size;

            b = b->next;
        }
    }

    template <typename Ty>
    int Pool<Ty>::getLeakCount() const
    {
        int count = 0;
        Bucket* b = m_firstBucket;
        int items_max = m_maxItemsPerBucket;
        while (b)    {
            count += (items_max - b->iter);
            b = b->next;
        }
        return count;
    }

    template <typename Ty>
    typename Pool<Ty>::Bucket* Pool<Ty>::createBucket()
    {
        size_t total_sz =
            sizeof(Bucket) +
            sizeof(Ty)*m_maxItemsPerBucket +
            sizeof(void*)*m_maxItemsPerBucket;
        uint8_t* buff = (uint8_t*)BX_ALLOC(m_alloc, total_sz);
        if (!buff)
            return nullptr;
        memset(buff, 0x00, total_sz);

        Bucket* bucket = (Bucket*)buff;
        buff += sizeof(Bucket);
        bucket->buffer = buff;
        buff += sizeof(Ty)*m_maxItemsPerBucket;
        bucket->ptrs = (Ty**)buff;

        // Assign pointer references
        for (int i = 0, c = m_maxItemsPerBucket; i < c; i++)
            bucket->ptrs[c-i-1] = (Ty*)bucket->buffer + i;
        bucket->iter = m_maxItemsPerBucket;

        // Add to linked-list
        bucket->next = bucket->prev = nullptr;
        if (m_firstBucket)  {
            m_firstBucket->prev = bucket;
            bucket->next = m_firstBucket;
        }
        m_firstBucket = bucket;

        m_numBuckets++;
        return bucket;
    }

    template <typename Ty>
    void Pool<Ty>::destroyBucket(typename Pool<Ty>::Bucket* bucket)
    {
        // Remove from linked-list
        if (m_firstBucket != bucket)    {
            if (bucket->next)
                bucket->next->prev = bucket->prev;
            if (bucket->prev)
                bucket->prev->next = bucket->next;
        }   else    {
            if (bucket->next)
                bucket->next->prev = nullptr;
            m_firstBucket = bucket->next;
        }

        //
        BX_FREE(m_alloc, bucket);
        m_numBuckets--;
    }

}