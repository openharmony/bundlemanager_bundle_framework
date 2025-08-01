/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "app_log_wrapper.h"
#include "skill.h"
#include <regex.h>
#include <unistd.h>
#include "mime_type_mgr.h"
#include "parcel_macro.h"
#include "json_util.h"
#include <fcntl.h>
#include "nlohmann/json.hpp"
#ifdef BUNDLE_FRAMEWORK_UDMF_ENABLED
#include "type_descriptor.h"
#include "utd_client.h"
#endif

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr const char* JSON_KEY_TYPE = "type";
constexpr const char* JSON_KEY_PERMISSIONS = "permissions";
constexpr const char* JSON_KEY_ACTIONS = "actions";
constexpr const char* JSON_KEY_ENTITIES = "entities";
constexpr const char* JSON_KEY_URIS = "uris";
constexpr const char* JSON_KEY_SCHEME = "scheme";
constexpr const char* JSON_KEY_HOST = "host";
constexpr const char* JSON_KEY_PORT = "port";
constexpr const char* JSON_KEY_PATH = "path";
constexpr const char* JSON_KEY_PATHSTARTWITH = "pathStartWith";
constexpr const char* JSON_KEY_PATHREGEX = "pathRegex";
constexpr const char* JSON_KEY_UTD = "utd";
constexpr const char* JSON_KEY_MAXFILESUPPORTED = "maxFileSupported";
constexpr const char* JSON_KEY_LINKFEATURE =  "linkFeature";
constexpr const char* JSON_KEY_DOMAINVERIFY = "domainVerify";
constexpr const char* BUNDLE_MODULE_PROFILE_KEY_PATHREGX = "pathRegx";
constexpr const char* PARAM_SEPARATOR = "?";
constexpr const char* PORT_SEPARATOR = ":";
constexpr const char* SCHEME_SEPARATOR = "://";
constexpr const char* PATH_SEPARATOR = "/";
constexpr const char* TYPE_WILDCARD = "*/*";
const char WILDCARD = '*';
constexpr const char* TYPE_ONLY_MATCH_WILDCARD = "reserved/wildcard";
const char* LINK_FEATURE = "linkFeature";
const char* GENERAL_OBJECT = "general.object";
const uint32_t PROTOCOL_OFFSET = 3;
}; // namespace

bool Skill::Match(const OHOS::AAFwk::Want &want) const
{
    std::string linkFeature = want.GetStringParam(LINK_FEATURE);
    if (!linkFeature.empty()) {
        size_t matchUriIndex = 0;
        return MatchLinkFeature(linkFeature, want, matchUriIndex);
    }

    if (!MatchLauncher(want)) {
        APP_LOGD("Action or entities does not match");
        return false;
    }
    std::vector<std::string> vecTypes = want.GetStringArrayParam(OHOS::AAFwk::Want::PARAM_ABILITY_URITYPES);
    std::string uriString = want.GetUriString();
    if (!vecTypes.empty()) {
        for (std::string strType : vecTypes) {
            if (MatchUriAndType(uriString, strType)) {
                APP_LOGD("type %{public}s, Is Matched", strType.c_str());
                return true;
            }
        }
        return false;
    }
    bool matchUriAndType = MatchUriAndType(uriString, want.GetType());
    if (!matchUriAndType) {
        APP_LOGD("Uri or Type does not match");
        return false;
    }
    return true;
}

