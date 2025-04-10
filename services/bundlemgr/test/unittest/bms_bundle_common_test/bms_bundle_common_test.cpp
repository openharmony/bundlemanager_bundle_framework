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
#define private public

#include <atomic>
#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <sstream>
#include <string>
#include <thread>

#include "bundle_mgr_service.h"
#include "serial_queue.h"
#include "single_delayed_task_mgr.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace {
    constexpr uint32_t DISCONNECT_DELAY = 20000;
    constexpr uint32_t DISCONNECT_DELAY1 = 0;
    const std::string DISCONNECT_DELAY_TASK = "DisconnectDelayTask";
}  // namespace

class BmsBundleCommonTest : public testing::Test {
public:
    BmsBundleCommonTest();
    ~BmsBundleCommonTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

private:
    static std::shared_ptr<BundleMgrService> bundleMgrService_;
};

std::shared_ptr<BundleMgrService> BmsBundleCommonTest::bundleMgrService_ =
    DelayedSingleton<BundleMgrService>::GetInstance();

BmsBundleCommonTest::BmsBundleCommonTest()
{}

BmsBundleCommonTest::~BmsBundleCommonTest()
{}

void BmsBundleCommonTest::SetUpTestCase()
{}

void BmsBundleCommonTest::TearDownTestCase()
{
    bundleMgrService_->OnStop();
}

void BmsBundleCommonTest::SetUp()
{}

void BmsBundleCommonTest::TearDown()
{}

/**
 * @tc.number: CheckAppInstallControl
 * @tc.name: test CheckAppInstallControl by InnerBundleInfo
 * @tc.desc: 1.CheckAppInstallControl test
 */
HWTEST_F(BmsBundleCommonTest, CancelDelayTask_0100, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    std::string taskName = "task";
    serialQueue.CancelDelayTask(taskName);
    EXPECT_EQ(serialQueue.taskMap_.size(), 0);
}

/**
 * @tc.number: CheckAppInstallControl
 * @tc.name: test CheckAppInstallControl by InnerBundleInfo
 * @tc.desc: 1.CheckAppInstallControl test
 */
HWTEST_F(BmsBundleCommonTest, CancelDelayTask_0200, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    auto registerEventListenerFunc = []() {
        return;
    };
    std::string taskName = "task";
    serialQueue.ScheduleDelayTask(taskName, DISCONNECT_DELAY, registerEventListenerFunc);

    serialQueue.CancelDelayTask(taskName);
    EXPECT_EQ(serialQueue.taskMap_.size(), 0);
}

/**
 * @tc.number: CheckAppInstallControl
 * @tc.name: test CheckAppInstallControl by InnerBundleInfo
 * @tc.desc: 1.CheckAppInstallControl test
 */
HWTEST_F(BmsBundleCommonTest, CancelDelayTask_0300, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    auto registerEventListenerFunc = []() {
        return;
    };
    std::string taskName = "task";
    serialQueue.ScheduleDelayTask(DISCONNECT_DELAY_TASK, DISCONNECT_DELAY, registerEventListenerFunc);

    serialQueue.CancelDelayTask(taskName);
    EXPECT_EQ(serialQueue.taskMap_.size(), 1);
}

/**
 * @tc.number: CheckAppInstallControl
 * @tc.name: test CheckAppInstallControl by InnerBundleInfo
 * @tc.desc: 1.CheckAppInstallControl test
 */
HWTEST_F(BmsBundleCommonTest, CancelDelayTask_0400, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    auto registerEventListenerFunc = []() {
        return;
    };
    std::string taskName = "task";
    serialQueue.ScheduleDelayTask(DISCONNECT_DELAY_TASK, DISCONNECT_DELAY1, registerEventListenerFunc);

    serialQueue.CancelDelayTask(taskName);
    EXPECT_EQ(serialQueue.taskMap_.size(), 1);
}

/**
 * @tc.number: CheckAppInstallControl
 * @tc.name: test CheckAppInstallControl by InnerBundleInfo
 * @tc.desc: 1.CheckAppInstallControl test
 */
HWTEST_F(BmsBundleCommonTest, CancelDelayTask_0500, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    auto registerEventListenerFunc = []() {
        return;
    };
    std::string taskName = "task";
    uint64_t ms;
    std::function<void()> func;
    serialQueue.ScheduleDelayTask(DISCONNECT_DELAY_TASK, 1000, registerEventListenerFunc);

    serialQueue.CancelDelayTask(taskName);
    EXPECT_EQ(serialQueue.taskMap_.size(), 1);
}

/**
 * @tc.number: SingleDelayedTaskMgr_0100
 * @tc.name: test ScheduleDelayedTask
 * @tc.desc: 1.call ScheduleDelayedTask, task execute success
 */
