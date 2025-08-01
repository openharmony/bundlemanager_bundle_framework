/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "aot/aot_executor.h"

#include <cerrno>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_extractor.h"
#include "bundle_service_constants.h"
#if defined(CODE_SIGNATURE_ENABLE)
#include "aot_compiler_client.h"
#include "code_sign_utils.h"
#endif
#include "installd/installd_operator.h"
#include "installd/installd_host_impl.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr const char* ABC_RELATIVE_PATH = "ets/modules.abc";
constexpr const char* STATIC_ABC_RELATIVE_PATH = "ets/modules_static.abc";
constexpr const char* HEX_PREFIX = "0x";
constexpr const char* BUNDLE_NAME = "bundleName";
constexpr const char* MODULE_NAME = "moduleName";
constexpr const char* PKG_PATH = "pkgPath";
constexpr const char* ABC_NAME = "abcName";
constexpr const char* ABC_PATH = "ABC-Path";
constexpr const char* AN_FILE_NAME = "anFileName";
constexpr const char* ABC_OFFSET = "abcOffset";
constexpr const char* ABC_SIZE = "abcSize";
constexpr const char* PROCESS_UID = "processUid";
constexpr const char* BUNDLE_UID = "bundleUid";
constexpr const char* APP_IDENTIFIER = "appIdentifier";
constexpr const char* IS_ENCRYPTED_BUNDLE = "isEncryptedBundle";
constexpr const char* IS_SCREEN_OFF = "isScreenOff";
constexpr const char* PGO_DIR = "pgoDir";
constexpr const char* IS_SYS_COMP = "isSysComp";
constexpr const char* IS_SYS_COMP_FALSE = "0";
constexpr const char* IS_SYS_COMP_TRUE = "1";
#if defined(CODE_SIGNATURE_ENABLE)
constexpr int16_t ERR_AOT_COMPILER_SIGN_FAILED = 10004;
constexpr int16_t ERR_AOT_COMPILER_CALL_CRASH = 10008;
constexpr int16_t ERR_AOT_COMPILER_CALL_CANCELLED = 10009;
#endif
}

AOTExecutor& AOTExecutor::GetInstance()
{
    static AOTExecutor executor;
    return executor;
}

std::string AOTExecutor::DecToHex(uint32_t decimal) const
{
    APP_LOGD("DecToHex begin, decimal : %{public}u", decimal);
    std::stringstream ss;
    ss << std::hex << decimal;
    std::string hexString = HEX_PREFIX + ss.str();
    APP_LOGD("hex : %{public}s", hexString.c_str());
    return hexString;
}

bool AOTExecutor::CheckArgs(const AOTArgs &aotArgs) const
{
    if (aotArgs.compileMode.empty() || aotArgs.hapPath.empty() || aotArgs.outputPath.empty()) {
        APP_LOGE("aotArgs check failed");
        return false;
    }
    if (aotArgs.compileMode == ServiceConstants::COMPILE_PARTIAL && aotArgs.arkProfilePath.empty()) {
        APP_LOGE("partial mode, arkProfilePath can't be empty");
        return false;
    }
    return true;
}

std::string AOTExecutor::GetAbcRelativePath(const std::string &moduleArkTSMode) const
{
    if (moduleArkTSMode == Constants::ARKTS_MODE_DYNAMIC) {
        return ABC_RELATIVE_PATH;
    }
    if (moduleArkTSMode == Constants::ARKTS_MODE_STATIC || moduleArkTSMode == Constants::ARKTS_MODE_HYBRID) {
        return STATIC_ABC_RELATIVE_PATH;
    }
    APP_LOGW("invalid moduleArkTSMode : %{public}s", moduleArkTSMode.c_str());
    return Constants::EMPTY_STRING;
}

bool AOTExecutor::GetAbcFileInfo(const std::string &hapPath, const std::string &moduleArkTSMode,
    uint32_t &offset, uint32_t &length) const
{
    BundleExtractor extractor(hapPath);
    if (!extractor.Init()) {
        APP_LOGE("init BundleExtractor failed");
        return false;
    }
    std::string abcRelativePath = GetAbcRelativePath(moduleArkTSMode);
    if (abcRelativePath.empty()) {
        APP_LOGE("abcRelativePath empty");
        return false;
    }
    if (!extractor.GetFileInfo(abcRelativePath, offset, length)) {
        APP_LOGE("GetFileInfo failed");
        return false;
    }
    APP_LOGD("GetFileInfo success, offset : %{public}u, length : %{public}u", offset, length);
    return true;
}

