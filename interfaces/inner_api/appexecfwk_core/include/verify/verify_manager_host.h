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

#ifndef FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_INNERKITS_APPEXECFWK_CORE_INCLUDE_VERIFY_VERIFY_MANAGER_HOST_H
#define FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_INNERKITS_APPEXECFWK_CORE_INCLUDE_VERIFY_VERIFY_MANAGER_HOST_H

#include "iremote_stub.h"
#include "nocopyable.h"
#include "verify_manager_interface.h"

namespace OHOS {
namespace AppExecFwk {
class VerifyManagerHost : public IRemoteStub<IVerifyManager> {
public:
    VerifyManagerHost();
    virtual ~VerifyManagerHost();

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    ErrCode HandleVerify(MessageParcel& data, MessageParcel& reply);
    ErrCode HandleCopyFiles(MessageParcel& data, MessageParcel& reply);
    ErrCode HandleCreateFd(MessageParcel& data, MessageParcel& reply);

    DISALLOW_COPY_AND_MOVE(VerifyManagerHost);
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_INNERKITS_APPEXECFWK_CORE_INCLUDE_VERIFY_VERIFY_MANAGER_HOST_H