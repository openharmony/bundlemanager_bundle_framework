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

#ifndef FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_INNERKITS_APPEXECFWK_CORE_INCLUDE_VERIFY_VERIFY_MANAGER_PROXY_H
#define FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_INNERKITS_APPEXECFWK_CORE_INCLUDE_VERIFY_VERIFY_MANAGER_PROXY_H

#include "bundle_framework_core_ipc_interface_code.h"
#include "iremote_proxy.h"
#include "verify_manager_interface.h"

namespace OHOS {
namespace AppExecFwk {
class VerifyManagerProxy : public IRemoteProxy<IVerifyManager> {
public:
    explicit VerifyManagerProxy(const sptr<IRemoteObject> &object);
    virtual ~VerifyManagerProxy();

    virtual ErrCode Verify(const std::vector<std::string> &abcPaths,
        const std::vector<std::string> &abcNames, bool flag) override;

    virtual ErrCode RemoveFiles(const std::vector<std::string> &abcPaths) override;

private:
    virtual ErrCode CopyFiles(
        const std::vector<std::string> &sourceFiles, std::vector<std::string> &destFiles) override;
    virtual ErrCode CreateFd(const std::string &fileName, int32_t &fd, std::string &path) override;
    bool SendRequest(VerifyManagerInterfaceCode code, MessageParcel &data, MessageParcel &reply);

    static inline BrokerDelegator<VerifyManagerProxy> delegator_;
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_INNERKITS_APPEXECFWK_CORE_INCLUDE_VERIFY_VERIFY_MANAGER_PROXY_H