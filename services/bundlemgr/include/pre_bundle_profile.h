/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_BUNDLE_PROFILE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_BUNDLE_PROFILE_H

#include <set>

#include "appexecfwk_errors.h"
#include "json_util.h"
#include "pre_scan_info.h"
#include <unordered_set>
namespace OHOS {
namespace AppExecFwk {
class PreBundleProfile {
public:
    /**
     * @brief Transform the jsonBuf to scanInfos.
     * @param jsonBuf Indicates the jsonBuf.
     * @param scanInfos Indicates the scanInfos.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransformTo(
        const nlohmann::json &jsonBuf,
        std::set<PreScanInfo> &scanInfos) const;
    /**
     * @brief Transform the jsonBuf to scanAppListInfos.
     * @param jsonBuf Indicates the jsonBuf.
     * @param scanAppInfos Indicates the scanAppInfos.
     * @param scanDemandInfos Indicates the scanDemandInfos.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransformToAppList(const nlohmann::json &jsonBuf, std::set<PreScanInfo> &scanAppInfos,
        std::set<PreScanInfo> &scanDemandInfos) const;
    /**
     * @brief Transform the jsonBuf to scanDemandInfos.
     * @param jsonBuf Indicates the jsonBuf.
     * @param scanDemandInfos Indicates the scanDemandInfos.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransformToDemandList(const nlohmann::json &jsonBuf, std::set<PreScanInfo> &scanDemandInfos) const;
    /**
     * @brief Transform the jsonBuf to bundleNames.
     * @param jsonBuf Indicates the jsonBuf.
     * @param uninstallList Indicates the uninstallList.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransformTo(
        const nlohmann::json &jsonBuf,
        std::set<std::string> &uninstallList) const;
    /**
     * @brief Transform the jsonBuf to preBundleConfigInfos.
     * @param jsonBuf Indicates the jsonBuf.
     * @param preBundleConfigInfos Indicates the preBundleConfigInfos.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransformTo(
        const nlohmann::json &jsonBuf,
        std::set<PreBundleConfigInfo> &preBundleConfigInfos) const;
    /**
     * @brief Transform the jsonBuf to extensionTypeConfig.
     * @param jsonBuf Indicates the jsonBuf.
     * @param extensionTypeList Indicates the obtained extension type name list.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransformJsonToExtensionTypeList(
        const nlohmann::json &jsonBuf, std::set<std::string> &extensionTypeList) const;
    void ProcessOnDemandList(std::set<PreScanInfo> &scanAppInfos,
        std::set<PreScanInfo> &scanDemandInfos) const;
    /**
     * @brief Transform the jsonBuf to bundleNames.
     * @param jsonBuf Indicates the jsonBuf.
     * @param arkStartupCacheList Indicates the ark atsrtup cache bundlenames.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransToArkStartupCacheList(
        const nlohmann::json &jsonBuf,
        std::unordered_set<std::string> &arkStartupCacheList) const;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_BUNDLE_PROFILE_H