ErrCode AOTExecutor::PrepareArgs(const AOTArgs &aotArgs, AOTArgs &completeArgs) const
{
    APP_LOGD("PrepareArgs begin");
    if (aotArgs.isSysComp) {
        APP_LOGD("sysComp, no need to prepare args");
        completeArgs = aotArgs;
        return ERR_OK;
    }
    if (!CheckArgs(aotArgs)) {
        APP_LOGE("param check failed");
        return ERR_APPEXECFWK_INSTALLD_PARAM_ERROR;
    }
    completeArgs = aotArgs;
    if (!GetAbcFileInfo(completeArgs.hapPath, completeArgs.moduleArkTSMode,
        completeArgs.offset, completeArgs.length)) {
        APP_LOGE("GetAbcFileInfo failed");
        return ERR_APPEXECFWK_INSTALLD_AOT_ABC_NOT_EXIST;
    }
    // handle hsp
    for (auto &hspInfo : completeArgs.hspVector) {
        (void)GetAbcFileInfo(hspInfo.hapPath, hspInfo.moduleArkTSMode, hspInfo.offset, hspInfo.length);
    }
    APP_LOGD("PrepareArgs success");
    return ERR_OK;
}

nlohmann::json AOTExecutor::GetSubjectInfo(const AOTArgs &aotArgs) const
{
    /* obtain the uid of current process */
    int32_t currentProcessUid = static_cast<int32_t>(getuid());

    std::filesystem::path filePath(aotArgs.arkProfilePath);
    nlohmann::json subject;
    subject[BUNDLE_NAME] = aotArgs.bundleName;
    subject[MODULE_NAME] = aotArgs.moduleName;
    subject[PKG_PATH] = aotArgs.hapPath;
    subject[ABC_NAME] = GetAbcRelativePath(aotArgs.moduleArkTSMode);
    subject[ABC_OFFSET] = DecToHex(aotArgs.offset);
    subject[ABC_SIZE] = DecToHex(aotArgs.length);
    subject[PROCESS_UID] = DecToHex(currentProcessUid);
    subject[BUNDLE_UID] = DecToHex(aotArgs.bundleUid);
    subject[APP_IDENTIFIER] = aotArgs.appIdentifier;
    subject[IS_ENCRYPTED_BUNDLE] = DecToHex(aotArgs.isEncryptedBundle);
    subject[IS_SCREEN_OFF] = DecToHex(aotArgs.isScreenOff);
    subject[PGO_DIR] = filePath.parent_path().string();
    return subject;
}

void AOTExecutor::MapSysCompArgs(const AOTArgs &aotArgs, std::unordered_map<std::string, std::string> &argsMap)
{
    APP_LOGI_NOFUNC("MapSysCompArgs : %{public}s", aotArgs.ToString().c_str());
    argsMap.emplace(IS_SYS_COMP, IS_SYS_COMP_TRUE);
    argsMap.emplace(ABC_PATH, aotArgs.sysCompPath);
    argsMap.emplace(AN_FILE_NAME, aotArgs.anFileName);
    uid_t uid = getuid();
    if (uid > UINT32_MAX) {
        APP_LOGE_NOFUNC("invalid uid");
        return;
    }
    argsMap.emplace(PROCESS_UID, DecToHex(uid));
}