bool Skill::Match(const OHOS::AAFwk::Want &want, size_t &matchUriIndex) const
{
    std::string linkFeature = want.GetStringParam(LINK_FEATURE);
    if (!linkFeature.empty()) {
        return MatchLinkFeature(linkFeature, want, matchUriIndex);
    }

    if (!MatchLauncher(want)) {
        APP_LOGD("Action or entities does not match");
        return false;
    }

    std::vector<std::string> vecTypes = want.GetStringArrayParam(OHOS::AAFwk::Want::PARAM_ABILITY_URITYPES);
    if (vecTypes.size() > 0) {
        for (std::string strType : vecTypes) {
            if (MatchUriAndType(want.GetUriString(), strType, matchUriIndex)) {
                APP_LOGD("type %{public}s, Is Matched", strType.c_str());
                return true;
            }
        }
        return false;
    }
    bool matchUriAndType = MatchUriAndType(want.GetUriString(), want.GetType(), matchUriIndex);
    if (!matchUriAndType) {
        APP_LOGD("Uri or Type does not match");
        return false;
    }
    return true;
}


bool Skill::MatchLauncher(const OHOS::AAFwk::Want &want) const
{
    bool matchAction = MatchAction(want.GetAction());
    if (!matchAction) {
        APP_LOGD("Action does not match");
        return false;
    }
    bool matchEntities = MatchEntities(want.GetEntities());
    if (!matchEntities) {
        APP_LOGD("Entities does not match");
        return false;
    }
    return true;
}

bool Skill::MatchAction(const std::string &action) const
{
    // config actions empty, no match
    if (actions.empty()) {
        return false;
    }
    // config actions not empty, param empty, match
    if (action.empty()) {
        return true;
    }
    auto actionMatcher = [action] (const std::string &configAction) {
        if (action == configAction) {
            return true;
        }
        if (action == Constants::ACTION_HOME && configAction == Constants::WANT_ACTION_HOME) {
            return true;
        }
        if (action == Constants::WANT_ACTION_HOME && configAction == Constants::ACTION_HOME) {
            return true;
        }
        return false;
    };
    // config actions not empty, param not empty, if config actions contains param action, match
    return std::find_if(actions.cbegin(), actions.cend(), actionMatcher) != actions.cend();
}

bool Skill::MatchEntities(const std::vector<std::string> &paramEntities) const
{
    // param entities empty, match
    if (paramEntities.empty()) {
        return true;
    }
    // config entities empty, param entities not empty, not match
    if (entities.empty()) {
        return false;
    }
    // config entities not empty, param entities not empty, if every param entity in config entities, match
    std::vector<std::string>::size_type size = paramEntities.size();
    for (std::vector<std::string>::size_type i = 0; i < size; i++) {
        bool ret = std::find(entities.cbegin(), entities.cend(), paramEntities[i]) == entities.cend();
        if (ret) {
            return false;
        }
    }
    return true;
}

bool Skill::MatchUriAndType(const std::string &rawUriString, const std::string &type) const
{
    const std::string uriString = GetOptParamUri(rawUriString);
    if (uriString.empty() && type.empty()) {
        // case1 : param uri empty, param type empty
        if (uris.empty()) {
            return true;
        }
        for (const SkillUri &skillUri : uris) {
            if (skillUri.scheme.empty() && skillUri.type.empty()) {
                return true;
            }
        }
        return false;
    }
    if (uris.empty()) {
        return false;
    }
    if (!uriString.empty() && type.empty()) {
        // case2 : param uri not empty, param type empty
        for (const SkillUri &skillUri : uris) {
            if (MatchUri(uriString, skillUri) && skillUri.type.empty()) {
                return true;
            }
        }
        // if uri is a file path, match type by the suffix
        return MatchMimeType(uriString);
    } else if (uriString.empty() && !type.empty()) {
        // case3 : param uri empty, param type not empty
        for (const SkillUri &skillUri : uris) {
            if (skillUri.scheme.empty() && MatchType(type, skillUri.type)) {
                return true;
            }
        }
        return false;
    } else {
        // case4 : param uri not empty, param type not empty
        for (const SkillUri &skillUri : uris) {
            if (MatchUri(uriString, skillUri) && MatchType(type, skillUri.type)) {
                return true;
            }
        }
        return false;
    }
}

