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
#include "preinstallexceptionmgr_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "bundle_mgr_service.h"
#include "pre_install_exception_mgr.h"
#include "securec.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
constexpr size_t U32_AT_SIZE = 4;
const std::string BUNDLE_TEMP_NAME = "temp_bundle_name";
const std::string BUNDLE_PATH = "test.hap";

bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
{
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
    bundleMgrService_->InitBmsParam();
    bundleMgrService_->InitPreInstallExceptionMgr();

    auto preInstallExceptionMgr = bundleMgrService_->GetPreInstallExceptionMgr();
    bundleMgrService_->GetBmsParam();
    std::set<std::string> oldExceptionPaths;
    oldExceptionPaths.insert(std::string(data, size));
    std::set<std::string> oldExceptionBundleNames;
    oldExceptionBundleNames.insert(std::string(data, size));
    std::set<std::string> exceptionAppServicePaths;
    std::set<std::string> exceptionAppServiceBundleNames;
    preInstallExceptionMgr->GetAllPreInstallExceptionInfo(oldExceptionPaths, oldExceptionBundleNames,
        exceptionAppServicePaths, exceptionAppServiceBundleNames);

    preInstallExceptionMgr->ClearAll();
    std::set<std::string> exceptionPaths;
    exceptionPaths.insert(std::string(data, size));
    std::set<std::string> exceptionBundleNames;
    exceptionBundleNames.insert(std::string(data, size));
    preInstallExceptionMgr->GetAllPreInstallExceptionInfo(exceptionPaths, exceptionBundleNames,
        exceptionAppServicePaths, exceptionAppServiceBundleNames);

    preInstallExceptionMgr->SavePreInstallExceptionBundleName(BUNDLE_TEMP_NAME);
    preInstallExceptionMgr->SavePreInstallExceptionPath(BUNDLE_PATH);
    preInstallExceptionMgr->GetAllPreInstallExceptionInfo(exceptionPaths, exceptionBundleNames,
        exceptionAppServicePaths, exceptionAppServiceBundleNames);
    preInstallExceptionMgr->DeletePreInstallExceptionBundleName(BUNDLE_TEMP_NAME);
    preInstallExceptionMgr->DeletePreInstallExceptionPath(BUNDLE_PATH);
    preInstallExceptionMgr->GetAllPreInstallExceptionInfo(exceptionPaths, exceptionBundleNames,
        exceptionAppServicePaths, exceptionAppServiceBundleNames);

    if (oldExceptionPaths.size() > 0) {
        for (const auto& pathIter : oldExceptionPaths) {
            preInstallExceptionMgr->SavePreInstallExceptionPath(pathIter);
        }
    }

    if (oldExceptionBundleNames.size() > 0) {
        for (const auto& bundleNameIter : oldExceptionBundleNames) {
            preInstallExceptionMgr->SavePreInstallExceptionPath(bundleNameIter);
        }
    }
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

    if (size < OHOS::U32_AT_SIZE) {
        return 0;
    }

    char* ch = static_cast<char*>(malloc(size + 1));
    if (ch == nullptr) {
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        free(ch);
        ch = nullptr;
        return 0;
    }
    OHOS::DoSomethingInterestingWithMyAPI(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}