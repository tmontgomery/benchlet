/*
 * Copyright 2013 Informatica, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <math.h>

#include "benchlet/benchlet.hpp"

/*
 * Basic class to hold benchmark info. In this case, doesn't do much.
 */
class MyBench : public Benchmark
{
public:
    double val;
};

/*
 * Basic benchmark. Does nothing. Will have 1 batch of 1 iteration.
 */
BENCHMARK(MyBench, Run)
{

}

/*
 * Benchmarking writing to std::cout. Will have 1 batch of 5 iterations.
 */
BENCHMARK_ITERATIONS(MyBench, RunAx5, 5)
{
    std::cout << " MyBench RunAx5" << std::endl;
}

/*
 * Config that holds 5 batches and 10 iterations
 */
static struct Benchmark::Config cfg1[] = {
    { Benchmark::ITERATIONS, "10" },
    { Benchmark::BATCHES, "5" }
};

/*
 * Benchmarking of sqrt(). Will use config above, which is 5 batches of 10 iterations.
 */
BENCHMARK_CONFIG(MyBench, Run10x5, cfg1)
{
    val = sqrt(20.0);
}
