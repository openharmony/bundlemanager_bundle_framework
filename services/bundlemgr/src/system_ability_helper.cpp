/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "system_ability_helper.h"

#ifdef ABILITY_RUNTIME_ENABLE
#include "ability_manager_interface.h"
#endif

#include "app_log_wrapper.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
#ifdef ABILITY_RUNTIME_ENABLE
    constexpr const char* KILL_REASON = "Kill Reason: UpgradeApp";
#endif
}
sptr<IRemoteObject> SystemAbilityHelper::GetSystemAbility(const int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        APP_LOGE("fail get system ability manager to get %{public}d proxy", systemAbilityId);
        return nullptr;
    }
    return systemAbilityMgr->GetSystemAbility(systemAbilityId);
}

bool SystemAbilityHelper::AddSystemAbility(const int32_t systemAbilityId, const sptr<IRemoteObject> &systemAbility)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr && (systemAbilityMgr->AddSystemAbility(systemAbilityId, systemAbility) == 0)) {
        return true;
    }
    APP_LOGE("fail register %{public}d to system ability manager", systemAbilityId);
    return false;
}

bool SystemAbilityHelper::RemoveSystemAbility(const int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr && (systemAbilityMgr->RemoveSystemAbility(systemAbilityId) == 0)) {
        return true;
    }
    APP_LOGE("fail remove %{public}d from system ability manager", systemAbilityId);
    return false;
}

int SystemAbilityHelper::UninstallApp(const std::string &bundleName, int32_t uid, int32_t appIndex)
{
#ifdef ABILITY_RUNTIME_ENABLE
    sptr<AAFwk::IAbilityManager> abilityMgrProxy =
        iface_cast<AAFwk::IAbilityManager>(SystemAbilityHelper::GetSystemAbility(ABILITY_MGR_SERVICE_ID));
    if (abilityMgrProxy == nullptr) {
        APP_LOGE("fail to find the app mgr service to kill application");
        return -1;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = abilityMgrProxy->UninstallApp(bundleName, uid, appIndex);
    IPCSkeleton::SetCallingIdentity(identity);
    return ret;
#else
    return 0;
#endif
}

int SystemAbilityHelper::UpgradeApp(const std::string &bundleName, int32_t uid, int32_t appIndex)
{
#ifdef ABILITY_RUNTIME_ENABLE
    sptr<AAFwk::IAbilityManager> abilityMgrProxy =
        iface_cast<AAFwk::IAbilityManager>(SystemAbilityHelper::GetSystemAbility(ABILITY_MGR_SERVICE_ID));
    if (abilityMgrProxy == nullptr) {
        APP_LOGE("fail to find the app mgr service to kill application");
        return -1;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = abilityMgrProxy->UpgradeApp(bundleName, uid, KILL_REASON, appIndex);
    IPCSkeleton::SetCallingIdentity(identity);
    return ret;
#else
    return 0;
#endif
}

bool SystemAbilityHelper::UnloadSystemAbility(const int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr != nullptr && (systemAbilityMgr->UnloadSystemAbility(systemAbilityId) == 0)) {
        return true;
    }
    APP_LOGE("fail unload %{public}d from system ability manager", systemAbilityId);
    return false;
}

#ifdef ABILITY_RUNTIME_ENABLE
bool SystemAbilityHelper::IsAppRunning(const sptr<IAppMgr> appMgrProxy,
    const std::string &bundleName, int32_t appCloneIndex)
{
    bool running = false;
    if (appMgrProxy == nullptr) {
        APP_LOGE("fail to find the app mgr service to check app is running");
        return running;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    int32_t result = appMgrProxy->IsAppRunning(bundleName, appCloneIndex, running);
    IPCSkeleton::SetCallingIdentity(identity);
    if (result != 0) {
        APP_LOGW("IsAppRunning failed");
    }
    return running;
}
#endif
}  // namespace AppExecFwk
}  // namespace OHOS