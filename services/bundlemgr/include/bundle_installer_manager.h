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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_MANAGER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_MANAGER_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "nocopyable.h"

#include "bundle_installer.h"
#include "status_receiver_interface.h"
#include "thread_pool.h"

namespace OHOS {
namespace AppExecFwk {
using ThreadPoolTask = std::function<void()>;
class BundleInstallerManager : public std::enable_shared_from_this<BundleInstallerManager> {
public:
    BundleInstallerManager();
    ~BundleInstallerManager();
    /**
     * @brief Create a bundle installer object for installing a bundle.
     * @param bundleFilePath Indicates the path for storing the HAP of the bundle to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return
     */
    void CreateInstallTask(const std::string &bundleFilePath, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for installing a bundle by bundleName.
     * @param bundleName Indicates the bundleName.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return
     */
    void CreateInstallByBundleNameTask(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for installing a bundle by bundleName.
     * @param bundleFilePath Indicates the path for storing the HAP of the bundle to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return
     */
    void CreateRecoverTask(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for installing multiple haps of a bundle.
     * @param bundleFilePaths Indicates the paths for storing the HAPs of the bundle to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return
     */
    void CreateInstallTask(const std::vector<std::string> &bundleFilePaths, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for uninstalling an bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return
     */
    void CreateUninstallTask(
        const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for uninstalling a module.
     * @param bundleName Indicates the bundle name of the module to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return
     */
    void CreateUninstallTask(const std::string &bundleName, const std::string &modulePackage,
        const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for uninstalling a module.
     * @param uninstallParam Indicates the input of uninstall param.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return
     */
    void CreateUninstallTask(const UninstallParam &uninstallParam, const sptr<IStatusReceiver> &statusReceive);

    void CreateUninstallAndRecoverTask(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);

    void AddTask(const ThreadPoolTask &task, const std::string &taskName);
    size_t GetCurTaskNum();
    int32_t GetThreadsNum()
    {
        return threadNum_;
    }

private:
    /**
     * @brief Create a bundle installer object internal.
     * @param statusReceiver Indicates the callback object for this installer.
     * @return Returns a pointers to BundleInstaller object.
     */
    std::shared_ptr<BundleInstaller> CreateInstaller(const sptr<IStatusReceiver> &statusReceiver);

    void DelayStopThreadPool();

    DISALLOW_COPY_AND_MOVE(BundleInstallerManager);

    int32_t threadNum_ = std::thread::hardware_concurrency();
    std::mutex mutex_;
    std::shared_ptr<ThreadPool> threadPool_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_MANAGER_H
