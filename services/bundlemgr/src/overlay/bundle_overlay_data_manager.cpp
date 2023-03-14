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

#include "bundle_overlay_data_manager.h"

#include "bundle_common_event_mgr.h"
#include "bundle_mgr_service.h"
#include "ipc_skeleton.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
ErrCode OverlayDataMgr::UpdateOverlayInfo(const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    // 1. update internal overlay info
    if (newInfo.GetOverlayType() == OVERLAY_INTERNAL_BUNDLE) {
        return UpdateInternalOverlayInfo(newInfo, oldInfo);
    }
    // 2. update external overlay info
    if (newInfo.GetOverlayType() == OVERLAY_EXTERNAL_BUNDLE) {
        return UpdateExternalOverlayInfo(newInfo, oldInfo);
    }
    // 3. update overlay connection
    if (newInfo.GetOverlayType() == NON_OVERLAY_TYPE) {
        return BuildOverlayConnection(newInfo, oldInfo);
    }
    return ERR_OK;
}

bool OverlayDataMgr::IsExistedNonOverlayHap(const std::string &bundleName)
{
    if (bundleName.empty()) {
        APP_LOGE("bundle name %{public}s is invalid", bundleName.c_str());
        return false;
    }

    if (GetBundleDataMgr() != ERR_OK) {
        return false;
    }
    InnerBundleInfo innerBundleInfo;
    if (!dataMgr_->GetOverlayInnerBundleInfo(bundleName, innerBundleInfo)) {
        APP_LOGE("no bundle with bundleName %{public}s installed", bundleName.c_str());
        return false;
    }
    dataMgr_->EnableOverlayBundle(bundleName);
    const auto &innerModuleInfos = innerBundleInfo.GetInnerModuleInfos();
    if (innerModuleInfos.empty()) {
        APP_LOGE("innerModuleInfo in innerBundleInfo is empty");
        return false;
    }
    for (const auto &innerModuleInfo : innerModuleInfos) {
        if (innerModuleInfo.second.targetModuleName.empty()) {
            return true;
        }
    }
    APP_LOGW("only overlay hap existed");
    return false;
}

