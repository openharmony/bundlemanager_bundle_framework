/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "bundle_pack_info.h"

#include "bundle_constants.h"
#include "json_util.h"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
// version
const char* PACK_SUMMARY_APP_VERSION_CODE = "code";
const char* PACK_SUMMARY_APP_VERSION_NAME = "name";
const char* PACK_SUMMARY_APP_VERSION_MIN_COMPATIBLE_VERSION_CODE = "minCompatibleVersionCode";

// app
const char* PACK_SUMMARY_APP_BUNDLE_VERSION = "version";

// module ablities
const char* PACK_SUMMARY_MODULE_ABILITY_NAME = "name";
const char* PACK_SUMMARY_MODULE_ABILITY_LABEL = "label";
const char* PACK_SUMMARY_MODULE_ABILITY_VISIBLE = "visible";
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS = "forms";

// module extensionAbilities
const char* PACK_SUMMARY_MODULE_EXTENSION_ABILITIES_NAME = "name";
const char* PACK_SUMMARY_MODULE_EXTENSION_ABILITIES_FORMS = "forms";

// module ablities forms
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS_NAME = "name";
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS_TYPE = "type";
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS_UPDATE_ENABLED = "updateEnabled";
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS_SCHEDULE_DUPDATETIME = "scheduledUpdateTime";
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS_MULTI_SCHEDULE_DUPDATETIME = "multiScheduledUpdateTime";
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS_UPDATE_DURATION = "updateDuration";
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS_SUPPORT_DIMENSIONS = "supportDimensions";
const char* PACK_SUMMARY_MODULE_ABILITY_FORMS_DEFAULT_DIMENSION = "defaultDimension";

// module distro
const char* PACK_SUMMARY_MODULE_DISTRO_MODULE_TYPE = "moduleType";
const char* PACK_SUMMARY_MODULE_DISTRO_INSTALLATION_FREE = "installationFree";
const char* PACK_SUMMARY_MODULE_DISTRO_DELIVERY_WITH_INSTALL = "deliveryWithInstall";

// module apiversion
const char* PACK_SUMMARY_MODULE_API_VERSION_COMPATIBLE = "compatible";
const char* PACK_SUMMARY_MODULE_API_VERSION_RELEASE_TYPE = "releaseType";
const char* PACK_SUMMARY_MODULE_API_VERSION_TARGET = "target";

// package module
const char* PACK_SUMMARY_MODULE_MAIN_ABILITY = "mainAbility";
const char* PACK_SUMMARY_MODULE_DEVICE_TYPE = "deviceType";
const char* PACK_SUMMARY_MODULE_ABILITIES = "abilities";
const char* PACK_SUMMARY_MODULE_DISTRO = "distro";
const char* PACK_SUMMARY_MODULE_API_VERSION = "apiVersion";
const char* PACK_SUMMARY_MODULE_EXTENSION_ABILITIES = "extensionAbilities";

// summary
const char* PACK_SUMMARY_APP = "app";
const char* PACK_SUMMARY_MODULE = "modules";

// packages
const char* PACK_PACKAGES_DEVICE_TYPE = "deviceType";
const char* PACK_PACKAGES_MODULE_TYPE = "moduleType";
const char* PACK_PACKAGES_DELIVERY_WITH_INSTALL = "deliveryWithInstall";
const char* PACK_PACKAGES_NAME = "name";

// bundle pack info
const char* BUNDLE_PACK_INFO_SUMMARY = "summary";
const char* BUNDLE_PACK_INFO_PACKAGES = "packages";


} // namespace

void to_json(nlohmann::json &jsonObject, const Version &version)
{
    jsonObject = nlohmann::json {
        {PACK_SUMMARY_APP_VERSION_CODE, version.code},
        {PACK_SUMMARY_APP_VERSION_NAME, version.name},
        {PACK_SUMMARY_APP_VERSION_MIN_COMPATIBLE_VERSION_CODE, version.minCompatibleVersionCode}
    };
}

void from_json(const nlohmann::json &jsonObject, Version &version)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_APP_VERSION_CODE,
        version.code,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_APP_VERSION_NAME,
        version.name,
        false,
        parseResult);
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_APP_VERSION_MIN_COMPATIBLE_VERSION_CODE,
        version.minCompatibleVersionCode,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read version error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const PackageApp &app)
{
    jsonObject = nlohmann::json {
        {Constants::BUNDLE_NAME, app.bundleName},
        {PACK_SUMMARY_APP_BUNDLE_VERSION, app.version}
    };
}

