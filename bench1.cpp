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
#include "benchlet/benchlet.hpp"

class MyBench : public Benchmark
{
public:
};

BENCHMARK(MyBench, RunA, 1)
{
    std::cout << "MyBench RunA" << std::endl;
}

BENCHMARK(MyBench, RunAx5, 5)
{
    std::cout << "MyBench RunAx5" << std::endl;
}
