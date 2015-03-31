/* The MIT License (MIT)
 *
 * Copyright (c) 2014 Microsoft
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file
 * 
 * @brief Header file for the Configurator class and some helper data structures.
 */

#ifndef __CONFIGURATOR_H
#define __CONFIGURATOR_H

//Headers
#include <common.h>
#include <optionparser.h>
#include <MyArg.h>

//Libraries
#include <cstdint>
#include <string>

namespace xmem {
	/**
	 * @brief Enumerates all possible types of command-line options
	 */
	enum optionIndex {
		UNKNOWN,
		ALL,
		CHUNK_SIZE,
		EXTENSIONS,
		OUTPUT_FILE,
		HELP,
		BASE_TEST_INDEX,
		NUM_WORKER_THREADS,
		MEAS_LATENCY,
		ITERATIONS,
		RANDOM_ACCESS_PATTERN,
		SEQUENTIAL_ACCESS_PATTERN,
		MEAS_THROUGHPUT,
		NUMA_DISABLE,
		VERBOSE,
		WORKING_SET_SIZE_PER_THREAD,
		USE_LARGE_PAGES,
		USE_READS,
		USE_WRITES,
		STRIDE_SIZE
	};

	/**
	 * @brief Command-line option descriptors as needed by stuff in optionparser.h>. This is basically the help message content.
	 */
	const Descriptor usage[] = {
		{ UNKNOWN, 0, "", "", Arg::None, "\nUSAGE: xmem [options]\n\n"
		"Options:" },
		{ ALL, 0, "a", "all", Arg::None, "    -a, --all    \tRun all possible benchmark modes and settings supported by X-Mem. This will override any other relevant user inputs. Note that X-Mem may run for a long time." },
		{ CHUNK_SIZE, 0, "c", "chunk_size", MyArg::PositiveInteger, "    -c, --chunk_size    \tA chunk size in bits to use for load traffic-generating threads used in throughput and loaded latency benchmarks. A chunk is the size of each memory access in a benchmark. Allowed values: 32, 64, 128, and 256. If no chunk sizes specified, use 64-bit chunks by default. Note that some chunk sizes may not be supported on some hardware. 32-bit chunks are not compatible with random-access patterns; these combinations of settings will be skipped if they occur. DEFAULT: 64"},
		{ EXTENSIONS, 0, "e", "extensions", Arg::None, "    -e, --extensions    \tRun custom X-Mem extensions defined by the user at build time." },
		{ OUTPUT_FILE, 0, "f", "output_file", MyArg::Required, "    -f, --output_file    \tGenerate an output file in CSV format using the given filename." },
		{ HELP, 0, "h", "help", Arg::None, "    -h, --help    \tPrint X-Mem usage and exit." },
		{ BASE_TEST_INDEX, 0, "i", "base_test_index", MyArg::NonnegativeInteger, "    -i, --base_test_index    \tBase index for the first benchmark to run. This option is provided for user convenience in enumerating benchmark tests across several subsequent runs of X-Mem. DEFAULT: 1" },
		{ NUM_WORKER_THREADS, 1, "j", "num_worker_threads", MyArg::PositiveInteger, "    -j, --num_worker_threads    \tNumber of worker threads to use in benchmarks. This may not exceed the number of logical CPUs in the system. For throughput benchmarks, this is the number of independent load-generating threads. For latency benchmarks, this is the number of independent load-generating threads plus one latency measurement thread. In latency benchmarks, 1 worker thread indicates no loading is applied. DEFAULT: 1" },
		{ MEAS_LATENCY, 0, "l", "latency", Arg::None, "    -l, --latency    \tUnloaded or loaded latency benchmarking mode. If 1 thread is used, unloaded latency is measured using 64-bit random reads. Otherwise, 1 thread is always dedicated to the 64-bit random read latency measurement, and remaining threads are used for load traffic generation using access patterns, chunk sizes, etc. specified by other arguments. See the throughput option for more information on load traffic generation."},
		{ ITERATIONS, 0, "n", "iterations", MyArg::PositiveInteger, "    -n, --iterations    \tIterations per benchmark. Multiple independent iterations may be performed on each benchmark setting to ensure consistent results. DEFAULT: 1" },
		{ RANDOM_ACCESS_PATTERN, 0, "r", "random_access", Arg::None, "    -r, --random_access    \tUse a random access pattern for load traffic-generating threads used in throughput and loaded latency benchmarks." },
		{ SEQUENTIAL_ACCESS_PATTERN, 0, "s", "sequential_access", Arg::None, "    -s, --sequential_access    \tUse a sequential and/or strided access pattern for load traffic generating-threads used in throughput and loaded latency benchmarks." },
		{ MEAS_THROUGHPUT, 0, "t", "throughput", Arg::None, "    -t, --throughput    \tThroughput benchmarking mode. Aggregate throughput is measured across all worker threads. Each load traffic-generating worker in a particular benchmark runs an identical kernel. Multiple distinct benchmarks may be run depending on the specified benchmark settings (e.g., aggregated 64-bit and 256-bit sequential read throughput using strides of 1 and -8 chunks)." },
		{ NUMA_DISABLE, 0, "u", "ignore_numa", Arg::None, "    -u, --ignore_numa    \tForce uniform memory access (UMA) mode. This only has an effect in non-uniform memory access (NUMA) systems. Limits benchmarking to CPU and memory NUMA node 0 instead of all intra-node and inter-node combinations. This mode can be useful in situations where the user is not interested in cross-node effects or node asymmetry. This option may also be required if large pages are desired on GNU/Linux systems due to lack of NUMA support in current versions of hugetlbfs. See the large_pages option." },
		{ VERBOSE, 0, "v", "verbose", Arg::None, "    -v, --verbose    \tVerbose mode increases the level of detail in X-Mem console reporting." },
		{ WORKING_SET_SIZE_PER_THREAD, 0, "w", "working_set_size", MyArg::PositiveInteger, "    -w, --working_set_size    \tWorking set size per worker thread in KB. This must be a multiple of 4KB. In all benchmarks, each worker thread works on its own \"private\" region of memory. For example, 4-thread throughput benchmarking with a working set size of 4 KB might result in measuring the aggregate throughput of four L1 caches corresponding to four physical cores, with no data sharing between threads. Similarly, an 8-thread loaded latency benchmark with a working set size of 64 MB would use 512 MB of memory in total for benchmarking, with no data sharing between threads. This would result in performance measurement of the shared DRAM physical interface, the shared L3 cache, etc." },
		{ USE_LARGE_PAGES, 1, "L", "large_pages", Arg::None, "    -L, --large_pages    \tUse large pages. This might enable better memory performance by reducing the translation-lookaside buffer (TLB) bottleneck. However, this is not supported on all systems. On GNU/Linux, you need hugetlbfs support with pre-reserved huge pages prior to running X-Mem. On GNU/Linux, you also must use the ignore_numa option, as hugetlbfs is not NUMA-aware at this time." },
		{ USE_READS, 0, "R", "reads", Arg::None, "    -R, --reads    \tUse memory read-based patterns in load traffic-generating threads." },
		{ USE_WRITES, 0, "W", "writes", Arg::None, "    -W, --writes    \tUse memory write-based patterns in load traffic-generating threads." },
		{ STRIDE_SIZE, 0, "S", "stride_size", MyArg::Integer, "    -S, --stride_size    \tA stride size to use for load traffic-generating threads, specified in powers-of-two multiples of the chunk size(s). Allowed values: 1, -1, 2, -2, 4, -4, 8, -8, 16, -16. Positive indicates the forward direction (increasing addresses), while negative indicates the reverse direction." },
		{ UNKNOWN, 0, "", "", Arg::None,
		"\n"
		"If a given option is not specified, X-Mem defaults will be used where appropriate.\n"
		"\n"
		"=======================================================================\n"
		"============================ EXAMPLE USAGE ============================\n"
		"=======================================================================\n"
		"\n"
		"Print X-Mem usage message and exit. If --help or -h is specified, benchmarks will not run regardless of other options.\n"
		"\n"
		"        xmem --help\n"
		"        xmem -h\n"
		"\n"
		"\n"
		"Run unloaded latency benchmarks with 5 iterations of each distinct benchmark setting. The chunk size of 32 bits and sequential access pattern options will be ignored as they only apply to load traffic-generating threads, which are unused here as the default number of worker threads is 1. Console reporting will be verbose.\n"
		"\n"
		"        xmem -l --verbose -n5 --chunk_size=32 -s\n"
		"\n"
		"\n"
		"Run throughput and loaded latency benchmarks on a per-thread working set size of 512 MB for a grand total of 1 GB of memory space. Use chunk sizes of 32 and 256 bits for load traffic-generating threads, and ignore NUMA effects. Number the first benchmark test starting at 101 both in console reporting and CSV file output (results.csv).\n"
		"\n"
		"        xmem -t --latency -w524288 -f results.csv -c32 -c256 -i 101 -u -j2\n"
		"\n"
		"\n"
		"Run 3 iterations of throughput, loaded latency, and extended benchmark modes on a working set of 128 KB per thread. Use 4 worker threads in total. For load traffic-generating threads, use all combinations of read and write memory accesses, random-access patterns, forward sequential, and strided patterns of size -4 and -16 chunks. Ignore NUMA effects in the system and use large pages. Finally, increase verbosity of console output.\n"
		"\n"
		"        xmem -w128 -n3 -j4 -l -t --extensions -s -S1 -S-4 -r -S16 -R -W -u -L -v\n"
		"\n"
		"\n"
		"Run EVERYTHING and dump results to file.\n"
		"\n"
		"        xmem -a -v -ftest.csv\n"
		"\n"
		"Have fun! =]\n"
		},
		{ 0, 0, 0, 0, 0, 0 }
	};