bool Skill::MatchLinkFeature(const std::string &linkFeature, const OHOS::AAFwk::Want &want, size_t &matchUriIndex) const
{
    std::string paramUriString = GetOptParamUri(want.GetUriString());
    std::string paramType = want.GetType();
    // only linkFeature
    if (paramUriString.empty() && paramType.empty()) {
        for (size_t uriIndex = 0; uriIndex < uris.size(); ++uriIndex) {
            const SkillUri &skillUri = uris[uriIndex];
            if (linkFeature == skillUri.linkFeature) {
                matchUriIndex = uriIndex;
                return true;
            }
        }
        return false;
    }
    // linkFeature + uri + type
    bool onlyUri = !paramUriString.empty() && paramType.empty();
    for (size_t uriIndex = 0; uriIndex < uris.size(); ++uriIndex) {
        const SkillUri &skillUri = uris[uriIndex];
        if (linkFeature != skillUri.linkFeature) {
            continue;
        }
        if (MatchUri(paramUriString, skillUri) && MatchType(paramType, skillUri.type)) {
            matchUriIndex = uriIndex;
            return true;
        }
        if (!onlyUri) {
            continue;
        }
        std::vector<std::string> paramUtdVector;
        if (!MimeTypeMgr::GetUtdVectorByUri(paramUriString, paramUtdVector)) {
            continue;
        }
        for (const std::string &paramUtd : paramUtdVector) {
            if ((MatchUri(paramUriString, skillUri) ||
                (skillUri.scheme.empty() && paramUriString.find(SCHEME_SEPARATOR) == std::string::npos)) &&
                MatchType(paramUtd, skillUri.type)) {
                matchUriIndex = uriIndex;
                return true;
            }
        }
    }
    return false;
}

bool Skill::MatchUriAndType(const std::string &rawUriString, const std::string &type, size_t &matchUriIndex) const
{
    const std::string uriString = GetOptParamUri(rawUriString);
    if (uriString.empty() && type.empty()) {
        // case1 : param uri empty, param type empty
        if (uris.empty()) {
            return true;
        }
        for (size_t uriIndex = 0; uriIndex < uris.size(); ++uriIndex) {
            const SkillUri &skillUri = uris[uriIndex];
            if (skillUri.scheme.empty() && skillUri.type.empty()) {
                matchUriIndex = uriIndex;
                return true;
            }
        }
        return false;
    }
    if (uris.empty()) {
        return false;
    }
    if (!uriString.empty() && type.empty()) {
        // case2 : param uri not empty, param type empty
        for (size_t uriIndex = 0; uriIndex < uris.size(); ++uriIndex) {
            const SkillUri &skillUri = uris[uriIndex];
            if (MatchUri(uriString, skillUri) && skillUri.type.empty()) {
                matchUriIndex = uriIndex;
                return true;
            }
        }
        // if uri is a file path, match type by the suffix
        return MatchMimeType(uriString, matchUriIndex);
    } else if (uriString.empty() && !type.empty()) {
        // case3 : param uri empty, param type not empty
        for (size_t uriIndex = 0; uriIndex < uris.size(); ++uriIndex) {
            const SkillUri &skillUri = uris[uriIndex];
            if (skillUri.scheme.empty() && MatchType(type, skillUri.type)) {
                matchUriIndex = uriIndex;
                return true;
            }
        }
        return false;
    } else {
        // case4 : param uri not empty, param type not empty
        for (size_t uriIndex = 0; uriIndex < uris.size(); ++uriIndex) {
            const SkillUri &skillUri = uris[uriIndex];
            if (MatchUri(uriString, skillUri) && MatchType(type, skillUri.type)) {
                matchUriIndex = uriIndex;
                return true;
            }
        }
        return false;
    }
}

bool Skill::StartsWith(const std::string &sourceString, const std::string &targetPrefix) const
{
    return sourceString.rfind(targetPrefix, 0) == 0;
}