HWTEST_F(BmsBundleCommonTest, SingleDelayedTaskMgr_0100, Function | SmallTest | Level1)
{
    std::string taskName = "SingleDelayedTaskMgr_0100";
    uint64_t delayedTimeMs = 50;
    std::shared_ptr<SingleDelayedTaskMgr> mgr = std::make_shared<SingleDelayedTaskMgr>(taskName, delayedTimeMs);
    std::atomic<bool> flag = false;
    auto func = [&flag]() {
        flag = true;
    };
    mgr->ScheduleDelayedTask(func);
    EXPECT_TRUE(mgr->isRunning_);
    uint64_t sleepTimeMs = 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
    EXPECT_TRUE(flag);
    EXPECT_FALSE(mgr->isRunning_);
}

/**
 * @tc.number: SingleDelayedTaskMgr_0200
 * @tc.name: test GetExecuteTime
 * @tc.desc: 1.call GetExecuteTime, executeTime_ equal to now
 */
HWTEST_F(BmsBundleCommonTest, SingleDelayedTaskMgr_0200, Function | SmallTest | Level1)
{
    std::string taskName = "SingleDelayedTaskMgr_0200";
    uint64_t delayedTimeMs = 50;
    std::shared_ptr<SingleDelayedTaskMgr> mgr = std::make_shared<SingleDelayedTaskMgr>(taskName, delayedTimeMs);
    auto now = std::chrono::steady_clock::now();
    mgr->executeTime_ = now;
    auto ret = mgr->GetExecuteTime();
    EXPECT_TRUE(now == ret);
}

/**
 * @tc.number: SingleDelayedTaskMgr_0300
 * @tc.name: test SetTaskFinished
 * @tc.desc: 1.call SetTaskFinished, isRunning_ is false
 */
HWTEST_F(BmsBundleCommonTest, SingleDelayedTaskMgr_0300, Function | SmallTest | Level1)
{
    std::string taskName = "SingleDelayedTaskMgr_0300";
    uint64_t delayedTimeMs = 50;
    std::shared_ptr<SingleDelayedTaskMgr> mgr = std::make_shared<SingleDelayedTaskMgr>(taskName, delayedTimeMs);
    mgr->isRunning_ = true;
    mgr->SetTaskFinished();
    EXPECT_FALSE(mgr->isRunning_);
}

/**
 * @tc.number: UpdateUninstallBundleInfo
 * @tc.name: ReScheduleDelayTask_0100
 * @tc.desc: test bool UninstallDataMgrStorageRdb::UpdateUninstallBundleInfo(
 *  const std::string &bundleName, const UninstallBundleInfo &uninstallbundleInfo)
 */
HWTEST_F(BmsBundleCommonTest, ReScheduleDelayTask_0100, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    auto registerEventListenerFunc = []() {
        return;
    };
    std::string taskName = "task";
    uint64_t ms = 1000;
    serialQueue.ReScheduleDelayTask(taskName, ms, registerEventListenerFunc);
    serialQueue.CancelDelayTask(taskName);
    EXPECT_EQ(serialQueue.taskMap_.size(), 0);
}

/**
 * @tc.number: UpdateUninstallBundleInfo
 * @tc.name: ReScheduleDelayTask_0200_MsOverflow
 * @tc.desc: test bool UninstallDataMgrStorageRdb::UpdateUninstallBundleInfo(
 *  const std::string &bundleName, const UninstallBundleInfo &uninstallbundleInfo)
 */
HWTEST_F(BmsBundleCommonTest, ReScheduleDelayTask_0200, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    auto registerEventListenerFunc = []() {
        return;
    };
    std::string taskName = "task";
    uint64_t ms = std::numeric_limits<uint64_t>::max() / 1000 + 1;
    serialQueue.ReScheduleDelayTask(taskName, ms, registerEventListenerFunc);
    EXPECT_EQ(serialQueue.taskMap_.size(), 0);
}

/**
 * @tc.number: UpdateUninstallBundleInfo
 * @tc.name: ReScheduleDelayTask_0300_ExistingTask
 * @tc.desc: test bool UninstallDataMgrStorageRdb::UpdateUninstallBundleInfo(
 *  const std::string &bundleName, const UninstallBundleInfo &uninstallbundleInfo)
 */
HWTEST_F(BmsBundleCommonTest, ReScheduleDelayTask_0300, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    auto registerEventListenerFunc = []() {
        return;
    };
    std::string taskName = "task";
    uint64_t ms = 1000;
    // first add task
    serialQueue.ReScheduleDelayTask(taskName, ms, registerEventListenerFunc);
    EXPECT_EQ(serialQueue.taskMap_.size(), 1);
    // second add task, overwrite the first task
    serialQueue.ReScheduleDelayTask(taskName, ms, registerEventListenerFunc);
    EXPECT_EQ(serialQueue.taskMap_.size(), 1);
}

/**
 * @tc.number: UpdateUninstallBundleInfo
 * @tc.name: ReScheduleDelayTask_0400_CancelFailed
 * @tc.desc: test bool UninstallDataMgrStorageRdb::UpdateUninstallBundleInfo(
 *  const std::string &bundleName, const UninstallBundleInfo &uninstallbundleInfo)
 */
