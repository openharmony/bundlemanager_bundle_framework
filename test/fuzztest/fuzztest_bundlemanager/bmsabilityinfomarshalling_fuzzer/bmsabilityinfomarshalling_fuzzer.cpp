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

#include "ability_info.h"
#include "bmsabilityinfomarshalling_fuzzer.h"
#include "bms_fuzztest_util.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::AppExecFwk::BMSFuzzTestUtil;
namespace OHOS {
    bool FuzzAbilityInfoMarshalling(const uint8_t* data, size_t size)
    {
        Parcel dataMessageParcel;
        AbilityInfo abilityInfo;
        FuzzedDataProvider fdp(data, size);
        GenerateAbilityInfo<AbilityInfo>(fdp, abilityInfo);
        if (dataMessageParcel.WriteBuffer(data, size)) {
            bool marshallingRes = abilityInfo.Marshalling(dataMessageParcel);
            return marshallingRes;
        }
        return false;
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::FuzzAbilityInfoMarshalling(data, size);
    return 0;
}