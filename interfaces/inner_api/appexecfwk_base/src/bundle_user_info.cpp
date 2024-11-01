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

#include "bundle_user_info.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const char* BUNDLE_USER_INFO_USER_ID = "userId";
const char* BUNDLE_USER_INFO_ENABLE = "enabled";
const char* BUNDLE_USER_INFO_DISABLE_ABILITIES = "disabledAbilities";
const char* BUNDLE_USER_INFO_OVERLAY_STATE = "overlayState";
const char* BUNDLE_USER_INFO_SET_ENABLED_CALLER = "setEnabledCaller";
} // namespace

bool BundleUserInfo::ReadFromParcel(Parcel &parcel)
{
    userId = parcel.ReadInt32();
    enabled = parcel.ReadBool();
    int32_t disabledAbilitiesSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, disabledAbilitiesSize);
    CONTAINER_SECURITY_VERIFY(parcel, disabledAbilitiesSize, &disabledAbilities);
    for (int32_t i = 0; i < disabledAbilitiesSize; i++) {
        disabledAbilities.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t overlayStateSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, overlayStateSize);
    CONTAINER_SECURITY_VERIFY(parcel, overlayStateSize, &overlayModulesState);
    for (int32_t i = 0; i < overlayStateSize; i++) {
        overlayModulesState.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }
    setEnabledCaller = Str16ToStr8(parcel.ReadString16());
    return true;
}

BundleUserInfo *BundleUserInfo::Unmarshalling(Parcel &parcel)
{
    BundleUserInfo *info = new (std::nothrow) BundleUserInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }

    return info;
}

bool BundleUserInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, userId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enabled);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, disabledAbilities.size());
    for (auto &disabledAbility : disabledAbilities) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(disabledAbility));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, overlayModulesState.size());
    for (const auto &overlayModuleState : overlayModulesState) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(overlayModuleState));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(setEnabledCaller));
    return true;
}

void BundleUserInfo::Dump(const std::string &prefix, int fd)
{
    APP_LOGI("called dump BundleUserInfo");
    if (fd < 0) {
        APP_LOGE("dump BundleUserInfo fd error");
        return;
    }
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        APP_LOGE("BundleUserInfo fcntl error %{public}s", strerror(errno));
        return;
    }
    uint uflags = static_cast<uint>(flags);
    uflags &= O_ACCMODE;
    if ((uflags == O_WRONLY) || (uflags == O_RDWR)) {
        nlohmann::json jsonObject = *this;
        std::string result;
        result.append(prefix);
        result.append(jsonObject.dump(Constants::DUMP_INDENT));
        int ret = TEMP_FAILURE_RETRY(write(fd, result.c_str(), result.size()));
        if (ret < 0) {
            APP_LOGE("BundleUserInfo write error %{public}s", strerror(errno));
        }
    }
}

bool BundleUserInfo::IsInitialState() const
{
    return enabled && disabledAbilities.empty();
}

void BundleUserInfo::Reset()
{
    enabled = true;
    disabledAbilities.clear();
}

void to_json(nlohmann::json& jsonObject, const BundleUserInfo& bundleUserInfo)
{
    jsonObject = nlohmann::json {
        {BUNDLE_USER_INFO_USER_ID, bundleUserInfo.userId},
        {BUNDLE_USER_INFO_ENABLE, bundleUserInfo.enabled},
        {BUNDLE_USER_INFO_DISABLE_ABILITIES, bundleUserInfo.disabledAbilities},
        {BUNDLE_USER_INFO_OVERLAY_STATE, bundleUserInfo.overlayModulesState},
        {BUNDLE_USER_INFO_SET_ENABLED_CALLER, bundleUserInfo.setEnabledCaller},
    };
}

void from_json(const nlohmann::json& jsonObject, BundleUserInfo& bundleUserInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_USER_INFO_USER_ID,
        bundleUserInfo.userId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    BMSJsonUtil::GetBoolValueIfFindKey(jsonObject,
        jsonObjectEnd,
        BUNDLE_USER_INFO_ENABLE,
        bundleUserInfo.enabled,
        false,
        parseResult);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_USER_INFO_DISABLE_ABILITIES,
        bundleUserInfo.disabledAbilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_USER_INFO_OVERLAY_STATE,
        bundleUserInfo.overlayModulesState,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        BUNDLE_USER_INFO_SET_ENABLED_CALLER,
        bundleUserInfo.setEnabledCaller,
        false,
        parseResult);
    if (parseResult != ERR_OK) {
        APP_LOGE("module bundleUserInfo jsonObject error : %{public}d", parseResult);
    }
}
} // namespace AppExecFwk
} // namespace OHOS