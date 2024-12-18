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

#ifndef BUNDLE_MGR_H
#define BUNDLE_MGR_H
#include <vector>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "application_info.h"
#include "bundle_death_recipient.h"
#include "bundle_mgr_interface.h"
#include "cleancache_callback.h"
#include "js_runtime_utils.h"
#ifdef BUNDLE_FRAMEWORK_GRAPHICS
#include "pixel_map.h"
#endif
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
struct AsyncWorkData {
    explicit AsyncWorkData(napi_env napiEnv);
    virtual ~AsyncWorkData();
    napi_env env;
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
};

struct BaseCallbackInfo: public AsyncWorkData {
    explicit BaseCallbackInfo(napi_env napiEnv) : AsyncWorkData(napiEnv) {}
    int32_t err = 0;
    std::string message;
};

struct QueryParameter {
    int flags;
    std::string userId;
};

struct BundleOptions {
    int32_t userId = Constants::UNSPECIFIED_USERID;
};

struct AbilityEnableCallbackInfo : public BaseCallbackInfo {
    explicit AbilityEnableCallbackInfo(napi_env napiEnv) : BaseCallbackInfo(napiEnv) {}
    AbilityInfo abilityInfo;
    bool isEnable = false;
};

struct ApplicationEnableCallbackInfo : public BaseCallbackInfo {
    explicit ApplicationEnableCallbackInfo(napi_env napiEnv) : BaseCallbackInfo(napiEnv) {}
    std::string bundleName;
    bool isEnable = false;
};

struct LaunchWantCallbackInfo : public BaseCallbackInfo {
    explicit LaunchWantCallbackInfo(napi_env napiEnv) : BaseCallbackInfo(napiEnv) {}
    std::string bundleName;
    int32_t userId = Constants::UNSPECIFIED_USERID;
    OHOS::AAFwk::Want want;
    ErrCode ret = ERR_OK;
};

struct GetBundleArchiveInfoCallbackInfo : public BaseCallbackInfo {
    explicit GetBundleArchiveInfoCallbackInfo(napi_env napiEnv) : BaseCallbackInfo(napiEnv) {}
    std::string hapFilePath;
    int32_t flags = 0;
    BundleInfo bundleInfo;
    bool ret = false;
};

struct AbilityIconCallbackInfo : public BaseCallbackInfo {
    explicit AbilityIconCallbackInfo(napi_env napiEnv) : BaseCallbackInfo(napiEnv) {}
    std::string bundleName;
    std::string moduleName;
    std::string abilityName;
    bool hasModuleName = false;
    ErrCode ret = ERR_OK;
#ifdef BUNDLE_FRAMEWORK_GRAPHICS
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
#endif
};

struct AsyncAbilityInfoCallbackInfo : public AsyncWorkData {
    explicit AsyncAbilityInfoCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    OHOS::AAFwk::Want want;
    int32_t flags = 0;
    int32_t userId = Constants::UNSPECIFIED_USERID;
    std::vector<OHOS::AppExecFwk::AbilityInfo> abilityInfos;
    bool ret = false;
    int32_t err = 0;
};

struct AsyncAbilityInfosCallbackInfo : public AsyncWorkData {
    explicit AsyncAbilityInfosCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    int32_t flags = 0;
    std::string bundleName;
    std::string abilityName;
    std::string moduleName = "";
    bool hasModuleName = false;
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    bool ret = false;
    int32_t err = 0;
    std::string message;
};

struct AsyncBundleInfoCallbackInfo : public AsyncWorkData {
    explicit AsyncBundleInfoCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string param;
    int32_t flags = 0;
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    bool ret = false;
    int32_t err = 0;
    std::string message;
    BundleOptions bundleOptions;
};

struct AsyncApplicationInfoCallbackInfo : public AsyncWorkData {
    explicit AsyncApplicationInfoCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName;
    int32_t flags = 0;
    int32_t userId = Constants::UNSPECIFIED_USERID;
    OHOS::AppExecFwk::ApplicationInfo appInfo;
    bool ret = false;
    int32_t err = 0;
    std::string message;
};

struct AsyncPermissionDefCallbackInfo : public AsyncWorkData {
    explicit AsyncPermissionDefCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string permissionName;
    OHOS::AppExecFwk::PermissionDef permissionDef;
    bool ret = false;
    int32_t err = 0;
    std::string message;
};

struct AsyncBundleInfosCallbackInfo : public AsyncWorkData {
    explicit AsyncBundleInfosCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    int32_t flags = 0;
    std::vector<OHOS::AppExecFwk::BundleInfo> bundleInfos;
    bool ret = false;
    int32_t err = 0;
    std::string message;
    int32_t userId = Constants::UNSPECIFIED_USERID;
};

