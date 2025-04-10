/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef APPEXECFWK_CLEANCACHE_CALLBACK_H
#define APPEXECFWK_CLEANCACHE_CALLBACK_H

#include <future>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "clean_cache_callback_host.h"
#include "nocopyable.h"

class CleanCacheCallback : public OHOS::AppExecFwk::CleanCacheCallbackHost {
public:
    explicit CleanCacheCallback(int32_t err);
    virtual ~CleanCacheCallback();
    virtual void OnCleanCacheFinished(bool err) override;

    int32_t GetErr() const
    {
        return err_;
    }

public:
    bool WaitForCompletion();

private:
    int32_t err_;
    std::mutex mutex_;
    bool complete_ = false;
    std::promise<void> promise_;
    std::future<void> future_ = promise_.get_future();
    DISALLOW_COPY_AND_MOVE(CleanCacheCallback);
};

#endif // APPEXECFWK_CLEANCACHE_CALLBACK_H

