/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_STATUS_CALLBACK_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_STATUS_CALLBACK_H

#include "bundle_connect_ability_mgr.h"
#include "iremote_object.h"
#include "iremote_stub.h"
#include "service_center_status_callback_stub.h"

namespace OHOS {
namespace AppExecFwk {
class BundleConnectAbilityMgr;
/**
 * @class ServiceCenterStatusCallback
 * ServiceCenterStatusCallback.
 */
class ServiceCenterStatusCallback : public ServiceCenterStatusCallbackStub {
public:
    explicit ServiceCenterStatusCallback(const std::weak_ptr<BundleConnectAbilityMgr> &server);
    virtual ~ServiceCenterStatusCallback() = default;
    /**
     * @brief Free install is complete.
     * @param installResult Install result
     */
    int32_t OnInstallFinished(std::string installResult);

    /**
     * @brief delayed_heartbeat process of free install.
     * @param installResult Install result
     */
    int32_t OnDelayedHeartbeat(std::string installResult);

    /**
     * @brief Executed when a service callback
     * @param installResultStr The json string of InstallResult
     */
    int32_t OnServiceCenterReceived(std::string installResultStr);
private:
    std::weak_ptr<BundleConnectAbilityMgr> server_;
};
}  //  namespace AppExecFwk
}  //  namespace OHOS
#endif  //  FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_STATUS_CALLBACK_H
