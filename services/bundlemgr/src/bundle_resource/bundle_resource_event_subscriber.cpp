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

#include "bundle_resource_event_subscriber.h"

#include "bundle_resource_callback.h"
#include "common_event_support.h"

namespace OHOS {
namespace AppExecFwk {
BundleResourceEventSubscriber::BundleResourceEventSubscriber(
    const EventFwk::CommonEventSubscribeInfo &subscribeInfo) : EventFwk::CommonEventSubscriber(subscribeInfo)
{}

BundleResourceEventSubscriber::~BundleResourceEventSubscriber()
{}

void BundleResourceEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    BundleResourceCallback callback;
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        callback.OnUserIdSwitched(data.GetCode());
    }
    // for other event
}
}  // namespace AppExecFwk
}  // namespace OHOS