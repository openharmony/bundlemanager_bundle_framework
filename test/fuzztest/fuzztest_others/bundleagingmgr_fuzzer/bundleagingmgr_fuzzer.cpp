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
#include "bundleagingmgr_fuzzer.h"

#include "bundle_aging_mgr.h"
#include "securec.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
constexpr size_t U32_AT_SIZE = 4;
constexpr size_t BASE_SIZE = 2;
bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
{
    auto bundleAgingMgr = std::make_shared<BundleAgingMgr>();
    std::shared_ptr<BundleDataMgr> dataMgr = nullptr;
    BundleAgingMgr::AgingTriggertype triggerType = reinterpret_cast<uintptr_t>(data) % OHOS::BASE_SIZE ?
        BundleAgingMgr::AgingTriggertype::FREE_INSTALL : BundleAgingMgr::AgingTriggertype::UPDATE_REMOVABLE_FLAG;
    bundleAgingMgr->Start(triggerType);
    bundleAgingMgr->InitAgingtTimer();
    bundleAgingMgr->ResetRequest();
    bundleAgingMgr->IsReachStartAgingThreshold();
    std::vector<DeviceUsageStats::BundleActivePackageStats> results;
    bundleAgingMgr->QueryBundleStatsInfoByInterval(results);
    bundleAgingMgr->InitAgingRequest();
    bundleAgingMgr->Process(dataMgr);
    return true;
}

} // namespace OHOS

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    /* Validate the length of size */
    if (size < OHOS::U32_AT_SIZE) {
        return 0;
    }

    char* ch = (char*)malloc(size + 1);
    if (ch == nullptr) {
        std::cout << "malloc failed." << std::endl;
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size + 1, data, size) != EOK) {
        std::cout << "copy failed." << std::endl;
        free(ch);
        ch = nullptr;
        return 0;
    }

    OHOS::DoSomethingInterestingWithMyAPI(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}