/*
 * benchmark_list_allocator.cpp
 *
 *  Created on: Apr 17, 2023
 *      Author: ionut
 */

#include <algorithm>
#include <iostream>
#include <memory>
#include <list>
#include <array>
#include <type_traits>
#include <utility>
#include <benchmark/benchmark.h>
#include <memory_resource>

#define __likely(x)    __builtin_expect (!!(x), 1)
#define __unlikely(x)  __builtin_expect (!!(x), 0)

struct bucket_t
{
	char* 	   m_data;
	size_t     m_data_size;
	size_t	   m_allocated;
	bucket_t()
	{
		m_data = (char*) malloc(100000);
		m_data_size = 100000;
		m_allocated = 0;
	}
};

template<class T>
struct MonotonicAllocator
{
	using pool_type_t = bucket_t;
    typedef T value_type;
    bucket_t  		 *allocated_pool;
    size_t 			 max_alloc;
    bool			 m_own_the_pool;
    MonotonicAllocator ()
    {
    	allocated_pool = new bucket_t;
    	max_alloc = std::numeric_limits<std::size_t>::max() / sizeof(T);
    	m_own_the_pool = true;
    }


    template<class U>
    constexpr MonotonicAllocator (const MonotonicAllocator <U>& other) noexcept
    {
    	allocated_pool = other.allocated_pool;
    	m_own_the_pool = false;
    	max_alloc = std::numeric_limits<std::size_t>::max() / sizeof(T);
    }


    [[nodiscard]]  T*  allocate(const std::size_t& n)
    {

        if ( __unlikely(n > max_alloc))
            throw std::bad_array_new_length();

        if( allocated_pool->m_data_size- allocated_pool->m_allocated <  n*sizeof(T)  )
        	throw std::bad_alloc();


        auto p = reinterpret_cast<T*>( allocated_pool->m_data + allocated_pool->m_allocated );
        if(__likely( p ) )
        {
        	//__builtin_prefetch( allocated_pool.m_data + allocated_pool.m_allocated  );

        	allocated_pool->m_allocated  += n * sizeof(T);
        	__builtin_prefetch( allocated_pool->m_data + allocated_pool->m_allocated  );
            return  p;
        }

        throw std::bad_alloc();
    }
    void release()
    {
    	allocated_pool->m_allocated = 0;
    }


    void deallocate(T* p, std::size_t n) noexcept
    {
    }
private:

};

template <typename T, typename U>
inline bool operator == (const MonotonicAllocator<T>&, const MonotonicAllocator<U>&) {
    return true;
}

template <typename T, typename U>
inline bool operator != (const MonotonicAllocator<T>&, const MonotonicAllocator<U>&) {
    return false;
}

#define NUM_ELEMENTS 1000

static void BM_ListCustomAllocator(benchmark::State& state) {
  MonotonicAllocator<int> allocator;
  for (auto _ : state)
  {
    std::list<int, MonotonicAllocator<int>> lst{allocator};
    for( int i = 0; i < NUM_ELEMENTS; ++ i)
    	lst.push_back(i);
    allocator.release();
  }
}

BENCHMARK(BM_ListCustomAllocator);

static void BM_ListDefaultAllocator(benchmark::State& state) {
  for (auto _ : state)
  {
	  std::list<int> lst;
	  for( int i = 0; i < NUM_ELEMENTS; ++ i)
		  lst.push_back(i);
  }
}
BENCHMARK(BM_ListDefaultAllocator);


static void BM_ListPMRAllocator(benchmark::State& state) {
  for (auto _ : state)
  {
	  std::pmr::list<int> lst;
	  for( int i = 0; i < NUM_ELEMENTS; ++ i)
		  lst.push_back(i);
  }
}
BENCHMARK(BM_ListPMRAllocator);


static void BM_ListPMRSeparateAllocator(benchmark::State& state) {
  std::pmr::polymorphic_allocator<int> allocator;
  std::array<unsigned char, 100000> memory;
  std::pmr::monotonic_buffer_resource pool{ memory.data(), memory.size() };
  std::pmr::polymorphic_allocator<int> pa{&pool};
  for (auto _ : state)
  {
    std::pmr::list<int> lst{pa };
    for( int i = 0; i < NUM_ELEMENTS; ++ i)
    	lst.push_back(i);
    pool.release();
  }
}
BENCHMARK(BM_ListPMRSeparateAllocator);
