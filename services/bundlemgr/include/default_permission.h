/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DEFAULT_PERMISSION_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DEFAULT_PERMISSION_H

#include <string>
#include <vector>

namespace OHOS {
namespace AppExecFwk {
struct PermissionInfo {
    bool userCancellable = false;
    std::string name;
};

struct DefaultPermission {
    std::string bundleName;
    std::vector<PermissionInfo> grantPermission;
    std::vector<std::string> appSignature;
    bool operator < (const DefaultPermission &defaultPermission) const
    {
        return bundleName < defaultPermission.bundleName;
    }
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DEFAULT_PERMISSION_H