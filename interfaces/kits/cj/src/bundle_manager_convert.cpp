/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <string>

#include "bundle_manager_utils.h"
#include "ipc_skeleton.h"
#include "bundle_info.h"
#include "bundle_mgr_proxy.h"
#include "common_func.h"
#include "bundle_manager_convert.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace CJSystemapi {
namespace BundleManager {
namespace Convert {

const std::string PATH_PREFIX = "/data/app/el1/bundle/public";
const std::string CODE_PATH_PREFIX = "/data/storage/el1/bundle/";
const std::string CONTEXT_DATA_STORAGE_BUNDLE("/data/storage/el1/bundle/");

char *MallocCString(const std::string &origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto len = origin.length() + 1;
    char* res = static_cast<char *>(malloc(sizeof(char) * len));
    if (res == nullptr) {
        APP_LOGE("MallocCString malloc failed");
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), len);
}

void ClearCharPointer(char** ptr, int count)
{
    for (int i = 0; i < count; i++) {
        free(ptr[i]);
        ptr[i] = nullptr;
    }
}

CArrString ConvertArrString(std::vector<std::string> vecStr)
{
    if (vecStr.size() == 0) {
        return {nullptr, 0};
    }
    char **retValue = static_cast<char **>(malloc(sizeof(char *) * vecStr.size()));
    if (retValue == nullptr) {
        return {nullptr, 0};
    }

    for (size_t i = 0; i < vecStr.size(); i++) {
        retValue[i] = MallocCString(vecStr[i]);
        if (retValue[i] == nullptr) {
            ClearCharPointer(retValue, i);
            free(retValue);
            return {nullptr, 0};
        }
    }

    return {retValue, vecStr.size()};
}

RetUsedScene ConvertUsedScene(const AppExecFwk::RequestPermissionUsedScene& usedScence)
{
    RetUsedScene uScene;
    uScene.abilities = ConvertArrString(usedScence.abilities);
    uScene.when = MallocCString(usedScence.when);
    return uScene;
}

RetMetadata ConvertMetadata(const AppExecFwk::Metadata& cData)
{
    RetMetadata data;
    data.name = MallocCString(cData.name);
    data.value = MallocCString(cData.value);
    data.resource = MallocCString(cData.resource);
    return data;
}

CResource ConvertResource(const AppExecFwk::Resource& cRes)
{
    CResource res;
    res.bundleName = MallocCString(cRes.bundleName);
    res.moduleName = MallocCString(cRes.moduleName);
    res.id = cRes.id;
    return res;
}

CArrMetadata ConvertArrMetadata(const std::vector<AppExecFwk::Metadata>& cData)
{
    CArrMetadata data;
    data.size = static_cast<int64_t>(cData.size());
    data.head = nullptr;
    if (data.size > 0) {
        RetMetadata *retValue = reinterpret_cast<RetMetadata *>(malloc(sizeof(RetMetadata) * data.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < data.size; i++) {
                retValue[i] = ConvertMetadata(cData[i]);
            }
            data.head = retValue;
        } else {
            APP_LOGE("ConvertArrMetadata malloc failed");
            return data;
        }
    }
    return data;
}

CArrMoMeta ConvertArrMoMeta(const std::map<std::string, std::vector<AppExecFwk::Metadata>>& metadata)
{
    CArrMoMeta arrMdata;
    arrMdata.size = static_cast<int64_t>(metadata.size());
    arrMdata.head = nullptr;
    if (arrMdata.size > 0) {
        ModuleMetadata* retValue = reinterpret_cast<ModuleMetadata *>(malloc(sizeof(ModuleMetadata) * arrMdata.size));
        if (retValue != nullptr) {
            int32_t i = 0;
            for (const auto &item : metadata) {
                retValue[i].moduleName = MallocCString(item.first);
                retValue[i++].metadata = ConvertArrMetadata(item.second);
            }
        } else {
            APP_LOGE("ConvertArrMoMeta malloc failed");
            return arrMdata;
        }
        arrMdata.head = retValue;
    }
    return arrMdata;
}


RetSkillUri ConvertSkillUri(const AppExecFwk::SkillUri& cUri)
{
    RetSkillUri skillUri;
    skillUri.scheme = MallocCString(cUri.scheme);
    skillUri.host = MallocCString(cUri.host);
    skillUri.port = MallocCString(cUri.port);
    skillUri.path = MallocCString(cUri.path);
    skillUri.pathStartWith = MallocCString(cUri.pathStartWith);
    skillUri.pathRegex = MallocCString(cUri.pathRegex);
    skillUri.type = MallocCString(cUri.type);
    skillUri.utd = MallocCString(cUri.utd);
    skillUri.maxFileSupported = cUri.maxFileSupported;
    skillUri.linkFeature = MallocCString(cUri.linkFeature);
    return skillUri;
}

RetCArrSkillUri ConvertArrSkillUris(const std::vector<AppExecFwk::SkillUri>& cUris)
{
    RetCArrSkillUri skillUris;
    skillUris.size = static_cast<int64_t>(cUris.size());
    skillUris.head = nullptr;

    if (skillUris.size == 0) {
        return skillUris;
    }
    RetSkillUri *retValue = reinterpret_cast<RetSkillUri *>(malloc(sizeof(RetSkillUri) * skillUris.size));
    if (retValue != nullptr) {
        for (int32_t i = 0; i < skillUris.size; i++) {
            retValue[i] = ConvertSkillUri(cUris[i]);
        }
        skillUris.head = retValue;
    } else {
        APP_LOGE("ConvertArrSkillUris malloc failed");
        return skillUris;
    }
    return skillUris;
}

RetSkill ConvertSkill(const AppExecFwk::Skill& cSkill)
{
    RetSkill skill;
    skill.actions = ConvertArrString(cSkill.actions);
    skill.entities = ConvertArrString(cSkill.entities);
    skill.uris = ConvertArrSkillUris(cSkill.uris);
    skill.domainVerify = cSkill.domainVerify;
    return skill;
}

RetCArrSkill ConvertSkills(const std::vector<AppExecFwk::Skill>& cSkills)
{
    RetCArrSkill skills;
    skills.size = static_cast<int64_t>(cSkills.size());
    skills.head = nullptr;
    if (skills.size == 0) {
        return skills;
    }
    RetSkill *retValue = reinterpret_cast<RetSkill *>(malloc(sizeof(RetSkill) * skills.size));
    if (retValue != nullptr) {
        for (int32_t i = 0; i < skills.size; i++) {
            retValue[i] = ConvertSkill(cSkills[i]);
        }
        skills.head = retValue;
    } else {
        APP_LOGE("ConvertSkills malloc failed");
        return skills;
    }
    return skills;
}

RetReqPermissionDetail ConvertRequestPermission(const AppExecFwk::RequestPermission& requestPermission)
{
    RetReqPermissionDetail reqPer;
    reqPer.name = MallocCString(requestPermission.name);
    reqPer.moduleName = MallocCString(requestPermission.moduleName);
    reqPer.reason = MallocCString(requestPermission.reason);
    reqPer.reasonId = requestPermission.reasonId;
    reqPer.usedScence = ConvertUsedScene(requestPermission.usedScene);
    return reqPer;
}

RetApplicationInfo ConvertApplicationInfo(const AppExecFwk::ApplicationInfo& cAppInfo)
{
    RetApplicationInfo appInfo;
    appInfo.name = MallocCString(cAppInfo.name);
    appInfo.description = MallocCString(cAppInfo.description);
    appInfo.descriptionId = cAppInfo.descriptionId;
    appInfo.enabled = cAppInfo.enabled;
    appInfo.label = MallocCString(cAppInfo.label);
    appInfo.labelId = cAppInfo.labelId;
    appInfo.icon = MallocCString(cAppInfo.iconPath);
    appInfo.iconId = cAppInfo.iconId;
    appInfo.process = MallocCString(cAppInfo.process);

    appInfo.permissions = ConvertArrString(cAppInfo.permissions);

    appInfo.codePath = MallocCString(cAppInfo.codePath);

    appInfo.metadataArray = ConvertArrMoMeta(cAppInfo.metadata);

    appInfo.removable = cAppInfo.removable;
    appInfo.accessTokenId = cAppInfo.accessTokenId;
    appInfo.uid = cAppInfo.uid;

    appInfo.iconResource = ConvertResource(cAppInfo.iconResource);
    appInfo.labelResource = ConvertResource(cAppInfo.labelResource);
    appInfo.descriptionResource = ConvertResource(cAppInfo.descriptionResource);

    appInfo.appDistributionType = MallocCString(cAppInfo.appDistributionType);
    appInfo.appProvisionType = MallocCString(cAppInfo.appProvisionType);
    appInfo.systemApp = cAppInfo.isSystemApp;
    appInfo.bundleType = static_cast<int32_t>(cAppInfo.bundleType);
    appInfo.debug = cAppInfo.debug;
    appInfo.dataUnclearable = !cAppInfo.userDataClearable;
    appInfo.cloudFileSyncEnabled = cAppInfo.cloudFileSyncEnabled;
    return appInfo;
}

RetApplicationInfoV2 ConvertApplicationInfoV2(const AppExecFwk::ApplicationInfo& cAppInfo)
{
    RetApplicationInfoV2 appInfo;
    appInfo.name = MallocCString(cAppInfo.name);
    appInfo.description = MallocCString(cAppInfo.description);
    appInfo.descriptionId = cAppInfo.descriptionId;
    appInfo.enabled = cAppInfo.enabled;
    appInfo.label = MallocCString(cAppInfo.label);
    appInfo.labelId = cAppInfo.labelId;
    appInfo.icon = MallocCString(cAppInfo.iconPath);
    appInfo.iconId = cAppInfo.iconId;
    appInfo.process = MallocCString(cAppInfo.process);

    appInfo.permissions = ConvertArrString(cAppInfo.permissions);

    appInfo.codePath = MallocCString(cAppInfo.codePath);

    appInfo.metadataArray = ConvertArrMoMeta(cAppInfo.metadata);

    appInfo.removable = cAppInfo.removable;
    appInfo.accessTokenId = cAppInfo.accessTokenId;
    appInfo.uid = cAppInfo.uid;

    appInfo.iconResource = ConvertResource(cAppInfo.iconResource);
    appInfo.labelResource = ConvertResource(cAppInfo.labelResource);
    appInfo.descriptionResource = ConvertResource(cAppInfo.descriptionResource);

    appInfo.appDistributionType = MallocCString(cAppInfo.appDistributionType);
    appInfo.appProvisionType = MallocCString(cAppInfo.appProvisionType);
    appInfo.systemApp = cAppInfo.isSystemApp;
    appInfo.bundleType = static_cast<int32_t>(cAppInfo.bundleType);
    appInfo.debug = cAppInfo.debug;
    appInfo.dataUnclearable = !cAppInfo.userDataClearable;
    appInfo.cloudFileSyncEnabled = cAppInfo.cloudFileSyncEnabled;
    std::string externalNativeLibraryPath = "";
    if (!cAppInfo.nativeLibraryPath.empty()) {
        externalNativeLibraryPath = CONTEXT_DATA_STORAGE_BUNDLE + cAppInfo.nativeLibraryPath;
    }
    appInfo.nativeLibraryPath = MallocCString(externalNativeLibraryPath);
    appInfo.multiAppMode.multiAppModeType = static_cast<int32_t>(cAppInfo.multiAppMode.multiAppModeType);
    appInfo.multiAppMode.count = cAppInfo.multiAppMode.maxCount;
    appInfo.appIndex = cAppInfo.appIndex;
    appInfo.installSource =  MallocCString(cAppInfo.installSource);
    appInfo.releaseType = MallocCString(cAppInfo.apiReleaseType);
    return appInfo;
}

RetExtensionAbilityInfo ConvertExtensionAbilityInfo(const AppExecFwk::ExtensionAbilityInfo& extensionInfos)
{
    RetExtensionAbilityInfo exInfo;
    exInfo.bundleName = MallocCString(extensionInfos.bundleName);
    exInfo.moduleName = MallocCString(extensionInfos.moduleName);
    exInfo.name = MallocCString(extensionInfos.name);
    exInfo.labelId = extensionInfos.labelId;
    exInfo.descriptionId = extensionInfos.descriptionId;
    exInfo.iconId = extensionInfos.iconId;
    exInfo.exported = extensionInfos.visible;
    exInfo.extensionAbilityType = static_cast<int32_t>(extensionInfos.type);
    exInfo.permissions = ConvertArrString(extensionInfos.permissions);
    exInfo.applicationInfo = ConvertApplicationInfo(extensionInfos.applicationInfo);
    exInfo.metadata = ConvertArrMetadata(extensionInfos.metadata);
    exInfo.enabled = extensionInfos.enabled;
    exInfo.readPermission = MallocCString(extensionInfos.readPermission);
    exInfo.writePermission = MallocCString(extensionInfos.writePermission);
    exInfo.extensionAbilityTypeName = MallocCString(extensionInfos.extensionTypeName);
    return exInfo;
}

RetExtensionAbilityInfoV2 ConvertExtensionAbilityInfoV2(const AppExecFwk::ExtensionAbilityInfo& extensionInfos)
{
    RetExtensionAbilityInfoV2 exInfo;
    exInfo.bundleName = MallocCString(extensionInfos.bundleName);
    exInfo.moduleName = MallocCString(extensionInfos.moduleName);
    exInfo.name = MallocCString(extensionInfos.name);
    exInfo.labelId = extensionInfos.labelId;
    exInfo.descriptionId = extensionInfos.descriptionId;
    exInfo.iconId = extensionInfos.iconId;
    exInfo.exported = extensionInfos.visible;
    exInfo.extensionAbilityType = static_cast<int32_t>(extensionInfos.type);
    exInfo.permissions = ConvertArrString(extensionInfos.permissions);
    exInfo.applicationInfo = ConvertApplicationInfoV2(extensionInfos.applicationInfo);
    exInfo.metadata = ConvertArrMetadata(extensionInfos.metadata);
    exInfo.enabled = extensionInfos.enabled;
    exInfo.readPermission = MallocCString(extensionInfos.readPermission);
    exInfo.writePermission = MallocCString(extensionInfos.writePermission);
    exInfo.extensionAbilityTypeName = MallocCString(extensionInfos.extensionTypeName);
    exInfo.skills = ConvertSkills(extensionInfos.skills);
    exInfo.appIndex = extensionInfos.appIndex;
    return exInfo;
}

CArrRetExtensionAbilityInfo ConvertArrExtensionAbilityInfo(
    const std::vector<AppExecFwk::ExtensionAbilityInfo>& extensionInfos)
{
    CArrRetExtensionAbilityInfo exAbInfo;
    exAbInfo.size = static_cast<int64_t>(extensionInfos.size());
    exAbInfo.head = nullptr;
    if (exAbInfo.size > 0) {
        RetExtensionAbilityInfo *retValue = reinterpret_cast<RetExtensionAbilityInfo *>
        (malloc(sizeof(RetExtensionAbilityInfo) * exAbInfo.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < exAbInfo.size; i++) {
                retValue[i] = ConvertExtensionAbilityInfo(extensionInfos[i]);
            }
            exAbInfo.head = retValue;
        } else {
            APP_LOGE("ConvertArrExtensionAbilityInfo malloc failed");
            return exAbInfo;
        }
    }
    return exAbInfo;
}

CArrRetExtensionAbilityInfoV2 ConvertArrExtensionAbilityInfoV2(
    const std::vector<AppExecFwk::ExtensionAbilityInfo>& extensionInfos)
{
    CArrRetExtensionAbilityInfoV2 exAbInfo;
    exAbInfo.size = static_cast<int64_t>(extensionInfos.size());
    exAbInfo.head = nullptr;
    if (exAbInfo.size > 0) {
        RetExtensionAbilityInfoV2 *retValue = reinterpret_cast<RetExtensionAbilityInfoV2 *>
        (malloc(sizeof(RetExtensionAbilityInfoV2) * exAbInfo.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < exAbInfo.size; i++) {
                retValue[i] = ConvertExtensionAbilityInfoV2(extensionInfos[i]);
            }
            exAbInfo.head = retValue;
        } else {
            APP_LOGE("ConvertArrExtensionAbilityInfo malloc failed");
            return exAbInfo;
        }
    }
    return exAbInfo;
}

RetSignatureInfo ConvertSignatureInfo(const AppExecFwk::SignatureInfo& cSignatureInfo)
{
    RetSignatureInfo signatureInfo;
    signatureInfo.appId = MallocCString(cSignatureInfo.appId);
    signatureInfo.fingerprint = MallocCString(cSignatureInfo.fingerprint);
    signatureInfo.appIdentifier = MallocCString(cSignatureInfo.appIdentifier);
    return signatureInfo;
}

RetAbilityInfo ConvertAbilityInfo(const AppExecFwk::AbilityInfo& cAbilityInfos)
{
    RetAbilityInfo abInfo;
    abInfo.bundleName = MallocCString(cAbilityInfos.bundleName);
    abInfo.moduleName = MallocCString(cAbilityInfos.moduleName);
    abInfo.name = MallocCString(cAbilityInfos.name);
    abInfo.label = MallocCString(cAbilityInfos.label);
    abInfo.labelId = cAbilityInfos.labelId;
    abInfo.description = MallocCString(cAbilityInfos.description);
    abInfo.descriptionId = cAbilityInfos.descriptionId;
    abInfo.icon = MallocCString(cAbilityInfos.iconPath);
    abInfo.iconId = cAbilityInfos.iconId;
    abInfo.process = MallocCString(cAbilityInfos.process);
    abInfo.exported = cAbilityInfos.visible;
    abInfo.orientation = static_cast<int32_t>(cAbilityInfos.orientation);
    abInfo.launchType = static_cast<int32_t>(cAbilityInfos.launchMode);
    abInfo.permissions = ConvertArrString(cAbilityInfos.permissions);
    abInfo.deviceTypes = ConvertArrString(cAbilityInfos.deviceTypes);
    abInfo.applicationInfo = ConvertApplicationInfo(cAbilityInfos.applicationInfo);
    abInfo.metadata = ConvertArrMetadata(cAbilityInfos.metadata);
    abInfo.enabled = cAbilityInfos.enabled;

    abInfo.supportWindowModes.size = static_cast<int64_t>(cAbilityInfos.windowModes.size());
    abInfo.supportWindowModes.head = nullptr;
    if (abInfo.supportWindowModes.size > 0) {
        int32_t *retValue = static_cast<int32_t *>(malloc(sizeof(int32_t) * abInfo.supportWindowModes.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < abInfo.supportWindowModes.size; i++) {
                retValue[i] = static_cast<int32_t>(cAbilityInfos.windowModes[i]);
            }
            abInfo.supportWindowModes.head = retValue;
        } else {
            APP_LOGE("ConvertAbilityInfo malloc failed");
            return abInfo;
        }
    }

    abInfo.windowSize.maxWindowRatio = cAbilityInfos.maxWindowRatio;
    abInfo.windowSize.minWindowRatio = cAbilityInfos.minWindowRatio;
    abInfo.windowSize.maxWindowWidth = cAbilityInfos.maxWindowWidth;
    abInfo.windowSize.minWindowWidth = cAbilityInfos.minWindowWidth;
    abInfo.windowSize.maxWindowHeight = cAbilityInfos.maxWindowHeight;
    abInfo.windowSize.minWindowHeight = cAbilityInfos.minWindowHeight;
    return abInfo;
}

RetAbilityInfoV2 ConvertAbilityInfoV2(const AppExecFwk::AbilityInfo& cAbilityInfos)
{
    RetAbilityInfoV2 abInfo;
    abInfo.bundleName = MallocCString(cAbilityInfos.bundleName);
    abInfo.moduleName = MallocCString(cAbilityInfos.moduleName);
    abInfo.name = MallocCString(cAbilityInfos.name);
    abInfo.label = MallocCString(cAbilityInfos.label);
    abInfo.labelId = cAbilityInfos.labelId;
    abInfo.description = MallocCString(cAbilityInfos.description);
    abInfo.descriptionId = cAbilityInfos.descriptionId;
    abInfo.icon = MallocCString(cAbilityInfos.iconPath);
    abInfo.iconId = cAbilityInfos.iconId;
    abInfo.process = MallocCString(cAbilityInfos.process);
    abInfo.exported = cAbilityInfos.visible;
    abInfo.orientation = static_cast<int32_t>(cAbilityInfos.orientation);
    abInfo.launchType = static_cast<int32_t>(cAbilityInfos.launchMode);
    abInfo.permissions = ConvertArrString(cAbilityInfos.permissions);
    abInfo.deviceTypes = ConvertArrString(cAbilityInfos.deviceTypes);
    abInfo.applicationInfo = ConvertApplicationInfoV2(cAbilityInfos.applicationInfo);
    abInfo.metadata = ConvertArrMetadata(cAbilityInfos.metadata);
    abInfo.enabled = cAbilityInfos.enabled;

    abInfo.supportWindowModes.size = static_cast<int64_t>(cAbilityInfos.windowModes.size());
    abInfo.supportWindowModes.head = nullptr;
    if (abInfo.supportWindowModes.size > 0) {
        int32_t *retValue = static_cast<int32_t *>(malloc(sizeof(int32_t) * abInfo.supportWindowModes.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < abInfo.supportWindowModes.size; i++) {
                retValue[i] = static_cast<int32_t>(cAbilityInfos.windowModes[i]);
            }
            abInfo.supportWindowModes.head = retValue;
        } else {
            APP_LOGE("ConvertAbilityInfo malloc failed");
            return abInfo;
        }
    }

    abInfo.windowSize.maxWindowRatio = cAbilityInfos.maxWindowRatio;
    abInfo.windowSize.minWindowRatio = cAbilityInfos.minWindowRatio;
    abInfo.windowSize.maxWindowWidth = cAbilityInfos.maxWindowWidth;
    abInfo.windowSize.minWindowWidth = cAbilityInfos.minWindowWidth;
    abInfo.windowSize.maxWindowHeight = cAbilityInfos.maxWindowHeight;
    abInfo.windowSize.minWindowHeight = cAbilityInfos.minWindowHeight;

    abInfo.excludeFromDock = cAbilityInfos.excludeFromDock;
    abInfo.skills = ConvertSkills(cAbilityInfos.skills);
    return abInfo;
}

CArrRetAbilityInfo ConvertArrAbilityInfo(const std::vector<AppExecFwk::AbilityInfo>& abilityInfos)
{
    CArrRetAbilityInfo abInfo;
    abInfo.size = static_cast<int64_t>(abilityInfos.size());
    abInfo.head = nullptr;
    if (abInfo.size > 0) {
        RetAbilityInfo *retValue = reinterpret_cast<RetAbilityInfo *>(malloc(sizeof(RetAbilityInfo) * abInfo.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < abInfo.size; i++) {
                retValue[i] = ConvertAbilityInfo(abilityInfos[i]);
            }
            abInfo.head = retValue;
        } else {
            APP_LOGE("ConvertArrAbilityInfo malloc failed");
            return abInfo;
        }
    }
    return abInfo;
}

CArrRetAbilityInfoV2 ConvertArrAbilityInfoV2(const std::vector<AppExecFwk::AbilityInfo>& abilityInfos)
{
    CArrRetAbilityInfoV2 abInfo;
    abInfo.size = static_cast<int64_t>(abilityInfos.size());
    abInfo.head = nullptr;
    if (abInfo.size > 0) {
        RetAbilityInfoV2 *retValue =
            reinterpret_cast<RetAbilityInfoV2 *>(malloc(sizeof(RetAbilityInfoV2) * abInfo.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < abInfo.size; i++) {
                retValue[i] = ConvertAbilityInfoV2(abilityInfos[i]);
            }
            abInfo.head = retValue;
        } else {
            APP_LOGE("ConvertArrAbilityInfo malloc failed");
            return abInfo;
        }
    }
    return abInfo;
}

CArrRetPreloadItem ConvertPreloadItem(const std::vector<AppExecFwk::PreloadItem>& preloads)
{
    CArrRetPreloadItem pLoad;
    pLoad.size = static_cast<int64_t>(preloads.size());
    pLoad.head = nullptr;
    if (pLoad.size > 0) {
        RetPreloadItem *retValue = reinterpret_cast<RetPreloadItem *>(malloc(sizeof(RetPreloadItem) * pLoad.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < pLoad.size; i++) {
                retValue[i].moduleName = MallocCString(preloads[i].moduleName);
            }
            pLoad.head = retValue;
        } else {
            APP_LOGE("ConvertPreloadItem malloc failed");
            return pLoad;
        }
    }
    return pLoad;
}

CArrRetDependency ConvertDependency(const std::vector<AppExecFwk::Dependency>& dependencies)
{
    CArrRetDependency dep;
    dep.size = static_cast<int64_t>(dependencies.size());
    dep.head = nullptr;
    if (dep.size > 0) {
        RetDependency *retValue = reinterpret_cast<RetDependency *>(malloc(sizeof(RetDependency) * dep.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < dep.size; i++) {
                retValue[i].bundleName = MallocCString(dependencies[i].bundleName);
                retValue[i].moduleName = MallocCString(dependencies[i].moduleName);
                retValue[i].versionCode = dependencies[i].versionCode;
            }
            dep.head = retValue;
        } else {
            APP_LOGE("ConvertDependency malloc failed");
            return dep;
        }
    }
    return dep;
}

CArrDataItem ConvertArrDataItem(const std::map<std::string, std::string>& data)
{
    CArrDataItem dataItems;
    dataItems.size = static_cast<int64_t>(data.size());
    dataItems.head = nullptr;

    if (dataItems.size == 0) {
        return dataItems;
    }
    CDataItem *retValue = reinterpret_cast<CDataItem *>
                                        (malloc(sizeof(CDataItem) * dataItems.size));
    if (retValue != nullptr) {
        int i = 0;
        for (auto it = data.begin(); it != data.end(); ++it) {
            retValue[i].key = MallocCString(it->first);
            retValue[i].value = MallocCString(it->second);
            i = i + 1;
        }
        dataItems.head = retValue;
    } else {
        APP_LOGE("ConvertArrDataItem malloc failed");
        return dataItems;
    }
    return dataItems;
}

CRouterItem ConvertRouterItem(const AppExecFwk::RouterItem& router)
{
    CRouterItem routerItem;
    routerItem.name = MallocCString(router.name);
    routerItem.pageSourceFile = MallocCString(router.pageSourceFile);
    routerItem.buildFunction = MallocCString(router.buildFunction);
    routerItem.data = ConvertArrDataItem(router.data);
    routerItem.customData = MallocCString(router.customData);
    return routerItem;
}

CArrRouterItem ConvertRouterMap(const std::vector<AppExecFwk::RouterItem>& routerArray)
{
    CArrRouterItem routerMap;
    routerMap.size = static_cast<int64_t>(routerArray.size());
    routerMap.head = nullptr;

    if (routerMap.size == 0) {
        return routerMap;
    }

    CRouterItem *retValue = reinterpret_cast<CRouterItem *>
                                        (malloc(sizeof(CRouterItem) * routerMap.size));
    if (retValue != nullptr) {
        for (int32_t i = 0; i < routerMap.size; i++) {
            retValue[i] = ConvertRouterItem(routerArray[i]);
        }
        routerMap.head = retValue;
    } else {
        APP_LOGE("ConvertRouterMap malloc failed");
        return routerMap;
    }
    return routerMap;
}

RetHapModuleInfo ConvertHapModuleInfo(const AppExecFwk::HapModuleInfo& hapModuleInfo)
{
    RetHapModuleInfo hapInfo;
    hapInfo.name = MallocCString(hapModuleInfo.name);
    hapInfo.icon = MallocCString(hapModuleInfo.iconPath);
    hapInfo.iconId = hapModuleInfo.iconId;
    hapInfo.label = MallocCString(hapModuleInfo.label);
    hapInfo.labelId = hapModuleInfo.labelId;
    hapInfo.description = MallocCString(hapModuleInfo.description);
    hapInfo.descriptionId = hapModuleInfo.descriptionId;
    hapInfo.mainElementName = MallocCString(hapModuleInfo.mainElementName);

    hapInfo.abilitiesInfo = ConvertArrAbilityInfo(hapModuleInfo.abilityInfos);

    hapInfo.extensionAbilitiesInfo = ConvertArrExtensionAbilityInfo(hapModuleInfo.extensionInfos);

    hapInfo.metadata = ConvertArrMetadata(hapModuleInfo.metadata);

    hapInfo.deviceTypes = ConvertArrString(hapModuleInfo.deviceTypes);

    hapInfo.installationFree = hapModuleInfo.installationFree;
    hapInfo.hashValue = MallocCString(hapModuleInfo.hashValue);
    hapInfo.moduleType = static_cast<int32_t>(hapModuleInfo.moduleType);

    hapInfo.preloads = ConvertPreloadItem(hapModuleInfo.preloads);

    hapInfo.dependencies = ConvertDependency(hapModuleInfo.dependencies);

    if (!hapModuleInfo.fileContextMenu.empty()) {
        hapInfo.fileContextMenuConfig = MallocCString(hapModuleInfo.fileContextMenu);
    } else {
        hapInfo.fileContextMenuConfig = MallocCString("");
    }
    return hapInfo;
}

RetHapModuleInfoV2 ConvertHapModuleInfoV2(const AppExecFwk::HapModuleInfo& hapModuleInfo)
{
    RetHapModuleInfoV2 hapInfo;
    hapInfo.name = MallocCString(hapModuleInfo.name);
    hapInfo.icon = MallocCString(hapModuleInfo.iconPath);
    hapInfo.iconId = hapModuleInfo.iconId;
    hapInfo.label = MallocCString(hapModuleInfo.label);
    hapInfo.labelId = hapModuleInfo.labelId;
    hapInfo.description = MallocCString(hapModuleInfo.description);
    hapInfo.descriptionId = hapModuleInfo.descriptionId;
    hapInfo.mainElementName = MallocCString(hapModuleInfo.mainElementName);

    hapInfo.abilitiesInfo = ConvertArrAbilityInfoV2(hapModuleInfo.abilityInfos);

    hapInfo.extensionAbilitiesInfo = ConvertArrExtensionAbilityInfoV2(hapModuleInfo.extensionInfos);

    hapInfo.metadata = ConvertArrMetadata(hapModuleInfo.metadata);

    hapInfo.deviceTypes = ConvertArrString(hapModuleInfo.deviceTypes);

    hapInfo.installationFree = hapModuleInfo.installationFree;
    hapInfo.hashValue = MallocCString(hapModuleInfo.hashValue);
    hapInfo.moduleType = static_cast<int32_t>(hapModuleInfo.moduleType);

    hapInfo.preloads = ConvertPreloadItem(hapModuleInfo.preloads);

    hapInfo.dependencies = ConvertDependency(hapModuleInfo.dependencies);

    if (!hapModuleInfo.fileContextMenu.empty()) {
        hapInfo.fileContextMenuConfig = MallocCString(hapModuleInfo.fileContextMenu);
    } else {
        hapInfo.fileContextMenuConfig = MallocCString("");
    }
    hapInfo.routerMap = ConvertRouterMap(hapModuleInfo.routerArray);

    size_t result = hapModuleInfo.hapPath.find(PATH_PREFIX);
    if (result != std::string::npos) {
        size_t pos = hapModuleInfo.hapPath.find_last_of('/');
        std::string codePath = CODE_PATH_PREFIX;
        if (pos != std::string::npos && pos != hapModuleInfo.hapPath.size() - 1) {
            codePath += hapModuleInfo.hapPath.substr(pos + 1);
        }
        hapInfo.codePath = MallocCString(codePath);
    } else {
        hapInfo.codePath = MallocCString(hapModuleInfo.hapPath);
    }

    std::string externalNativeLibraryPath = "";
    if (!hapModuleInfo.nativeLibraryPath.empty() && !hapModuleInfo.moduleName.empty()) {
        externalNativeLibraryPath = CONTEXT_DATA_STORAGE_BUNDLE + hapModuleInfo.nativeLibraryPath;
    }
    hapInfo.nativeLibraryPath = MallocCString(externalNativeLibraryPath);
    return hapInfo;
}

extern "C" {
#define EXPORT __attribute__((visibility("default")))
EXPORT RetAbilityInfoV2 OHOS_ConvertAbilityInfoV2(void* param)
{
    RetAbilityInfoV2 retInfo = {};
    auto abilityInfo = reinterpret_cast<AppExecFwk::AbilityInfo*>(param);
    if (abilityInfo == nullptr) {
        return retInfo;
    }
    return ConvertAbilityInfoV2(*abilityInfo);
}

EXPORT RetHapModuleInfoV2 OHOS_ConvertHapInfoV2(void* param)
{
    RetHapModuleInfoV2 retInfo = {};
    auto hapModuleInfo = reinterpret_cast<AppExecFwk::HapModuleInfo*>(param);
    if (hapModuleInfo == nullptr) {
        return retInfo;
    }
    return ConvertHapModuleInfoV2(*hapModuleInfo);
}
}

CArrHapInfo ConvertArrHapInfo(const std::vector<AppExecFwk::HapModuleInfo>& hapModuleInfos)
{
    CArrHapInfo hapInfos;
    hapInfos.size = static_cast<int64_t>(hapModuleInfos.size());
    hapInfos.head = nullptr;
    if (hapInfos.size == 0) {
        return hapInfos;
    }
    RetHapModuleInfo *retValue = reinterpret_cast<RetHapModuleInfo *>(malloc(sizeof(RetHapModuleInfo) * hapInfos.size));
    if (retValue == nullptr) {
        APP_LOGE("ConvertArrHapInfo malloc failed");
        return hapInfos;
    }
    for (int32_t i = 0; i < hapInfos.size; i++) {
        retValue[i] = ConvertHapModuleInfo(hapModuleInfos[i]);
    }
    hapInfos.head = retValue;
    return hapInfos;
}

CArrHapInfoV2 ConvertArrHapInfoV2(const std::vector<AppExecFwk::HapModuleInfo>& hapModuleInfos)
{
    CArrHapInfoV2 hapInfos;
    hapInfos.size = static_cast<int64_t>(hapModuleInfos.size());
    hapInfos.head = nullptr;
    if (hapInfos.size == 0) {
        return hapInfos;
    }
    RetHapModuleInfoV2 *retValue =
        reinterpret_cast<RetHapModuleInfoV2 *>(malloc(sizeof(RetHapModuleInfoV2) * hapInfos.size));
    if (retValue == nullptr) {
        APP_LOGE("ConvertArrHapInfo malloc failed");
        return hapInfos;
    }
    for (int32_t i = 0; i < hapInfos.size; i++) {
        retValue[i] = ConvertHapModuleInfoV2(hapModuleInfos[i]);
    }
    hapInfos.head = retValue;
    return hapInfos;
}

CArrReqPerDetail ConvertArrReqPerDetail(const std::vector<AppExecFwk::RequestPermission>& reqPermissionDetails)
{
    CArrReqPerDetail perDetail;
    perDetail.size = static_cast<int64_t>(reqPermissionDetails.size());
    perDetail.head = nullptr;
    if (perDetail.size > 0) {
        RetReqPermissionDetail *retValue = reinterpret_cast<RetReqPermissionDetail *>
                                            (malloc(sizeof(RetReqPermissionDetail) * perDetail.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < perDetail.size; i++) {
                retValue[i] = ConvertRequestPermission(reqPermissionDetails[i]);
            }
            perDetail.head = retValue;
        } else {
            APP_LOGE("ConvertArrReqPerDetail malloc failed");
            return perDetail;
        }
    }
    return perDetail;
}

RetSignatureInfo InitSignInfo()
{
    RetSignatureInfo signatureInfo = {nullptr, nullptr, nullptr};
    return signatureInfo;
}

RetApplicationInfo InitApplicationInfo()
{
    RetApplicationInfo appInfo;
    appInfo.name = nullptr;
    appInfo.description = nullptr;
    appInfo.descriptionId = 0;
    appInfo.enabled = true;
    appInfo.label = nullptr;
    appInfo.labelId = 0;
    appInfo.icon = nullptr;
    appInfo.iconId = 0;
    appInfo.process = nullptr;
    appInfo.permissions = {nullptr, 0};
    appInfo.codePath = nullptr;
    appInfo.metadataArray = {nullptr, 0};
    appInfo.removable = true;
    appInfo.accessTokenId = 0;
    appInfo.uid = -1;
    appInfo.iconResource = {nullptr, nullptr, 0};
    appInfo.labelResource = {nullptr, nullptr, 0};
    appInfo.descriptionResource = {nullptr, nullptr, 0};
    appInfo.appDistributionType = MallocCString("none");
    appInfo.appProvisionType = MallocCString("release");
    appInfo.systemApp = false;
    appInfo.bundleType = 0;
    appInfo.debug = false;
    appInfo.dataUnclearable = false;
    appInfo.cloudFileSyncEnabled = false;
    return appInfo;
}

RetApplicationInfoV2 InitApplicationInfoV2()
{
    RetApplicationInfoV2 appInfo;
    appInfo.name = nullptr;
    appInfo.description = nullptr;
    appInfo.descriptionId = 0;
    appInfo.enabled = true;
    appInfo.label = nullptr;
    appInfo.labelId = 0;
    appInfo.icon = nullptr;
    appInfo.iconId = 0;
    appInfo.process = nullptr;
    appInfo.permissions = {nullptr, 0};
    appInfo.codePath = nullptr;
    appInfo.metadataArray = {nullptr, 0};
    appInfo.removable = true;
    appInfo.accessTokenId = 0;
    appInfo.uid = -1;
    appInfo.iconResource = {nullptr, nullptr, 0};
    appInfo.labelResource = {nullptr, nullptr, 0};
    appInfo.descriptionResource = {nullptr, nullptr, 0};
    appInfo.appDistributionType = MallocCString("none");
    appInfo.appProvisionType = MallocCString("release");
    appInfo.systemApp = false;
    appInfo.bundleType = 0;
    appInfo.debug = false;
    appInfo.dataUnclearable = false;
    appInfo.cloudFileSyncEnabled = false;
    appInfo.nativeLibraryPath = MallocCString("");
    appInfo.multiAppMode.multiAppModeType = static_cast<uint8_t>(0);
    appInfo.multiAppMode.count = 0;
    appInfo.appIndex = 0;
    appInfo.installSource = MallocCString("");
    appInfo.releaseType = MallocCString("");
    return appInfo;
}

RetBundleInfo ConvertBundleInfo(const AppExecFwk::BundleInfo& cBundleInfo, int32_t flags)
{
    RetBundleInfo bundleInfo;
    bundleInfo.name = MallocCString(cBundleInfo.name);
    bundleInfo.vendor = MallocCString(cBundleInfo.vendor);
    bundleInfo.versionCode = cBundleInfo.versionCode;
    bundleInfo.versionName = MallocCString(cBundleInfo.versionName);
    bundleInfo.minCompatibleVersionCode = cBundleInfo.minCompatibleVersionCode;
    bundleInfo.targetVersion = cBundleInfo.targetVersion;
    if ((static_cast<uint32_t>(flags) &
        static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION))
        == static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION)) {
        bundleInfo.appInfo = ConvertApplicationInfo(cBundleInfo.applicationInfo);
    } else {
        bundleInfo.appInfo = InitApplicationInfo();
    }

    bundleInfo.hapInfo = ConvertArrHapInfo(cBundleInfo.hapModuleInfos);
    bundleInfo.perDetail = ConvertArrReqPerDetail(cBundleInfo.reqPermissionDetails);

    bundleInfo.state.size = static_cast<int64_t>(cBundleInfo.reqPermissionStates.size());
    bundleInfo.state.head = nullptr;
    if (bundleInfo.state.size > 0) {
        int32_t *retValue = static_cast<int32_t *>(malloc(sizeof(int32_t) * bundleInfo.state.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < bundleInfo.state.size; i++) {
                retValue[i] = static_cast<int32_t>(cBundleInfo.reqPermissionStates[i]);
            }
            bundleInfo.state.head = retValue;
        } else {
            APP_LOGE("ConvertBundleInfo malloc failed");
            return bundleInfo;
        }
    }

    if ((static_cast<uint32_t>(flags) &
        static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO))
        == static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO)) {
        bundleInfo.signInfo = ConvertSignatureInfo(cBundleInfo.signatureInfo);
    } else {
        bundleInfo.signInfo = InitSignInfo();
    }
    bundleInfo.installTime = cBundleInfo.installTime;
    bundleInfo.updateTime = cBundleInfo.updateTime;
    bundleInfo.uid = cBundleInfo.uid;
    return bundleInfo;
}

