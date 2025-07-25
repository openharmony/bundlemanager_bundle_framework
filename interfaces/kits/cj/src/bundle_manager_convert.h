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

#ifndef OHOS_BUNDLE_MANAGER_CONVERT_H
#define OHOS_BUNDLE_MANAGER_CONVERT_H
 
#include <cstdint>
#include <string>
#include "bundle_manager_utils.h"
#include "bundle_info.h"
#include "recoverable_application_info.h"
 
namespace OHOS {
namespace CJSystemapi {
namespace BundleManager {
 
namespace Convert {
    RetApplicationInfo ConvertApplicationInfo(const AppExecFwk::ApplicationInfo& cAppInfo);
    RetExtensionAbilityInfo ConvertExtensionAbilityInfo(const AppExecFwk::ExtensionAbilityInfo& extensionInfos);
    CArrRetExtensionAbilityInfo ConvertArrExtensionAbilityInfo(
        const std::vector<AppExecFwk::ExtensionAbilityInfo>& extensionInfos);
    RetAbilityInfo ConvertAbilityInfo(const AppExecFwk::AbilityInfo& cAbilityInfos);
    RetHapModuleInfo ConvertHapModuleInfo(const AppExecFwk::HapModuleInfo& hapModuleInfo);
    RetBundleInfo ConvertBundleInfo(const AppExecFwk::BundleInfo& cBundleInfo, int32_t flags);
    RetApplicationInfoV2 ConvertApplicationInfoV2(const AppExecFwk::ApplicationInfo& cAppInfo);
    RetExtensionAbilityInfoV2 ConvertExtensionAbilityInfoV2(const AppExecFwk::ExtensionAbilityInfo& extensionInfos);
    CArrRetExtensionAbilityInfoV2 ConvertArrExtensionAbilityInfoV2(
        const std::vector<AppExecFwk::ExtensionAbilityInfo>& extensionInfos);
    RetAbilityInfoV2 ConvertAbilityInfoV2(const AppExecFwk::AbilityInfo& cAbilityInfos);
    RetHapModuleInfoV2 ConvertHapModuleInfoV2(const AppExecFwk::HapModuleInfo& hapModuleInfo);
    RetBundleInfoV2 ConvertBundleInfoV2(const AppExecFwk::BundleInfo& cBundleInfo, int32_t flags);
    char* MallocCString(const std::string& origin);
    void FreeRetBundleInfoV2(RetBundleInfoV2& bundleInfo);
} // Convert
 
} // BundleManager
} // CJSys
} // OHOS

#endif