struct AsyncApplicationInfosCallbackInfo : public AsyncWorkData {
    explicit AsyncApplicationInfosCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    int32_t flags = 0;
    int32_t userId = Constants::UNSPECIFIED_USERID;
    std::vector<OHOS::AppExecFwk::ApplicationInfo> appInfos;
    bool ret = false;
    int32_t err = 0;
    std::string message;
};

struct AsyncAbilityLabelCallbackInfo : public AsyncWorkData {
    explicit AsyncAbilityLabelCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName;
    std::string className;
    std::string moduleName = "";
    bool hasModuleName = false;
    std::string abilityLabel;
    int32_t err = 0;
    std::string message;
};

struct InstallResult {
    std::string resultMsg;
    int32_t resultCode = 0;
};

struct AsyncInstallCallbackInfo : public AsyncWorkData {
    explicit AsyncInstallCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::vector<std::string> hapFiles;
    std::string bundleName;
    std::string param;
    OHOS::AppExecFwk::InstallParam installParam;
    InstallResult installResult;
    int32_t errCode = 0;
};

struct AsyncGetBundleInstallerCallbackInfo : public AsyncWorkData {
    explicit AsyncGetBundleInstallerCallbackInfo(napi_env env) : AsyncWorkData(env) {}
};

struct AsyncFormInfosCallbackInfo : public AsyncWorkData {
    explicit AsyncFormInfosCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::vector<OHOS::AppExecFwk::FormInfo> formInfos;
    bool ret = false;
};

struct AsyncFormInfosByModuleCallbackInfo : public AsyncWorkData {
    explicit AsyncFormInfosByModuleCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName;
    std::string moduleName;
    std::vector<OHOS::AppExecFwk::FormInfo> formInfos;
    bool ret = false;
};

struct AsyncFormInfosByAppCallbackInfo : public AsyncWorkData {
    explicit AsyncFormInfosByAppCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName;
    std::vector<OHOS::AppExecFwk::FormInfo> formInfos;
    bool ret = false;
};

struct AsyncLaunchWantForBundleCallbackInfo : public AsyncWorkData {
    explicit AsyncLaunchWantForBundleCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName;
    OHOS::AAFwk::Want want;
    bool ret = false;
    int32_t err = 0;
};

struct AsyncGetBundleGidsCallbackInfo : public AsyncWorkData {
    explicit AsyncGetBundleGidsCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName;
    std::vector<int32_t> gids;
    int32_t err = 0;
    bool ret = false;
    std::string message;
};

struct AsyncGetNameByUidInfo : public AsyncWorkData {
    explicit AsyncGetNameByUidInfo(napi_env env) : AsyncWorkData(env) {}
    int32_t uid = 0;
    std::string bundleName;
    int32_t err = 0;
    bool ret = false;
};

struct AsyncHandleBundleContext : public AsyncWorkData {
    explicit AsyncHandleBundleContext(napi_env env) : AsyncWorkData(env) {}
    OHOS::sptr<CleanCacheCallback> cleanCacheCallback;
    std::string bundleName;
    std::string className;
    uint32_t labelId = 0;
    uint32_t iconId = 0;
    bool ret = false;
    int32_t err = 0;
};

struct EnabledInfo : public AsyncWorkData {
    explicit EnabledInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName;
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    bool isEnable = false;
    bool result = false;
    int32_t errCode = 0;
    std::string errMssage;
};

struct AsyncAbilityInfo : public AsyncWorkData {
    explicit AsyncAbilityInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName;
    std::string abilityName;
    std::string moduleName = "";
    bool hasModuleName = false;
#ifdef BUNDLE_FRAMEWORK_GRAPHICS
    std::shared_ptr<Media::PixelMap> pixelMap;
#endif
    int32_t errCode = 0;
    bool result = false;
    std::string errMssage;
};

struct Query {
    std::string bundleName_;
    std::string interfaceType_;
    int32_t flags_ = 0;
    int32_t userId_ = Constants::UNSPECIFIED_USERID;
    napi_env env_;
    Query(const std::string &bundleName, const std::string &interfaceType, int32_t flags, int32_t userId, napi_env env)
        : bundleName_(bundleName), interfaceType_(interfaceType), flags_(flags), userId_(userId), env_(env) {}

    bool operator==(const Query &query) const
    {
        return bundleName_ == query.bundleName_ && interfaceType_ == query.interfaceType_ &&
            flags_ == query.flags_ && userId_ == query.userId_ && env_ == query.env_;
    }
};

struct QueryHash  {
    size_t operator()(const Query &query) const
    {
        return std::hash<std::string>()(query.bundleName_) ^ std::hash<std::string>()(query.interfaceType_) ^
            std::hash<int32_t>()(query.flags_) ^ std::hash<int32_t>()(query.userId_);
    }
};

class BundleMgrDeathRecipient : public IRemoteObject::DeathRecipient {
    virtual void OnRemoteDied([[maybe_unused]] const wptr<IRemoteObject>& remote) override;
};

