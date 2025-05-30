/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_BUNDLE_USER_INFO_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_BUNDLE_USER_INFO_H

#include "bundle_user_info.h"
#include "inner_bundle_clone_info.h"
#include "json_util.h"
#include "plugin/plugin_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {
struct InnerBundleUserInfo {
    // app install control
    bool isRemovable = true;

    int32_t uid = Constants::INVALID_UID;
    uint32_t accessTokenId = 0;
    uint64_t accessTokenIdEx = 0;

    // The time(unix time) will be recalculated
    // if the application is reinstalled after being uninstalled.
    int64_t installTime = 0;

    // The time(unix time) will be recalculated
    // if the application is uninstalled after being installed.
    int64_t updateTime = 0;

    // app first install time
    int64_t firstInstallTime = 0;
    std::string bundleName;

    // encryption key id
    std::string keyId;
    std::vector<int32_t> gids;

    // dynamic icon
    std::string curDynamicIconModule;

    // appIndex -> cloneInfo
    std::map<std::string, InnerBundleCloneInfo> cloneInfos;

    // use to record plugin that exist in current user
    std::unordered_set<std::string> installedPluginSet;
    BundleUserInfo bundleUserInfo;

    bool operator() (const InnerBundleUserInfo& info) const
    {
        if (bundleName == info.bundleName) {
            return bundleUserInfo.userId == info.bundleUserInfo.userId;
        }

        return false;
    }

    static std::string AppIndexToKey(const int32_t appIndex)
    {
        return std::to_string(appIndex);
    }

    bool IsPluginInstalled(const std::string &pluginBundleName) const
    {
        return installedPluginSet.find(pluginBundleName) != installedPluginSet.end();
    }
};

void from_json(const nlohmann::json& jsonObject, InnerBundleUserInfo& bundleUserInfo);
void to_json(nlohmann::json& jsonObject, const InnerBundleUserInfo& bundleUserInfo);
} // namespace AppExecFwk
} // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_BUNDLE_USER_INFO_H