void from_json(const nlohmann::json &jsonObject, PackageApp &app)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        Constants::BUNDLE_NAME,
        app.bundleName,
        false,
        parseResult);
    GetValueIfFindKey<Version>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_APP_BUNDLE_VERSION,
        app.version,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read package app error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const ExtensionAbilities &extensionAbilities)
{
    jsonObject = nlohmann::json {
        {PACK_SUMMARY_MODULE_EXTENSION_ABILITIES_NAME, extensionAbilities.name},
        {PACK_SUMMARY_MODULE_EXTENSION_ABILITIES_FORMS, extensionAbilities.forms}
    };
}

void from_json(const nlohmann::json &jsonObject, ExtensionAbilities &extensionAbilities)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_EXTENSION_ABILITIES_NAME,
        extensionAbilities.name,
        false,
        parseResult);
    GetValueIfFindKey<std::vector<AbilityFormInfo>>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_EXTENSION_ABILITIES_FORMS,
        extensionAbilities.forms,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    if (parseResult != ERR_OK) {
        APP_LOGE("read abilityinfo error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const ModuleAbilityInfo &abilityinfo)
{
    jsonObject = nlohmann::json {
        {PACK_SUMMARY_MODULE_ABILITY_NAME, abilityinfo.name},
        {PACK_SUMMARY_MODULE_ABILITY_LABEL, abilityinfo.label},
        {PACK_SUMMARY_MODULE_ABILITY_VISIBLE, abilityinfo.visible},
        {PACK_SUMMARY_MODULE_ABILITY_FORMS, abilityinfo.forms}
    };
}

void from_json(const nlohmann::json &jsonObject, ModuleAbilityInfo &abilityinfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_ABILITY_NAME,
        abilityinfo.name,
        false,
        parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_ABILITY_LABEL,
        abilityinfo.label,
        false,
        parseResult);
    BMSJsonUtil::GetBoolValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_ABILITY_VISIBLE,
        abilityinfo.visible,
        false,
        parseResult);
    GetValueIfFindKey<std::vector<AbilityFormInfo>>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_ABILITY_FORMS,
        abilityinfo.forms,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    if (parseResult != ERR_OK) {
        APP_LOGE("read abilityinfo error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const AbilityFormInfo &abilityFormInfo)
{
    jsonObject = nlohmann::json {
        {PACK_SUMMARY_MODULE_ABILITY_FORMS_NAME, abilityFormInfo.name},
        {PACK_SUMMARY_MODULE_ABILITY_FORMS_TYPE, abilityFormInfo.type},
        {PACK_SUMMARY_MODULE_ABILITY_FORMS_UPDATE_ENABLED, abilityFormInfo.updateEnabled},
        {PACK_SUMMARY_MODULE_ABILITY_FORMS_SCHEDULE_DUPDATETIME, abilityFormInfo.scheduledUpdateTime},
        {PACK_SUMMARY_MODULE_ABILITY_FORMS_MULTI_SCHEDULE_DUPDATETIME, abilityFormInfo.multiScheduledUpdateTime},
        {PACK_SUMMARY_MODULE_ABILITY_FORMS_UPDATE_DURATION, abilityFormInfo.updateDuration},
        {PACK_SUMMARY_MODULE_ABILITY_FORMS_SUPPORT_DIMENSIONS, abilityFormInfo.supportDimensions},
        {PACK_SUMMARY_MODULE_ABILITY_FORMS_DEFAULT_DIMENSION, abilityFormInfo.defaultDimension}};
}

void from_json(const nlohmann::json &jsonObject, AbilityFormInfo &abilityFormInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, PACK_SUMMARY_MODULE_ABILITY_FORMS_NAME,
        abilityFormInfo.name, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, PACK_SUMMARY_MODULE_ABILITY_FORMS_TYPE,
        abilityFormInfo.type, false, parseResult);
    BMSJsonUtil::GetBoolValueIfFindKey(jsonObject, jsonObjectEnd, PACK_SUMMARY_MODULE_ABILITY_FORMS_UPDATE_ENABLED,
        abilityFormInfo.updateEnabled, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, PACK_SUMMARY_MODULE_ABILITY_FORMS_SCHEDULE_DUPDATETIME,
        abilityFormInfo.scheduledUpdateTime, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd,
        PACK_SUMMARY_MODULE_ABILITY_FORMS_MULTI_SCHEDULE_DUPDATETIME,
        abilityFormInfo.multiScheduledUpdateTime, false, parseResult);
    GetValueIfFindKey<uint32_t>(jsonObject, jsonObjectEnd, PACK_SUMMARY_MODULE_ABILITY_FORMS_UPDATE_DURATION,
        abilityFormInfo.updateDuration, JsonType::NUMBER, false, parseResult, ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject, jsonObjectEnd,
        PACK_SUMMARY_MODULE_ABILITY_FORMS_SUPPORT_DIMENSIONS, abilityFormInfo.supportDimensions, JsonType::ARRAY, false,
        parseResult, ArrayType::STRING);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, PACK_SUMMARY_MODULE_ABILITY_FORMS_DEFAULT_DIMENSION,
        abilityFormInfo.defaultDimension, false, parseResult);
    if (parseResult != ERR_OK) {
        APP_LOGE("read BundleConfigInfo error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const ModuleDistro &distro)
{
    jsonObject = nlohmann::json {
        {PACK_SUMMARY_MODULE_DISTRO_MODULE_TYPE, distro.moduleType},
        {Constants::MODULE_NAME, distro.moduleName},
        {PACK_SUMMARY_MODULE_DISTRO_INSTALLATION_FREE, distro.installationFree},
        {PACK_SUMMARY_MODULE_DISTRO_DELIVERY_WITH_INSTALL, distro.deliveryWithInstall}
    };
}

void from_json(const nlohmann::json &jsonObject, ModuleDistro &distro)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_DISTRO_MODULE_TYPE,
        distro.moduleType,
        false,
        parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        Constants::MODULE_NAME,
        distro.moduleName,
        false,
        parseResult);
    BMSJsonUtil::GetBoolValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_DISTRO_INSTALLATION_FREE,
        distro.installationFree,
        false,
        parseResult);
    BMSJsonUtil::GetBoolValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_DISTRO_DELIVERY_WITH_INSTALL,
        distro.deliveryWithInstall,
        false,
        parseResult);
    if (parseResult != ERR_OK) {
        APP_LOGE("read abilityinfo error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const ApiVersion &apiVersion)
{
    jsonObject = nlohmann::json {
        {PACK_SUMMARY_MODULE_API_VERSION_COMPATIBLE, apiVersion.compatible},
        {PACK_SUMMARY_MODULE_API_VERSION_RELEASE_TYPE, apiVersion.releaseType},
        {PACK_SUMMARY_MODULE_API_VERSION_TARGET, apiVersion.target}
    };
}

void from_json(const nlohmann::json &jsonObject, ApiVersion &apiVersion)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_API_VERSION_COMPATIBLE,
        apiVersion.compatible,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_API_VERSION_RELEASE_TYPE,
        apiVersion.releaseType,
        false,
        parseResult);
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_API_VERSION_TARGET,
        apiVersion.target,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read abilityinfo error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const PackageModule &packageModule)
{
    jsonObject = nlohmann::json {
        {PACK_SUMMARY_MODULE_MAIN_ABILITY, packageModule.mainAbility},
        {PACK_SUMMARY_MODULE_DEVICE_TYPE, packageModule.deviceType},
        {PACK_SUMMARY_MODULE_ABILITIES, packageModule.abilities},
        {PACK_SUMMARY_MODULE_EXTENSION_ABILITIES, packageModule.extensionAbilities},
        {PACK_SUMMARY_MODULE_DISTRO, packageModule.distro},
        {PACK_SUMMARY_MODULE_API_VERSION, packageModule.apiVersion}
    };
}

void from_json(const nlohmann::json &jsonObject, PackageModule &packageModule)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_MAIN_ABILITY,
        packageModule.mainAbility,
        false,
        parseResult);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_DEVICE_TYPE,
        packageModule.deviceType,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<ModuleAbilityInfo>>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_ABILITIES,
        packageModule.abilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<ExtensionAbilities>>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_EXTENSION_ABILITIES,
        packageModule.extensionAbilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<ModuleDistro>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_DISTRO,
        packageModule.distro,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<ApiVersion>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE_API_VERSION,
        packageModule.apiVersion,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read abilityinfo error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const Summary &summary)
{
    jsonObject = nlohmann::json {
        {PACK_SUMMARY_APP, summary.app},
        {PACK_SUMMARY_MODULE, summary.modules}
    };
}