void AOTExecutor::MapHapArgs(const AOTArgs &aotArgs, std::unordered_map<std::string, std::string> &argsMap)
{
    APP_LOGI_NOFUNC("MapHapArgs : %{public}s", aotArgs.ToString().c_str());
    nlohmann::json subject = GetSubjectInfo(aotArgs);

    nlohmann::json objectArray = nlohmann::json::array();
    for (const auto &hspInfo : aotArgs.hspVector) {
        nlohmann::json object;
        object[BUNDLE_NAME] = hspInfo.bundleName;
        object[MODULE_NAME] = hspInfo.moduleName;
        object[PKG_PATH] = hspInfo.hapPath;
        object[ABC_NAME] = GetAbcRelativePath(hspInfo.moduleArkTSMode);
        object[Constants::MODULE_ARKTS_MODE] = hspInfo.moduleArkTSMode;
        object[ABC_OFFSET] = DecToHex(hspInfo.offset);
        object[ABC_SIZE] = DecToHex(hspInfo.length);
        objectArray.push_back(object);
    }
    argsMap.emplace("target-compiler-mode", aotArgs.compileMode);
    argsMap.emplace("aot-file", aotArgs.outputPath + ServiceConstants::PATH_SEPARATOR + aotArgs.moduleName);
    argsMap.emplace("compiler-pkg-info", subject.dump());
    argsMap.emplace("compiler-external-pkg-info", objectArray.dump());
    argsMap.emplace("compiler-opt-bc-range", aotArgs.optBCRangeList);
    argsMap.emplace("compiler-device-state", std::to_string(aotArgs.isScreenOff));
    argsMap.emplace("compiler-baseline-pgo", std::to_string(aotArgs.isEnableBaselinePgo));
    std::string abcPath =
        aotArgs.hapPath + ServiceConstants::PATH_SEPARATOR + GetAbcRelativePath(aotArgs.moduleArkTSMode);
    argsMap.emplace(ABC_PATH, abcPath);
    argsMap.emplace("BundleUid", std::to_string(aotArgs.bundleUid));
    argsMap.emplace("BundleGid", std::to_string(aotArgs.bundleGid));
    argsMap.emplace(AN_FILE_NAME, aotArgs.anFileName);
    argsMap.emplace("appIdentifier", aotArgs.appIdentifier);
    argsMap.emplace(Constants::MODULE_ARKTS_MODE, aotArgs.moduleArkTSMode);
    argsMap.emplace(IS_SYS_COMP, IS_SYS_COMP_FALSE);

    for (const auto &arg : argsMap) {
        APP_LOGI("%{public}s: %{public}s", arg.first.c_str(), arg.second.c_str());
    }
}

ErrCode AOTExecutor::PendSignAOT(const std::string &anFileName, const std::vector<uint8_t> &signData) const
{
    return EnforceCodeSign(anFileName, signData);
}

ErrCode AOTExecutor::EnforceCodeSign(const std::string &anFileName, const std::vector<uint8_t> &signData) const
{
#if defined(CODE_SIGNATURE_ENABLE)
    if (signData.empty()) {
        APP_LOGI("not enforce code sign if no aot file save");
        return ERR_OK;
    }
    uint32_t dataSize = static_cast<uint32_t>(signData.size());
    auto retCS =
        Security::CodeSign::CodeSignUtils::EnforceCodeSignForFile(anFileName, signData.data(), dataSize);
    if (retCS == VerifyErrCode::CS_ERR_ENABLE) {
        APP_LOGI("pending enforce code sign as screen not first unlock after reboot");
        return ERR_APPEXECFWK_INSTALLD_SIGN_AOT_DISABLE;
    }
    if (retCS != CommonErrCode::CS_SUCCESS) {
        APP_LOGE("fail to enable code signature for the aot file");
        return ERR_APPEXECFWK_INSTALLD_SIGN_AOT_FAILED;
    }
    APP_LOGI("sign aot file success");
    return ERR_OK;
#else
    APP_LOGI("code signature disable, ignore");
    return ERR_APPEXECFWK_INSTALLD_AOT_EXECUTE_FAILED;
#endif
}

