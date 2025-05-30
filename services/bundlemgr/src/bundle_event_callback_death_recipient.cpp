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

#include "bundle_event_callback_death_recipient.h"

#include "bundle_mgr_service.h"

namespace OHOS {
namespace AppExecFwk {
BundleEventCallbackDeathRecipient::BundleEventCallbackDeathRecipient()
{
    APP_LOGD("create BundleEventCallbackDeathRecipient");
}

BundleEventCallbackDeathRecipient::~BundleEventCallbackDeathRecipient()
{
    APP_LOGD("destroy BundleEventCallbackDeathRecipient");
}

void BundleEventCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    APP_LOGD("bundle event service died, remove the proxy object");
    sptr<IBundleEventCallback> callback = iface_cast<IBundleEventCallback>(remote.promote());
    if (callback == nullptr) {
        APP_LOGE("callback is nullptr");
        return;
    }
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return;
    }
    dataMgr->UnregisterBundleEventCallback(callback);
}
}  // namespace AppExecFwk
}  // namespace OHOS
