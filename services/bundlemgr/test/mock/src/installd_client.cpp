/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "installd_client.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "installd_death_recipient.h"
#include "system_ability_definition.h"
#include "system_ability_helper.h"

namespace OHOS {
namespace AppExecFwk {
ErrCode InstalldClient::CreateBundleDir(const std::string &bundleDir)
{
    if (bundleDir.empty()) {
        APP_LOGE("bundle dir is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::CreateBundleDir, bundleDir);
}

ErrCode InstalldClient::ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath,
    const std::string &targetSoPath, const std::string &cpuAbi)
{
    if (srcModulePath.empty() || targetPath.empty()) {
        APP_LOGE("src module path or target path is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::ExtractModuleFiles, srcModulePath, targetPath, targetSoPath, cpuAbi);
}

ErrCode InstalldClient::ExtractFiles(const ExtractParam &extractParam)
{
    if (extractParam.srcPath.empty() || extractParam.targetPath.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ExtractFiles, extractParam);
}

ErrCode InstalldClient::ExecuteAOT(const AOTArgs &aotArgs)
{
    return CallService(&IInstalld::ExecuteAOT, aotArgs);
}

ErrCode InstalldClient::RenameModuleDir(const std::string &oldPath, const std::string &newPath)
{
    if (oldPath.empty() || newPath.empty()) {
        APP_LOGE("rename path is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::RenameModuleDir, oldPath, newPath);
}

ErrCode InstalldClient::CreateBundleDataDir(const CreateDirParam &createDirParam)
{
    if (createDirParam.bundleName.empty() || createDirParam.userId < 0
        || createDirParam.uid < 0 || createDirParam.gid < 0) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::CreateBundleDataDir, createDirParam);
}

ErrCode InstalldClient::RemoveBundleDataDir(
    const std::string &bundleName, const int userid)
{
    if (bundleName.empty() || userid < 0) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::RemoveBundleDataDir, bundleName, userid);
}

ErrCode InstalldClient::RemoveModuleDataDir(const std::string &ModuleName, const int userid)
{
    if (ModuleName.empty() || userid < 0) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::RemoveModuleDataDir, ModuleName, userid);
}

ErrCode InstalldClient::RemoveDir(const std::string &dir)
{
    if (dir.empty()) {
        APP_LOGE("dir removed is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::RemoveDir, dir);
}

ErrCode InstalldClient::CleanBundleDataDir(const std::string &bundleDir)
{
    if (bundleDir.empty()) {
        APP_LOGE("bundle dir is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::CleanBundleDataDir, bundleDir);
}

ErrCode InstalldClient::GetBundleStats(
    const std::string &bundleName, const int32_t userId, std::vector<int64_t> &bundleStats)
{
    if (bundleName.empty()) {
        APP_LOGE("bundleName is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::GetBundleStats, bundleName, userId, bundleStats);
}

ErrCode InstalldClient::SetDirApl(const std::string &dir, const std::string &bundleName, const std::string &apl,
    bool isPreInstallApp, bool debug)
{
    if (dir.empty() || bundleName.empty() || apl.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::SetDirApl, dir, bundleName, apl, isPreInstallApp, debug);
}

ErrCode InstalldClient::GetBundleCachePath(const std::string &dir, std::vector<std::string> &cachePath)
{
    if (dir.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::GetBundleCachePath, dir, cachePath);
}

void InstalldClient::ResetInstalldProxy()
{
    if ((installdProxy_ != nullptr) && (installdProxy_->AsObject() != nullptr)) {
        installdProxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    installdProxy_ = nullptr;
}

bool InstalldClient::GetInstalldProxy()
{
    if (installdProxy_ == nullptr) {
        APP_LOGD("try to get installd proxy");
        std::lock_guard<std::mutex> lock(mutex_);
        if (installdProxy_ == nullptr) {
            sptr<IInstalld> tempProxy =
                iface_cast<IInstalld>(SystemAbilityHelper::GetSystemAbility(INSTALLD_SERVICE_ID));
            if ((tempProxy == nullptr) || (tempProxy->AsObject() == nullptr)) {
                APP_LOGE("the installd proxy or remote object is null");
                return false;
            }
            recipient_ = new (std::nothrow) InstalldDeathRecipient();
            if (recipient_ == nullptr) {
                APP_LOGE("the death recipient is nullptr");
                return false;
            }
            tempProxy->AsObject()->AddDeathRecipient(recipient_);
            installdProxy_ = tempProxy;
        }
    }
    return true;
}

ErrCode InstalldClient::ScanDir(
    const std::string &dir, ScanMode scanMode, ResultMode resultMode, std::vector<std::string> &paths)
{
    if (dir.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::ScanDir, dir, scanMode, resultMode, paths);
}

ErrCode InstalldClient::MoveFile(const std::string &oldPath, const std::string &newPath)
{
    if (oldPath.empty() || newPath.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::MoveFile, oldPath, newPath);
}

ErrCode InstalldClient::CopyFile(const std::string &oldPath, const std::string &newPath,
    const std::string &signatureFilePath)
{
    if (oldPath.empty() || newPath.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::CopyFile, oldPath, newPath, signatureFilePath);
}

ErrCode InstalldClient::Mkdir(
    const std::string &dir, const int32_t mode, const int32_t uid, const int32_t gid)
{
    if (dir.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::Mkdir, dir, mode, uid, gid);
}

ErrCode InstalldClient::GetFileStat(const std::string &file, FileStat &fileStat)
{
    if (file.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::GetFileStat, file, fileStat);
}

ErrCode InstalldClient::ExtractDiffFiles(const std::string &filePath, const std::string &targetPath,
    const std::string &cpuAbi)
{
    if (filePath.empty() || targetPath.empty() || cpuAbi.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ExtractDiffFiles, filePath, targetPath, cpuAbi);
}

ErrCode InstalldClient::ApplyDiffPatch(const std::string &oldSoPath, const std::string &diffFilePath,
    const std::string &newSoPath)
{
    if (oldSoPath.empty() || diffFilePath.empty() || newSoPath.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ApplyDiffPatch, oldSoPath, diffFilePath, newSoPath);
}

ErrCode InstalldClient::IsExistDir(const std::string &dir, bool &isExist)
{
    return CallService(&IInstalld::IsExistDir, dir, isExist);
}

ErrCode InstalldClient::IsExistFile(const std::string &path, bool &isExist)
{
    return CallService(&IInstalld::IsExistFile, path, isExist);
}

ErrCode InstalldClient::IsDirEmpty(const std::string &dir, bool &isDirEmpty)
{
    return CallService(&IInstalld::IsDirEmpty, dir, isDirEmpty);
}

ErrCode InstalldClient::ObtainQuickFixFileDir(const std::string &dir, std::vector<std::string> &dirVec)
{
    return CallService(&IInstalld::ObtainQuickFixFileDir, dir, dirVec);
}

ErrCode InstalldClient::CopyFiles(const std::string &sourceDir, const std::string &destinationDir)
{
    return CallService(&IInstalld::CopyFiles, sourceDir, destinationDir);
}

ErrCode InstalldClient::GetNativeLibraryFileNames(const std::string &filePath, const std::string &cpuAbi,
    std::vector<std::string> &fileNames)
{
    return CallService(&IInstalld::GetNativeLibraryFileNames, filePath, cpuAbi, fileNames);
}

ErrCode InstalldClient::VerifyCodeSignature(const std::string &modulePath, const std::string &cpuAbi,
    const std::string &targetSoPath, const std::string &signatureFileDir)
{
    if (modulePath.empty() || cpuAbi.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::VerifyCodeSignature, modulePath, cpuAbi, targetSoPath,
        signatureFileDir);
}

ErrCode InstalldClient::MoveFiles(const std::string &srcDir, const std::string &desDir)
{
    if (srcDir.empty() || desDir.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::MoveFiles, srcDir, desDir);
}

bool InstalldClient::StartInstalldService()
{
    return GetInstalldProxy();
}
}  // namespace AppExecFwk
}  // namespace OHOS