	/**
	 * @brief Handles all user input interpretation and generates the necessary flags for running benchmarks.
	 */
	class Configurator {
	public:
		/**
		 * @brief Default constructor. A default configuration is set. You will want to run configureFromInput() most likely.
		 */
		Configurator();

		/**
		 * @brief Specialized constructor for when you don't want to get config from input, and you want to pass it in directly.
		 * @param runCustomExtensions Indicates if user-defined code should be run in addition to other standard functionality.
		 * @param runLatency Indicates latency benchmarks should be run.
		 * @param runThroughput Indicates throughput benchmarks should be run.
		 * @param working_set_size_per_thread The total size of memory to test in all benchmarks, in bytes, per thread. This MUST be a multiple of 4KB pages.
		 * @param num_worker_threads The number of threads to use in throughput benchmarks, loaded latency benchmarks, and stress tests.
		 * @param use_chunk_32b If true, include 32-bit chunks for relevant benchmarks.
		 * @param use_chunk_64b If true, include 64-bit chunks for relevant benchmarks.
		 * @param use_chunk_128b If true, include 128-bit chunks for relevant benchmarks.
		 * @param use_chunk_256b If true, include 256-bit chunks for relevant benchmarks.
		 * @param numa_enable If true, then test all combinations of CPU/memory NUMA nodes.
		 * @param iterations_per_test For each unique benchmark test, this is the number of times to repeat it.
		 * @param use_random_access_pattern If true, use random-access patterns in throughput benchmarks.
		 * @param use_sequential_access_pattern If true, use sequential-access patterns in throughput benchmarks.
		 * @param starting_test_index Numerical index to use for the first test. This is an aid for end-user interpreting and post-processing of result CSV file, if relevant.
		 * @param filename Output filename to use.
		 * @param use_output_file If true, use the provided output filename.
		 * @param verbose If true, then X-Mem should be more verbose in its console reporting.
		 * @param use_large_pages If true, then X-Mem will attempt to force usage of large pages.
		 * @param use_reads If true, then throughput benchmarks should use reads.
		 * @param use_writes If true, then throughput benchmarks should use writes.
		 * @param use_stride_p1 If true, include stride of +1 for relevant benchmarks.
		 * @param use_stride_n1 If true, include stride of -1 for relevant benchmarks.
		 * @param use_stride_p2 If true, include stride of +2 for relevant benchmarks.
		 * @param use_stride_n2 If true, include stride of -2 for relevant benchmarks.
		 * @param use_stride_p4 If true, include stride of +4 for relevant benchmarks.
		 * @param use_stride_n4 If true, include stride of -4 for relevant benchmarks.
		 * @param use_stride_p8 If true, include stride of +8 for relevant benchmarks.
		 * @param use_stride_n8 If true, include stride of -8 for relevant benchmarks.
		 * @param use_stride_p16 If true, include stride of +16 for relevant benchmarks.
		 * @param use_stride_n16 If true, include stride of -16 for relevant benchmarks.
		 */
		Configurator(
			bool runCustomExtensions,
			bool runLatency,
			bool runThroughput,
			size_t working_set_size_per_thread,
			uint32_t num_worker_threads,
			bool use_chunk_32b,
			bool use_chunk_64b,
			bool use_chunk_128b,
			bool use_chunk_256b,
			bool numa_enable,
			uint32_t iterations_per_test,
			bool use_random_access_pattern,
			bool use_sequential_access_pattern,
			uint32_t starting_test_index,
			std::string filename,
			bool use_output_file,
			bool verbose,
			bool use_large_pages,
			bool use_reads,
			bool use_writes,
			bool use_stride_p1,
			bool use_stride_n1,
			bool use_stride_p2,
			bool use_stride_n2,
			bool use_stride_p4,
			bool use_stride_n4,
			bool use_stride_p8,
			bool use_stride_n8,
			bool use_stride_p16,
			bool use_stride_n16
		);

