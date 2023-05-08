#include <benchmark/benchmark.h>
#include <string.h>
#include <iostream>
#include <unordered_map>

#define DATA_SIZE 100000

class AllOnStack
{
private:
	char m_data[DATA_SIZE];
public:
	AllOnStack()
	{

	}

	AllOnStack( const AllOnStack& rhs )
	{
		memcpy( this->m_data, rhs.m_data, DATA_SIZE);
	}

	AllOnStack& operator = ( const AllOnStack& rhs )
	{
		memcpy( this->m_data, rhs.m_data, DATA_SIZE);
		return *this;
	}
};

class DataOnHeapRaw
{
private:
	char* m_data;
public:
	DataOnHeapRaw()
	{
		m_data = (char*) malloc( DATA_SIZE );
	}

	DataOnHeapRaw( const DataOnHeapRaw& rhs )
	{
		m_data = (char*) malloc( DATA_SIZE );
		memcpy( this->m_data, rhs.m_data, DATA_SIZE);
	}

	DataOnHeapRaw( DataOnHeapRaw&& rhs )
	{
		m_data = rhs.m_data;
		rhs.m_data = NULL;
	}

	DataOnHeapRaw& operator = ( const DataOnHeapRaw& rhs )
	{
		memcpy( this->m_data, rhs.m_data, DATA_SIZE);
		return *this;
	}

	~DataOnHeapRaw()
	{
		if( m_data )
			delete m_data;
	}
};

class Allocator
{
	static constexpr size_t N_CHUNKS  = 10;
	using counter_type_t = std::int_fast16_t;
	char m_allocator_usage_table[N_CHUNKS];
	char m_allocator_data[ N_CHUNKS * DATA_SIZE ];

public:
	void alloc( std::pair<size_t, void*> &retval )
	{
		for( counter_type_t i = 0; i < N_CHUNKS; ++i)
		{
			if( ! m_allocator_usage_table[i] )
			{
				m_allocator_usage_table[i] = 0;
				retval.first = i;
				retval.second = m_allocator_data + i * DATA_SIZE;
				return;
			}
		}
		retval.first = 0U;
		retval.second=nullptr;

	}
	void de_alloc(size_t chunk_index)
	{
		m_allocator_usage_table[chunk_index] = 0;
	}

	Allocator()
	{
		for( int i = 0; i < N_CHUNKS; ++ i)
		{
			// mark all as not used
			m_allocator_usage_table[i] = 0;
		}
	}
} allocator;

class DataOnHeapAllocator
{
public:
	char* m_data;
	int   m_allocator_index;

	DataOnHeapAllocator()
	{
		//auto alloc = allocator.alloc();
		auto alloc = std::make_pair<size_t, void*>(0, nullptr);
		allocator.alloc(alloc);
		m_allocator_index = alloc.first;
		m_data =  (char*)alloc.second;
		m_data = nullptr;
	}

	DataOnHeapAllocator( const DataOnHeapAllocator& rhs )
	{
		m_data = (char*) malloc( DATA_SIZE );
		memcpy( this->m_data, rhs.m_data, DATA_SIZE);
	}

	DataOnHeapAllocator( DataOnHeapAllocator&& rhs )
	{
		m_data = rhs.m_data;
		rhs.m_data = nullptr;
	}

	DataOnHeapAllocator& operator = ( const DataOnHeapAllocator& rhs )
	{
		memcpy( this->m_data, rhs.m_data, DATA_SIZE);
		return *this;
	}

	~DataOnHeapAllocator()
	{
		if( m_data )
			allocator.de_alloc(m_allocator_index);
	}
};

static void AllOnStackCopy(benchmark::State& state) {
  for (auto _ : state)
  {
	  AllOnStack instance;
	  AllOnStack instance2 = std::move(instance);
  }
}
// Register the function as a benchmark
BENCHMARK(AllOnStackCopy);

static void DataOnHeapRawCopy(benchmark::State& state) {
  for (auto _ : state)
  {
	  DataOnHeapRaw instance;
	  DataOnHeapRaw instance2 = std::move(instance);
  }
}

BENCHMARK(DataOnHeapRawCopy);


static void DataOnHeapAllocatorCopy(benchmark::State& state) {
  for (auto _ : state)
  {
	  DataOnHeapAllocator instance;
	  DataOnHeapAllocator instance2 = std::move(instance);
  }
}

BENCHMARK(DataOnHeapAllocatorCopy);
