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

#include "bmsgetbundleinfos_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>

#include "bundle_mgr_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "bms_fuzztest_util.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::AppExecFwk::BMSFuzzTestUtil;
namespace OHOS {
sptr<OHOS::AppExecFwk::IBundleMgr> GetBundleMgr()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        std::cout << "GetBundleMgr GetSystemAbilityManager is null";
        return nullptr;
    }

    auto bundleMgrSa = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleMgrSa == nullptr) {
        std::cout << "[fuzz] GetBundleMgr GetSystemAbility is null";
        return nullptr;
    }

    return iface_cast<IBundleMgr>(bundleMgrSa);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    auto bundleMgr = GetBundleMgr();
    if (bundleMgr == nullptr) {
        return false;
    }

    std::vector<BundleInfo> bundleInfos;
    FuzzedDataProvider fdp(data, size);
    int32_t userId = BMSFuzzTestUtil::GenerateRandomUser(fdp);
    bundleMgr->GetBundleInfos(userId, bundleInfos);
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}