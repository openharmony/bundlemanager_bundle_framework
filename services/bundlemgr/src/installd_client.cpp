/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "installd/installd_load_callback.h"
#include "installd_death_recipient.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "system_ability_helper.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr int16_t LOAD_SA_TIMEOUT_MS = 4 * 1000;
} // namespace

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
        APP_LOGE("src path or target path is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ExtractFiles, extractParam);
}

ErrCode InstalldClient::ExtractHnpFiles(const std::string &hnpPackageInfo, const ExtractParam &extractParam)
{
    if (extractParam.srcPath.empty() || extractParam.targetPath.empty() || hnpPackageInfo.empty()) {
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ExtractHnpFiles, hnpPackageInfo, extractParam);
}

ErrCode InstalldClient::ProcessBundleInstallNative(const std::string &userId, const std::string &hnpRootPath,
    const std::string &hapPath, const std::string &cpuAbi, const std::string &packageName)
{
    return CallService(&IInstalld::ProcessBundleInstallNative, userId, hnpRootPath,
        hapPath, cpuAbi, packageName);
}

ErrCode InstalldClient::ProcessBundleUnInstallNative(const std::string &userId, const std::string &packageName)
{
    return CallService(&IInstalld::ProcessBundleUnInstallNative, userId, packageName);
}

ErrCode InstalldClient::ExecuteAOT(const AOTArgs &aotArgs, std::vector<uint8_t> &pendSignData)
{
    return CallService(&IInstalld::ExecuteAOT, aotArgs, pendSignData);
}

ErrCode InstalldClient::PendSignAOT(const std::string &anFileName, const std::vector<uint8_t> &signData)
{
    return CallService(&IInstalld::PendSignAOT, anFileName, signData);
}

ErrCode InstalldClient::StopAOT()
{
    return CallService(&IInstalld::StopAOT);
}

