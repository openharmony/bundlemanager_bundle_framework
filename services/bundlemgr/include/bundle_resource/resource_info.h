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

#ifndef FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_RESOURCE_INFO_H
#define FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_RESOURCE_INFO_H

#include <string>

namespace OHOS {
namespace AppExecFwk {
class ResourceInfo {
public:
    ResourceInfo();
    ~ResourceInfo();
    /**
     * key:
     * 1. bundleName
     * 2. bundleName/moduleName/abilityName
     */
    std::string GetKey() const;
    void ParseKey(const std::string &key);
    // key
    std::string bundleName_;
    std::string moduleName_;
    std::string abilityName_;
    // label resource
    int32_t labelId_ = 0;
    std::string label_;
    // icon resource
    int32_t iconId_ = 0;
    std::string icon_;
    // resource info update time
    int64_t updateTime_ = 0;
    // used for parse label and icon
    std::string hapPath_;
    // used for parse default icon
    std::string defaultIconHapPath_;
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_RESOURCE_INFO_H