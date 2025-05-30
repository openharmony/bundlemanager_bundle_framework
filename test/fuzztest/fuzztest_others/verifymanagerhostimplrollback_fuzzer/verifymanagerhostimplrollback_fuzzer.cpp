/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define private public
#include <cstddef>
#include <cstdint>
#include "verifymanagerhostimplrollback_fuzzer.h"
#include "verify_manager_host_impl.h"
#include "securec.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
    constexpr size_t U32_AT_SIZE = 4;

    bool DoSomethingInterestingWithMyAPI(const std::string &str)
    {
        if (str.length() == 1) {
            return true;
        }
        VerifyManagerHostImpl impl;
        std::vector<std::string> Paths;
        std::string rootDir;
        std::vector<std::string> names;
        impl.Rollback(Paths);
        impl.Rollback(rootDir, names);
        return true;
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::U32_AT_SIZE) {
        return 0;
    }

    std::string str(reinterpret_cast<const char *>(data), size);
    OHOS::DoSomethingInterestingWithMyAPI(str);
    return 0;
}