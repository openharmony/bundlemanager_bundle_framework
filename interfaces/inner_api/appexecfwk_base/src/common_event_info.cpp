/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "common_event_info.h"

#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include "json_serializer.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"
#include "parcel_macro.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "json_util.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const char* JSON_KEY_NAME = "name";
const char* JSON_KEY_UID = "uid";
const char* JSON_KEY_PERMISSION = "permission";
const char* JSON_KEY_DATA = "data";
const char* JSON_KEY_TYPE = "type";
const char* JSON_KEY_EVENTS = "events";
}  // namespace

bool CommonEventInfo::ReadFromParcel(Parcel &parcel)
{
    name = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    uid = parcel.ReadInt32();
    permission = Str16ToStr8(parcel.ReadString16());

    int32_t typeSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, typeSize);
    CONTAINER_SECURITY_VERIFY(parcel, typeSize, &data);
    for (int32_t i = 0; i < typeSize; i++) {
        data.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }
    int32_t dataSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, dataSize);
    CONTAINER_SECURITY_VERIFY(parcel, dataSize, &type);
    for (int32_t i = 0; i < dataSize; i++) {
        type.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }
    int32_t eventsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, eventsSize);
    CONTAINER_SECURITY_VERIFY(parcel, eventsSize, &events);
    for (int32_t i = 0; i < eventsSize; i++) {
        events.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }
    return true;
}

CommonEventInfo *CommonEventInfo::Unmarshalling(Parcel &parcel)
{
    std::unique_ptr<CommonEventInfo> info = std::make_unique<CommonEventInfo>();
    if (!info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        info = nullptr;
    }
    return info.release();
}

bool CommonEventInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, uid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(permission));

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, data.size());
    for (auto &dataSingle : data) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(dataSingle));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, type.size());
    for (auto &typeSingle : type) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(typeSingle));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, events.size());
    for (auto &event : events) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(event));
    }
    return true;
}

void to_json(nlohmann::json &jsonObject, const CommonEventInfo &commonEvent)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_NAME, commonEvent.name},
        {Constants::BUNDLE_NAME, commonEvent.bundleName},
        {JSON_KEY_UID, commonEvent.uid},
        {JSON_KEY_PERMISSION, commonEvent.permission},
        {JSON_KEY_DATA, commonEvent.data},
        {JSON_KEY_TYPE, commonEvent.type},
        {JSON_KEY_EVENTS, commonEvent.events}
        };
}

void from_json(const nlohmann::json &jsonObject, CommonEventInfo &commonEvent)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        JSON_KEY_NAME,
        commonEvent.name,
        false,
        parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        Constants::BUNDLE_NAME,
        commonEvent.bundleName,
        false,
        parseResult);
    GetValueIfFindKey<int>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_UID,
        commonEvent.uid,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        JSON_KEY_PERMISSION,
        commonEvent.permission,
        false,
        parseResult);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_DATA,
        commonEvent.data,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TYPE,
        commonEvent.type,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_EVENTS,
        commonEvent.events,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    if (parseResult != ERR_OK) {
        APP_LOGE("module commonEvent jsonObject error : %{public}d", parseResult);
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
