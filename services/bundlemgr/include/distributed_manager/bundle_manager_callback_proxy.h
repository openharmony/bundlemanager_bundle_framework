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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MANAGER_BUNDLE_MANAGER_CALLBACK_PROXY_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MANAGER_BUNDLE_MANAGER_CALLBACK_PROXY_H

#include <string>

#include "i_bundle_manager_callback.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * interface for BundleManagerCallbackProxy.
 */
class BundleManagerCallbackProxy : public IRemoteProxy<IBundleManagerCallback> {
public:
    explicit BundleManagerCallbackProxy(const sptr<IRemoteObject> &impl);
    /**
     * @brief Will be execute when free query rpc id is complete.
     * @param RpcIdResult the json of InstallResult
     */
    virtual int32_t OnQueryRpcIdFinished(const std::string &RpcIdResult) override;

private:
    static inline BrokerDelegator<BundleManagerCallbackProxy> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MANAGER_BUNDLE_MANAGER_CALLBACK_PROXY_H