void from_json(const nlohmann::json &jsonObject, Summary &summary)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<PackageApp>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_APP,
        summary.app,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<PackageModule>>(jsonObject,
        jsonObjectEnd,
        PACK_SUMMARY_MODULE,
        summary.modules,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    if (parseResult != ERR_OK) {
        APP_LOGE("read abilityinfo error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const Packages &packages)
{
    jsonObject = nlohmann::json {
        {PACK_PACKAGES_DEVICE_TYPE, packages.deviceType},
        {PACK_PACKAGES_MODULE_TYPE, packages.moduleType},
        {PACK_PACKAGES_DELIVERY_WITH_INSTALL, packages.deliveryWithInstall},
        {PACK_PACKAGES_NAME, packages.name}
    };
}

void from_json(const nlohmann::json &jsonObject, Packages &packages)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        PACK_PACKAGES_DEVICE_TYPE,
        packages.deviceType,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_PACKAGES_MODULE_TYPE,
        packages.moduleType,
        false,
        parseResult);
    BMSJsonUtil::GetBoolValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_PACKAGES_DELIVERY_WITH_INSTALL,
        packages.deliveryWithInstall,
        false,
        parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject,
        jsonObjectEnd,
        PACK_PACKAGES_NAME,
        packages.name,
        false,
        parseResult);
    if (parseResult != ERR_OK) {
        APP_LOGE("read abilityinfo error %{public}d", parseResult);
    }
}

