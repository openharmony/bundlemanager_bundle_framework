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

#include "mock_status_receiver.h"

#include "appexecfwk_errors.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t WAIT_TIME = 5; // 5s
} // namespace

void MockStatusReceiver::OnFinished(const int32_t resultCode, [[maybe_unused]] const std::string &resultMsg)
{
    std::lock_guard<std::mutex> lock(setValueMutex_);
    if (!isSetValue) {
        isSetValue = true;
        signal_.set_value(resultCode);
    }
}

void MockStatusReceiver::OnStatusNotify(const int32_t progress)
{
    return;
}

sptr<IRemoteObject> MockStatusReceiver::AsObject()
{
    return nullptr;
}

int32_t MockStatusReceiver::GetResultCode()
{
    auto future = signal_.get_future();
    if (future.wait_for(std::chrono::seconds(WAIT_TIME)) == std::future_status::ready) {
        int32_t resultCode = future.get();
        return resultCode;
    }
    return ERR_APPEXECFWK_OPERATION_TIME_OUT;
}

void MockStatusReceiver::SetStreamInstallId(uint32_t installerId)
{
    return;
}
}  // namespace AppExecFwk
}  // namespace OHOS