		/**
		 * @brief Configures the tool based on user's command-line inputs.
		 * @param argc The argc from main().
		 * @param argv The argv from main().
		 * @returns 0 on success.
		 */
		int32_t configureFromInput(int argc, char* argv[]);

		/**
		 * @brief Determines whether custom user extensions are enabled.
		 * @returns True if custom extensions are enabled.
		 */
		bool customExtensionsEnabled() const { return __runCustomExtensions; }

		/**
		 * @brief Indicates if the latency test has been selected.
		 * @returns True if the latency test has been selected to run.
		 */
		bool latencyTestSelected() const { return __runLatency; }

		/**
		 * @brief Indicates if the throughput test has been selected.
		 * @returns True if the throughput test has been selected to run.
		 */
		bool throughputTestSelected() const { return __runThroughput; }

		/**
		 * @brief Gets the working set size in bytes for each worker thread, if applicable.
		 * @returns The working set size in bytes.
		 */
		size_t getWorkingSetSizePerThread() const { return __working_set_size_per_thread; }

		/**
		 * @brief Determines if chunk size of 32 bits should be used in relevant benchmarks.
		 * @returns True if 32-bit chunks should be used.
		 */
		bool useChunk32b() const { return __use_chunk_32b; }

		/**
		 * @brief Determines if chunk size of 64 bits should be used in relevant benchmarks.
		 * @returns True if 64-bit chunks should be used.
		 */
		bool useChunk64b() const { return __use_chunk_64b; }