void to_json(nlohmann::json &jsonObject, const BundlePackInfo &bundlePackInfo)
{
    jsonObject = nlohmann::json {
        {BUNDLE_PACK_INFO_SUMMARY, bundlePackInfo.summary},
        {BUNDLE_PACK_INFO_PACKAGES, bundlePackInfo.packages}
    };
}

void from_json(const nlohmann::json &jsonObject, BundlePackInfo &bundlePackInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<Summary>(jsonObject,
        jsonObjectEnd,
        BUNDLE_PACK_INFO_SUMMARY,
        bundlePackInfo.summary,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Packages>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_PACK_INFO_PACKAGES,
        bundlePackInfo.packages,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    if (parseResult != ERR_OK) {
        APP_LOGE("read abilityinfo error %{public}d", parseResult);
    }
}

bool BundlePackInfo::ReadFromParcel(Parcel &parcel)
{
    MessageParcel *messageParcel = reinterpret_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        APP_LOGE("Type conversion failed");
        return false;
    }
    uint32_t length = messageParcel->ReadUint32();
    if (length == 0) {
        APP_LOGE("Invalid data length");
        return false;
    }
    const char *data = reinterpret_cast<const char *>(messageParcel->ReadRawData(length));
    if (!data) {
        APP_LOGE("Fail read raw length = %{public}d", length);
        return false;
    }
    nlohmann::json jsonObject = nlohmann::json::parse(data, nullptr, false);
    if (jsonObject.is_discarded()) {
        APP_LOGE("failed to parse BundleInfo");
        return false;
    }
    *this = jsonObject.get<BundlePackInfo>();
    return true;
}

bool BundlePackInfo::Marshalling(Parcel &parcel) const
{
    MessageParcel *messageParcel = reinterpret_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        APP_LOGE("Type conversion failed");
        return false;
    }
    nlohmann::json jsonObject = *this;
    std::string str = jsonObject.dump();
    if (!messageParcel->WriteUint32(str.size() + 1)) {
        APP_LOGE("Failed to write;data size");
        return false;
    }
    if (!messageParcel->WriteRawData(str.c_str(), str.size() + 1)) {
        APP_LOGE("Failed to write data");
        return false;
    }
    return true;
}

BundlePackInfo *BundlePackInfo::Unmarshalling(Parcel &parcel)
{
    BundlePackInfo *info = new (std::nothrow) BundlePackInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}
} // AppExecFwk
} // OHOS
