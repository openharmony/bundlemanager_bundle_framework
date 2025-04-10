/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_I_BUNDLE_MGR_EXT_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_I_BUNDLE_MGR_EXT_H

#include <vector>

#include "appexecfwk_errors.h"
#include "iremote_broker.h"

namespace OHOS {
namespace AppExecFwk {
class IBundleMgrExt : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.bundleManager.ext");

    /**
     * @brief Obtains the bundleNames associated with the given UID.
     * @param uid Indicates the uid.
     * @param bundleNames Indicates the bundleNames.
     * @return Returns ERR_OK if execute success; returns errCode otherwise.
     */
    virtual ErrCode GetBundleNamesForUidExt(const int32_t uid, std::vector<std::string> &bundleNames)
    {
        return ERR_BUNDLE_MANAGER_INTERNAL_ERROR;
    }
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_I_BUNDLE_MGR_EXT_H
