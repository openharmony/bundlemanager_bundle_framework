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

#include "bms_fuzztest_util.h"
#include "remote_ability_info.h"
#include "parcel.h"

#include "bmsremoteabilityinfo_fuzzer.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::AppExecFwk::BMSFuzzTestUtil;
namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        Parcel dataMessageParcel;
        std::string name (reinterpret_cast<const char*>(data), size);
        RemoteAbilityInfo info;
        FuzzedDataProvider fdp(data, size);
        info.label = fdp.ConsumeRandomLengthString(STRING_MAX_LENGTH);
        info.icon = fdp.ConsumeRandomLengthString(STRING_MAX_LENGTH);
        if (!info.Marshalling(dataMessageParcel)) {
            return false;
        }
        auto infoPtr = RemoteAbilityInfo::Unmarshalling(dataMessageParcel);
        if (infoPtr == nullptr) {
            return false;
        }
        delete infoPtr;
        infoPtr = nullptr;
        RemoteAbilityInfo *realInfo = new (std::nothrow) RemoteAbilityInfo();
        if (realInfo == nullptr) {
            return false;
        }
        bool ret = realInfo->ReadFromParcel(dataMessageParcel);
        delete realInfo;
        realInfo = nullptr;
        return ret;
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