extern thread_local napi_ref g_classBundleInstaller;

napi_value WrapVoidToJS(napi_env env);
napi_value GetApplicationInfos(napi_env env, napi_callback_info info);
napi_value QueryAbilityInfos(napi_env env, napi_callback_info info);
napi_value GetBundleInfos(napi_env env, napi_callback_info info);
napi_value GetPermissionDef(napi_env env, napi_callback_info info);
napi_value GetBundleInfo(napi_env env, napi_callback_info info);
napi_value GetNameForUid(napi_env env, napi_callback_info info);
napi_value GetPermissionDef(napi_env env, napi_callback_info info);
napi_value GetAllFormsInfo(napi_env env, napi_callback_info info);
napi_value GetFormsInfoByApp(napi_env env, napi_callback_info info);
napi_value GetFormsInfoByModule(napi_env env, napi_callback_info info);
napi_value GetShortcutInfos(napi_env env, napi_callback_info info);
napi_value UnregisterPermissionsChanged(napi_env env, napi_callback_info info);
napi_value ClearBundleCache(napi_env env, napi_callback_info info);
napi_value GetBundleArchiveInfo(napi_env env, napi_callback_info info);
napi_value GetAbilityIcon(napi_env env, napi_callback_info info);
napi_value GetLaunchWantForBundle(napi_env env, napi_callback_info info);
napi_value IsApplicationEnabled(napi_env env, napi_callback_info info);
napi_value IsAbilityEnabled(napi_env env, napi_callback_info info);
napi_value SetAbilityEnabled(napi_env env, napi_callback_info info);
napi_value SetApplicationEnabled(napi_env env, napi_callback_info info);
napi_value GetBundleGids(napi_env env, napi_callback_info info);
napi_value GetAbilityInfo(napi_env env, napi_callback_info info);
napi_value GetAbilityLabel(napi_env env, napi_callback_info info);
napi_value GetBundleInstaller(napi_env env, napi_callback_info info);
napi_value Install(napi_env env, napi_callback_info info);
napi_value Recover(napi_env env, napi_callback_info info);
napi_value Uninstall(napi_env env, napi_callback_info info);
napi_value BundleInstallerConstructor(napi_env env, napi_callback_info info);
bool UnwrapAbilityInfo(napi_env env, napi_value param, OHOS::AppExecFwk::AbilityInfo& abilityInfo);
void CreateInstallErrorCodeObject(napi_env env, napi_value value);

napi_value CreateAbilityTypeObject(napi_env env);
napi_value CreateAbilitySubTypeObject(napi_env env);
napi_value CreateDisplayOrientationObject(napi_env env);
napi_value CreateLaunchModeObject(napi_env env);
napi_value CreateColorModeObject(napi_env env);
napi_value CreateGrantStatusObject(napi_env env);
napi_value CreateModuleRemoveFlagObject(napi_env env);
napi_value CreateSignatureCompareResultObject(napi_env env);
napi_value CreateShortcutExistenceObject(napi_env env);
napi_value CreateQueryShortCutFlagObject(napi_env env);
napi_value CreateBundleFlagObject(napi_env env);
napi_value GetAllApplicationInfo(napi_env env, napi_callback_info info);
napi_value GetApplicationInfo(napi_env env, napi_callback_info info);
class JsBundleMgr {
public:
    JsBundleMgr() = default;
    ~JsBundleMgr() = default;

    struct JsAbilityInfo {
        std::string bundleName;
        std::string abilityName;
        std::string moduleName = "";
        bool hasModuleName = false;
        OHOS::AppExecFwk::AbilityInfo abilityInfo;
        bool ret = false;
    };

    struct JsNameForUid {
        std::string bundleName;
        int32_t uid;
        bool ret = false;
    };

    struct JsAbilityLabel {
        std::string bundleName;
        std::string className;
        std::string moduleName = "";
        bool hasModuleName = false;
        std::string abilityLabel;
    };

    struct JsAbilityIcon {
        std::string bundleName;
        std::string abilityName;
        std::string moduleName = "";
        bool hasModuleName = false;
    };

    struct JsGetPermissionDef {
        std::string permissionName;
        OHOS::AppExecFwk::PermissionDef permissionDef;
        bool ret = false;
    };

    struct JsQueryAbilityInfo {
        std::vector<AbilityInfo> abilityInfos;
        bool ret = false;
        bool getCache = false;
    };
    std::string errMessage_;

private:
};

class JsBundleInstall {
public:
    JsBundleInstall() = default;
    ~JsBundleInstall() = default;
    struct BundleInstallResult {
        int32_t resCode = 0;
        std::string resMessage;
    };
private:
    static void ConvertInstallResult(std::shared_ptr<BundleInstallResult> installResult);
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif /* BUNDLE_MGR_H */