		/**
		 * @brief Determines if chunk size of 128 bits should be used in relevant benchmarks.
		 * @returns True if 128-bit chunks should be used.
		 */
		bool useChunk128b() const { return __use_chunk_128b; }

		/**
		 * @brief Determines if chunk size of 256 bits should be used in relevant benchmarks.
		 * @returns True if 256-bit chunks should be used.
		 */
		bool useChunk256b() const { return __use_chunk_256b; }

		/**
		 * @brief Determines if the benchmarks should test for all CPU/memory NUMA combinations.
		 * @returns True if all NUMA nodes should be tested.
		 */
		bool isNUMAEnabled() const { return __numa_enabled; }

		/**
		 * @brief Gets the number of iterations that should be run of each benchmark.
		 * @returns The iterations for each test.
		 */
		uint32_t getIterationsPerTest() const { return __iterations; }

		/**
		 * @brief Determines if throughput benchmarks should use a random access pattern.
		 * @returns True if random access should be used.
		 */
		bool useRandomAccessPattern() const { return __use_random_access_pattern; }
		
		/**
		 * @brief Determines if throughput benchmarks should use a sequential access pattern.
		 * @returns True if sequential access should be used.
		 */
		bool useSequentialAccessPattern() const { return __use_sequential_access_pattern; }

