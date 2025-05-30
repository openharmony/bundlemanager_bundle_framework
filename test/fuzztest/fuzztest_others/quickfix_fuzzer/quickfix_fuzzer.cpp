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
#define protected public

#include "quickfix_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "app_control_proxy.h"
#include "quick_fixer.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    std::vector<std::string> bundleFilePaths;
    std::string bundleFilePath;
    bundleFilePaths.push_back(bundleFilePath);
    bool isDebug = false;
    std::string targetPath = std::string(reinterpret_cast<const char*>(data), size);
    std::string bundleName;
    bool enable = false;
    sptr<IQuickFixStatusCallback> statusCallback;
    QuickFixer quickFixMgr_(statusCallback);
    quickFixMgr_.DeployQuickFix(bundleFilePaths, isDebug, targetPath);
    quickFixMgr_.SwitchQuickFix(bundleName, enable);
    quickFixMgr_.DeleteQuickFix(bundleName);
    return true;
}
} // namespace OHOS

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}