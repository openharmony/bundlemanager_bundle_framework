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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DRIVER_INSTALL_EXT_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DRIVER_INSTALL_EXT_H
#include <string>
#include <vector>

#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {

using RedirectPathFunc = void (*)(std::string &);
using GetDriverExtPathsFunc = void (*)(std::vector<std::string> &);

class DriverInstallExtHandler : public DelayedSingleton<DriverInstallExtHandler> {
public:
    DriverInstallExtHandler();
    ~DriverInstallExtHandler();

    void RedirectDriverInstallExtPath(std::string &path);
    void GetDriverExecuteExtPaths(std::vector<std::string> &paths);

private:
    bool OpenDriverInstallHandler();
    bool IsExtSpaceEnable();
    void ClearSymbols();

    void *driverInstallerHandle_ = nullptr;

    RedirectPathFunc redirectDriverInstallExtPath_ = nullptr;
    GetDriverExtPathsFunc getDriverExecuteExtPathsFunc_ = nullptr;
};
}  // AppExecFwk
}  // OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DRIVER_INSTALL_EXT_H