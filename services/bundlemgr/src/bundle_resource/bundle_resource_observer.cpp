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

#include "bundle_resource_observer.h"

#include "app_log_wrapper.h"
#include "bundle_resource_callback.h"
#include "bundle_system_state.h"

#ifdef ABILITY_RUNTIME_ENABLE
#include "configuration.h"
#endif

namespace OHOS {
namespace AppExecFwk {
#ifdef ABILITY_RUNTIME_ENABLE
BundleResourceObserver::BundleResourceObserver()
{}

BundleResourceObserver::~BundleResourceObserver()
{}

void BundleResourceObserver::OnConfigurationUpdated(const AppExecFwk::Configuration& configuration)
{
    APP_LOGI("called");
    BundleResourceCallback callback;
    std::string colorMode = configuration.GetItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE);
    if (!colorMode.empty() && (colorMode != BundleSystemState::GetInstance().GetSystemColorMode())) {
        APP_LOGD("OnSystemColorModeChanged colorMode:%{public}s", colorMode.c_str());
        callback.OnSystemColorModeChanged(colorMode);
    }
    std::string language = configuration.GetItem(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE);
    if (!language.empty() && (language != BundleSystemState::GetInstance().GetSystemLanguage())) {
        APP_LOGD("OnSystemLanguageChange language:%{public}s", language.c_str());
        callback.OnSystemLanguageChange(language);
    }
}
#endif
} // AppExecFwk
} // OHOS