ErrCode OverlayDataMgr::UpdateInternalOverlayInfo(const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGD("start to update internal overlay info");
    auto &innerModuleInfos = newInfo.GetInnerModuleInfos();
    if (innerModuleInfos.empty()) {
        APP_LOGE("innerModuleInfos is empty");
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    // build module overlay info
    OverlayModuleInfo overlayModuleInfo;
    overlayModuleInfo.bundleName = newInfo.GetBundleName();
    overlayModuleInfo.moduleName = (innerModuleInfos.begin()->second).moduleName;
    overlayModuleInfo.targetModuleName = (innerModuleInfos.begin()->second).targetModuleName;
    overlayModuleInfo.hapPath = newInfo.GetModuleHapPath(newInfo.GetCurrentModulePackage());
    overlayModuleInfo.priority = (innerModuleInfos.begin()->second).targetPriority;
    oldInfo.AddOverlayModuleInfo(overlayModuleInfo);
    SaveInternalOverlayModuleState(overlayModuleInfo, oldInfo);
    return ERR_OK;
}

ErrCode OverlayDataMgr::UpdateExternalOverlayInfo(const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGD("start to update external overlay info");
    std::string targetBundleName = newInfo.GetTargetBundleName();

    if (GetBundleDataMgr() != ERR_OK) {
        return false;
    }

    InnerBundleInfo targetInnerBundleInfo;
    if (!dataMgr_->GetOverlayInnerBundleInfo(targetBundleName, targetInnerBundleInfo)) {
        APP_LOGE("no target bundle %{public}s is installed", targetBundleName.c_str());
        return ERR_OK;
    }

    const auto &innerModuleInfos = newInfo.GetInnerModuleInfos();
    if (innerModuleInfos.empty()) {
        APP_LOGE("innerModuleInfos is empty");
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    OverlayModuleInfo overlayModuleInfo;
    overlayModuleInfo.bundleName = newInfo.GetBundleName();
    overlayModuleInfo.moduleName = (innerModuleInfos.begin()->second).moduleName;
    overlayModuleInfo.targetModuleName = (innerModuleInfos.begin()->second).targetModuleName;
    overlayModuleInfo.hapPath = newInfo.GetModuleHapPath(newInfo.GetCurrentModulePackage());
    overlayModuleInfo.priority = (innerModuleInfos.begin()->second).targetPriority;

    if (SaveExternalOverlayModuleState(overlayModuleInfo, targetBundleName, newInfo.GetUserId(), oldInfo) != ERR_OK) {
        APP_LOGE("save external overlay module state failed");
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }

    // build bundle overlay info
    std::string bundleDir;
    const std::string &moduleHapPath = newInfo.GetModuleHapPath(newInfo.GetCurrentModulePackage());
    GetBundleDir(moduleHapPath, bundleDir);
    OverlayBundleInfo overlayBundleInfo;
    overlayBundleInfo.bundleName = newInfo.GetBundleName();
    overlayBundleInfo.bundleDir = bundleDir;
    overlayBundleInfo.state = newInfo.GetOverlayState();
    overlayBundleInfo.priority = newInfo.GetTargetPriority();
    targetInnerBundleInfo.AddOverlayBundleInfo(overlayBundleInfo);
    targetInnerBundleInfo.AddOverlayModuleInfo(overlayModuleInfo);

    // storage target bundle info
    dataMgr_->SaveOverlayInfo(targetBundleName, targetInnerBundleInfo);
    dataMgr_->EnableOverlayBundle(targetBundleName);
    return ERR_OK;
}

ErrCode OverlayDataMgr::BuildOverlayConnection(const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGD("start to update overlay connection");
#ifdef BUNDLE_FRAMEWORK_OVERLAY_INSTALLATION
    // 1. build overlay connection for internal overlay
    const auto &moduleInfos = newInfo.GetInnerModuleInfos();
    std::string moduleName = (moduleInfos.begin()->second).moduleName;
    BuildInternalOverlayConnection(moduleName, oldInfo, newInfo.GetUserId());

    // 2. build overlay connection for external overlay
    BuildExternalOverlayConnection(moduleName, oldInfo, newInfo.GetUserId());
#endif
    return ERR_OK;
}

void OverlayDataMgr::BuildInternalOverlayConnection(const std::string &moduleName, InnerBundleInfo &oldInfo,
    int32_t userId)
{
    APP_LOGD("start to update internal overlay connection of module %{public}s under user %{public}d",
        moduleName.c_str(), userId);
    if (oldInfo.GetOverlayType() != OVERLAY_INTERNAL_BUNDLE) {
        APP_LOGW("the old bundle is not internal overlay");
        return;
    }
    auto &oldInnerModuleInfos = oldInfo.FetchInnerModuleInfos();
    std::vector<std::string> overlayModuleVec;
    for (auto &moduleInfo : oldInnerModuleInfos) {
        if (moduleInfo.second.targetModuleName == moduleName) {
            OverlayModuleInfo overlayModuleInfo;
            overlayModuleInfo.bundleName = oldInfo.GetBundleName();
            overlayModuleInfo.moduleName = moduleInfo.second.moduleName;
            overlayModuleInfo.targetModuleName = moduleInfo.second.targetModuleName;
            overlayModuleInfo.hapPath = oldInfo.GetModuleHapPath(moduleInfo.second.moduleName);
            overlayModuleInfo.priority = moduleInfo.second.targetPriority;
            oldInfo.AddOverlayModuleInfo(overlayModuleInfo);
            overlayModuleVec.emplace_back(moduleInfo.second.moduleName);
        }
    }
    if (GetBundleDataMgr() != ERR_OK) {
        return;
    }
    auto userSet = dataMgr_->GetAllUser();
    for (const auto &innerUserId : userSet) {
        InnerBundleUserInfo userInfo;
        if (!oldInfo.GetInnerBundleUserInfo(innerUserId, userInfo)) {
            APP_LOGW("no userInfo of bundleName %{public}s under userId %{public}d", oldInfo.GetBundleName().c_str(),
                innerUserId);
            continue;
        }
        for (const auto &overlayModule : overlayModuleVec) {
            int32_t state = OVERLAY_INVALID;
            oldInfo.GetOverlayModuleState(overlayModule, innerUserId, state);
            if (state == OVERLAY_INVALID) {
                oldInfo.SetOverlayModuleState(overlayModule, OVERLAY_ENABLE, innerUserId);
            }
        }
    }
}

void OverlayDataMgr::BuildExternalOverlayConnection(const std::string &moduleName, InnerBundleInfo &oldInfo,
    int32_t userId)
{
    APP_LOGD("start to update external overlay connection of module %{public}s under user %{public}d",
        moduleName.c_str(), userId);
    if (GetBundleDataMgr() != ERR_OK) {
        return;
    }

    auto bundleInfos = dataMgr_->GetAllOverlayInnerbundleInfos();
    for (const auto &info : bundleInfos) {
        if (info.second.GetTargetBundleName() != oldInfo.GetBundleName()) {
            continue;
        }
        // check target bundle is preInstall application
        if (!oldInfo.IsPreInstallApp()) {
            APP_LOGW("target bundle is not preInstall application");
            return;
        }

        // check fingerprint of current bundle with target bundle
        if (oldInfo.GetCertificateFingerprint() != info.second.GetCertificateFingerprint()) {
            APP_LOGW("target bundle has different fingerprint with current bundle");
            return;
        }
        // external overlay does not support FA model
        if (!oldInfo.GetIsNewVersion()) {
            APP_LOGW("target bundle is not stage model");
            return;
        }
        // external overlay does not support service
        if (oldInfo.GetEntryInstallationFree()) {
            APP_LOGW("target bundle is service");
            return;
        }

        const auto &innerModuleInfos = info.second.GetInnerModuleInfos();
        std::vector<std::string> overlayModuleVec;
        for (const auto &moduleInfo : innerModuleInfos) {
            if (moduleInfo.second.targetModuleName != moduleName) {
                continue;
            }
            OverlayModuleInfo overlayModuleInfo;
            overlayModuleInfo.bundleName = info.second.GetBundleName();
            overlayModuleInfo.moduleName = moduleInfo.second.moduleName;
            overlayModuleInfo.targetModuleName = moduleInfo.second.targetModuleName;
            overlayModuleInfo.hapPath = info.second.GetModuleHapPath(moduleInfo.second.moduleName);
            overlayModuleInfo.priority = moduleInfo.second.targetPriority;
            oldInfo.AddOverlayModuleInfo(overlayModuleInfo);
            overlayModuleVec.emplace_back(moduleInfo.second.moduleName);
        }
        std::string bundleDir;
        const std::string &moduleHapPath =
            info.second.GetModuleHapPath((innerModuleInfos.begin()->second).moduleName);
        GetBundleDir(moduleHapPath, bundleDir);
        OverlayBundleInfo overlayBundleInfo;
        overlayBundleInfo.bundleName = info.second.GetBundleName();
        overlayBundleInfo.bundleDir = bundleDir;
        overlayBundleInfo.state = info.second.GetOverlayState();
        overlayBundleInfo.priority = info.second.GetTargetPriority();
        oldInfo.AddOverlayBundleInfo(overlayBundleInfo);
        auto userSet = dataMgr_->GetAllUser();
        for (const auto &innerUserId : userSet) {
            InnerBundleInfo innerBundleInfo = info.second;
            for (const auto &overlayModule : overlayModuleVec) {
                int32_t state = OVERLAY_INVALID;
                innerBundleInfo.GetOverlayModuleState(overlayModule, innerUserId, state);
                if (state == OVERLAY_INVALID) {
                    innerBundleInfo.SetOverlayModuleState(overlayModule, OVERLAY_ENABLE, innerUserId);
                }
            }
            dataMgr_->SaveOverlayInfo(innerBundleInfo.GetBundleName(), innerBundleInfo);
        }
    }
}

ErrCode OverlayDataMgr::GetBundleDir(const std::string &moduleHapPath, std::string &bundleDir) const
{
    bundleDir = moduleHapPath;
    if (moduleHapPath.back() == Constants::FILE_SEPARATOR_CHAR) {
        bundleDir = moduleHapPath.substr(0, moduleHapPath.length() - 1);
    }
    size_t pos = bundleDir.find_last_of(Constants::PATH_SEPARATOR);
    if (pos == std::string::npos) {
        APP_LOGE("bundleDir is invalid");
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INVALID_BUNDLE_DIR;
    }
    bundleDir = bundleDir.substr(0, pos);
    APP_LOGD("bundleDir is %{public}s", bundleDir.c_str());
    return ERR_OK;
}

ErrCode OverlayDataMgr::RemoveOverlayModuleConnection(const InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGD("start to remove overlay connection before update");
    const auto &innerModuleInfos = newInfo.GetInnerModuleInfos();
    if (innerModuleInfos.empty()) {
        APP_LOGE("innerModuleInfos is empty");
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    std::string moduleName = (innerModuleInfos.begin()->second).moduleName;
    const auto &oldInnerModuleInfos = oldInfo.GetInnerModuleInfos();
    if (oldInnerModuleInfos.find(moduleName) == oldInnerModuleInfos.end()) {
        APP_LOGE("module %{public}s is not existed", moduleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    if (newInfo.GetOverlayType() == OVERLAY_INTERNAL_BUNDLE) {
        APP_LOGD("start to remove internal overlay connection before update");
        oldInfo.RemoveOverlayModuleInfo(oldInnerModuleInfos.at(moduleName).targetModuleName, newInfo.GetBundleName(),
            moduleName);
        return ERR_OK;
    }
    if (newInfo.GetOverlayType() == OVERLAY_EXTERNAL_BUNDLE) {
        APP_LOGD("start to remove external overlay connection before update");
        if (GetBundleDataMgr() != ERR_OK) {
            return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
        }
        InnerBundleInfo targetInnerBundleInfo;
        const auto &targetBundleName = oldInfo.GetTargetBundleName();
        if (!dataMgr_->GetOverlayInnerBundleInfo(targetBundleName, targetInnerBundleInfo)) {
            APP_LOGE("no bundle with bundleName %{public}s installed", targetBundleName.c_str());
            return ERR_OK;
        }
        // external overlay bundle change target bundle name
        if (newInfo.GetTargetBundleName() != oldInfo.GetTargetBundleName()) {
            // remove all module connection in old targetBundle
            targetInnerBundleInfo.RemoveAllOverlayModuleInfo(newInfo.GetBundleName());
            targetInnerBundleInfo.RemoveOverLayBundleInfo(newInfo.GetBundleName());
        } else {
            targetInnerBundleInfo.RemoveOverlayModuleInfo(oldInnerModuleInfos.at(moduleName).targetModuleName,
                newInfo.GetBundleName(), moduleName);
        }
        // save target innerBundleInfo
        dataMgr_->SaveOverlayInfo(targetBundleName, targetInnerBundleInfo);
        dataMgr_->EnableOverlayBundle(targetBundleName);
    }
    return ERR_OK;
}

void OverlayDataMgr::RemoveOverlayBundleInfo(const std::string &targetBundleName, const std::string &bundleName)
{
    APP_LOGD("start to remove overlay bundleInfo under uninstalling external overlay");
    if (GetBundleDataMgr() != ERR_OK) {
        return;
    }

    InnerBundleInfo targetInnerBundleInfo;
    if (!dataMgr_->GetOverlayInnerBundleInfo(targetBundleName, targetInnerBundleInfo)) {
        APP_LOGD("target bundle %{public}s is not installed", targetBundleName.c_str());
        return;
    }

    targetInnerBundleInfo.RemoveOverLayBundleInfo(bundleName);
    targetInnerBundleInfo.RemoveAllOverlayModuleInfo(bundleName);

    // save target innerBundleInfo
    dataMgr_->SaveOverlayInfo(targetBundleName, targetInnerBundleInfo);
    dataMgr_->EnableOverlayBundle(targetBundleName);
    APP_LOGD("finish to remove overlay bundleInfo");
}

void OverlayDataMgr::RemoveOverlayModuleInfo(
    const std::string &bundleName, const std::string &modulePackage, InnerBundleInfo &oldInfo)
{
    APP_LOGD("start to remove overlay moduleInfo under uninstalling overlay module");
    if (!oldInfo.FindModule(modulePackage)) {
        return;
    }
    auto &innerModuleInfos = oldInfo.FetchInnerModuleInfos();
    std::string targetModuleName = innerModuleInfos.at(modulePackage).targetModuleName;
    // remove internal overlay info from target module
    if (oldInfo.GetOverlayType() == OVERLAY_INTERNAL_BUNDLE) {
        // uninstall non-overlay module and state of overlay module will be OVERLAY_INVALID
        if (targetModuleName.empty()) {
            ResetInternalOverlayModuleState(innerModuleInfos, modulePackage, oldInfo);
            return;
        }
        // uninstall overlay module, remove state info from innerUserInfo
        oldInfo.RemoveOverlayModuleInfo(targetModuleName, bundleName, modulePackage);
        oldInfo.ClearOverlayModuleStates(modulePackage);
        return;
    }

    // remove external overlay info from target bundle
    if (oldInfo.GetOverlayType() == OVERLAY_EXTERNAL_BUNDLE) {
        std::string targetBundleName = oldInfo.GetTargetBundleName();
        if (GetBundleDataMgr() != ERR_OK) {
            return;
        }
        InnerBundleInfo targetInnerBundleInfo;
        if (!dataMgr_->GetOverlayInnerBundleInfo(targetBundleName, targetInnerBundleInfo)) {
            APP_LOGD("target bundle %{public}s is not installed", targetBundleName.c_str());
            return;
        }

        targetInnerBundleInfo.RemoveOverlayModuleInfo(targetModuleName, bundleName, modulePackage);
        // save target innerBundleInfo
        dataMgr_->SaveOverlayInfo(targetBundleName, targetInnerBundleInfo);
        dataMgr_->EnableOverlayBundle(targetBundleName);
        // uninstall overlay module, remove state info from innerUserInfo
        oldInfo.ClearOverlayModuleStates(modulePackage);
    }

    // remove target module and overlay module state will change to OVERLAY_INVALID
    if (oldInfo.GetOverlayType() == NON_OVERLAY_TYPE) {
        ResetExternalOverlayModuleState(bundleName, modulePackage);
    }
    APP_LOGD("finish to remove overlay moduleInfo");
}

void OverlayDataMgr::ResetInternalOverlayModuleState(const std::map<std::string, InnerModuleInfo> &innerModuleInfos,
    const std::string &modulePackage, InnerBundleInfo &oldInfo)
{
    for (const auto &moduleInfo : innerModuleInfos) {
        if (moduleInfo.second.targetModuleName == modulePackage) {
            oldInfo.SetOverlayModuleState(moduleInfo.second.moduleName, OVERLAY_INVALID);
            return;
        }
    }
}

void OverlayDataMgr::ResetExternalOverlayModuleState(const std::string &bundleName, const std::string &modulePackage)
{
    if (GetBundleDataMgr() != ERR_OK) {
        return;
    }
    auto bundleInfos = dataMgr_->GetAllOverlayInnerbundleInfos();
    for (const auto &info : bundleInfos) {
        if (info.second.GetTargetBundleName() != bundleName) {
            continue;
        }
        const auto &innerModuleInfos = info.second.GetInnerModuleInfos();
        InnerBundleInfo innerBundleInfo = info.second;
        for (const auto &moduleInfo : innerModuleInfos) {
            if (moduleInfo.second.targetModuleName == modulePackage) {
                innerBundleInfo.SetOverlayModuleState(moduleInfo.second.moduleName, OVERLAY_INVALID);
                break;
            }
        }
        dataMgr_->SaveOverlayInfo(innerBundleInfo.GetBundleName(), innerBundleInfo);
    }
}

void OverlayDataMgr::ResetExternalOverlayModuleState(const std::string &bundleName)
{
    if (GetBundleDataMgr() != ERR_OK) {
        return;
    }
    auto bundleInfos = dataMgr_->GetAllOverlayInnerbundleInfos();
    for (const auto &info : bundleInfos) {
        if (info.second.GetTargetBundleName() != bundleName) {
            continue;
        }
        const auto &innerModuleInfos = info.second.GetInnerModuleInfos();
        InnerBundleInfo innerBundleInfo = info.second;
        for (const auto &moduleInfo : innerModuleInfos) {
            innerBundleInfo.SetOverlayModuleState(moduleInfo.second.moduleName, OVERLAY_INVALID);
        }
        dataMgr_->SaveOverlayInfo(info.second.GetBundleName(), innerBundleInfo);
    }
}

bool OverlayDataMgr::GetOverlayInnerBundleInfo(const std::string &bundleName, InnerBundleInfo &info)
{
    if (GetBundleDataMgr() != ERR_OK) {
        return false;
    }
    if (!dataMgr_->GetOverlayInnerBundleInfo(bundleName, info)) {
        APP_LOGE("target bundle %{public}s is not installed", bundleName.c_str());
        return false;
    }
    return true;
}

void OverlayDataMgr::EnableOverlayBundle(const std::string &bundleName)
{
    if (GetBundleDataMgr() != ERR_OK) {
        return;
    }
    dataMgr_->EnableOverlayBundle(bundleName);
}

ErrCode OverlayDataMgr::GetBundleDataMgr()
{
    if (dataMgr_ == nullptr) {
        dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
        if (dataMgr_ == nullptr) {
            APP_LOGE("overlayDataMgr gets data mgr ptr failed");
            return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_SERVICE_EXCEPTION;
        }
    }
    return ERR_OK;
}

ErrCode OverlayDataMgr::SaveInternalOverlayModuleState(const OverlayModuleInfo &overlayModuleInfo,
    InnerBundleInfo &innerBundleInfo)
{
    if (GetBundleDataMgr() != ERR_OK) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_SERVICE_EXCEPTION;
    }
    auto userSet = dataMgr_->GetAllUser();
    for (const auto &userId : userSet) {
        APP_LOGD("start to save internal overlay module state under %{public}d", userId);
        InnerBundleUserInfo userInfo;
        if (!innerBundleInfo.GetInnerBundleUserInfo(userId, userInfo)) {
            APP_LOGW("no userInfo of bundleName %{public}s under userId %{public}d",
                innerBundleInfo.GetBundleName().c_str(), userId);
            continue;
        }

        int32_t state = OVERLAY_INVALID;
        if (innerBundleInfo.FindModule(overlayModuleInfo.targetModuleName)) {
            state = OVERLAY_ENABLE;
        }
        auto &overlayStates = userInfo.bundleUserInfo.overlayModulesState;
        auto iter = std::find_if(overlayStates.begin(), overlayStates.end(), [&overlayModuleInfo](const auto &item) {
            if (item.find(overlayModuleInfo.moduleName + Constants::FILE_UNDERLINE) != std::string::npos) {
                return true;
            }
            return false;
        });
        if (iter != overlayStates.end()) {
            overlayStates.erase(iter);
        }
        std::string overlayModuleState =
            overlayModuleInfo.moduleName + Constants::FILE_UNDERLINE + std::to_string(state);
        overlayStates.emplace_back(overlayModuleState);
        innerBundleInfo.AddInnerBundleUserInfo(userInfo);
    }
    return ERR_OK;
}

ErrCode OverlayDataMgr::SaveExternalOverlayModuleState(const OverlayModuleInfo &overlayModuleInfo,
    const std::string &targetBundleName, int32_t userId, InnerBundleInfo &innerBundleInfo)
{
    APP_LOGD("start to save external overlay module state under %{public}d", userId);
    if (GetBundleDataMgr() != ERR_OK) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    int32_t state = OVERLAY_INVALID;
    InnerBundleInfo targetInnerBundleInfo;
    if ((dataMgr_->GetOverlayInnerBundleInfo(targetBundleName, targetInnerBundleInfo)) &&
        targetInnerBundleInfo.FindModule(overlayModuleInfo.targetModuleName)) {
        dataMgr_->EnableOverlayBundle(targetBundleName);
        state = OVERLAY_ENABLE;
    }
    InnerBundleUserInfo userInfo;
    if (!innerBundleInfo.GetInnerBundleUserInfo(userId, userInfo)) {
        APP_LOGE("no userInfo of bundleName %{public}s under userId %{public}d",
            innerBundleInfo.GetBundleName().c_str(), userId);
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    auto &overlayStates = userInfo.bundleUserInfo.overlayModulesState;
    auto iter = std::find_if(overlayStates.begin(), overlayStates.end(), [&overlayModuleInfo](const auto &item) {
        if (item.find(overlayModuleInfo.moduleName + Constants::FILE_UNDERLINE) != std::string::npos) {
            return true;
        }
        return false;
    });
    if (iter != overlayStates.end()) {
        overlayStates.erase(iter);
    }
    std::string overlayModuleState = overlayModuleInfo.moduleName + Constants::FILE_UNDERLINE + std::to_string(state);
    overlayStates.emplace_back(overlayModuleState);
    innerBundleInfo.AddInnerBundleUserInfo(userInfo);
    return ERR_OK;
}

ErrCode OverlayDataMgr::GetAllOverlayModuleInfo(const std::string &bundleName,
    std::vector<OverlayModuleInfo> &overlayModuleInfos, int32_t userId)
{
    APP_LOGD("start to get all overlay moduleInfo");
    if (GetBundleDataMgr() != ERR_OK) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    InnerBundleInfo info;
    if (!dataMgr_->GetOverlayInnerBundleInfo(bundleName, info)) {
        APP_LOGE("overlay bundle is not existed %{public}s", bundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_MISSING_OVERLAY_BUNDLE;
    }
    dataMgr_->EnableOverlayBundle(bundleName);
    InnerBundleUserInfo userInfo;
    if (!info.GetInnerBundleUserInfo(userId, userInfo)) {
        APP_LOGE("the bundle %{public}s is not installed at user %{public}d", bundleName.c_str(), userId);
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_BUNDLE_NOT_INSTALLED_AT_SPECIFIED_USERID;
    }

    if (info.GetOverlayType() == NON_OVERLAY_TYPE) {
        APP_LOGE("bundle %{public}s is non-overlay bundle", bundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_NON_OVERLAY_BUNDLE;
    }
    auto overlayModulesStateMap = GetModulesStateFromUserInfo(userInfo);
    const auto &InnerModuleInfos = info.GetInnerModuleInfos();
    for (const auto &moduleInfo : InnerModuleInfos) {
        if (!moduleInfo.second.targetModuleName.empty()) {
            OverlayModuleInfo overlayModuleInfo;
            overlayModuleInfo.bundleName = bundleName;
            overlayModuleInfo.moduleName = moduleInfo.second.moduleName;
            overlayModuleInfo.targetModuleName = moduleInfo.second.targetModuleName;
            overlayModuleInfo.hapPath = info.GetModuleHapPath(moduleInfo.second.moduleName);
            overlayModuleInfo.priority = moduleInfo.second.targetPriority;
            if (overlayModulesStateMap.find(moduleInfo.second.moduleName) != overlayModulesStateMap.end()) {
                overlayModuleInfo.state = overlayModulesStateMap[moduleInfo.second.moduleName];
            } else {
                overlayModuleInfo.state = OVERLAY_INVALID;
            }
            overlayModuleInfos.emplace_back(overlayModuleInfo);
        }
    }

    if (overlayModuleInfos.empty()) {
        APP_LOGW("no overlay moduleInfo can be queried of bundleName %{public}s", bundleName.c_str());
    }
    return ERR_OK;
}

ErrCode OverlayDataMgr::GetOverlayModuleInfo(const std::string &bundleName, const std::string &moduleName,
    OverlayModuleInfo &overlayModuleInfo, int32_t userId)
{
    APP_LOGD("start to get specific overlay moduleInfo");
    if (GetBundleDataMgr() != ERR_OK) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    InnerBundleInfo info;
    if (!dataMgr_->GetOverlayInnerBundleInfo(bundleName, info)) {
        APP_LOGE("overlay bundle is not existed %{public}s", bundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_MISSING_OVERLAY_BUNDLE;
    }
    dataMgr_->EnableOverlayBundle(bundleName);
    InnerBundleUserInfo userInfo;
    if (!info.GetInnerBundleUserInfo(userId, userInfo)) {
        APP_LOGE("the bundle %{public}s is not installed at user %{public}d", bundleName.c_str(), userId);
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_BUNDLE_NOT_INSTALLED_AT_SPECIFIED_USERID;
    }

    if (info.GetOverlayType() == NON_OVERLAY_TYPE) {
        APP_LOGE("bundle %{public}s is non-overlay bundle", bundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_NON_OVERLAY_BUNDLE;
    }

    if (!info.FindModule(moduleName)) {
        APP_LOGE("overlay bundle %{public}s does not contain module %{public}s", bundleName.c_str(),
            moduleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_MISSING_OVERLAY_MODULE;
    }

    if (!info.isOverlayModule(moduleName)) {
        APP_LOGE("module %{public}s is non-overlay module", moduleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_NON_OVERLAY_MODULE;
    }

    auto innerModuleInfo = info.GetInnerModuleInfoByModuleName(moduleName);
    if (innerModuleInfo == std::nullopt) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }

    const auto &moduleInfo = innerModuleInfo.value();
    overlayModuleInfo.bundleName = bundleName;
    overlayModuleInfo.moduleName = moduleName;
    overlayModuleInfo.targetModuleName = moduleInfo.targetModuleName;
    overlayModuleInfo.hapPath = info.GetModuleHapPath(moduleInfo.moduleName);
    overlayModuleInfo.priority = moduleInfo.targetPriority;
    if (!info.GetOverlayModuleState(moduleName, userId, overlayModuleInfo.state)) {
        APP_LOGE("GetOverlayModuleState failed of bundleName %{public}s and moduleName %{public}s",
            bundleName.c_str(), moduleName.c_str());
    }
    return ERR_OK;
}

ErrCode OverlayDataMgr::GetOverlayBundleInfoForTarget(const std::string &targetBundleName,
    std::vector<OverlayBundleInfo> &overlayBundleInfo, int32_t userId)
{
    APP_LOGD("start to get target overlay bundle info");
    if (GetBundleDataMgr() != ERR_OK) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    InnerBundleInfo targetInnerBundleInfo;
    if (!dataMgr_->GetOverlayInnerBundleInfo(targetBundleName, targetInnerBundleInfo)) {
        APP_LOGE("target bundle is not existed %{public}s", targetBundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_TARGET_BUNDLE_NOT_EXISTED;
    }
    dataMgr_->EnableOverlayBundle(targetBundleName);
    InnerBundleUserInfo userInfo;
    if (!targetInnerBundleInfo.GetInnerBundleUserInfo(userId, userInfo)) {
        APP_LOGE("the bundle %{public}s is not installed at user %{public}d", targetBundleName.c_str(), userId);
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_BUNDLE_NOT_INSTALLED_AT_SPECIFIED_USERID;
    }

    overlayBundleInfo = targetInnerBundleInfo.GetOverlayBundleInfo();
    if (overlayBundleInfo.empty()) {
        APP_LOGW("no overlay bundle info in data mgr");
    }
    return ERR_OK;
}

ErrCode OverlayDataMgr::GetOverlayModuleInfoForTarget(const std::string &targetBundleName,
    const std::string &targetModuleName, std::vector<OverlayModuleInfo> &overlayModuleInfo, int32_t userId)
{
    APP_LOGD("start to get target overlay module infos");
    if (GetBundleDataMgr() != ERR_OK) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    InnerBundleInfo targetInnerBundleInfo;
    if (!dataMgr_->GetOverlayInnerBundleInfo(targetBundleName, targetInnerBundleInfo)) {
        APP_LOGE("target bundle is not existed %{public}s", targetBundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_TARGET_BUNDLE_NOT_EXISTED;
    }
    dataMgr_->EnableOverlayBundle(targetBundleName);

    if (targetInnerBundleInfo.GetOverlayType() == OVERLAY_EXTERNAL_BUNDLE) {
        APP_LOGE("the bundle %{public}s is external overlay bundle", targetBundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_TARGET_BUNDLE_IS_OVERLAY_BUNDLE;
    }
    InnerBundleUserInfo userInfo;
    if (!targetInnerBundleInfo.GetInnerBundleUserInfo(userId, userInfo)) {
        APP_LOGE("the bundle %{public}s is not installed at user %{public}d", targetBundleName.c_str(), userId);
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_BUNDLE_NOT_INSTALLED_AT_SPECIFIED_USERID;
    }
    if (targetModuleName.empty()) {
        APP_LOGD("to get all target overlay module infos in target bundle");
        return GetOverlayModuleInfoForTarget(targetInnerBundleInfo, overlayModuleInfo, userId);
    }

    if (!targetInnerBundleInfo.FindModule(targetModuleName)) {
        APP_LOGE("the target module %{public}s is not existed in bundle %{public}s", targetModuleName.c_str(),
            targetBundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_TARGET_MODULE_NOT_EXISTED;
    }
    if (targetInnerBundleInfo.isOverlayModule(targetModuleName)) {
        APP_LOGE("the target module %{public}s is overlay module", targetModuleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_TARGET_MODULE_IS_OVERLAY_MODULE;
    }
    auto targetModuleInfo = targetInnerBundleInfo.GetInnerModuleInfoByModuleName(targetModuleName);
    if (targetModuleInfo == std::nullopt) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    const auto &moduleInfo = targetModuleInfo.value();
    overlayModuleInfo = moduleInfo.overlayModuleInfo;
    if (overlayModuleInfo.empty()) {
        APP_LOGE("no overlay module info in target module %{public}s", targetModuleName.c_str());
        return ERR_OK;
    }

    for (auto &overlayInfo : overlayModuleInfo) {
        auto res = ObtainOverlayModuleState(overlayInfo, userId);
        if (res != ERR_OK) {
            APP_LOGE("failed to obtain the state of overlay module %{public}s", overlayInfo.moduleName.c_str());
            return res;
        }
    }
    return ERR_OK;
}

ErrCode OverlayDataMgr::GetOverlayModuleInfoForTarget(const InnerBundleInfo &innerBundleInfo,
    std::vector<OverlayModuleInfo> &overlayModuleInfo, int32_t userId)
{
    APP_LOGD("start to get all target overlay module infos");
    const auto &InnerModuleInfos = innerBundleInfo.GetInnerModuleInfos();
    for (const auto &moduleInfo : InnerModuleInfos) {
        if (!moduleInfo.second.overlayModuleInfo.empty()) {
            for (const auto &overlayInfo : moduleInfo.second.overlayModuleInfo) {
                OverlayModuleInfo innerOverlayModuleInfo = overlayInfo;
                ObtainOverlayModuleState(innerOverlayModuleInfo, userId);
                overlayModuleInfo.emplace_back(innerOverlayModuleInfo);
            }
        }
    }

    if (overlayModuleInfo.empty()) {
        APP_LOGW("no overlay moduleInfo can be queried of bundleName %{public}s",
            innerBundleInfo.GetBundleName().c_str());
    }
    return ERR_OK;
}

std::map<std::string, int32_t> OverlayDataMgr::GetModulesStateFromUserInfo(
    const InnerBundleUserInfo &userInfo) const
{
    std::map<std::string, int32_t> statesMap;
    const auto &overlayStates = userInfo.bundleUserInfo.overlayModulesState;
    if (overlayStates.empty()) {
        APP_LOGW("no overlay states info in innerUserInfo");
        return statesMap;
    }

    for_each(overlayStates.begin(), overlayStates.end(), [&statesMap](const auto &item) {
        size_t pos = item.find(Constants::FILE_UNDERLINE);
        if (pos != std::string::npos) {
            std::string moduleName = item.substr(0, pos);
            int32_t state = OVERLAY_INVALID;
            OHOS::StrToInt(item.substr(pos + 1), state);
            APP_LOGD("overlay module %{public}s is under state %{public}d", moduleName.c_str(), state);
            if (statesMap.find(moduleName) == statesMap.end()) {
                statesMap.emplace(moduleName, state);
            } else {
                statesMap[moduleName] = state;
            }
        }
    });
    return statesMap;
}

ErrCode OverlayDataMgr::ObtainOverlayModuleState(OverlayModuleInfo &overlayModuleInfo, int32_t userId)
{
    if (GetBundleDataMgr() != ERR_OK) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    InnerBundleInfo innerBundleInfo;
    if (!dataMgr_->GetOverlayInnerBundleInfo(overlayModuleInfo.bundleName, innerBundleInfo)) {
        APP_LOGE("target bundle is not existed %{public}s", overlayModuleInfo.bundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_MISSING_OVERLAY_BUNDLE;
    }
    dataMgr_->EnableOverlayBundle(overlayModuleInfo.bundleName);

    if (!innerBundleInfo.GetOverlayModuleState(overlayModuleInfo.moduleName, userId, overlayModuleInfo.state)) {
        APP_LOGE("GetOverlayModuleState of bundleName %{public}s failed", overlayModuleInfo.bundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }

    return ERR_OK;
}

ErrCode OverlayDataMgr::SetOverlayEnabled(const std::string &bundleName, const std::string &moduleName, bool isEnabled,
    int32_t userId)
{
    if (GetBundleDataMgr() != ERR_OK) {
        return ERR_BUNDLEMANAGER_OVERLAY_INSTALLATION_FAILED_INTERNAL_ERROR;
    }
    InnerBundleInfo innerBundleInfo;
    if (!dataMgr_->GetOverlayInnerBundleInfo(bundleName, innerBundleInfo)) {
        APP_LOGE("bundle is not existed %{public}s", bundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_MISSING_OVERLAY_BUNDLE;
    }
    dataMgr_->EnableOverlayBundle(bundleName);
    // 1. whether the specified bundle is installed under the specified userid
    InnerBundleUserInfo userInfo;
    if (!innerBundleInfo.GetInnerBundleUserInfo(userId, userInfo)) {
        APP_LOGE("no userInfo of bundleName %{public}s under userId %{public}d",
            innerBundleInfo.GetBundleName().c_str(), userId);
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_BUNDLE_NOT_INSTALLED_AT_SPECIFIED_USERID;
    }
    // 2. whether bundle is overlay bundle
    if ((GetCallingBundleName() != bundleName) && innerBundleInfo.GetOverlayType() == NON_OVERLAY_TYPE) {
        APP_LOGE("current bundle %{public}s is non-overlay bundle", bundleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_NON_OVERLAY_BUNDLE;
    }

    if (!innerBundleInfo.FindModule(moduleName)) {
        APP_LOGE("overlay bundle %{public}s does not contain module %{public}s", bundleName.c_str(),
            moduleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_MISSING_OVERLAY_MODULE;
    }

    // 3. whether module is overlay module
    if (!innerBundleInfo.isOverlayModule(moduleName)) {
        APP_LOGE("module %{public}s is non-overlay module", moduleName.c_str());
        return ERR_BUNDLEMANAGER_OVERLAY_QUERY_FAILED_NON_OVERLAY_MODULE;
    }

    // 4. set enable state
    auto &statesVec = userInfo.bundleUserInfo.overlayModulesState;
    for (auto &item : statesVec) {
        if (item.find(moduleName + Constants::FILE_UNDERLINE) == std::string::npos) {
            continue;
        }
        item = isEnabled ? (moduleName + Constants::FILE_UNDERLINE + std::to_string(OVERLAY_ENABLE)) :
            (moduleName + Constants::FILE_UNDERLINE + std::to_string(OVERLAY_DISABLED));
        break;
    }

    // 5. save to date storage
    innerBundleInfo.AddInnerBundleUserInfo(userInfo);
    dataMgr_->SaveOverlayInfo(bundleName, innerBundleInfo);

    // 6. publish the common event "usual.event.OVERLAY_STATE_CHANGED"
    std::shared_ptr<BundleCommonEventMgr> commonEventMgr = std::make_shared<BundleCommonEventMgr>();
    commonEventMgr->NotifyOverlayModuleStateStatus(bundleName, moduleName, isEnabled, userId, userInfo.uid);
    return ERR_OK;
}

std::string OverlayDataMgr::GetCallingBundleName()
{
    std::string callingBundleName;
    if (GetBundleDataMgr() != ERR_OK) {
        return callingBundleName;
    }
    bool ret = dataMgr_->GetBundleNameForUid(IPCSkeleton::GetCallingUid(), callingBundleName);
    if (!ret || callingBundleName.empty()) {
        APP_LOGE("calling GetBundleNameForUid failed by calling uid %{public}d", IPCSkeleton::GetCallingUid());
    }
    return callingBundleName;
}

void OverlayDataMgr::AddOverlayModuleStates(const InnerBundleInfo &innerBundleInfo, InnerBundleUserInfo &userInfo)
{
    APP_LOGD("start to add overlay module state info at new userId %{public}d", userInfo.bundleUserInfo.userId);
    if (GetBundleDataMgr() != ERR_OK) {
        APP_LOGE("get dataMgr failed");
        return;
    }

    auto userSet = dataMgr_->GetAllUser();
    std::vector<std::string> overlayModuleStatesVec;
    for (const auto &userId : userSet) {
        if (userId == userInfo.bundleUserInfo.userId) {
            continue;
        }
        InnerBundleUserInfo innerUserInfo;
        if (!innerBundleInfo.GetInnerBundleUserInfo(userId, innerUserInfo)) {
            APP_LOGW("no userInfo of bundleName %{public}s under userId %{public}d",
                innerBundleInfo.GetBundleName().c_str(), userId);
            continue;
        }
        overlayModuleStatesVec = innerUserInfo.bundleUserInfo.overlayModulesState;
        if (overlayModuleStatesVec.empty()) {
            continue;
        }
        break;
    }
    for (auto &item : overlayModuleStatesVec) {
        size_t pos = item.find(Constants::FILE_UNDERLINE);
        if (pos == std::string::npos) {
            continue;
        }
        std::string moduleName = item.substr(0, pos);
        auto innerModuleInfo = innerBundleInfo.GetInnerModuleInfoByModuleName(moduleName);
        if (innerModuleInfo == std::nullopt) {
            APP_LOGW("no innerModuleInfo in dataMgr");
            continue;
        }
        const auto &moduleInfo = innerModuleInfo.value();
        if (innerBundleInfo.GetOverlayType() == OVERLAY_INTERNAL_BUNDLE) {
            bool isTargetModuleExisted = innerBundleInfo.FindModule(moduleInfo.targetModuleName);
            item = isTargetModuleExisted ? (moduleName + Constants::FILE_UNDERLINE + std::to_string(OVERLAY_ENABLE)) :
                (moduleName + Constants::FILE_UNDERLINE + std::to_string(OVERLAY_INVALID));
        }
        if (innerBundleInfo.GetOverlayType() == OVERLAY_EXTERNAL_BUNDLE) {
            std::string targetBundleName = innerBundleInfo.GetTargetBundleName();
            InnerBundleInfo targetInnerBundleInfo;
            if (!dataMgr_->GetOverlayInnerBundleInfo(targetBundleName, targetInnerBundleInfo)) {
                APP_LOGD("target bundle %{public}s is not installed", targetBundleName.c_str());
                item = moduleName + Constants::FILE_UNDERLINE + std::to_string(OVERLAY_INVALID);
                continue;
            }
            dataMgr_->EnableOverlayBundle(targetBundleName);
            bool isTargetModuleExisted = targetInnerBundleInfo.FindModule(moduleInfo.targetModuleName);
            item = isTargetModuleExisted ? (moduleName + Constants::FILE_UNDERLINE + std::to_string(OVERLAY_ENABLE)) :
                (moduleName + Constants::FILE_UNDERLINE + std::to_string(OVERLAY_INVALID));
        }
    }
    userInfo.bundleUserInfo.overlayModulesState = overlayModuleStatesVec;
}
} // AppExecFwk
} // OHOS