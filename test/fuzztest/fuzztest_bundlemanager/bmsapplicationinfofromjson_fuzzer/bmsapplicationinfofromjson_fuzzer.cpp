/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdint>
 #include <fuzzer/FuzzedDataProvider.h>

#include "application_info.h"
#include "json_serializer.h"
#include "nlohmann/json.hpp"

#include "bmsapplicationinfofromjson_fuzzer.h"
#include "bms_fuzztest_util.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
namespace {
const char NAME[] = "name";
}
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        nlohmann::json infoJson;

        FuzzedDataProvider fdp(data, size);
        std::string name = fdp.ConsumeRandomLengthString(BMSFuzzTestUtil::STRING_MAX_LENGTH);
        infoJson[NAME] = name;
        ApplicationInfo applicationInfo = infoJson;
        return true;
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}