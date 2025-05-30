/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H

#include "base_bundle_installer.h"

namespace OHOS {
namespace AppExecFwk {
class SystemBundleInstaller : public BaseBundleInstaller {
public:
    SystemBundleInstaller();
    virtual ~SystemBundleInstaller() override;
    /**
     * @brief Install preinstall app.
     * @param filePath Indicates the filePath.
     * @param installParam Indicates the installParam.
     * @param appType Indicates the bundle type.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    ErrCode InstallSystemBundle(
        const std::string &filePath,
        InstallParam &installParam,
        Constants::AppType appType);
    /**
     * @brief Install preinstall hsp.
     * @param installParam Indicates the installParam.
     * @param isOTA OTA install or not.
     * @param appType Indicates the bundle type.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool InstallSystemSharedBundle(InstallParam &installParam, bool isOTA, Constants::AppType appType);
    /**
     * @brief OTA Install preinstall app.
     * @param filePath Indicates the filePaths.
     * @param installParam Indicates the installParam.
     * @param appType Indicates the bundle type.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    ErrCode OTAInstallSystemBundle(
        const std::vector<std::string> &filePaths,
        InstallParam &installParam,
        Constants::AppType appType);
    /**
     * @brief OTA Install preinstall app.
     * @param filePath Indicates the filePaths.
     * @param installParam Indicates the installParam.
     * @param bundleName Indicates the bundleName.
     * @param appType Indicates the bundle type.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    ErrCode OTAInstallSystemBundleNeedCheckUser(
        const std::vector<std::string> &filePaths,
        InstallParam &installParam,
        const std::string &bundleName,
        Constants::AppType appType);
    /**
     * @brief OTA Install preinstall app.
     * @param filePath Indicates the filePaths.
     * @param installParam Indicates the installParam.
     * @param bundleName Indicates the bundleName.
     * @param appType Indicates the bundle type.
     * @param userIds Indicates the user to which the app needs to be installed.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    ErrCode OTAInstallSystemBundleTargetUser(
        const std::vector<std::string> &filePaths,
        InstallParam &installParam,
        const std::string &bundleName,
        Constants::AppType appType,
        const std::vector<int32_t> &userIds);
    /**
     * @brief Uninstall preinstall app.
     * @param bundleName Indicates the bundle name.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool UninstallSystemBundle(const std::string &bundleName);

    bool UninstallSystemBundle(const std::string &bundleName, const InstallParam &installParam);
    /**
     * @brief Deleting apps while retaining user data.
     * @param bundleName Indicates the bundle name.
     * @param isKeepData Delete app keep app data.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool UninstallSystemBundle(const std::string &bundleName, bool isKeepData);
    /**
     * @brief Uninstall preinstall app by modulePackage.
     * @param bundleName Indicates the bundle name.
     * @param moduleName Indicates the module name.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool UninstallSystemBundle(const std::string &bundleName, const std::string &modulePackage);
    void CheckUninstallSystemHsp(const std::string &bundleName);

private:

    DISALLOW_COPY_AND_MOVE(SystemBundleInstaller);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H