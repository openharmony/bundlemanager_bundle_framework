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

#ifndef FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_MANAGER_H
#define FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_MANAGER_H

#include <string>
#include <vector>

#include "bundle_resource_rdb.h"
#include "bundle_system_state.h"
#include "inner_bundle_info.h"
#include "resource_info.h"
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
class BundleResourceManager : public DelayedSingleton<BundleResourceManager> {
public:
    BundleResourceManager();

    ~BundleResourceManager();
    /**
     * add bundle resource and launcher ability resource by innerBundleInfo, used when install hap
     */
    bool AddResourceInfo(const InnerBundleInfo &innerBundleInfo, const int32_t userId, std::string hapPath = "");
    /**
     * delete resource info
     */
    bool DeleteResourceInfo(const std::string &key);
    /**
     * add all resource info in system, used when system configuration changed, like:
     * language, colorMode, theme, and so on
     */
    bool AddAllResourceInfo(const int32_t userId);
    /**
     * delete all resource info
     */
    bool DeleteAllResourceInfo();
    /**
     * add bundle resource info by bundleName, used when bundle state changed, like:
     * enable or disable
     */
    bool AddResourceInfoByBundleName(const std::string &bundleName, const int32_t userId);
    /**
     * add launcher ability resource info by abilityName, used when ability state changed, like:
     * enable or disable
     */
    bool AddResourceInfoByAbility(const std::string &bundleName, const std::string &moduleName,
        const std::string &abilityName, const int32_t userId);

    bool GetAllResourceName(std::vector<std::string> &keyNames);
    /**
     * add resource info by colorMode changed
     */
    bool AddResourceInfoByColorModeChanged(const int32_t userId);

private:
    bool AddResourceInfo(ResourceInfo &resourceInfo);

    bool AddResourceInfos(std::vector<ResourceInfo> &resourceInfos);

    std::shared_ptr<BundleResourceRdb> bundleResourceRdb_;
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_MANAGER_H