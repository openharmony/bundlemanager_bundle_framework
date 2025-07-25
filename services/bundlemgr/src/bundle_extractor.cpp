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

#include "bundle_extractor.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr const char* BUNDLE_PROFILE_NAME = "config.json";
constexpr const char* MODULE_PROFILE_NAME = "module.json";
constexpr const char* BUNDLE_PACKFILE_NAME = "pack.info";
}
BundleExtractor::BundleExtractor(const std::string &source, bool parallel) : BaseExtractor(source, parallel)
{
    APP_LOGD("BundleExtractor is created");
}

BundleExtractor::~BundleExtractor()
{
    APP_LOGD("destroyed");
}

bool BundleExtractor::ExtractProfile(std::ostream &dest) const
{
    if (IsNewVersion()) {
        APP_LOGD("profile is module.json");
        return ExtractByName(MODULE_PROFILE_NAME, dest);
    }
    APP_LOGD("profile is config.json");
    return ExtractByName(BUNDLE_PROFILE_NAME, dest);
}

bool BundleExtractor::ExtractPackFile(std::ostream &dest) const
{
    APP_LOGD("start to parse pack.info");
    return ExtractByName(BUNDLE_PACKFILE_NAME, dest);
}

BundleParallelExtractor::BundleParallelExtractor(const std::string &source) : BundleExtractor(source, true)
{
    APP_LOGD("BundleParallelExtractor is created");
}

BundleParallelExtractor::~BundleParallelExtractor()
{
    APP_LOGD("BundleParallelExtractor is destroyed");
}

bool BundleParallelExtractor::ExtractByName(const std::string &fileName, std::ostream &dest) const
{
    if (!initial_) {
        APP_LOGE("extractor is not initial");
        return false;
    }
    if (!zipFile_.ExtractFileParallel(fileName, dest)) {
        APP_LOGE("extractor is not ExtractFile");
        return false;
    }
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
