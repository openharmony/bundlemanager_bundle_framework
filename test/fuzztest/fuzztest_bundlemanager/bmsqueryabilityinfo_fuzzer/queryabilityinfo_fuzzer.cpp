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

#include "bundle_mgr_proxy.h"

#include "queryabilityinfo_fuzzer.h"

using Want = OHOS::AAFwk::Want;

using namespace OHOS::AppExecFwk;
namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        sptr<IRemoteObject> object;
        BundleMgrProxy bundleMgrProxy(object);
        AbilityInfo abilityInfo;
        Want want;
        std::string bundleName (reinterpret_cast<const char*>(data), size);
        want.SetAction(bundleName);
        bundleMgrProxy.QueryAbilityInfo (want, abilityInfo);
        bundleMgrProxy.QueryAbilityInfo (want, reinterpret_cast<uintptr_t>(data),
            reinterpret_cast<uintptr_t>(data), abilityInfo, object);
        bundleMgrProxy.QueryAbilityInfo (want, reinterpret_cast<uintptr_t>(data),
            reinterpret_cast<uintptr_t>(data), abilityInfo);
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
