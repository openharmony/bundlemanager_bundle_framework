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

#ifndef FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_IMAGE_INFO_H
#define FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_IMAGE_INFO_H

#include <string>

#ifdef BUNDLE_FRAMEWORK_GRAPHICS
#include "pixel_map.h"
#endif

namespace OHOS {
namespace AppExecFwk {
class BundleResourceImageInfo {
public:
    bool ConvertToBase64(const std::unique_ptr<uint8_t[]> originalData, const size_t length, std::string &imageInfo);

#ifdef BUNDLE_FRAMEWORK_GRAPHICS
    bool ConvertToString(const std::shared_ptr<Media::PixelMap> pixelMap, std::string &imageInfo);

private:
    bool EncodeBase64(const std::string &originalData, std::string &dstData);
#endif
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_IMAGE_INFO_H
