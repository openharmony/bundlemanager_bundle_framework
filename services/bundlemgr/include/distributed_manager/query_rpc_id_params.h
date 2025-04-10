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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MANAGER_QUERY_RPC_ID_PARAMS_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MANAGER_QUERY_RPC_ID_PARAMS_H

#include <string>

#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
enum ErrorCode : int8_t {
    NO_ERROR = 0,
    CONNECT_FAILED = -100,
    SEND_REQUEST_FAILED = -101,
    WAITING_TIMEOUT = -102,
    GET_DEVICE_PROFILE_FAILED = -103,
    DECODE_SYS_CAP_FAILED = -104,
    COMPARE_PC_ID_FAILED = -105,
    ERR_BMS_DEVICE_INFO_MANAGER_ENABLE_DISABLED = -106,
};

struct QueryRpcIdParams : public virtual RefBase {
    int32_t missionId;
    uint32_t versionCode;
    sptr<IRemoteObject> callback;
    OHOS::AAFwk::Want want;
};
}
}
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MANAGER_QUERY_RPC_ID_PARAMS_H