RetBundleInfoV2 ConvertBundleInfoV2(const AppExecFwk::BundleInfo& cBundleInfo, int32_t flags)
{
    RetBundleInfoV2 bundleInfo;
    bundleInfo.name = MallocCString(cBundleInfo.name);
    bundleInfo.vendor = MallocCString(cBundleInfo.vendor);
    bundleInfo.versionCode = cBundleInfo.versionCode;
    bundleInfo.versionName = MallocCString(cBundleInfo.versionName);
    bundleInfo.minCompatibleVersionCode = cBundleInfo.minCompatibleVersionCode;
    bundleInfo.targetVersion = cBundleInfo.targetVersion;
    if ((static_cast<uint32_t>(flags) &
        static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION))
        == static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION)) {
        bundleInfo.appInfo = ConvertApplicationInfoV2(cBundleInfo.applicationInfo);
    } else {
        bundleInfo.appInfo = InitApplicationInfoV2();
    }

    bundleInfo.hapInfo = ConvertArrHapInfoV2(cBundleInfo.hapModuleInfos);
    bundleInfo.perDetail = ConvertArrReqPerDetail(cBundleInfo.reqPermissionDetails);

    bundleInfo.state.size = static_cast<int64_t>(cBundleInfo.reqPermissionStates.size());
    bundleInfo.state.head = nullptr;
    if (bundleInfo.state.size > 0) {
        int32_t *retValue = static_cast<int32_t *>(malloc(sizeof(int32_t) * bundleInfo.state.size));
        if (retValue != nullptr) {
            for (int32_t i = 0; i < bundleInfo.state.size; i++) {
                retValue[i] = static_cast<int32_t>(cBundleInfo.reqPermissionStates[i]);
            }
            bundleInfo.state.head = retValue;
        } else {
            APP_LOGE("ConvertBundleInfo malloc failed");
            return bundleInfo;
        }
    }

    if ((static_cast<uint32_t>(flags) &
        static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO))
        == static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO)) {
        bundleInfo.signInfo = ConvertSignatureInfo(cBundleInfo.signatureInfo);
    } else {
        bundleInfo.signInfo = InitSignInfo();
    }
    bundleInfo.installTime = cBundleInfo.installTime;
    bundleInfo.updateTime = cBundleInfo.updateTime;
    bundleInfo.uid = cBundleInfo.uid;
    bundleInfo.routerMap = ConvertRouterMap(cBundleInfo.routerArray);
    bundleInfo.appIndex = cBundleInfo.appIndex;
    return bundleInfo;
}

} // Convert
} // BundleManager
} // CJSystemapi
} // OHOS