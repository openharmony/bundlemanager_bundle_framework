/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_CHECK_ENCRYPTION_PARAM_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_CHECK_ENCRYPTION_PARAM_H

#include <string>
#include "message_parcel.h"

namespace OHOS {
namespace AppExecFwk {
enum class InstallBundleType : uint8_t {
    HAP = 0,
    INTER_APP_HSP = 1,
    CROSS_APP_HSP = 2,
    HQF = 3,
};

struct CodeCryptoHapInfo {
    std::string appIdentifier;
    uint32_t versionCode;
    InstallBundleType type;
    bool libCompressed;
};

struct CheckEncryptionParam : public Parcelable {
    bool isCompressNativeLibrary = false;
    InstallBundleType installBundleType = InstallBundleType::HAP;
    int32_t bundleId = -1;
    std::string modulePath;
    std::string cpuAbi;
    std::string targetSoPath;
    std::string appIdentifier;
    uint32_t versionCode;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static CheckEncryptionParam *Unmarshalling(Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_CHECK_ENCRYPTION_PARAM_H
