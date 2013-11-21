/*
 * Copyright 2013 Todd L. Montgomery
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
#ifndef _BENCHLET_HPP
#define _BENCHLET_HPP

#include <stdio.h>

#include <iostream>
#include <vector>

class Benchmark
{
public:
    virtual void setUp(void) {};
    virtual void tearDown(void) {};
    virtual void benchmarkBody(void) {};

    void name(const char *name) { name_ = name; };
    const char *name(void ) const { return name_; };

    void runName(const char *runName) { runName_ = runName; };
    const char *runName(void ) const { return runName_; };

    void iterations(const unsigned int i) { iterations_ = i; };
    unsigned int iterations(void) const { return iterations_; };
private:
    const char *name_;
    const char *runName_;
    unsigned int iterations_;
    // save start time, etc.
};

class BenchmarkRunner
{
public:
    static Benchmark *registerBenchmark(const char *name, const char *runName, Benchmark *impl, unsigned int iters)
    {
        impl->name(name);
        impl->runName(runName);
        impl->iterations(iters);
        table().push_back(impl);
        std::cout << "Registering " << name << " run " << runName << " iterations " << iters << std::endl;
        return impl;
    };        

    static void run(void)
    {
        std::cout << "Run " <<  table().size() << " benchmarks" << std::endl;
        for (std::vector<Benchmark *>::iterator it = table().begin(); it != table().end(); ++it)
        {
            Benchmark *benchmark = *it;

            std::cout << "Running... " << benchmark->name() << " run " << benchmark->runName() << std::endl;
            benchmark->setUp();
            for (int i = 0, max = benchmark->iterations(); i < max; i++)
            {
                benchmark->benchmarkBody();
            }
            benchmark->tearDown();
        }
    };

    static std::vector<Benchmark *> &table(void)
    {
        static std::vector<Benchmark *> table;
        return table;
    };
};

#define BENCHMARK_CLASS_NAME(x,y) x##y

#define BENCHMARK(c,r,i) \
    class BENCHMARK_CLASS_NAME(c,r) : public c { \
    public: \
      BENCHMARK_CLASS_NAME(c,r)() {}; \
      virtual void benchmarkBody(void); \
    private: \
      static Benchmark *instance_; \
    };                            \
    Benchmark *BENCHMARK_CLASS_NAME(c,r)::instance_ = BenchmarkRunner::registerBenchmark(#c, #r, new BENCHMARK_CLASS_NAME(c,r)(), i); \
    void BENCHMARK_CLASS_NAME(c,r)::benchmarkBody(void)

#endif /* _BENCHLET_HPP */
