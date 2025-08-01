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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_FORM_INFO_BASE_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_FORM_INFO_BASE_H

#include <string>

namespace OHOS {
namespace AppExecFwk {
enum class FormsColorMode : int8_t {
    AUTO_MODE = -1,
    DARK_MODE = 0,
    LIGHT_MODE = 1,
};

enum class FormsRenderingMode : int8_t {
    AUTO_COLOR = 0,
    FULL_COLOR = 1,
    SINGLE_COLOR = 2
};

struct FormCustomizeData {
    std::string name;
    std::string value;
};

struct FormWindow {
    bool autoDesignWidth = false;
    int32_t designWidth = 720;
};

struct FormFunInteractionParams {
    std::string abilityName;
    std::string targetBundleName;
    std::string subBundleName;
    int32_t keepStateDuration = 10000;
};

struct FormSceneAnimationParams {
    std::string abilityName;
    std::string disabledDesktopBehaviors;
};

enum class FormType {
    JAVA = 0,
    JS = 1,
    ETS = 2,
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_FORM_INFO_BASE_H
