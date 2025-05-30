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

#include "quick_fix/patch_extractor.h"

#include "app_log_tag_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr const char* PATCH_PROFILE_NAME = "patch.json";
}

PatchExtractor::PatchExtractor(const std::string &source) : BaseExtractor(source) {}

bool PatchExtractor::ExtractPatchProfile(std::ostream &dest) const
{
    LOG_D(BMS_TAG_DEFAULT, "start to extract patch.json");
    return ExtractByName(PATCH_PROFILE_NAME, dest);
}
}  // namespace AppExecFwk
}  // namespace OHOS