HWTEST_F(BmsBundleCommonTest, ReScheduleDelayTask_0400, Function | SmallTest | Level1)
{
    std::string queueName;
    SerialQueue serialQueue(queueName);
    auto registerEventListenerFunc = []() {
        return;
    };
    std::string taskName = "task";
    uint64_t ms = 1000;
    serialQueue.ReScheduleDelayTask(taskName, ms, registerEventListenerFunc);
    EXPECT_EQ(serialQueue.taskMap_.size(), 1);
    serialQueue.taskMap_[taskName] = nullptr;
    serialQueue.ReScheduleDelayTask(taskName, ms, registerEventListenerFunc);
    EXPECT_EQ(serialQueue.taskMap_.size(), 1);
}

/**
 * @tc.number: AppspawnCreateDefaultMsg
 * @tc.name: AppspawnCreateDefaultMsg_0100
 * @tc.desc: int32_t BundleAppSpawnClient::AppspawnCreateDefaultMsg
 * (const AppSpawnRemoveSandboxDirMsg &removeSandboxDirMsg,AppSpawnReqMsgHandle reqHandle)
 */
HWTEST_F(BmsBundleCommonTest, AppspawnCreateDefaultMsg_0100, Function | SmallTest | Level1)
{
    AppSpawnRemoveSandboxDirMsg removeSandboxDirMsg = {
        .bundleName = "com.example.test",
        .bundleIndex = 0,
    };
    AppSpawnReqMsgHandle reqHandle = nullptr;
    // AppSpawnReqMsgHandle reqHandle;
    int32_t ret = AppSpawnReqMsgSetBundleInfo(reqHandle, removeSandboxDirMsg.bundleIndex,
        removeSandboxDirMsg.bundleName.c_str());
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number: AppspawnCreateDefaultMsg
 * @tc.name: AppspawnCreateDefaultMsg_0200
 * @tc.desc: int32_t BundleAppSpawnClient::AppspawnCreateDefaultMsg
 * (const AppSpawnRemoveSandboxDirMsg &removeSandboxDirMsg,AppSpawnReqMsgHandle reqHandle)
 */
HWTEST_F(BmsBundleCommonTest, AppspawnCreateDefaultMsg_0200, Function | SmallTest | Level1)
{
    AppDacInfo appDacInfo = {0};
    AppSpawnRemoveSandboxDirMsg removeSandboxDirMsg = {
        .uid = 0,
    };
    AppSpawnReqMsgHandle reqHandle = nullptr;
    appDacInfo.uid = removeSandboxDirMsg.uid;
    int32_t ret = AppSpawnReqMsgSetAppDacInfo(reqHandle, &appDacInfo);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number: AppspawnCreateDefaultMsg
 * @tc.name: AppspawnCreateDefaultMsg_0300
 * @tc.desc: int32_t BundleAppSpawnClient::AppspawnCreateDefaultMsg
 * (const AppSpawnRemoveSandboxDirMsg &removeSandboxDirMsg,AppSpawnReqMsgHandle reqHandle)
 */
HWTEST_F(BmsBundleCommonTest, AppspawnCreateDefaultMsg_0300, Function | SmallTest | Level1)
{
    AppSpawnRemoveSandboxDirMsg removeSandboxDirMsg = {
        .flags = APP_FLAGS_COLD_BOOT,
    };
    AppSpawnReqMsgHandle reqHandle = nullptr;
    int32_t ret = AppSpawnReqMsgSetAppFlag(reqHandle, removeSandboxDirMsg.flags);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number: RemoveSandboxDir
 * @tc.name: RemoveSandboxDir_0100
 * @tc.desc: int32_t BundleAppSpawnClient::RemoveSandboxDir
 * (const AppSpawnRemoveSandboxDirMsg &removeSandboxDirMsg)
 */
HWTEST_F(BmsBundleCommonTest, RemoveSandboxDir_0100, Function | SmallTest | Level1)
{
    AppSpawnReqMsgHandle reqHandle = nullptr;
    AppSpawnMsgType msgType = MSG_APP_SPAWN;
    AppSpawnRemoveSandboxDirMsg removeSandboxDirMsg = {
        .bundleName = "com.example.test"
    };
    int32_t ret = AppSpawnReqMsgCreate(msgType, removeSandboxDirMsg.bundleName.c_str(), &reqHandle);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number: RemoveSandboxDir
 * @tc.name: RemoveSandboxDir_0200
 * @tc.desc: int32_t BundleAppSpawnClient::RemoveSandboxDir
 * (const AppSpawnRemoveSandboxDirMsg &removeSandboxDirMsg)
 */
HWTEST_F(BmsBundleCommonTest, RemoveSandboxDir_0200, Function | SmallTest | Level1)
{
    AppSpawnClientHandle handle_ = nullptr;
    AppSpawnResult result = {1};
    AppSpawnReqMsgHandle reqHandle = nullptr;
    int32_t ret = AppSpawnClientSendMsg(handle_, reqHandle, &result);
    EXPECT_NE(ret, 0);
}
} // OHOS