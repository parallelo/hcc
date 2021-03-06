//--------------------------------------------------------------------------------------
// File: test.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
// file except in compliance with the License.  You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR
// CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
//
// See the Apache Version 2.0 License for specific language governing permissions
// and limitations under the License.
//
//--------------------------------------------------------------------------------------
//
/// <tags>P1</tags>
/// <summary>Create an AV, create a read/write view over it and use that to write data</summary>

#include <amptest.h>
#include <amptest_main.h>
#include <vector>
#include <algorithm>

using namespace Concurrency;
using namespace Concurrency::Test;

runall_result test_main()
{
    accelerator acc = require_device();
	
    if(acc.get_supports_cpu_shared_memory())
    {
        acc.set_default_cpu_access_type(ACCESS_TYPE);
    }
	
    array<int> a(extent<1>(100));
    array_view<int, 1> av(a);

    std::vector<int> random_data(a.get_extent().size());
    Fill(random_data);
    array<int, 1> random_array(a.get_extent(), random_data.begin(), random_data.end());

    // create a new read/write av and write to that
    parallel_for_each(av.get_extent(), [=, &random_array](index<1> i) __GPU {
        array_view<int, 1> other(av);
        other[i] = random_array[i];
    });

    return VerifyDataOnCpu(a, random_data) ? runall_pass : runall_fail;
}
