/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "native_interface_bundle.h"

#include <mutex>
#include <string>

#include "application_info.h"
#include "bundle_info.h"
#include "app_log_wrapper.h"
#include "bundle_mgr_proxy_native.h"
#include "ipc_skeleton.h"
#include "securec.h"
namespace {
const size_t CHAR_MAX_LENGTH = 10240;
}

// Helper function to release char* memory
static void ReleaseMemory(char* &str)
{
    if (str != nullptr) {
        free(str);
        str = nullptr;
    }
}

template <typename... Args>
void ReleaseStrings(Args... args)
{
    (ReleaseMemory(args), ...);
}

bool CopyStringToChar(char* &name, const std::string &value)
{
    size_t length = value.size();
    if ((length == 0) || (length + 1) > CHAR_MAX_LENGTH) {
        APP_LOGE("failed due to the length of value is empty or too long");
        return false;
    }
    name = static_cast<char*>(malloc(length + 1));
    if (name == nullptr) {
        APP_LOGE("failed due to malloc error");
        return false;
    }
    if (strcpy_s(name, length + 1, value.c_str()) != EOK) {
        APP_LOGE("failed due to strcpy_s error");
        ReleaseStrings(name);
        return false;
    }
    return true;
}

bool GetElementNameByAbilityInfo(
    const OHOS::AppExecFwk::AbilityInfo &abilityInfo, OH_NativeBundle_ElementName &elementName)
{
    if (!CopyStringToChar(elementName.bundleName, abilityInfo.bundleName)) {
        APP_LOGE("failed to obtains bundleName");
        return false;
    }

    if (!CopyStringToChar(elementName.moduleName, abilityInfo.moduleName)) {
        APP_LOGE("failed to obtains moduleName");
        ReleaseStrings(elementName.bundleName);
        return false;
    }

    if (!CopyStringToChar(elementName.abilityName, abilityInfo.name)) {
        APP_LOGE("failed to obtains abilityName");
        ReleaseStrings(elementName.bundleName, elementName.moduleName);
        return false;
    }
    return true;
}

bool GetElementNameByModuleInfo(
    const OHOS::AppExecFwk::HapModuleInfo &hapModuleInfo, OH_NativeBundle_ElementName &elementName)
{
    for (const auto &abilityInfo : hapModuleInfo.abilityInfos) {
        if (abilityInfo.name.compare(hapModuleInfo.mainElementName) == 0) {
            return GetElementNameByAbilityInfo(abilityInfo, elementName);
        }
    }
    return false;
}

OH_NativeBundle_ApplicationInfo OH_NativeBundle_GetCurrentApplicationInfo()
{
    OH_NativeBundle_ApplicationInfo nativeApplicationInfo;
    OHOS::AppExecFwk::BundleMgrProxyNative bundleMgrProxyNative;
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    auto bundleInfoFlag = static_cast<int32_t>(OHOS::AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION) |
        static_cast<int32_t>(OHOS::AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO);

    if (!bundleMgrProxyNative.GetBundleInfoForSelf(bundleInfoFlag, bundleInfo)) {
        APP_LOGE("can not get bundleInfo for self");
        return nativeApplicationInfo;
    };
    size_t bundleNameLen = bundleInfo.applicationInfo.bundleName.size();
    if ((bundleNameLen == 0) || (bundleNameLen + 1) > CHAR_MAX_LENGTH) {
        APP_LOGE("failed due to the length of bundleName is empty or too long");
        return nativeApplicationInfo;
    }
    nativeApplicationInfo.bundleName = static_cast<char*>(malloc(bundleNameLen + 1));
    if (nativeApplicationInfo.bundleName == nullptr) {
        APP_LOGE("failed due to malloc error");
        return nativeApplicationInfo;
    }
    if (strcpy_s(nativeApplicationInfo.bundleName, bundleNameLen + 1,
        bundleInfo.applicationInfo.bundleName.c_str()) != EOK) {
        APP_LOGE("failed due to strcpy_s error");
        ReleaseStrings(nativeApplicationInfo.bundleName);
        return nativeApplicationInfo;
    }
    size_t fingerprintLen = bundleInfo.signatureInfo.fingerprint.size();
    if ((fingerprintLen == 0) || (fingerprintLen + 1) > CHAR_MAX_LENGTH) {
        APP_LOGE("failed due to the length of fingerprint is empty or too long");
        ReleaseStrings(nativeApplicationInfo.bundleName);
        return nativeApplicationInfo;
    }
    nativeApplicationInfo.fingerprint = static_cast<char*>(malloc(fingerprintLen + 1));
    if (nativeApplicationInfo.fingerprint == nullptr) {
        APP_LOGE("failed due to malloc error");
        ReleaseStrings(nativeApplicationInfo.bundleName);
        return nativeApplicationInfo;
    }
    if (strcpy_s(nativeApplicationInfo.fingerprint, fingerprintLen + 1,
        bundleInfo.signatureInfo.fingerprint.c_str()) != EOK) {
        APP_LOGE("failed due to strcpy_s error");
        ReleaseStrings(nativeApplicationInfo.bundleName, nativeApplicationInfo.fingerprint);
        return nativeApplicationInfo;
    }
    APP_LOGI("OH_NativeBundle_GetCurrentApplicationInfo success");
    return nativeApplicationInfo;
}