		/** 
		 * @brief Gets the number of worker threads to use.
		 * @returns The number of worker threads.
		 */
		uint32_t getNumWorkerThreads() const { return __num_worker_threads; }

		/** 
		 * @brief Gets the numerical index of the first benchmark for CSV output purposes.
		 * @returns The starting benchmark index.
		 */
		uint32_t getStartingTestIndex() const { return __starting_test_index; }

		/**
		 * @brief Gets the output filename to use, if applicable.
		 * @returns The output filename to use if useOutputFile() returns true. Otherwise return value is "".
		 */
		std::string getOutputFilename() const { return __filename; }

		/**
		 * @brief Determines whether to generate an output CSV file.
		 * @returns True if an output file should be used.
		 */
		bool useOutputFile() const { return __use_output_file; }

		/**
		 * @brief Changes whether an output file should be used.
		 * @param use If true, then use the output file.
		 */
		void setUseOutputFile(bool use) { __use_output_file = use; }

		/** 
		 * @brief Determines whether X-Mem is in verbose mode.
		 * @returns True if verbose mode is enabled.
		 */
		bool verboseMode() const { return __verbose; }

		/**
		 * @brief Determines whether X-Mem should use large pages.
		 * @param True if large pages should be used.
		 */
		bool useLargePages() const { return __use_large_pages; }

		/**
		 * @brief Determines whether reads should be used in throughput benchmarks.
		 * @returns True if reads should be used.
		 */
		bool useReads() const { return __use_reads; }
		
		/**
		 * @brief Determines whether writes should be used in throughput benchmarks.
		 * @returns True if writes should be used.
		 */
		bool useWrites() const { return __use_writes; }

		/**
		 * @brief Determines if a stride of +1 should be used in relevant benchmarks.
		 * @returns True if a stride of +1 should be used.
		 */
		bool useStrideP1() const { return __use_stride_p1; }
		
		/**
		 * @brief Determines if a stride of -1 should be used in relevant benchmarks.
		 * @returns True if a stride of -1 should be used.
		 */
		bool useStrideN1() const { return __use_stride_n1; }
		
		/**
		 * @brief Determines if a stride of +2 should be used in relevant benchmarks.
		 * @returns True if a stride of +2 should be used.
		 */
		bool useStrideP2() const { return __use_stride_p2; }
		
		/**
		 * @brief Determines if a stride of -2 should be used in relevant benchmarks.
		 * @returns True if a stride of -2 should be used.
		 */
		bool useStrideN2() const { return __use_stride_n2; }
		
		/**
		 * @brief Determines if a stride of +4 should be used in relevant benchmarks.
		 * @returns True if a stride of +4 should be used.
		 */
		bool useStrideP4() const { return __use_stride_p4; }
		
		/**
		 * @brief Determines if a stride of -4 should be used in relevant benchmarks.
		 * @returns True if a stride of -4 should be used.
		 */
		bool useStrideN4() const { return __use_stride_n4; }
		
		/**
		 * @brief Determines if a stride of +8 should be used in relevant benchmarks.
		 * @returns True if a stride of +8 should be used.
		 */
		bool useStrideP8() const { return __use_stride_p8; }
		
