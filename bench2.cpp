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
#include "benchlet/benchlet.hpp"

/*
 * Class to hold benchmark state. setUp will be called once before all runs.
 * tearDown called after all runs.
 */
class MyBench2 : public Benchmark
{
public:
    virtual void setUp(void)
    {
        std::cout << " Setting Up..." << std::endl;
        i_ = 0;
    };

    virtual void tearDown(void)
    {
        std::cout << " Tearing Down... total is " << i_ << std::endl;

    };

    int i_;
};

/*
 * Benchmark of 1 batch of 1 iteration.
 */
BENCHMARK(MyBench2, RunB)
{
    std::cout << " MyBench2 RunB" << std::endl;
}

/*
 * Config that holds 100 iterations and 10 batches.
 */
static struct Benchmark::Config cfg1[] = {
    { Benchmark::ITERATIONS, "100" },
    { Benchmark::BATCHES, "10" }
};

/*
 * Benchmark to incrementally add an integer 10x100 times.
 */
BENCHMARK_CONFIG(MyBench2, Run10x100, cfg1)
{
    i_ += 1000;
}
