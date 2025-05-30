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

#ifndef FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_DRAWABLE_H
#define FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_DRAWABLE_H

#include <string>

#include "resource_manager.h"
#include "resource_info.h"

#ifdef BUNDLE_FRAMEWORK_GRAPHICS
#include "pixel_map.h"
#endif

namespace OHOS {
namespace AppExecFwk {
class BundleResourceDrawable {
public:
    bool GetIconResourceByTheme(
        const uint32_t iconId,
        const int32_t density,
        std::shared_ptr<Global::Resource::ResourceManager> resourceManager,
        ResourceInfo &resourceInfo);

    bool GetIconResourceByHap(
        const uint32_t iconId,
        const int32_t density,
        std::shared_ptr<Global::Resource::ResourceManager> resourceManager,
        ResourceInfo &resourceInfo);

#ifdef BUNDLE_FRAMEWORK_GRAPHICS
    bool GetBadgedIconResource(
        const std::shared_ptr<Media::PixelMap> layeredPixelMap,
        const std::shared_ptr<Media::PixelMap> badgedPixelMap,
        ResourceInfo &resourceInfo);
#endif
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_SERVICES_BUNDLEMGR_BUNDLE_RESOURCE_DRAWABLE_H
