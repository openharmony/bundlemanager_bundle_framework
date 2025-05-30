/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INTERFACES_KITS_JS_ZIP_NAPI_COMMON_NAPI_EXPORTER_H
#define INTERFACES_KITS_JS_ZIP_NAPI_COMMON_NAPI_EXPORTER_H

#include "napi_value.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
class NapiExporter {
public:
    NapiExporter(napi_env env, napi_value exports) : exports_(env, exports){};
    virtual ~NapiExporter() = default;

    virtual bool Export() = 0;
    virtual std::string GetClassName() = 0;

protected:
    NapiValue exports_;
};
}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // INTERFACES_KITS_JS_ZIP_NAPI_COMMON_NAPI_EXPORTER_H