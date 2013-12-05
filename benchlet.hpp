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
#ifndef _BENCHLET_HPP
#define _BENCHLET_HPP

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if defined(Darwin)
#   include <mach/mach.h>
#   include <mach/mach_time.h>
#elif defined(__linux__)
#   include <time.h>
#else
#   error "Must define Darwin or __linux__"
#endif /* platform includes */

#include <iostream>
#include <vector>

#define DEFAULT_ITERATIONS 1
#define DEFAULT_ITERATIONS_STRING "1"

#define DEFAULT_BATCHES 1
#define DEFAULT_BATCHES_STRING "1"

#define DEFAULT_RETAIN_STATS false

class Benchmark
{
public:
    enum ConfigVariable
    {
        ITERATIONS,
        BATCHES,
        RETAIN_STATS
    };

    struct Config
    {
        ConfigVariable key;
        const char *value;
    };

    virtual void setUp(void) {};
    virtual void tearDown(void) {};
    virtual void benchmarkBody(void) {};

    void name(const char *name) { name_ = name; };
    const char *name(void ) const { return name_; };

    void runName(const char *runName) { runName_ = runName; };
    const char *runName(void ) const { return runName_; };

    void iterations(const unsigned int i) { iterations_ = i; };
    unsigned int iterations(void) const { return iterations_; };

    void batches(const unsigned int i) { batches_ = i; };
    unsigned int batches(void) const { return batches_; };

    void config(const struct Config *cfg, unsigned int numConfigs) { config_ = cfg; numConfigs_ = numConfigs; };
    const struct Config *config(void) const { return config_; };
    unsigned int numConfigs(void) const { return numConfigs_; };

    void stats(uint64_t *stats) { stats_ = stats; };
    uint64_t *stats(void) { return stats_; };

    void retainStats(bool retain) { retainStats_ = retain; };
    bool retainStats(void) const { return retainStats_; };
private:
    const char *name_;
    const char *runName_;
    unsigned int iterations_;
    unsigned int batches_;
    const struct Config *config_;
    unsigned int numConfigs_;
    uint64_t *stats_;
    bool retainStats_;
    // save start time, etc.
};

class BenchmarkRunner
{
public:
    static Benchmark *registerBenchmark(const char *name, const char *runName, Benchmark *impl, struct Benchmark::Config *cfg, int numCfgs)
    {
        impl->name(name);
        impl->runName(runName);
        impl->config(cfg, numCfgs);
        impl->iterations(DEFAULT_ITERATIONS);
        impl->batches(DEFAULT_BATCHES);
        impl->retainStats(DEFAULT_RETAIN_STATS);
        for (int i = 0, max = numCfgs; i < max; i++)
        {
            if (cfg[i].key == Benchmark::ITERATIONS)
            {
                impl->iterations(atoi(cfg[i].value));
            }
            else if (cfg[i].key == Benchmark::BATCHES)
            {
                impl->batches(atoi(cfg[i].value));
            }
            else if (cfg[i].key == Benchmark::RETAIN_STATS)
            {
                if (strcmp(cfg[i].value, "true") == 0)
                {
                    impl->retainStats(true);
                }
                else if (strcmp(cfg[i].value, "false") == 0)
                {
                    impl->retainStats(false);
                }
            }
        }
        table().push_back(impl);
        std::cout << "Registering " << name << " run \"" << runName << "\" total iterations " << impl->iterations() * impl->batches() << std::endl;
        return impl;
    };

