/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "bundle_event_callback_host.h"

#include "app_log_wrapper.h"
#include "bundle_framework_core_ipc_interface_code.h"
#include "bundle_memory_guard.h"
#include "ipc_types.h"

namespace OHOS {
namespace AppExecFwk {
BundleEventCallbackHost::BundleEventCallbackHost()
{
    APP_LOGD("create BundleEventCallbackHost");
}

BundleEventCallbackHost::~BundleEventCallbackHost()
{
    APP_LOGD("destroy BundleEventCallbackHost");
}

int BundleEventCallbackHost::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    BundleMemoryGuard memoryGuard;
    APP_LOGD("BundleEventCallbackHost OnRemoteRequest, code : %{public}u", code);
    std::u16string descriptor = BundleEventCallbackHost::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        APP_LOGE("verify descriptor failed");
        return OBJECT_NULL;
    }

    switch (code) {
        case static_cast<uint32_t>(BundleEventCallbackInterfaceCode::ON_RECEIVE_EVENT): {
            std::unique_ptr<EventFwk::CommonEventData> dataPtr(data.ReadParcelable<EventFwk::CommonEventData>());
            if (dataPtr == nullptr) {
                APP_LOGE("get CommonEventData failed");
                return OBJECT_NULL;
            }
            OnReceiveEvent(*(dataPtr.get()));
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
}  // namespace AppExecFwk
}  // namespace OHOS