std::string Skill::GetOptParamUri(const std::string &uriString)
{
    std::size_t pos = uriString.find(PARAM_SEPARATOR);
    if (pos == std::string::npos) {
        return uriString;
    }
    return uriString.substr(0, pos);
}

std::string Skill::ConvertUriToLower(const std::string& uri) const
{
    size_t protocolEnd = uri.find(SCHEME_SEPARATOR);
    if (protocolEnd == std::string::npos || protocolEnd + PROTOCOL_OFFSET == uri.size()) {
        return ConvertToLower(uri);
    }
    std::string protocol = uri.substr(0, protocolEnd);
    size_t startHost = protocolEnd + PROTOCOL_OFFSET;
    size_t endHost = uri.find(PATH_SEPARATOR, startHost);
    std::string host;
    if (endHost == std::string::npos) {
        host = uri.substr(startHost);
    } else {
        host = uri.substr(startHost, endHost - startHost);
    }

    std::string path = (endHost == std::string::npos) ? "" : uri.substr(endHost);
    std::transform(protocol.begin(), protocol.end(), protocol.begin(), [](unsigned char c) { return std::tolower(c); });
    std::transform(host.begin(), host.end(), host.begin(), [](unsigned char c) { return std::tolower(c); });
    return protocol + SCHEME_SEPARATOR + host + path;
}

inline std::string Skill::ConvertToLower(const std::string &str) const
{
    std::string lowerCaseStr = str;
    std::transform(lowerCaseStr.begin(), lowerCaseStr.end(), lowerCaseStr.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return lowerCaseStr;
}

bool Skill::MatchUri(const std::string &uriString, const SkillUri &skillUri) const
{
    if (uriString.empty() && skillUri.scheme.empty()) {
        return true;
    }
    if (uriString.empty() || skillUri.scheme.empty()) {
        return false;
    }
    if (skillUri.host.empty()) {
        // config uri is : scheme
        // belows are param uri matched conditions:
        // 1.scheme
        // 2.scheme:
        // 3.scheme:/
        // 4.scheme://
        std::string uriStr = ConvertUriToLower(GetOptParamUri(uriString));
        std::string skillScheme = ConvertToLower(skillUri.scheme);
        return uriStr == skillScheme || StartsWith(uriStr, skillScheme + PORT_SEPARATOR);
    }
    std::string optParamUri = ConvertUriToLower(GetOptParamUri(uriString));
    std::string skillUriTmpString;
    skillUriTmpString.append(skillUri.scheme).append(SCHEME_SEPARATOR).append(skillUri.host);
    std::string skillUriString = ConvertUriToLower(skillUriTmpString);

    if (!skillUri.port.empty()) {
        skillUriString.append(PORT_SEPARATOR).append(skillUri.port);
    }
    if (skillUri.path.empty() && skillUri.pathStartWith.empty() && skillUri.pathRegex.empty()) {
        // with port, config uri is : scheme://host:port
        // belows are param uri matched conditions:
        // 1.scheme://host:port
        // 2.scheme://host:port/path

        // without port, config uri is : scheme://host
        // belows are param uri matched conditions:
        // 1.scheme://host
        // 2.scheme://host/path
        // 3.scheme://host:port     scheme://host:port/path
        bool ret = (optParamUri == skillUriString || StartsWith(optParamUri, skillUriString + PATH_SEPARATOR));
        if (skillUri.port.empty()) {
            ret = ret || StartsWith(optParamUri, skillUriString + PORT_SEPARATOR);
        }
        return ret;
    }
    skillUriString.append(PATH_SEPARATOR);
    // if one of path, pathStartWith, pathRegex match, then match
    if (!skillUri.path.empty()) {
        // path match
        std::string pathUri(skillUriString);
        pathUri.append(skillUri.path);
        if (optParamUri == pathUri) {
            return true;
        }
    }
    if (!skillUri.pathStartWith.empty()) {
        // pathStartWith match
        std::string pathStartWithUri(skillUriString);
        pathStartWithUri.append(skillUri.pathStartWith);
        if (StartsWith(optParamUri, pathStartWithUri)) {
            return true;
        }
    }
    if (!skillUri.pathRegex.empty()) {
        // pathRegex match
        std::string pathRegexUri(skillUriString);
        pathRegexUri.append(skillUri.pathRegex);
        std::string modifiedPathRegex = "^" + pathRegexUri + "$";
        const char* pattern = modifiedPathRegex.c_str();
        regex_t regex;
        if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
            APP_LOGE("regex compilation failed");
            return false;
        }
        int32_t ret = regexec(&regex, optParamUri.c_str(), 0, NULL, 0);
        if (ret == 0) {
            regfree(&regex);
            return true;
        }
        APP_LOGE("failed to perform a match");
        regfree(&regex);
        return false;
    }
    return false;
}

