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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_USER_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_USER_INFO_H

#include <vector>

#include "parcel.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {
struct BundleUserInfo : public Parcelable {
    // Indicates whether the bundle is disabled.
    bool enabled = true;
    int32_t userId = Constants::INVALID_USERID;
    std::string setEnabledCaller;

    // disabled abilities of the user.
    std::vector<std::string> disabledAbilities;

    // overlay module state
    // element is moduleName_state
    std::vector<std::string> overlayModulesState;

    bool IsInitialState() const;
    void Reset();
    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static BundleUserInfo *Unmarshalling(Parcel &parcel);
    void Dump(const std::string &prefix, int fd);
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_USER_INFO_H