char* OH_NativeBundle_GetAppId()
{
    OHOS::AppExecFwk::BundleMgrProxyNative bundleMgrProxyNative;
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    auto bundleInfoFlag =
        static_cast<int32_t>(OHOS::AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO);

    if (!bundleMgrProxyNative.GetBundleInfoForSelf(bundleInfoFlag, bundleInfo)) {
        APP_LOGE("can not get bundleInfo for self");
        return nullptr;
    };

    size_t appIdLen = bundleInfo.signatureInfo.appId.size();
    if ((appIdLen == 0) || (appIdLen + 1) > CHAR_MAX_LENGTH) {
        APP_LOGE("failed due to the length of appId is empty or too long");
        return nullptr;
    }
    char *appId = static_cast<char*>(malloc(appIdLen + 1));
    if (appId == nullptr) {
        APP_LOGE("failed due to malloc error");
        return nullptr;
    }
    if (strcpy_s(appId, appIdLen + 1, bundleInfo.signatureInfo.appId.c_str()) != EOK) {
        APP_LOGE("failed due to strcpy_s error");
        free(appId);
        return nullptr;
    }
    APP_LOGI("OH_NativeBundle_GetAppId success");
    return appId;
}

char* OH_NativeBundle_GetAppIdentifier()
{
    OHOS::AppExecFwk::BundleMgrProxyNative bundleMgrProxyNative;
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    auto bundleInfoFlag =
        static_cast<int32_t>(OHOS::AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO);

    if (!bundleMgrProxyNative.GetBundleInfoForSelf(bundleInfoFlag, bundleInfo)) {
        APP_LOGE("can not get bundleInfo for self");
        return nullptr;
    };

    size_t appIdentifierLen = bundleInfo.signatureInfo.appIdentifier.size();
    if (appIdentifierLen + 1 > CHAR_MAX_LENGTH) {
        APP_LOGE("failed due to the length of appIdentifier is too long");
        return nullptr;
    }
    char* appIdentifier = static_cast<char*>(malloc(appIdentifierLen + 1));
    if (appIdentifier == nullptr) {
        APP_LOGE("failed due to malloc error");
        return nullptr;
    }
    if (strcpy_s(appIdentifier, appIdentifierLen + 1,
        bundleInfo.signatureInfo.appIdentifier.c_str()) != EOK) {
        APP_LOGE("failed due to strcpy_s error");
        free(appIdentifier);
        return nullptr;
    }
    APP_LOGI("OH_NativeBundle_GetAppIdentifier success");
    return appIdentifier;
}

OH_NativeBundle_ElementName OH_NativeBundle_GetMainElementName()
{
    OH_NativeBundle_ElementName elementName;
    OHOS::AppExecFwk::BundleMgrProxyNative bundleMgrProxyNative;
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    auto bundleInfoFlag = static_cast<int32_t>(OHOS::AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE) |
                          static_cast<int32_t>(OHOS::AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_ABILITY);

    if (!bundleMgrProxyNative.GetBundleInfoForSelf(bundleInfoFlag, bundleInfo)) {
        APP_LOGE("can not get bundleInfo for self");
        return elementName;
    };

    for (const auto &hapModuleInfo : bundleInfo.hapModuleInfos) {
        if (hapModuleInfo.moduleType != OHOS::AppExecFwk::ModuleType::ENTRY) {
            continue;
        }
        if (GetElementNameByModuleInfo(hapModuleInfo, elementName)) {
            return elementName;
        }
    }

    for (const auto &hapModuleInfo : bundleInfo.hapModuleInfos) {
        if (hapModuleInfo.moduleType != OHOS::AppExecFwk::ModuleType::FEATURE) {
            continue;
        }
        if (GetElementNameByModuleInfo(hapModuleInfo, elementName)) {
            return elementName;
        }
    }

    for (const auto &hapModuleInfo : bundleInfo.hapModuleInfos) {
        for (const auto &abilityInfo : hapModuleInfo.abilityInfos) {
            GetElementNameByAbilityInfo(abilityInfo, elementName);
            return elementName;
        }
    }
    return elementName;
}

char* OH_NativeBundle_GetCompatibleDeviceType()
{
    OHOS::AppExecFwk::BundleMgrProxyNative bundleMgrProxyNative;
    std::string deviceType;
    if (!bundleMgrProxyNative.GetCompatibleDeviceTypeNative(deviceType)) {
        APP_LOGE("can not get compatible device type");
        return nullptr;
    }
    if (deviceType.size() + 1 > CHAR_MAX_LENGTH) {
        APP_LOGE("failed due to the length of device type is too long");
        return nullptr;
    }
    char* deviceTypeC = static_cast<char*>(malloc(deviceType.size() + 1));
    if (deviceTypeC == nullptr) {
        APP_LOGE("failed due to malloc error");
        return nullptr;
    }
    if (strcpy_s(deviceTypeC, deviceType.size() + 1, deviceType.c_str()) != EOK) {
        APP_LOGE("failed due to strcpy_s error");
        free(deviceTypeC);
        return nullptr;
    }
    APP_LOGI("OH_NativeBundle_GetCompatibleDeviceType success");
    return deviceTypeC;
}