bool Skill::MatchType(const std::string &type, const std::string &skillUriType) const
{
    if (type.empty() && skillUriType.empty()) {
        return true;
    }
    if (type.empty() || skillUriType.empty()) {
        return false;
    }

    // only match */* or general.object
    if (type == TYPE_ONLY_MATCH_WILDCARD) {
        return skillUriType == TYPE_WILDCARD || skillUriType == GENERAL_OBJECT;
    }

    bool containsUtd = false;
    bool matchUtdRet = MatchUtd(type, skillUriType, containsUtd);
    if (containsUtd) {
        return matchUtdRet;
    }

    if (type == TYPE_WILDCARD || skillUriType == TYPE_WILDCARD) {
        // param is */* or config is */*
        return true;
    }
    bool paramTypeRegex = type.back() == WILDCARD;
    if (paramTypeRegex) {
        // param is string/*
        std::string prefix = type.substr(0, type.length() - 1);
        return skillUriType.find(prefix) == 0;
    }
    bool typeRegex = skillUriType.back() == WILDCARD;
    if (typeRegex) {
        // config is string/*
        std::string prefix = skillUriType.substr(0, skillUriType.length() - 1);
        return type.find(prefix) == 0;
    } else {
        return type == skillUriType;
    }
}

bool Skill::MatchUtd(const std::string &paramType, const std::string &skillUriType, bool &containsUtd) const
{
#ifdef BUNDLE_FRAMEWORK_UDMF_ENABLED
    bool isParamUtd = IsUtd(paramType);
    bool isSkillUtd = IsUtd(skillUriType);

    containsUtd = isParamUtd || isSkillUtd;

    if (!isParamUtd && !isSkillUtd) {
        // 1.param : mimeType, skill : mimeType
        return false;
    } else if (isParamUtd && isSkillUtd) {
        // 2.param : utd, skill : utd
        return IsUtdMatch(paramType, skillUriType);
    } else if (!isParamUtd && isSkillUtd) {
        // 3.param : mimeType, skill : utd
        std::vector<std::string> paramUtdVector;
        auto ret = UDMF::UtdClient::GetInstance().GetUniformDataTypesByMIMEType(paramType, paramUtdVector);
        if (ret != ERR_OK || paramUtdVector.empty()) {
            return false;
        }
        for (const std::string &paramUtd : paramUtdVector) {
            if (IsUtdMatch(paramUtd, skillUriType)) {
                return true;
            }
        }
        return false;
    } else {
        // 4.param : utd, skill : mimeType
        std::vector<std::string> skillUtdVector;
        auto ret = UDMF::UtdClient::GetInstance().GetUniformDataTypesByMIMEType(skillUriType, skillUtdVector);
        if (ret != ERR_OK || skillUtdVector.empty()) {
            return false;
        }
        for (const std::string &skillUtd : skillUtdVector) {
            if (IsUtdMatch(paramType, skillUtd)) {
                return true;
            }
        }
        return false;
    }
#else
    containsUtd = false;
    return false;
#endif
}