ErrCode InstalldClient::DeleteUninstallTmpDirs(const std::vector<std::string> &dirs)
{
    if (dirs.empty()) {
        APP_LOGE("dirs empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::DeleteUninstallTmpDirs, dirs);
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

ErrCode InstalldClient::CreateBundleDataDirWithVector(const std::vector<CreateDirParam> &createDirParams)
{
    return CallService(&IInstalld::CreateBundleDataDirWithVector, createDirParams);
}

ErrCode InstalldClient::RemoveBundleDataDir(
    const std::string &bundleName, const int32_t userId, bool isAtomicService, const bool async)
{
    if (bundleName.empty() || userId < 0) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::RemoveBundleDataDir, bundleName, userId, isAtomicService, async);
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

ErrCode InstalldClient::GetDiskUsage(const std::string &dir, int64_t &statSize, bool isRealPath)
{
    if (dir.empty()) {
        APP_LOGE("bundle dir is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::GetDiskUsage, dir, statSize, isRealPath);
}

ErrCode InstalldClient::GetDiskUsageFromPath(const std::vector<std::string> &path, int64_t &statSize)
{
    if (path.empty()) {
        APP_LOGE("path is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::GetDiskUsageFromPath, path, statSize);
}

ErrCode InstalldClient::CleanBundleDataDir(const std::string &bundleDir)
{
    if (bundleDir.empty()) {
        APP_LOGE("bundle dir is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::CleanBundleDataDir, bundleDir);
}

ErrCode InstalldClient::CleanBundleDataDirByName(const std::string &bundleName, const int userid, const int appIndex)
{
    if (bundleName.empty() || userid < 0 || appIndex < 0 || appIndex > Constants::INITIAL_SANDBOX_APP_INDEX) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::CleanBundleDataDirByName, bundleName, userid, appIndex);
}

ErrCode InstalldClient::GetBundleStats(const std::string &bundleName, const int32_t userId,
    std::vector<int64_t> &bundleStats, const int32_t uid, const int32_t appIndex,
    const uint32_t statFlag, const std::vector<std::string> &moduleNameList)
{
    if (bundleName.empty()) {
        APP_LOGE("bundleName is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::GetBundleStats, bundleName, userId, bundleStats,
        uid, appIndex, statFlag, moduleNameList);
}

ErrCode InstalldClient::BatchGetBundleStats(const std::vector<std::string> &bundleNames, const int32_t userId,
    const std::unordered_map<std::string, int32_t> &uidMap,
    std::vector<BundleStorageStats> &bundleStats)
{
    if (bundleNames.empty() || uidMap.empty()) {
        APP_LOGE("bundleNames or uidMap is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::BatchGetBundleStats, bundleNames, userId, uidMap, bundleStats);
}

ErrCode InstalldClient::GetAllBundleStats(const int32_t userId,
    std::vector<int64_t> &bundleStats, const std::vector<int32_t> &uids)
{
    if (uids.empty()) {
        APP_LOGE("uids is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::GetAllBundleStats, userId, bundleStats, uids);
}

ErrCode InstalldClient::SetDirApl(const std::string &dir, const std::string &bundleName, const std::string &apl,
    bool isPreInstallApp, bool debug, int32_t uid)
{
    if (dir.empty() || bundleName.empty() || apl.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::SetDirApl, dir, bundleName, apl, isPreInstallApp, debug, uid);
}

ErrCode InstalldClient::SetArkStartupCacheApl(const std::string &dir)
{
    if (dir.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::SetArkStartupCacheApl, dir);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if ((installdProxy_ != nullptr) && (installdProxy_->AsObject() != nullptr)) {
        installdProxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    installdProxy_ = nullptr;
}

bool InstalldClient::LoadInstalldService()
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        APP_LOGE("Failed to get SystemAbilityManager");
        return false;
    }
    sptr<InstalldLoadCallback> loadCallback = new (std::nothrow) InstalldLoadCallback();
    if (loadCallback == nullptr) {
        APP_LOGE("Create load callback failed");
        return false;
    }
    auto ret = systemAbilityMgr->LoadSystemAbility(INSTALLD_SERVICE_ID, loadCallback);
    if (ret != 0) {
        APP_LOGE("Load system ability %{public}d failed with %{public}d", INSTALLD_SERVICE_ID, ret);
        return false;
    }

    auto waitStatus = loadSaCondition_.wait_for(lock, std::chrono::milliseconds(LOAD_SA_TIMEOUT_MS),
        [this]() {
            return installdProxy_ != nullptr;
        });
    if (!waitStatus) {
        APP_LOGE("Wait for load sa timeout");
        return false;
    }
    return true;
}

sptr<IInstalld> InstalldClient::GetInstalldProxy()
{
    std::lock_guard<std::mutex> lockProxy(getProxyMutex_);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (installdProxy_ != nullptr) {
            APP_LOGD("installd ready");
            return installdProxy_;
        }
    }

    APP_LOGI("try to get installd proxy");
    if (!LoadInstalldService()) {
        APP_LOGE("load installd service failed");
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if ((installdProxy_ == nullptr) || (installdProxy_->AsObject() == nullptr)) {
        APP_LOGE("the installd proxy or remote object is null");
        return nullptr;
    }

    recipient_ = new (std::nothrow) InstalldDeathRecipient();
    if (recipient_ == nullptr) {
        APP_LOGE("the death recipient is nullptr");
        return nullptr;
    }
    installdProxy_->AsObject()->AddDeathRecipient(recipient_);
    return installdProxy_;
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
        APP_LOGE("file path or target path or cpuAbi is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ExtractDiffFiles, filePath, targetPath, cpuAbi);
}

ErrCode InstalldClient::ApplyDiffPatch(const std::string &oldSoPath, const std::string &diffFilePath,
    const std::string &newSoPath, int32_t uid)
{
    if (oldSoPath.empty() || diffFilePath.empty() || newSoPath.empty()) {
        APP_LOGE("old path or diff file path or new so path is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ApplyDiffPatch, oldSoPath, diffFilePath, newSoPath, uid);
}

ErrCode InstalldClient::IsExistDir(const std::string &dir, bool &isExist)
{
    return CallService(&IInstalld::IsExistDir, dir, isExist);
}

ErrCode InstalldClient::IsExistFile(const std::string &path, bool &isExist)
{
    return CallService(&IInstalld::IsExistFile, path, isExist);
}

ErrCode InstalldClient::IsExistApFile(const std::string &path, bool &isExist)
{
    return CallService(&IInstalld::IsExistApFile, path, isExist);
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

ErrCode InstalldClient::VerifyCodeSignature(const CodeSignatureParam &codeSignatureParam)
{
    if (codeSignatureParam.modulePath.empty()) {
        APP_LOGE("module path is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::VerifyCodeSignature, codeSignatureParam);
}

ErrCode InstalldClient::CheckEncryption(const CheckEncryptionParam &checkEncryptionParam, bool &isEncryption)
{
    if (checkEncryptionParam.modulePath.empty()) {
        APP_LOGE("module path is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::CheckEncryption, checkEncryptionParam, isEncryption);
}

ErrCode InstalldClient::MoveFiles(const std::string &srcDir, const std::string &desDir)
{
    if (srcDir.empty() || desDir.empty()) {
        APP_LOGE("src dir or des dir is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::MoveFiles, srcDir, desDir);
}


ErrCode InstalldClient::ExtractDriverSoFiles(const std::string &srcPath,
    const std::unordered_multimap<std::string, std::string> &dirMap)
{
    if (srcPath.empty() || dirMap.empty()) {
        APP_LOGE("src path or dir map is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ExtractDriverSoFiles, srcPath, dirMap);
}

ErrCode InstalldClient::VerifyCodeSignatureForHap(const CodeSignatureParam &codeSignatureParam)
{
    if (codeSignatureParam.modulePath.empty()) {
        APP_LOGE("module path is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::VerifyCodeSignatureForHap, codeSignatureParam);
}

ErrCode InstalldClient::DeliverySignProfile(const std::string &bundleName, int32_t profileBlockLength,
    const unsigned char *profileBlock)
{
    if (bundleName.empty() || profileBlock == nullptr) {
        APP_LOGE("bundle name or profile block is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::DeliverySignProfile, bundleName, profileBlockLength, profileBlock);
}

ErrCode InstalldClient::RemoveSignProfile(const std::string &bundleName)
{
    if (bundleName.empty()) {
        APP_LOGE("bundle name is empty");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::RemoveSignProfile, bundleName);
}

void InstalldClient::OnLoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject)
{
    std::lock_guard<std::mutex> lock(mutex_);
    installdProxy_ = iface_cast<IInstalld>(remoteObject);
    loadSaCondition_.notify_one();
}

void InstalldClient::OnLoadSystemAbilityFail()
{
    std::lock_guard<std::mutex> lock(mutex_);
    installdProxy_ = nullptr;
    loadSaCondition_.notify_one();
}

bool InstalldClient::StartInstalldService()
{
    return GetInstalldProxy() != nullptr;
}

ErrCode InstalldClient::ExtractEncryptedSoFiles(const std::string &hapPath, const std::string &realSoFilesPath,
    const std::string &cpuAbi, const std::string &tmpSoPath, int32_t uid)
{
    if (hapPath.empty() || tmpSoPath.empty() || cpuAbi.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::ExtractEncryptedSoFiles, hapPath, realSoFilesPath, cpuAbi, tmpSoPath, uid);
}

ErrCode InstalldClient::SetEncryptionPolicy(const EncryptionParam &encryptionParam, std::string &keyId)
{
    if (encryptionParam.bundleName.empty() && encryptionParam.groupId.empty()) {
        APP_LOGE("param error");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::SetEncryptionPolicy, encryptionParam, keyId);
}

ErrCode InstalldClient::MigrateData(const std::vector<std::string> &sourcePaths, const std::string &destinationPath)
{
    if (sourcePaths.empty()) {
        APP_LOGE("sourcePaths param is invalid");
        return ERR_BUNDLE_MANAGER_MIGRATE_DATA_SOURCE_PATH_INVALID;
    }
    if (destinationPath.empty()) {
        APP_LOGE("destinationPath param is invalid");
        return ERR_BUNDLE_MANAGER_MIGRATE_DATA_DESTINATION_PATH_INVALID;
    }
    return CallService(&IInstalld::MigrateData, sourcePaths, destinationPath);
}

ErrCode InstalldClient::DeleteEncryptionKeyId(const EncryptionParam &encryptionParam)
{
    if (encryptionParam.bundleName.empty() && encryptionParam.groupId.empty()) {
        APP_LOGE("param error");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::DeleteEncryptionKeyId, encryptionParam);
}

ErrCode InstalldClient::RemoveExtensionDir(int32_t userId, const std::vector<std::string> &extensionBundleDirs)
{
    if (extensionBundleDirs.empty() || userId < 0) {
        APP_LOGI("extensionBundleDirs empty or userId invalid");
        return ERR_OK;
    }
    return CallService(&IInstalld::RemoveExtensionDir, userId, extensionBundleDirs);
}

ErrCode InstalldClient::IsExistExtensionDir(int32_t userId, const std::string &extensionBundleDir, bool &isExist)
{
    if (extensionBundleDir.empty() || userId < 0) {
        APP_LOGE("extensionBundleDir is empty or userId is invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::IsExistExtensionDir, userId, extensionBundleDir, isExist);
}

ErrCode InstalldClient::CreateExtensionDataDir(const CreateDirParam &createDirParam)
{
    if (createDirParam.bundleName.empty() || createDirParam.userId < 0
        || createDirParam.uid < 0 || createDirParam.gid < 0 || createDirParam.extensionDirs.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::CreateExtensionDataDir, createDirParam);
}

ErrCode InstalldClient::GetExtensionSandboxTypeList(std::vector<std::string> &typeList)
{
    return CallService(&IInstalld::GetExtensionSandboxTypeList, typeList);
}

ErrCode InstalldClient::AddUserDirDeleteDfx(int32_t userId)
{
    return CallService(&IInstalld::AddUserDirDeleteDfx, userId);
}

ErrCode InstalldClient::MoveHapToCodeDir(const std::string &originPath, const std::string &targetPath)
{
    if (originPath.empty() || targetPath.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }

    return CallService(&IInstalld::MoveHapToCodeDir, originPath, targetPath);
}

ErrCode InstalldClient::CreateDataGroupDirs(const std::vector<CreateDirParam> &params)
{
    if (params.empty()) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::CreateDataGroupDirs, params);
}

ErrCode InstalldClient::DeleteDataGroupDirs(const std::vector<std::string> &uuidList, int32_t userId)
{
    if (uuidList.empty() || userId < 0) {
        APP_LOGE("params are invalid");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    return CallService(&IInstalld::DeleteDataGroupDirs, uuidList, userId);
}

ErrCode InstalldClient::LoadInstalls()
{
    return CallService(&IInstalld::LoadInstalls);
}

ErrCode InstalldClient::ClearDir(const std::string &dir)
{
    return CallService(&IInstalld::ClearDir, dir);
}
}  // namespace AppExecFwk
}  // namespace OHOS
