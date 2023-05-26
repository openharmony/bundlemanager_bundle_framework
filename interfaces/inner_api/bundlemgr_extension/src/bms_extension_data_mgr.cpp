/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <dlfcn.h>

#include "app_log_wrapper.h"
#include "bms_extension_data_mgr.h"
#include "bms_extension_profile.h"
#include "bundle_mgr_ext_register.h"

namespace OHOS {
namespace AppExecFwk {
BmsExtension BmsExtensionDataMgr::bmsExtension_;
void *BmsExtensionDataMgr::handler_ = nullptr;
namespace {
const std::string BMS_EXTENSION_PATH = "/system/etc/app/bms-extensions.json";
}

BmsExtensionDataMgr::BmsExtensionDataMgr()
{
}

ErrCode BmsExtensionDataMgr::Init()
{
    std::lock_guard<std::mutex> stateLock(stateMutex_);
    if (bmsExtension_.bmsExtensionBundleMgr.extensionName.empty() || !handler_) {
        BmsExtensionProfile bmsExtensionProfile;
        auto res = bmsExtensionProfile.ParseBmsExtension(BMS_EXTENSION_PATH, bmsExtension_);
        if (res != ERR_OK) {
            APP_LOGW("ParseBmsExtension failed, errCode is %{public}d", res);
            return ERR_APPEXECFWK_PARSE_UNEXPECTED;
        }
        APP_LOGD("parse bms-extension.json success, which is: %{public}s", bmsExtension_.ToString().c_str());
        if (!OpenHandler()) {
            APP_LOGW("dlopen bms-extension so failed");
            return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
        }
    }
    return ERR_OK;
}

bool BmsExtensionDataMgr::OpenHandler()
{
    APP_LOGI("OpenHandler start");
    auto handle = &handler_;
    if (handle == nullptr) {
        APP_LOGE("OpenHandler error handle is nullptr.");
        return false;
    }
    auto libPath = bmsExtension_.bmsExtensionBundleMgr.libPath.c_str();
    auto lib64Path = bmsExtension_.bmsExtensionBundleMgr.lib64Path.c_str();
    *handle = dlopen(lib64Path, RTLD_NOW | RTLD_GLOBAL);
    if (*handle == nullptr) {
        APP_LOGW("failed to open %{public}s, err:%{public}s", lib64Path, dlerror());
        *handle = dlopen(libPath, RTLD_NOW | RTLD_GLOBAL);
    }
    if (*handle == nullptr) {
        APP_LOGE("failed to open %{public}s, err:%{public}s", libPath, dlerror());
        return false;
    }
    APP_LOGI("OpenHandler end");
    return true;
}

bool BmsExtensionDataMgr::CheckApiInfo(const BundleInfo &bundleInfo)
{
    auto ret = Init();

    if (ret != ERR_OK) {
        APP_LOGE("Init failed, ErrCode: %{public}d", ret);
        return false;
    }
    if (handler_) {
        auto bundleMgrExtPtr =
            BundleMgrExtRegister::GetInstance().GetBundleMgrExt(bmsExtension_.bmsExtensionBundleMgr.extensionName);
        if (bundleMgrExtPtr) {
            return bundleMgrExtPtr->CheckApiInfo(bundleInfo);
        }
        APP_LOGE("create class: %{public}s failed.", bmsExtension_.bmsExtensionBundleMgr.extensionName.c_str());
        return false;
    }
    APP_LOGE("dlopen so file failed.");
    return false;
}
} // AppExecFwk
} // OHOS