ErrCode AOTExecutor::StartAOTCompiler(const AOTArgs &aotArgs, std::vector<uint8_t> &signData)
{
#if defined(CODE_SIGNATURE_ENABLE)
    std::unordered_map<std::string, std::string> argsMap;
    if (aotArgs.isSysComp) {
        MapSysCompArgs(aotArgs, argsMap);
    } else {
        MapHapArgs(aotArgs, argsMap);
        std::string aotFilePath = ServiceConstants::ARK_CACHE_PATH + aotArgs.bundleName;
        ErrCode result = InstalldHostImpl().Mkdir(aotFilePath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
            aotArgs.bundleUid, aotArgs.bundleGid);
        if (result != ERR_OK) {
            APP_LOGE("make aot file output directory fail");
            return ERR_APPEXECFWK_INSTALLD_AOT_EXECUTE_FAILED;
        }
    }
    APP_LOGI("start to aot compiler");
    std::vector<int16_t> fileData;
    ErrCode ret = ArkCompiler::AotCompilerClient::GetInstance().AotCompiler(argsMap, fileData);
    if (ret == ERR_AOT_COMPILER_SIGN_FAILED) {
        APP_LOGE("aot compiler local signature fail");
        return ERR_APPEXECFWK_INSTALLD_SIGN_AOT_FAILED;
    } else if (ret == ERR_AOT_COMPILER_CALL_CRASH) {
        APP_LOGE("aot compiler crash");
        return ERR_APPEXECFWK_INSTALLD_AOT_EXECUTE_CRASH;
    } else if (ret == ERR_AOT_COMPILER_CALL_CANCELLED) {
        APP_LOGE("aot compiler cancel");
        return ERR_APPEXECFWK_INSTALLD_AOT_EXECUTE_CANCELLED;
    } else if (ret != ERR_OK) {
        APP_LOGE("aot compiler fail");
        return ERR_APPEXECFWK_INSTALLD_AOT_EXECUTE_FAILED;
    }
    uint32_t byteSize = static_cast<uint32_t>(fileData.size());
    for (uint32_t i = 0; i < byteSize; ++i) {
        signData.emplace_back(static_cast<uint8_t>(fileData[i]));
    }
    APP_LOGI("aot compiler success");
    return ERR_OK;
#else
    APP_LOGI("code signature disable, ignore");
    return ERR_APPEXECFWK_INSTALLD_AOT_EXECUTE_FAILED;
#endif
}

void AOTExecutor::ExecuteAOT(const AOTArgs &aotArgs, ErrCode &ret, std::vector<uint8_t> &pendSignData)
{
#if defined(CODE_SIGNATURE_ENABLE)
    APP_LOGI("begin to execute AOT");
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (state_.running) {
            APP_LOGI("AOT is running, ignore");
            ret = ERR_APPEXECFWK_INSTALLD_AOT_EXECUTE_FAILED;
            return;
        }
    }
    AOTArgs completeArgs;
    ret = PrepareArgs(aotArgs, completeArgs);
    if (ret != ERR_OK) {
        APP_LOGE("prepareArgs fail");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        InitState(aotArgs);
    }
    APP_LOGI("begin to call aot compiler");
    std::vector<uint8_t> signData;
    ret = StartAOTCompiler(completeArgs, signData);
    if (ret == ERR_OK) {
        ret = EnforceCodeSign(completeArgs.anFileName, signData);
    }
    if (ret == ERR_APPEXECFWK_INSTALLD_SIGN_AOT_DISABLE) {
        pendSignData = signData;
    }
    APP_LOGI("aot compiler finish");
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        ResetState();
    }
#else
    APP_LOGI("code signature disable, ignore");
    ret = ERR_APPEXECFWK_INSTALLD_AOT_EXECUTE_FAILED;
#endif
}

ErrCode AOTExecutor::StopAOT()
{
#if defined(CODE_SIGNATURE_ENABLE)
    APP_LOGI("begin to stop AOT");
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (!state_.running) {
        APP_LOGI("AOT not running, return directly");
        return ERR_OK;
    }
    int32_t ret = ArkCompiler::AotCompilerClient::GetInstance().StopAotCompiler();
    if (ret != ERR_OK) {
        APP_LOGE("stop aot compiler fail");
        return ERR_APPEXECFWK_INSTALLD_STOP_AOT_FAILED;
    }
    (void)InstalldOperator::DeleteDir(state_.outputPath);
    ResetState();
    return ERR_OK;
#else
    APP_LOGI("code signature disable, ignore");
    return ERR_APPEXECFWK_INSTALLD_STOP_AOT_FAILED;
#endif
}

void AOTExecutor::InitState(const AOTArgs &aotArgs)
{
    state_.running = true;
    state_.outputPath = aotArgs.outputPath;
}

void AOTExecutor::ResetState()
{
    state_.running = false;
    state_.outputPath.clear();
}
}  // namespace AppExecFwk
}  // namespace OHOS