bool Skill::IsUtdMatch(const std::string &paramUtd, const std::string &skillUtd) const
{
#ifdef BUNDLE_FRAMEWORK_UDMF_ENABLED
    std::shared_ptr<UDMF::TypeDescriptor> paramTypeDescriptor;
    auto ret = UDMF::UtdClient::GetInstance().GetTypeDescriptor(paramUtd, paramTypeDescriptor);
    if (ret != ERR_OK || paramTypeDescriptor == nullptr) {
        return false;
    }
    bool isMatch = false;
    ret = paramTypeDescriptor->BelongsTo(skillUtd, isMatch);
    if (ret != ERR_OK) {
        return false;
    }
    return isMatch;
#else
    return false;
#endif
}

bool Skill::IsUtd(const std::string &param) const
{
#ifdef BUNDLE_FRAMEWORK_UDMF_ENABLED
    bool isUtd = false;
    auto ret = UDMF::UtdClient::GetInstance().IsUtd(param, isUtd);
    return ret == ERR_OK && isUtd;
#else
    return false;
#endif
}

bool Skill::MatchMimeType(const std::string & uriString) const
{
    std::vector<std::string> paramUtdVector;
    if (!MimeTypeMgr::GetUtdVectorByUri(uriString, paramUtdVector)) {
        return false;
    }
    for (const SkillUri &skillUri : uris) {
        for (const std::string &paramUtd : paramUtdVector) {
            if ((MatchUri(uriString, skillUri) ||
                (skillUri.scheme.empty() && uriString.find(SCHEME_SEPARATOR) == std::string::npos)) &&
                MatchType(paramUtd, skillUri.type)) {
                return true;
            }
        }
    }
    return false;
}


bool Skill::MatchMimeType(const std::string & uriString, size_t &matchUriIndex) const
{
    std::vector<std::string> paramUtdVector;
    if (!MimeTypeMgr::GetUtdVectorByUri(uriString, paramUtdVector)) {
        return false;
    }
    for (size_t uriIndex = 0; uriIndex < uris.size(); ++uriIndex) {
        const SkillUri &skillUri = uris[uriIndex];
        for (const std::string &paramUtd : paramUtdVector) {
            if ((MatchUri(uriString, skillUri) ||
                (skillUri.scheme.empty() && uriString.find(SCHEME_SEPARATOR) == std::string::npos)) &&
                MatchType(paramUtd, skillUri.type)) {
                matchUriIndex = uriIndex;
                return true;
            }
        }
    }
    return false;
}

bool Skill::ReadFromParcel(Parcel &parcel)
{
    int32_t actionsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, actionsSize);
    CONTAINER_SECURITY_VERIFY(parcel, actionsSize, &actions);
    for (auto i = 0; i < actionsSize; i++) {
        actions.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t entitiesSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, entitiesSize);
    CONTAINER_SECURITY_VERIFY(parcel, entitiesSize, &entities);
    for (auto i = 0; i < entitiesSize; i++) {
        entities.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t urisSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, urisSize);
    CONTAINER_SECURITY_VERIFY(parcel, urisSize, &uris);
    for (auto i = 0; i < urisSize; i++) {
        SkillUri uri;
        uri.scheme = Str16ToStr8(parcel.ReadString16());
        uri.host = Str16ToStr8(parcel.ReadString16());
        uri.port = Str16ToStr8(parcel.ReadString16());
        uri.path = Str16ToStr8(parcel.ReadString16());
        uri.pathStartWith = Str16ToStr8(parcel.ReadString16());
        uri.pathRegex = Str16ToStr8(parcel.ReadString16());
        uri.type = Str16ToStr8(parcel.ReadString16());
        uri.utd = Str16ToStr8(parcel.ReadString16());
        uri.maxFileSupported = parcel.ReadInt32();
        uri.linkFeature = Str16ToStr8(parcel.ReadString16());
        uris.emplace_back(uri);
    }

    int32_t permissionsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissionsSize);
    CONTAINER_SECURITY_VERIFY(parcel, permissionsSize, &permissions);
    for (auto i = 0; i < permissionsSize; i++) {
        permissions.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }
    domainVerify = parcel.ReadBool();
    return true;
}