    static void run(void)
    {
        for (std::vector<Benchmark *>::iterator it = table().begin(); it != table().end(); ++it)
        {
            Benchmark *benchmark = *it;
            uint64_t startTimestamp, elapsedNanos;
            double nanospop, opspsec;
            uint64_t *stats = new uint64_t[benchmark->batches()];
            uint64_t total = 0;

            std::cout << "Running benchmark " << benchmark->name() << "." << benchmark->runName() << ". ";
            std::cout << benchmark->iterations() << " iterations X " << benchmark->batches() << " batches. " << std::endl;
            benchmark->setUp();
            for (int i = 0, max_i = benchmark->batches(); i < max_i; i++)
            {
                int j = 0, max_j = benchmark->iterations();

                startTimestamp = currentTimestamp();
                for (; j < max_j; j++)
                {
                    benchmark->benchmarkBody();
                }
                elapsedNanos = elapsedNanoseconds(startTimestamp, currentTimestamp());
                nanospop = (double)elapsedNanos / (double)benchmark->iterations();
                opspsec = 1000000000.0 / nanospop;
                stats[i] = elapsedNanos;
                total += elapsedNanos;
                std::cout << " Elapsed " << elapsedNanos << " nanoseconds. " << nanospop << " nanos/op. " << opspsec/1000.0 << " Kops/sec." << std::endl;
            }
            double elapsedPerBatch = (double)total / (double)benchmark->batches();
            double elapsedPerIteration = elapsedPerBatch / (double)benchmark->iterations();
            double throughputKopsps = 1000000.0 / elapsedPerIteration;
            std::cout << " Avg elapsed/batch " << elapsedPerBatch << " nanoseconds" << std::endl;
            std::cout << " Avg nanos/op " << elapsedPerIteration << " nanos/op" << std::endl;
            std::cout << " Throughput " << throughputKopsps << " Kops/sec." << std::endl;
            benchmark->stats(stats);
            benchmark->tearDown();
            total = 0;
            if (!benchmark->retainStats())
            {
                delete[] stats;
            }
        }
    };

    static std::vector<Benchmark *> &table(void)
    {
        static std::vector<Benchmark *> table;
        return table;
    };

#if defined(Darwin)
    static uint64_t currentTimestamp(void)
    {
        return mach_absolute_time();
    };

    // inspired from https://developer.apple.com/library/mac/qa/qa1398/_index.html
    static uint64_t elapsedNanoseconds(uint64_t start_timestamp, uint64_t end_timestamp)
    {
        static mach_timebase_info_data_t timebaseInfo;

        if (0 == timebaseInfo.denom)
        {
            (void)mach_timebase_info(&timebaseInfo);
        }
        return (end_timestamp - start_timestamp) * timebaseInfo.numer / timebaseInfo.denom;
    };
#elif defined(__linux__)
    static uint64_t currentTimestamp(void)
    {
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec * 1000000000 + ts.tv_nsec;
    };

    static uint64_t elapsedNanoseconds(uint64_t start_timestamp, uint64_t end_timestamp)
    {
        return end_timestamp - start_timestamp;
    };
#endif /* platform high resolution time */
};

#define BENCHMARK_CLASS_NAME(x,y) x##y

#define BENCHMARK_CONFIG(c,r,i) \
    class BENCHMARK_CLASS_NAME(c,r) : public c { \
    public: \
      BENCHMARK_CLASS_NAME(c,r)() {}; \
      virtual void benchmarkBody(void); \
    private: \
      static Benchmark *instance_; \
    };                            \
    Benchmark *BENCHMARK_CLASS_NAME(c,r)::instance_ = BenchmarkRunner::registerBenchmark(#c, #r, new BENCHMARK_CLASS_NAME(c,r)(), i, sizeof(i)/sizeof(Benchmark::Config)); \
    void BENCHMARK_CLASS_NAME(c,r)::benchmarkBody(void)

#define BENCHMARK_ITERATIONS(c,r,i) \
    struct Benchmark::Config c ## r ## _cfg[] = {{Benchmark::ITERATIONS, #i},{Benchmark::BATCHES, DEFAULT_BATCHES_STRING}}; \
    BENCHMARK_CONFIG(c,r, c ## r ## _cfg)

#define BENCHMARK(c,r) \
    struct Benchmark::Config c ## r ## _cfg[] = {{Benchmark::ITERATIONS, DEFAULT_ITERATIONS_STRING},{Benchmark::BATCHES, DEFAULT_BATCHES_STRING}}; \
    BENCHMARK_CONFIG(c,r, c ## r ## _cfg)

#endif /* _BENCHLET_HPP */