		/**
		 * @brief Determines if a stride of -8 should be used in relevant benchmarks.
		 * @returns True if a stride of -8 should be used.
		 */
		bool useStrideN8() const { return __use_stride_n8; }
		
		/**
		 * @brief Determines if a stride of +16 should be used in relevant benchmarks.
		 * @returns True if a stride of +16 should be used.
		 */
		bool useStrideP16() const { return __use_stride_p16; }
		
		/**
		 * @brief Determines if a stride of -16 should be used in relevant benchmarks.
		 * @returns True if a stride of -16 should be used.
		 */
		bool useStrideN16() const { return __use_stride_n16; }

	private:
		/**
		 * @brief Inspects a command line option (switch) to see if it occurred more than once, and warns the user if this is the case. The program only uses the first occurrence of any switch.
		 * @param opt The option to inspect.
		 * @returns True if the option only occurred once.
		 */
		bool __checkSingleOptionOccurrence(Option* opt) const;

		bool __configured; /**< If true, this object has been configured. configureFromInput() will only work if this is false. */

		bool __runCustomExtensions; /**< If true, run custom extensions before standard features. */ 

		bool __runLatency; /**< True if latency tests should be run. */
		bool __runThroughput; /**< True if throughput tests should be run. */
		size_t __working_set_size_per_thread; /**< Working set size in bytes for each thread, if applicable. */
		uint32_t __num_worker_threads; /**< Number of load threads to use for throughput benchmarks, loaded latency benchmarks, and stress tests. */
		bool __use_chunk_32b; /**< If true, use chunk sizes of 32-bits where applicable. */
		bool __use_chunk_64b; /**< If true, use chunk sizes of 64-bits where applicable. */
		bool __use_chunk_128b; /**< If true, use chunk sizes of 128-bits where applicable. */
		bool __use_chunk_256b; /**< If true, use chunk sizes of 256-bits where applicable. */
		bool __numa_enabled; /**< If true, test all combinations of CPU/memory NUMA nodes. Otherwise, just use node 0. */
		uint32_t __iterations; /**< Number of iterations to run for each benchmark test. */
		bool __use_random_access_pattern; /**< If true, run throughput benchmarks with random access pattern. */
		bool __use_sequential_access_pattern; /**< If true, run throughput benchmarks with sequential access pattern. */
		uint32_t __starting_test_index; /**< Numerical index to use for the first test. This is an aid for end-user interpreting and post-processing of result CSV file, if relevant. */
		std::string __filename; /**< The output filename if applicable. */
		bool __use_output_file; /**< If true, generate a CSV output file for results. */
		bool __verbose; /**< If true, then console reporting should be more detailed. */
		bool __use_large_pages; /**< If true, then large pages should be used. */
		bool __use_reads; /**< If true, throughput benchmarks should use reads. */
		bool __use_writes; /**< If true, throughput benchmarks should use writes. */
		bool __use_stride_p1; /**< If true, use a stride of +1 in relevant benchmarks. */
		bool __use_stride_n1; /**< If true, use a stride of -1 in relevant benchmarks. */
		bool __use_stride_p2; /**< If true, use a stride of +2 in relevant benchmarks. */
		bool __use_stride_n2; /**< If true, use a stride of -2 in relevant benchmarks. */
		bool __use_stride_p4; /**< If true, use a stride of +4 in relevant benchmarks. */
		bool __use_stride_n4; /**< If true, use a stride of -4 in relevant benchmarks. */
		bool __use_stride_p8; /**< If true, use a stride of +8 in relevant benchmarks. */
		bool __use_stride_n8; /**< If true, use a stride of -8 in relevant benchmarks. */
		bool __use_stride_p16; /**< If true, use a stride of +16 in relevant benchmarks. */
		bool __use_stride_n16; /**< If true, use a stride of -16 in relevant benchmarks. */
	};
};

#endif
