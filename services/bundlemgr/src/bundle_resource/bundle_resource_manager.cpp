/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "bundle_resource_manager.h"

#include "app_log_wrapper.h"
#include "bundle_resource_parser.h"
#include "bundle_resource_process.h"

namespace OHOS {
namespace AppExecFwk {
BundleResourceManager::BundleResourceManager()
{
    bundleResourceRdb_ = std::make_shared<BundleResourceRdb>();
}

BundleResourceManager::~BundleResourceManager()
{
}

bool BundleResourceManager::AddResourceInfo(const InnerBundleInfo &innerBundleInfo,
    const int32_t userId, std::string hapPath)
{
    std::vector<ResourceInfo> resourceInfos;
    if (!BundleResourceProcess::GetResourceInfo(innerBundleInfo, userId, resourceInfos)) {
        APP_LOGE("bundleName: %{public}s failed to GetResourceInfo", innerBundleInfo.GetBundleName().c_str());
        return false;
    }

    if (!hapPath.empty()) {
        for (auto &info : resourceInfos) {
            info.hapPath_ = hapPath;
        }
    }
    return AddResourceInfos(resourceInfos);
}

bool BundleResourceManager::AddResourceInfoByBundleName(const std::string &bundleName, const int32_t userId)
{
    std::vector<ResourceInfo> resourceInfos;
    if (!BundleResourceProcess::GetResourceInfoByBundleName(bundleName, userId, resourceInfos)) {
        APP_LOGE("bundleName: %{public}s GetResourceInfoByBundleName failed", bundleName.c_str());
        return false;
    }
    return AddResourceInfos(resourceInfos);
}

bool BundleResourceManager::AddResourceInfoByAbility(const std::string &bundleName, const std::string &moduleName,
    const std::string &abilityName, const int32_t userId)
{
    ResourceInfo resourceInfo;
    if (!BundleResourceProcess::GetResourceInfoByAbilityName(bundleName, moduleName, abilityName,
        userId, resourceInfo)) {
        APP_LOGE("bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s failed",
            bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
        return false;
    }
    return AddResourceInfo(resourceInfo);
}

bool BundleResourceManager::AddAllResourceInfo(const int32_t userId)
{
    std::vector<ResourceInfo> resourceInfos;
    if (!BundleResourceProcess::GetAllResourceInfo(userId, resourceInfos)) {
        APP_LOGE("GetAllResourceInfo failed, userId:%{public}d", userId);
        return false;
    }

    return AddResourceInfos(resourceInfos);
}

bool BundleResourceManager::DeleteAllResourceInfo()
{
    return bundleResourceRdb_->DeleteAllResourceInfo();
}

bool BundleResourceManager::AddResourceInfo(ResourceInfo &resourceInfo)
{
    // need to parse label and icon
    BundleResourceParser parser;
    if (!parser.ParseResourceInfo(resourceInfo)) {
        APP_LOGE("key: %{public}s ParseResourceInfo failed", resourceInfo.GetKey().c_str());
        return false;
    }
    return bundleResourceRdb_->AddResourceInfo(resourceInfo);
}

bool BundleResourceManager::AddResourceInfos(std::vector<ResourceInfo> &resourceInfos)
{
    if (resourceInfos.empty()) {
        APP_LOGE("resourceInfos is empty.");
        return false;
    }
    // need to parse label and icon
    BundleResourceParser parser;
    if (!parser.ParseResourceInfos(resourceInfos)) {
        APP_LOGE("Parse ResourceInfos failed");
        return false;
    }
    return bundleResourceRdb_->AddResourceInfos(resourceInfos);
}

bool BundleResourceManager::DeleteResourceInfo(const std::string &key)
{
    return bundleResourceRdb_->DeleteResourceInfo(key);
}

bool BundleResourceManager::GetAllResourceName(std::vector<std::string> &keyNames)
{
    return bundleResourceRdb_->GetAllResourceName(keyNames);
}

bool BundleResourceManager::AddResourceInfoByColorModeChanged(const int32_t userId)
{
    std::vector<ResourceInfo> resourceInfos;
    // to judge whether the current colorMode exists in the database
    bool isExist = bundleResourceRdb_->IsCurrentColorModeExist();
    if (isExist) {
        // exist then update new install bundles
        std::vector<std::string> names;
        if (!bundleResourceRdb_->GetAllResourceName(names)) {
            APP_LOGE("GetAllResourceName failed");
            return false;
        }
        if (!BundleResourceProcess::GetResourceInfoByColorModeChanged(names, resourceInfos)) {
            APP_LOGE("GetResourceInfoByColorModeChanged failed");
            return false;
        }
    } else {
        // not exist then update all resource info
        if (!BundleResourceProcess::GetAllResourceInfo(userId, resourceInfos)) {
            APP_LOGE("GetAllResourceInfo failed, userId:%{public}d", userId);
            return false;
        }
    }
    return AddResourceInfos(resourceInfos);
}
} // AppExecFwk
} // OHOS