Skill *Skill::Unmarshalling(Parcel &parcel)
{
    Skill *skill = new (std::nothrow) Skill();
    if (skill && !skill->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete skill;
        skill = nullptr;
    }
    return skill;
}

bool Skill::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, actions.size());
    for (auto &action : actions) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(action));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, entities.size());
    for (auto &entitiy : entities) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(entitiy));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, uris.size());
    for (auto &uri : uris) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.scheme));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.host));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.port));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.path));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.pathStartWith));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.pathRegex));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.type));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.utd));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, uri.maxFileSupported);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri.linkFeature));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissions.size());
    for (auto &permission : permissions) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(permission));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, domainVerify);
    return true;
}

void Skill::Dump(std::string prefix, int fd)
{
    APP_LOGI("called dump Skill");
    if (fd < 0) {
        APP_LOGE("dump Skill fd error");
        return;
    }
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        APP_LOGE("dump Skill fcntl error : %{public}d", errno);
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
            APP_LOGE("dump Abilityinfo write error : %{public}d", errno);
        }
    }
    return;
}

void from_json(const nlohmann::json &jsonObject, SkillUri &uri)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_SCHEME,
    uri.scheme, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_HOST,
        uri.host, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_PORT,
        uri.port, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_PATH,
        uri.path, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_PATHSTARTWITH,
        uri.pathStartWith, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, BUNDLE_MODULE_PROFILE_KEY_PATHREGX,
        uri.pathRegex, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_PATHREGEX,
        uri.pathRegex, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_TYPE,
        uri.type, false, parseResult);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_UTD,
        uri.utd, false, parseResult);
    GetValueIfFindKey<int32_t>(jsonObject, jsonObjectEnd, JSON_KEY_MAXFILESUPPORTED,
        uri.maxFileSupported, JsonType::NUMBER, false, parseResult, ArrayType::NOT_ARRAY);
    BMSJsonUtil::GetStrValueIfFindKey(jsonObject, jsonObjectEnd, JSON_KEY_LINKFEATURE,
        uri.linkFeature, false, parseResult);
}

void from_json(const nlohmann::json &jsonObject, Skill &skill)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ACTIONS,
        skill.actions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ENTITIES,
        skill.entities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<SkillUri>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_URIS,
        skill.uris,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_PERMISSIONS,
        skill.permissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    BMSJsonUtil::GetBoolValueIfFindKey(jsonObject,
        jsonObjectEnd,
        JSON_KEY_DOMAINVERIFY,
        skill.domainVerify,
        false,
        parseResult);
}

void to_json(nlohmann::json &jsonObject, const SkillUri &uri)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_SCHEME, uri.scheme},
        {JSON_KEY_HOST, uri.host},
        {JSON_KEY_PORT, uri.port},
        {JSON_KEY_PATH, uri.path},
        {JSON_KEY_PATHSTARTWITH,  uri.pathStartWith},
        {JSON_KEY_PATHREGEX, uri.pathRegex},
        {JSON_KEY_TYPE, uri.type},
        {JSON_KEY_UTD, uri.utd},
        {JSON_KEY_MAXFILESUPPORTED, uri.maxFileSupported},
        {JSON_KEY_LINKFEATURE, uri.linkFeature},
    };
}

void to_json(nlohmann::json &jsonObject, const Skill &skill)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_ACTIONS, skill.actions},
        {JSON_KEY_ENTITIES, skill.entities},
        {JSON_KEY_URIS, skill.uris},
        {JSON_KEY_PERMISSIONS, skill.permissions},
        {JSON_KEY_DOMAINVERIFY, skill.domainVerify}
    };
}
}  // namespace AppExecFwk
}  // namespace OHOS
