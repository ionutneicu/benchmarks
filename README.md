# benchmarks

Various performance benchmarks using GoogleBenchmark.


To install GoogleBenchmark, either install from apt:



sudo apt update
sudo apt install libbenchmark-dev

or install latest from source:

cd ~
git clone https://github.com/google/benchmark.git
cd benchmark
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="/usr/local" -DBENCHMARK_ENABLE_GTEST_TESTS=OFF ..
make
sudo make install


Contents:

Benchmark                            Time             CPU   Iterations
----------------------------------------------------------------------
BM_StringCreation                 8.03 ns         8.02 ns     84883146   -> Reference test from Google Examples
BM_StringCopy                     15.7 ns         15.7 ns     45832311   -> Reference test from Google Examples

BM_ListCustomAllocator          100399 ns       100265 ns         7086   -> Custom Monotonic std::list allocator
BM_ListDefaultAllocator         110513 ns       110465 ns         5800   -> Default std::list allocator
BM_ListPMRAllocator             135563 ns       135548 ns         5364   -> Default pmr::list allocator
BM_ListPMRSeparateAllocator     112961 ns       112954 ns         6052   -> Default pmr::list allocator with resources allocated at once

AllOnStackCopy                    4255 ns         4255 ns       169748   -> moving object, all its data on stack ( move has no effect )
DataOnHeapRawCopy                 52.0 ns         52.0 ns     13495114   -> moving object, data allocated on heap on constructor
DataOnHeapAllocatorCopy           18.0 ns         18.0 ns     38857273   ->

Results:

BM_ListCustomAllocator - seems fast, but not as in other results, i.e. 3x times faster ( list allocator uses infamous "rebind" ).

Copy up to 1000 bytes member allocated on stack is faster than allocated them with malloc on heap, then moving ( see DATA_SIZE in benchmark_move_copy.cpp ).


