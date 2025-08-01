/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "appexecfwk_errors.h"
#include "bundle_connect_ability_mgr.h"
#include "bundle_info.h"
#include "bundle_installer_proxy.h"
#include "bundle_mgr_service.h"
#include "bundle_mgr_proxy.h"
#include "bundle_pack_info.h"
#include "inner_bundle_info.h"
#include "install_result.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "scope_guard.h"
#include "service_center_connection.h"
#include "service_center_status_callback.h"
#include "perf_profile.h"
#include "want.h"

using namespace testing::ext;
using namespace std::chrono_literals;
using namespace OHOS::AppExecFwk;
using namespace OHOS::AAFwk;
using OHOS::AAFwk::Want;
using ::testing::_;
using ::testing::Invoke;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgReferee;

namespace OHOS {
namespace {
const std::string BUNDLE_NAME = "com.example.freeInstall";
const std::string BUNDLE_NAME_DEMO = "com.example.demo.freeInstall";
const std::string BUNDLE_NAME_EMPTY = "";
const std::string MODULE_NAME_TEST = "entry";
const std::string MODULE_NAME_TEST_ONE = "HAP1";
const std::string MODULE_NAME_TEST_TWO = "HAP2";
const std::string MODULE_NAME_EMPTY = "";
const std::string MODULE_NAME_NOT_EXIST = "notExist";
const std::string ABILITY_NAME_TEST = "MainAbility";
const std::string ABILITY_NAME_EMPTY = "";
const std::string DEVICE_ID = "PHONE-001";
const int32_t USERID = 100;
const int32_t OTHER_USERID = 101;
const int32_t WAIT_TIME = 5; // init mocked bms
const int32_t UPGRADE_FLAG = 1;
const int32_t FLAG_ONE = 1;
const int32_t FLAG_TWO = 2;
const int32_t INVALID_USER_ID = -1;
const std::string EMPTY_STRING = "";
const std::u16string SEEVICE_CENTER_CALLBACK_TOKEN = u"abilitydispatcherhm.openapi.hapinstall.IHapInstallCallback";
}  // namespace

class BmsBundleFreeInstallTest : public testing::Test {
public:
    BmsBundleFreeInstallTest();
    ~BmsBundleFreeInstallTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void AddInnerBundleInfo(const std::string bundleName, int32_t flag);
    void UpdateInnerBundleInfo(InnerBundleInfo &innerBundleInfo, int32_t flag);
    void UninstallBundleInfo(const std::string bundleName);
    BundlePackInfo CreateBundlePackInfo(const std::string &bundleName);
    const std::shared_ptr<BundleDataMgr> GetBundleDataMgr() const;
    const std::shared_ptr<BundleConnectAbilityMgr> GetBundleConnectAbilityMgr() const;
    static sptr<BundleMgrProxy> GetBundleMgrProxy();
    void StartBundleService();
    void ClearDataMgr();
    void ResetDataMgr();

private:
    static std::shared_ptr<BundleMgrService> bundleMgrService_;
};

std::shared_ptr<BundleMgrService> BmsBundleFreeInstallTest::bundleMgrService_ =
    DelayedSingleton<BundleMgrService>::GetInstance();

BmsBundleFreeInstallTest::BmsBundleFreeInstallTest()
{}

BmsBundleFreeInstallTest::~BmsBundleFreeInstallTest()
{}

void BmsBundleFreeInstallTest::SetUpTestCase()
{}

void BmsBundleFreeInstallTest::TearDownTestCase()
{
    bundleMgrService_->OnStop();
}

void BmsBundleFreeInstallTest::SetUp()
{
    StartBundleService();
    auto dataMgr = GetBundleDataMgr();
    if (dataMgr != nullptr) {
        dataMgr->AddUserId(USERID);
    }
}

void BmsBundleFreeInstallTest::TearDown()
{}

void BmsBundleFreeInstallTest::ClearDataMgr()
{
    bundleMgrService_->dataMgr_ = nullptr;
}

void BmsBundleFreeInstallTest::ResetDataMgr()
{
    bundleMgrService_->dataMgr_ = std::make_shared<BundleDataMgr>();
    EXPECT_NE(bundleMgrService_->dataMgr_, nullptr);
}

sptr<BundleMgrProxy> BmsBundleFreeInstallTest::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_LOGE("fail to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_LOGE("fail to get bundle manager proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle manager proxy success.");
    return iface_cast<BundleMgrProxy>(remoteObject);
}


void BmsBundleFreeInstallTest::UpdateInnerBundleInfo(InnerBundleInfo &innerBundleInfo, int32_t flag)
{
    InnerModuleInfo moduleInfo;
    moduleInfo.moduleName = MODULE_NAME_TEST_ONE;
    moduleInfo.name = MODULE_NAME_TEST_ONE;
    moduleInfo.modulePackage = MODULE_NAME_TEST_ONE;
    std::map<std::string, InnerModuleInfo> innerModuleInfoMap;
    innerModuleInfoMap[MODULE_NAME_TEST_ONE] = moduleInfo;
    innerBundleInfo.AddInnerModuleInfo(innerModuleInfoMap);

    std::vector<std::string> preloads;
    switch (flag) {
        case FLAG_ONE:
            preloads.emplace_back(MODULE_NAME_TEST_ONE);
            break;
        case FLAG_TWO:
            preloads.emplace_back(MODULE_NAME_TEST_ONE);
            preloads.emplace_back(MODULE_NAME_TEST_TWO);
            break;
        default:
            break;
    }
    auto ret = innerBundleInfo.SetInnerModuleAtomicPreload(MODULE_NAME_TEST, preloads);
    EXPECT_TRUE(ret);

    ret = innerBundleInfo.SetInnerModuleAtomicResizeable(MODULE_NAME_TEST, true);
    EXPECT_TRUE(ret);
}

void BmsBundleFreeInstallTest::AddInnerBundleInfo(const std::string bundleName, int32_t flag)
{
    BundleInfo bundleInfo;
    bundleInfo.name = bundleName;

    ApplicationInfo application;
    application.name = bundleName;
    application.bundleName = bundleName;

    InnerBundleUserInfo userInfo;
    userInfo.bundleName = bundleName;
    userInfo.bundleUserInfo.userId = USERID;

    InnerModuleInfo moduleInfo;
    moduleInfo.moduleName = MODULE_NAME_TEST;
    moduleInfo.name = MODULE_NAME_TEST;
    moduleInfo.modulePackage = MODULE_NAME_TEST;

    std::map<std::string, InnerModuleInfo> innerModuleInfoMap;
    innerModuleInfoMap[MODULE_NAME_TEST] = moduleInfo;

    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    innerBundleInfo.SetBaseApplicationInfo(application);
    innerBundleInfo.AddInnerBundleUserInfo(userInfo);
    innerBundleInfo.SetBundlePackInfo(CreateBundlePackInfo(bundleName));
    innerBundleInfo.AddInnerModuleInfo(innerModuleInfoMap);

    if (flag) {
        innerBundleInfo.SetApplicationBundleType(BundleType::ATOMIC_SERVICE);
        UpdateInnerBundleInfo(innerBundleInfo, flag);
    }
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);

    bool startRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::INSTALL_START);
    bool addRet = dataMgr->AddInnerBundleInfo(bundleName, innerBundleInfo);
    bool endRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::INSTALL_SUCCESS);

    EXPECT_TRUE(startRet);
    EXPECT_TRUE(addRet);
    EXPECT_TRUE(endRet);
}

BundlePackInfo BmsBundleFreeInstallTest::CreateBundlePackInfo(const std::string &bundleName)
{
    Packages packages;
    packages.name = bundleName;
    Summary summary;
    summary.app.bundleName = bundleName;
    PackageModule packageModule;
    packageModule.mainAbility = ABILITY_NAME_TEST;
    packageModule.distro.moduleName = MODULE_NAME_TEST;
    summary.modules.push_back(packageModule);

    BundlePackInfo packInfo;
    packInfo.packages.push_back(packages);
    packInfo.summary = summary;
    packInfo.SetValid(true);
    return packInfo;
}

void BmsBundleFreeInstallTest::UninstallBundleInfo(const std::string bundleName)
{
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    bool startRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_START);
    bool finishRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_SUCCESS);

    EXPECT_TRUE(startRet);
    EXPECT_TRUE(finishRet);
}

void BmsBundleFreeInstallTest::StartBundleService()
{
    if (!bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStart();
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
    }
}

const std::shared_ptr<BundleDataMgr> BmsBundleFreeInstallTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

const std::shared_ptr<BundleConnectAbilityMgr> BmsBundleFreeInstallTest::GetBundleConnectAbilityMgr() const
{
    auto bundleConnectAbility = bundleMgrService_->GetConnectAbility();
    EXPECT_NE(bundleConnectAbility, nullptr);
    return bundleConnectAbility;
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0001
 * Function: IsModuleRemovable
 * @tc.name: test IsModuleRemovable
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName and moduleName is empty
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0001, Function | SmallTest | Level0)
{
    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        bool isRemovable = false;
        ErrCode ret = bundleMgr->IsModuleRemovable("", "", isRemovable);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_PARAM_ERROR);
        EXPECT_FALSE(isRemovable);
    }
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0002
 * Function: IsModuleRemovable
 * @tc.name: test IsModuleRemovable
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName does not exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0002, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        bool isRemovable = false;
        ErrCode ret = bundleMgr->IsModuleRemovable(BUNDLE_NAME_DEMO, MODULE_NAME_TEST, isRemovable);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
        EXPECT_FALSE(isRemovable);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0003
 * Function: IsModuleRemovable
 * @tc.name: test IsModuleRemovable
 * @tc.require: issueI5MZ7R
 * @tc.desc: moduleName does not exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0003, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        bool isRemovable = false;
        ErrCode ret = bundleMgr->IsModuleRemovable(BUNDLE_NAME, MODULE_NAME_NOT_EXIST, isRemovable);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_MODULE_NOT_EXIST);
        EXPECT_FALSE(isRemovable);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0004
 * Function: IsModuleRemovable
 * @tc.name: test IsModuleRemovable
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName and moduleName both exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0004, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        bool isRemovable = false;
        ErrCode ret = bundleMgr->IsModuleRemovable(BUNDLE_NAME, MODULE_NAME_TEST, isRemovable);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_FALSE(isRemovable);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0005
 * Function: SetModuleRemovable
 * @tc.name: test SetModuleRemovable
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName and moduleName both exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0005, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        bool result = bundleMgr->SetModuleRemovable(BUNDLE_NAME, MODULE_NAME_TEST, true);
        EXPECT_TRUE(result);
        bool isRemovable = false;
        ErrCode ret = bundleMgr->IsModuleRemovable(BUNDLE_NAME, MODULE_NAME_TEST, isRemovable);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_TRUE(isRemovable);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0006
 * Function: SetModuleUpgradeFlag
 * @tc.name: test SetModuleUpgradeFlag
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName and moduleName are empty
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0006, Function | SmallTest | Level0)
{
    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        ErrCode ret = bundleMgr->SetModuleUpgradeFlag("", "", 0);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_PARAM_ERROR);
    }
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0007
 * Function: SetModuleUpgradeFlag
 * @tc.name: test SetModuleUpgradeFlag
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName not exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0007, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        ErrCode ret = bundleMgr->SetModuleUpgradeFlag(BUNDLE_NAME_DEMO, MODULE_NAME_TEST, 0);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0008
 * Function: SetModuleUpgradeFlag
 * @tc.name: test SetModuleUpgradeFlag
 * @tc.require: issueI5MZ7R
 * @tc.desc: moduleName not exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0008, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        ErrCode ret = bundleMgr->SetModuleUpgradeFlag(BUNDLE_NAME, MODULE_NAME_NOT_EXIST, 0);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_MODULE_NOT_EXIST);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0009
 * Function: SetModuleUpgradeFlag
 * @tc.name: test SetModuleUpgradeFlag
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName and moduleName both exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0009, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        ErrCode ret = bundleMgr->SetModuleUpgradeFlag(BUNDLE_NAME, MODULE_NAME_TEST, UPGRADE_FLAG);
        EXPECT_EQ(ret, ERR_OK);
        bool flag = bundleMgr->GetModuleUpgradeFlag(BUNDLE_NAME, MODULE_NAME_TEST);
        EXPECT_TRUE(flag);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0010
 * Function: GetModuleUpgradeFlag
 * @tc.name: test GetModuleUpgradeFlag
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName and moduleName are empty
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0010, Function | SmallTest | Level0)
{
    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        bool flag = bundleMgr->GetModuleUpgradeFlag("", "");
        EXPECT_FALSE(flag);
    }
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0011
 * Function: GetModuleUpgradeFlag
 * @tc.name: test GetModuleUpgradeFlag
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName not exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0011, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        bool flag = bundleMgr->GetModuleUpgradeFlag(BUNDLE_NAME_DEMO, MODULE_NAME_TEST);
        EXPECT_FALSE(flag);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0012
 * Function: GetModuleUpgradeFlag
 * @tc.name: test GetModuleUpgradeFlag
 * @tc.require: issueI5MZ7R
 * @tc.desc: moduleName not exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0012, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        bool flag = bundleMgr->GetModuleUpgradeFlag(BUNDLE_NAME, MODULE_NAME_NOT_EXIST);
        EXPECT_FALSE(flag);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0013
 * Function: GetBundlePackInfo
 * @tc.name: test GetBundlePackInfo
 * @tc.require: issueI5MZ7R
 * @tc.desc: bundleName not exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0013, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        BundlePackInfo packInfo;
        ErrCode ret = bundleMgr->GetBundlePackInfo(BUNDLE_NAME_DEMO,
            BundlePackFlag::GET_PACK_INFO_ALL, packInfo, USERID);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0014
 * Function: GetBundlePackInfo
 * @tc.name: test GetBundlePackInfo
 * @tc.require: issueI5MZ7R
 * @tc.desc: userId not exist
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0014, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        BundlePackInfo packInfo;
        ErrCode ret = bundleMgr->GetBundlePackInfo(BUNDLE_NAME,
            BundlePackFlag::GET_PACK_INFO_ALL, packInfo, INVALID_USER_ID);
        EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_INVALID_USER_ID);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0015
 * Function: GetBundlePackInfo
 * @tc.name: test GetBundlePackInfo
 * @tc.require: issueI5MZ7R
 * @tc.desc: test bundle pack flag GET_PACK_INFO_ALL
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0015, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        BundlePackInfo packInfo;
        ErrCode ret = bundleMgr->GetBundlePackInfo(BUNDLE_NAME,
            BundlePackFlag::GET_PACK_INFO_ALL, packInfo, USERID);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_FALSE(packInfo.packages.empty());
        if (!packInfo.packages.empty()) {
            EXPECT_EQ(packInfo.packages[0].name, BUNDLE_NAME);
        }

        EXPECT_EQ(packInfo.summary.app.bundleName, BUNDLE_NAME);
        EXPECT_FALSE(packInfo.summary.modules.empty());
        if (!packInfo.summary.modules.empty()) {
            EXPECT_EQ(packInfo.summary.modules[0].mainAbility, ABILITY_NAME_TEST);
            EXPECT_EQ(packInfo.summary.modules[0].distro.moduleName, MODULE_NAME_TEST);
        }
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0016
 * Function: GetBundlePackInfo
 * @tc.name: test GetBundlePackInfo
 * @tc.require: issueI5MZ7R
 * @tc.desc: test bundle pack flag GET_PACKAGES
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0016, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        BundlePackInfo packInfo;
        ErrCode ret  = bundleMgr->GetBundlePackInfo(BUNDLE_NAME,
            BundlePackFlag::GET_PACKAGES, packInfo, USERID);
        EXPECT_EQ(ret, ERR_OK);
        // GET_PACKAGES: include packages, not include summary
        EXPECT_FALSE(packInfo.packages.empty());
        if (!packInfo.packages.empty()) {
            EXPECT_EQ(packInfo.packages[0].name, BUNDLE_NAME);
        }
        EXPECT_EQ(packInfo.summary.app.bundleName, EMPTY_STRING);
        EXPECT_TRUE(packInfo.summary.modules.empty());
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0017
 * Function: GetBundlePackInfo
 * @tc.name: test GetBundlePackInfo
 * @tc.require: issueI5MZ7R
 * @tc.desc: test bundle pack flag GET_BUNDLE_SUMMARY
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0017, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        BundlePackInfo packInfo;
        ErrCode ret = bundleMgr->GetBundlePackInfo(BUNDLE_NAME,
            BundlePackFlag::GET_BUNDLE_SUMMARY, packInfo, USERID);
        EXPECT_EQ(ret, ERR_OK);
        // GET_PACKAGES: include summary, not include package
        EXPECT_TRUE(packInfo.packages.empty());

        EXPECT_FALSE(packInfo.summary.modules.empty());
        EXPECT_EQ(packInfo.summary.app.bundleName, BUNDLE_NAME);
        if (!packInfo.summary.modules.empty()) {
            EXPECT_EQ(packInfo.summary.modules[0].mainAbility, ABILITY_NAME_TEST);
            EXPECT_EQ(packInfo.summary.modules[0].distro.moduleName, MODULE_NAME_TEST);
        }
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0018
 * Function: GetBundlePackInfo
 * @tc.name: test GetBundlePackInfo
 * @tc.require: issueI5MZ7R
 * @tc.desc: test bundle pack flag GET_MODULE_SUMMARY
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0018, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleDataMgr();
    if (bundleMgr != nullptr) {
        BundlePackInfo packInfo;
        ErrCode ret = bundleMgr->GetBundlePackInfo(BUNDLE_NAME,
            BundlePackFlag::GET_MODULE_SUMMARY, packInfo, USERID);
        EXPECT_EQ(ret, ERR_OK);
        // GET_PACKAGES: include summary.modules, not include packages and summary.app
        EXPECT_TRUE(packInfo.packages.empty());
        EXPECT_EQ(packInfo.summary.app.bundleName, EMPTY_STRING);
        EXPECT_EQ(packInfo.summary.app.bundleName, EMPTY_STRING);

        EXPECT_FALSE(packInfo.summary.modules.empty());
        if (!packInfo.summary.modules.empty()) {
            EXPECT_EQ(packInfo.summary.modules[0].mainAbility, ABILITY_NAME_TEST);
            EXPECT_EQ(packInfo.summary.modules[0].distro.moduleName, MODULE_NAME_TEST);
        }
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0019
 * Function: SilentInstall
 * @tc.name: test SilentInstall
 * @tc.desc: test SilentInstall successed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0019, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    TargetAbilityInfo targetAbilityInfo;
    Want want;
    FreeInstallParams freeInstallParams;
    bool res = connectAbilityMgr->SilentInstall(targetAbilityInfo, want, freeInstallParams, USERID);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0021
 * Function: UpgradeInstall
 * @tc.name: test UpgradeInstall
 * @tc.desc: test UpgradeInstall successed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0021, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    TargetAbilityInfo targetAbilityInfo;
    Want want;
    FreeInstallParams freeInstallParams;
    bool res = connectAbilityMgr->UpgradeInstall(targetAbilityInfo, want, freeInstallParams, USERID);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0022
 * Function: SendRequestToServiceCenter
 * @tc.name: test SendRequestToServiceCenter
 * @tc.desc: test SendRequestToServiceCenter failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0022, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    ASSERT_NE(connectAbilityMgr, nullptr);
    int32_t flag = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
    TargetAbilityInfo targetAbilityInfo;
    Want want;
    FreeInstallParams freeInstallParams;
    bool res = connectAbilityMgr->SendRequestToServiceCenter(
        flag, targetAbilityInfo, want, USERID, freeInstallParams);
    EXPECT_FALSE(res);

    ClearDataMgr();
    ScopeGuard stateGuard([&] { ResetDataMgr(); });
    res = connectAbilityMgr->SendRequestToServiceCenter(flag, targetAbilityInfo, want, USERID, freeInstallParams);
    EXPECT_FALSE(res);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0023
 * Function: SendRequestToServiceCenter
 * @tc.name: test SendRequestToServiceCenter
 * @tc.desc: test SendRequestToServiceCenter failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0023, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    int32_t flag = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
    TargetAbilityInfo targetAbilityInfo;
    Want want;
    FreeInstallParams freeInstallParams;
    freeInstallParams.serviceCenterFunction = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
    bool res = connectAbilityMgr->SendRequestToServiceCenter(flag, targetAbilityInfo, want, USERID, freeInstallParams);
    EXPECT_FALSE(res);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    freeInstallParams.callback = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    connectAbilityMgr->freeInstallParamsMap_.insert(pair<std::string, FreeInstallParams>("1", freeInstallParams));
    connectAbilityMgr->DeathRecipientSendCallback();
    if (connectAbilityMgr->freeInstallParamsMap_.find("1") != connectAbilityMgr->freeInstallParamsMap_.end()) {
        connectAbilityMgr->freeInstallParamsMap_.erase("1");
    }
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0024
 * Function: OnServiceCenterCall
 * @tc.name: test OnServiceCenterCall
 * @tc.desc: test OnServiceCenterCall successsed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0024, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    FreeInstallParams freeInstallParams;
    connectAbilityMgr->freeInstallParamsMap_.insert(pair<std::string, FreeInstallParams>("1", freeInstallParams));
    std::string installResult = "{\"version\":\"1.0.0\", \"result\":{\"transactId\":\"1\","
        "\"resultMsg\":\"free install success\", \"retCode\":0}}";
    connectAbilityMgr->OnServiceCenterCall(installResult);
    EXPECT_NE(installResult, "");
    if (connectAbilityMgr->freeInstallParamsMap_.find("1") != connectAbilityMgr->freeInstallParamsMap_.end()) {
        connectAbilityMgr->freeInstallParamsMap_.erase("1");
    }
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0026
 * Function: OnServiceCenterCall
 * @tc.name: test OnServiceCenterCall
 * @tc.desc: test OnServiceCenterCall failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0026, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    FreeInstallParams freeInstallParams;
    connectAbilityMgr->freeInstallParamsMap_.insert(pair<std::string, FreeInstallParams>("1", freeInstallParams));
    std::string installResult = "{\"version\":\"1.0.0\", \"result\":{\"transactId\":\"1\","
        "\"resultMsg\":\"free install success\", \"retCode\":1}}";
    connectAbilityMgr->OnServiceCenterCall(installResult);
    if (connectAbilityMgr->freeInstallParamsMap_.find("1") != connectAbilityMgr->freeInstallParamsMap_.end()) {
        connectAbilityMgr->freeInstallParamsMap_.erase("1");
    }
    EXPECT_NE(installResult, "");
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0027
 * Function: OnServiceCenterCall
 * @tc.name: test OnServiceCenterCall
 * @tc.desc: test OnServiceCenterCall failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0027, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    FreeInstallParams freeInstallParams;
    freeInstallParams.serviceCenterFunction = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
    connectAbilityMgr->freeInstallParamsMap_.insert(pair<std::string, FreeInstallParams>("1", freeInstallParams));
    std::string installResult = "{\"version\":\"1.0.0\", \"result\":{\"transactId\":\"1\","
        "\"resultMsg\":\"free install success\", \"retCode\":-1}}";
    connectAbilityMgr->OnServiceCenterCall(installResult);
    EXPECT_NE(installResult, "");
    if (connectAbilityMgr->freeInstallParamsMap_.find("1") != connectAbilityMgr->freeInstallParamsMap_.end()) {
        connectAbilityMgr->freeInstallParamsMap_.erase("1");
    }
}


/**
 * @tc.number: BmsBundleFreeInstallTest_0028
 * Function: IsObtainAbilityInfo
 * @tc.name: test IsObtainAbilityInfo
 * @tc.desc: test IsObtainAbilityInfo failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0028, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    int32_t flag = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
    AbilityInfo abilityInfo;
    sptr<IRemoteObject> callBack;
    InnerBundleInfo innerBundleInfo;
    bool res = connectAbilityMgr->IsObtainAbilityInfo(want, flag, USERID, abilityInfo, callBack, innerBundleInfo);
    EXPECT_FALSE(res);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0029
 * Function: SendCallBack
 * @tc.name: test SendCallBack
 * @tc.desc: test SendCallBack failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0029, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    FreeInstallParams freeInstallParams;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    freeInstallParams.callback = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    connectAbilityMgr->freeInstallParamsMap_.insert(pair<std::string, FreeInstallParams>("1", freeInstallParams));
    int32_t resultCode = 0;
    Want want;
    int32_t userId = USERID;
    std::string transactId = "1";
    connectAbilityMgr->SendCallBack(resultCode, want, userId, transactId);
    EXPECT_EQ(transactId, "1");
    if (connectAbilityMgr->freeInstallParamsMap_.find(transactId) != connectAbilityMgr->freeInstallParamsMap_.end()) {
        connectAbilityMgr->freeInstallParamsMap_.erase(transactId);
    }
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0030
 * Function: SendCallBack
 * @tc.name: test SendCallBack
 * @tc.desc: test SendCallBack failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0030, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    FreeInstallParams freeInstallParams;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    freeInstallParams.callback = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    freeInstallParams.serviceCenterFunction = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
    connectAbilityMgr->freeInstallParamsMap_.insert(pair<std::string, FreeInstallParams>("1", freeInstallParams));
    int32_t resultCode = 1;
    Want want;
    int32_t userId = USERID;
    std::string transactId = "1";
    connectAbilityMgr->SendCallBack(resultCode, want, userId, transactId);
    EXPECT_EQ(transactId, "1");
    if (connectAbilityMgr->freeInstallParamsMap_.find("1") != connectAbilityMgr->freeInstallParamsMap_.end()) {
        connectAbilityMgr->freeInstallParamsMap_.erase("1");
    }
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0032
 * Function: BundleConnectAbilityMgr
 * @tc.name: test ProcessPreloadCheck
 * @tc.desc: test ProcessPreloadCheck failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0032, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    TargetAbilityInfo targetAbilityInfo;
    bool installResult = connectAbilityMgr->ProcessPreloadCheck(targetAbilityInfo);
    EXPECT_EQ(installResult, true);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0035
 * Function: GetPreloadList
 * @tc.name: test GetPreloadList
 * @tc.desc: test GetPreloadList failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0035, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    ClearDataMgr();
    sptr<TargetAbilityInfo> targetAbilityInfo;
    bool res = connectAbilityMgr->GetPreloadList(BUNDLE_NAME, MODULE_NAME_TEST, USERID, targetAbilityInfo);
    EXPECT_FALSE(res);
    ResetDataMgr();
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0036
 * Function: BundleConnectAbilityMgr
 * @tc.name: test ProcessPreload
 * @tc.desc: test ProcessPreload failed by moduleName
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0036, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 2);
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    want.SetElementName(DEVICE_ID, BUNDLE_NAME, ABILITY_NAME_TEST, MODULE_NAME_TEST_TWO);
    want.SetParam("uid", -800000);
    auto ret = connectAbilityMgr->ProcessPreload(want);
    EXPECT_FALSE(ret);
    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0038
 * Function: BundleConnectAbilityMgr
 * @tc.name: test ProcessPreload
 * @tc.desc: test ProcessPreload failed by not update InnerBundleInfo and wrong bundlName
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0038, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 1);
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    want.SetElementName(DEVICE_ID, BUNDLE_NAME_DEMO, ABILITY_NAME_TEST, MODULE_NAME_TEST);
    want.SetParam("uid", -800000);
    auto ret = connectAbilityMgr->ProcessPreload(want);
    EXPECT_FALSE(ret);
    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0040
 * Function: BundleConnectAbilityMgr
 * @tc.name: test ProcessPreload
 * @tc.desc: test ProcessPreload failed by not update InnerBundleInfo
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0040, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 1);
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    want.SetElementName(DEVICE_ID, BUNDLE_NAME, ABILITY_NAME_TEST, MODULE_NAME_TEST);
    want.SetParam("uid", -800000);
    auto ret = connectAbilityMgr->ProcessPreload(want);
    EXPECT_FALSE(ret);
    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0042
 * Function: BundleConnectAbilityMgr
 * @tc.name: test ProcessPreload
 * @tc.desc: test ProcessPreload failed by empty moduleName
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0042, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 2);
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    ElementName name;
    want.SetElementName(DEVICE_ID, BUNDLE_NAME, ABILITY_NAME_TEST, MODULE_NAME_EMPTY);
    want.SetParam("uid", -800000);
    auto ret = connectAbilityMgr->ProcessPreload(want);
    EXPECT_FALSE(ret);
    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BmsBundleFreeInstallTest_0043
 * Function: BundleConnectAbilityMgr
 * @tc.name: test ProcessPreload
 * @tc.desc: test ProcessPreload failed by not update InnerBundleInfo
 */
HWTEST_F(BmsBundleFreeInstallTest, BmsBundleFreeInstallTest_0043, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    want.SetElementName(DEVICE_ID, BUNDLE_NAME, ABILITY_NAME_TEST, MODULE_NAME_TEST);
    want.SetParam("uid", -800000);
    auto ret = connectAbilityMgr->ProcessPreload(want);
    EXPECT_FALSE(ret);
    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0001
 * Function: GetBundleConnectAbilityMgr
 * @tc.name: test GetBundleConnectAbilityMgr
 * @tc.require: issueI5MZ7R
 * @tc.desc: test QueryAbilityInfo
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0001, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleConnectAbilityMgr();
    if (bundleMgr != nullptr) {
        AbilityInfo abilityInfo;
        Want want;
        ElementName name;
        name.SetAbilityName(ABILITY_NAME_TEST);
        name.SetBundleName(BUNDLE_NAME);
        want.SetElement(name);
        bundleMgr->UpgradeAtomicService(want, USERID);
        int32_t connectState = 0;
        ffrt::condition_variable cv;
        const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
        ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
        bool ret = bundleMgr->QueryAbilityInfo(want,
            0, USERID, abilityInfo, connection.serviceCenterRemoteObject_);
        EXPECT_EQ(ret, false);
        bundleMgr->DeathRecipientSendCallback();
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0003
 * Function: GetBundleConnectAbilityMgr
 * @tc.name: test GetBundleConnectAbilityMgr
 * @tc.require: issueI5MZ7R
 * @tc.desc: test ConnectAbility
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0003, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleConnectAbilityMgr();
    if (bundleMgr != nullptr) {
        Want want;
        ElementName name;
        name.SetAbilityName(ABILITY_NAME_TEST);
        name.SetBundleName(BUNDLE_NAME);
        want.SetElement(name);
        bundleMgr->UpgradeAtomicService(want, USERID);
        bundleMgr->DisconnectDelay();
        int32_t connectState = 0;
        ffrt::condition_variable cv;
        const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
        ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
        bool ret = bundleMgr->ConnectAbility(want, connection.serviceCenterRemoteObject_);
        EXPECT_FALSE(ret);
        bundleMgr->DisconnectAbility();
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0004
 * Function: GetBundleConnectAbilityMgr
 * @tc.name: test GetBundleConnectAbilityMgr
 * @tc.require: issueI5MZ7R
 * @tc.desc: test CheckIsModuleNeedUpdate
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0004, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleConnectAbilityMgr();
    if (bundleMgr != nullptr) {
        Want want;
        ElementName name;
        name.SetAbilityName(ABILITY_NAME_TEST);
        name.SetBundleName(BUNDLE_NAME);
        want.SetElement(name);
        InnerBundleInfo innerBundleInfo;
        ffrt::condition_variable cv;
        int32_t connectState = 0;
        const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
        ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
        bool ret = bundleMgr->CheckIsModuleNeedUpdate(
            innerBundleInfo, want, 100, connection.serviceCenterRemoteObject_);
        EXPECT_FALSE(ret);
        ApplicationInfo appInfo;
        appInfo.bundleName = BUNDLE_NAME;
        innerBundleInfo.SetBaseApplicationInfo(appInfo);
        ret = bundleMgr->CheckIsModuleNeedUpdate(
            innerBundleInfo, want, 100, connection.serviceCenterRemoteObject_);
        EXPECT_FALSE(ret);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0005
 * Function: GetBundleConnectAbilityMgr
 * @tc.name: test GetBundleConnectAbilityMgr
 * @tc.require: issueI5MZ7R
 * @tc.desc: test UpgradeInstall and UpgradeCheck
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0005, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleConnectAbilityMgr();
    if (bundleMgr != nullptr) {
        TargetAbilityInfo targetAbilityInfo;
        FreeInstallParams freeInstallParams;
        Want want;
        ElementName name;
        name.SetAbilityName(ABILITY_NAME_TEST);
        name.SetBundleName(BUNDLE_NAME);
        want.SetElement(name);
        bool res = bundleMgr->UpgradeInstall(targetAbilityInfo, want, freeInstallParams, 100);
        EXPECT_EQ(res, true);
        res = bundleMgr->UpgradeCheck(targetAbilityInfo, want, freeInstallParams, 100);
        EXPECT_EQ(res, true);
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0003
 * Function: GetBundleConnectAbilityMgr
 * @tc.name: test GetBundleConnectAbilityMgr
 * @tc.require: issueI5MZ7R
 * @tc.desc: test SendCallBack
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0006, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleConnectAbilityMgr();
    if (bundleMgr != nullptr) {
        std::string transactId;
        Want want;
        ElementName name;
        name.SetAbilityName(ABILITY_NAME_TEST);
        name.SetBundleName(BUNDLE_NAME);
        want.SetElement(name);
        bundleMgr->SendCallBack(0, want, 100, transactId);
        EXPECT_EQ(transactId, "");
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0009
 * Function: SendCallBack and SendRequest
 * @tc.name: test SendCallBack and SendRequest
 * @tc.require: issueI5MZ7R
 * @tc.desc: test SendCallBack and SendRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0009, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto bundleMgr = GetBundleConnectAbilityMgr();
    if (bundleMgr != nullptr) {
        Want want;
        ElementName name;
        name.SetAbilityName(ABILITY_NAME_TEST);
        name.SetBundleName(BUNDLE_NAME);
        want.SetElement(name);
        TargetAbilityInfo targetAbilityInfo;
        FreeInstallParams freeInstallParams;
        freeInstallParams.callback = nullptr;
        bundleMgr->SendRequest(
            0, targetAbilityInfo, want, USERID, freeInstallParams);
        bundleMgr->SendCallBack(
            targetAbilityInfo.targetInfo.transactId, freeInstallParams);
        EXPECT_EQ(freeInstallParams.callback, nullptr);
        EXPECT_EQ(targetAbilityInfo.targetInfo.transactId, "");
    }

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0011
 * Function: CheckIsModuleNeedUpdate
 * @tc.name: test CheckIsModuleNeedUpdate
 * @tc.desc: test CheckIsModuleNeedUpdate failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0011, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, 0);

    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    InnerBundleInfo innerBundleInfo;
    Want want;
    ElementName name;
    name.SetAbilityName(ABILITY_NAME_TEST);
    name.SetBundleName(BUNDLE_NAME);
    want.SetElement(name);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> callBack = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    bool res = connectAbilityMgr->CheckIsModuleNeedUpdate(innerBundleInfo, want, USERID, callBack);
    EXPECT_FALSE(res);

    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0015
 * Function: OutTimeMonitor
 * @tc.name: test OutTimeMonitor
 * @tc.desc: test OutTimeMonitor
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0015, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    FreeInstallParams freeInstallParams;
    connectAbilityMgr->freeInstallParamsMap_.insert(pair<std::string, FreeInstallParams>("1", freeInstallParams));
    std::string transactId = "1";
    connectAbilityMgr->OutTimeMonitor(transactId);
    EXPECT_EQ(transactId, "1");
    if (connectAbilityMgr->freeInstallParamsMap_.find("1") != connectAbilityMgr->freeInstallParamsMap_.end()) {
        connectAbilityMgr->freeInstallParamsMap_.erase("1");
    }
}

/**
 * @tc.number: BundleConnectAbilityMgr_0017
 * Function: CheckIsModuleNeedUpdate
 * @tc.name: test CheckIsModuleNeedUpdate
 * @tc.desc: test CheckIsModuleNeedUpdate success
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0017, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    InnerBundleInfo innerBundleInfo;
    InnerModuleInfo innerModuleInfo;
    innerModuleInfo.moduleName = ".entry";
    innerModuleInfo.upgradeFlag = 1;
    innerBundleInfo.innerModuleInfos_.insert(pair<std::string, InnerModuleInfo>("1", innerModuleInfo));
    Want want;
    want.SetModuleName(".entry");
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> callBack = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    bool res = connectAbilityMgr->CheckIsModuleNeedUpdate(innerBundleInfo, want, USERID, callBack);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0018
 * Function: ExistBundleNameInCallingBundles
 * @tc.desc: test ExistBundleNameInCallingBundles by GetTargetAbilityInfo
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0018, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    std::vector<std::string> callingBundleNames;
    Want want;
    ElementName name;
    name.SetAbilityName(ABILITY_NAME_TEST);
    name.SetBundleName(BUNDLE_NAME);
    want.SetElement(name);
    InnerBundleInfo innerBundleInfo;
    sptr<TargetAbilityInfo> targetAbilityInfo = new(std::nothrow) TargetAbilityInfo();
    connectAbilityMgr->GetTargetAbilityInfo(want, 0, innerBundleInfo, targetAbilityInfo);
    EXPECT_EQ(targetAbilityInfo->targetInfo.bundleName, BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0019
 * Function: GetBundleConnectAbilityMgr
 * @tc.name: test GetBundleConnectAbilityMgr
 * @tc.desc: test GetCallingInfo
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0019, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    std::vector<std::string> bundleNames;
    std::vector<std::string> callingAppIds;
    ClearDataMgr();
    connectAbilityMgr->GetCallingInfo(USERID, USERID, bundleNames, callingAppIds);
    EXPECT_EQ(bundleNames.size(), 0);
    ResetDataMgr();
}

/**
 * @tc.number: BundleConnectAbilityMgr_0020
 * Function: GetBundleConnectAbilityMgr
 * @tc.name: test GetBundleConnectAbilityMgr
 * @tc.desc: test GetCallingInfo
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0020, Function | SmallTest | Level0)
{
    AddInnerBundleInfo(BUNDLE_NAME, FLAG_ONE);
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    std::vector<std::string> bundleNames;
    bundleNames.push_back(BUNDLE_NAME);
    std::vector<std::string> callingAppIds;
    connectAbilityMgr->GetCallingInfo(USERID, OTHER_USERID, bundleNames, callingAppIds);
    EXPECT_EQ(bundleNames.size(), 1);
    UninstallBundleInfo(BUNDLE_NAME);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0021
 * Function: IsObtainAbilityInfo
 * @tc.name: test IsObtainAbilityInfo
 * @tc.desc: test IsObtainAbilityInfo failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0021, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    ElementName name;
    name.SetAbilityName(ABILITY_NAME_TEST);
    name.SetBundleName(BUNDLE_NAME);
    want.SetElement(name);
    int32_t flag = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
    AbilityInfo abilityInfo;
    sptr<IRemoteObject> callBack;
    InnerBundleInfo innerBundleInfo;
    ClearDataMgr();
    connectAbilityMgr->UpgradeAtomicService(want, USERID);
    bool res = connectAbilityMgr->IsObtainAbilityInfo(want, flag, USERID, abilityInfo, callBack, innerBundleInfo);
    EXPECT_FALSE(res);
    ResetDataMgr();
}

/**
 * @tc.number: BundleConnectAbilityMgr_0023
 * @tc.name: CheckIsModuleNeedUpdate
 * @tc.desc: Check Is Module Need Update
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0023, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    InnerBundleInfo innerBundleInfo;
    std::string key = "key";
    InnerAbilityInfo innerAbilityInfo;
    innerAbilityInfo.name = "abilityName";
    innerBundleInfo.InsertAbilitiesInfo(key, innerAbilityInfo);

    Want want;
    ElementName name;
    name.SetAbilityName("abilityName");
    want.SetElement(name);
    sptr<IRemoteObject> callBack = nullptr;
    bool ret = connectAbilityMgr->CheckIsModuleNeedUpdate(innerBundleInfo, want, OTHER_USERID, callBack);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: BundleConnectAbilityMgr_0024
 * Function: IsObtainAbilityInfo
 * @tc.name: test IsObtainAbilityInfo
 * @tc.desc: test IsObtainAbilityInfo failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0024, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    ElementName name;
    name.SetAbilityName(ABILITY_NAME_TEST);
    name.SetBundleName(BUNDLE_NAME);
    name.SetModuleName(MODULE_NAME_TEST);
    want.SetElement(name);
    int32_t flag = ServiceCenterFunction::CONNECT_UPGRADE_INSTALL;
    AbilityInfo abilityInfo;
    sptr<IRemoteObject> callBack;
    InnerBundleInfo innerBundleInfo;
    ClearDataMgr();
    connectAbilityMgr->UpgradeAtomicService(want, USERID);
    bool res = connectAbilityMgr->IsObtainAbilityInfo(want, flag, USERID, abilityInfo, callBack, innerBundleInfo);
    EXPECT_FALSE(res);
    ResetDataMgr();
}

/**
 * @tc.number: BundleConnectAbilityMgr_0026
 * Function: BundleConnectAbilityMgr
 * @tc.name: test CheckDependencies
 * @tc.desc: test CheckDependencies failed
 */
HWTEST_F(BmsBundleFreeInstallTest, BundleConnectAbilityMgr_0026, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    ElementName name;
    name.SetAbilityName(ABILITY_NAME_TEST);
    name.SetBundleName(BUNDLE_NAME);
    want.SetElement(name);
    ErmsCallerInfo callerInfo;
    ClearDataMgr();
    connectAbilityMgr->GetEcologicalCallerInfo(want, callerInfo, USERID);
    EXPECT_EQ(callerInfo.uid, 0);
    ResetDataMgr();
}

/**
 * @tc.number: OnAbilityConnectDone_0001
 * Function: OnAbilityConnectDone
 * @tc.name: test OnAbilityConnectDone
 * @tc.desc: test OnAbilityConnectDone failed
 */
HWTEST_F(BmsBundleFreeInstallTest, OnAbilityConnectDone_0001, Function | SmallTest | Level0)
{
    int32_t connectState = 0;
    ffrt::condition_variable cv;
    const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
    ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
    ElementName element;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    int32_t resultCode = 1;
    connection.OnAbilityConnectDone(element, remoteObject, resultCode);
    EXPECT_TRUE(resultCode);
}

/**
 * @tc.number: OnAbilityConnectDone_0002
 * Function: OnAbilityConnectDone
 * @tc.name: test OnAbilityConnectDone
 * @tc.desc: test OnAbilityConnectDone failed
 */
HWTEST_F(BmsBundleFreeInstallTest, OnAbilityConnectDone_0002, Function | SmallTest | Level0)
{
    int32_t connectState = 0;
    ffrt::condition_variable cv;
    const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
    ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
    ElementName element;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = nullptr;
    int32_t resultCode = 0;
    connection.OnAbilityConnectDone(element, remoteObject, resultCode);
    EXPECT_FALSE(resultCode);
}

/**
 * @tc.number: OnAbilityConnectDone_0003
 * Function: OnAbilityConnectDone
 * @tc.name: test OnAbilityConnectDone
 * @tc.desc: test OnAbilityConnectDone success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnAbilityConnectDone_0003, Function | SmallTest | Level0)
{
    int32_t connectState = 0;
    ffrt::condition_variable cv;
    const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
    ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
    ElementName element;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    int32_t resultCode = 0;
    connection.OnAbilityConnectDone(element, remoteObject, resultCode);
    EXPECT_FALSE(resultCode);
}

/**
 * @tc.number: OnRemoteRequest_0001
 * Function: OnRemoteRequest
 * @tc.name: test OnRemoteRequest
 * @tc.desc: test OnRemoteRequest success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnRemoteRequest_0001, Function | SmallTest | Level0)
{
    const std::weak_ptr<BundleConnectAbilityMgr> server;
    ServiceCenterDeathRecipient recipient(server);
    wptr<IRemoteObject> wptrDeath;
    recipient.OnRemoteDied(wptrDeath);

    ServiceCenterStatusCallback callbackStub(server);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    callbackStub.OnRemoteRequest(code, data, reply, option);

    std::string installResult = "";
    callbackStub.OnInstallFinished(installResult);
    EXPECT_FALSE(code);
}

/**
 * @tc.number: PerfProfile_0100
 * Function: GetAppForkEndTime
 * @tc.desc: test GetAppForkEndTime of PerfProfile
 */
HWTEST_F(BmsBundleFreeInstallTest, PerfProfile_0100, Function | SmallTest | Level0)
{
    PerfProfile profile;
    int64_t ret = profile.GetAbilityLoadEndTime();
    EXPECT_EQ(ret, 0);
    ret = profile.GetAppForkEndTime();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number: InstallResult_0100
 * Function: Unmarshalling
 * @tc.desc: test Unmarshalling of InstallResult
 */
HWTEST_F(BmsBundleFreeInstallTest, InstallResult_0100, Function | SmallTest | Level0)
{
    InstallResult installResult;
    installResult.version = "1.0";
    Parcel parcel;
    InstallResult result;
    bool ret1 = installResult.Marshalling(parcel);
    EXPECT_EQ(ret1, true);
    result.Unmarshalling(parcel);
    installResult.ReadFromParcel(parcel);
    EXPECT_EQ(installResult.version, result.version);
}

/**
 * @tc.number: WriteFileToStream_0100
 * @tc.name: test WriteFileToStream
 * @tc.desc: 1.test WriteFileToStream of BundleInstallerProxy
 */
HWTEST_F(BmsBundleFreeInstallTest, WriteFileToStream_0100, Function | SmallTest | Level0)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    BundleInstallerProxy installerProxy(remoteObject);
    sptr<IBundleStreamInstaller> streamInstaller;
    std::string path = "";
    ErrCode ret = installerProxy.WriteHapFileToStream(streamInstaller, path);
    EXPECT_EQ(ret, ERR_APPEXECFWK_NULL_PTR);

    streamInstaller = iface_cast<IBundleStreamInstaller>(remoteObject);
    EXPECT_NE(streamInstaller, nullptr);
    ret = installerProxy.WriteHapFileToStream(streamInstaller, path);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID);

    path = "/";
    ret = installerProxy.WriteHapFileToStream(streamInstaller, path);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID);
}

/**
 * @tc.number: OnRemoteRequestTest_0001
 * Function: OnRemoteRequest
 * @tc.name: test OnRemoteRequest
 * @tc.desc: test OnRemoteRequest success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnRemoteRequestTest_0001, Function | SmallTest | Level0)
{
    std::shared_ptr<BundleConnectAbilityMgr> server = std::make_shared<BundleConnectAbilityMgr>();
    ServiceCenterStatusCallback callbackStub(server);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto result = callbackStub.OnRemoteRequest(code, data, reply, option);

    EXPECT_EQ(result, -1);
}

/**
 * @tc.number: OnRemoteRequestTest_0002
 * Function: OnRemoteRequest
 * @tc.name: test OnRemoteRequest
 * @tc.desc: test OnRemoteRequest success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnRemoteRequestTest_0002, Function | SmallTest | Level0)
{
    std::shared_ptr<BundleConnectAbilityMgr> server = nullptr;
    ServiceCenterStatusCallback callbackStub(server);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SEEVICE_CENTER_CALLBACK_TOKEN);
    auto result = callbackStub.OnRemoteRequest(code, data, reply, option);

    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.number: OnRemoteRequestTest_0003
 * Function: OnRemoteRequest
 * @tc.name: test OnRemoteRequest
 * @tc.desc: test OnRemoteRequest success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnRemoteRequestTest_0003, Function | SmallTest | Level0)
{
    std::shared_ptr<BundleConnectAbilityMgr> server = std::make_shared<BundleConnectAbilityMgr>();
    ServiceCenterStatusCallback callbackStub(server);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SEEVICE_CENTER_CALLBACK_TOKEN);
    data.WriteString("OK");
    auto result = callbackStub.OnRemoteRequest(code, data, reply, option);

    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.number:OnRemoteDied_0001
 * Function: OnRemoteDied
 * @tc.name: test OnRemoteDied
 * @tc.desc: test OnRemoteDied success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnRemoteDied_0001, Function | SmallTest | Level0)
{
    const std::weak_ptr<BundleConnectAbilityMgr> server;
    ServiceCenterDeathRecipient recipient(server);
    wptr<IRemoteObject> wptrDeath;
    recipient.OnRemoteDied(wptrDeath);
    EXPECT_TRUE(recipient.connectAbilityMgr_.lock() == nullptr);
}

/**
 * @tc.number:OnRemoteDied_0002
 * Function: OnRemoteDied
 * @tc.name: test OnRemoteDied
 * @tc.desc: test OnRemoteDied success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnRemoteDied_0002, Function | SmallTest | Level0)
{
    std::shared_ptr<BundleConnectAbilityMgr> server = std::make_shared<BundleConnectAbilityMgr>();
    ServiceCenterDeathRecipient recipient(server);
    wptr<IRemoteObject> wptrDeath;
    recipient.OnRemoteDied(wptrDeath);
    EXPECT_TRUE(recipient.connectAbilityMgr_.lock() != nullptr);
}

/**
 * @tc.number: OnRemoteRequest_0001
 * Function: OnRemoteRequest
 * @tc.name: test OnRemoteRequest
 * @tc.desc: test OnRemoteRequest success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnInstallFinished_0001, Function | SmallTest | Level0)
{
    std::shared_ptr<BundleConnectAbilityMgr> server = std::make_shared<BundleConnectAbilityMgr>();
    ServiceCenterStatusCallback callbackStub(server);
    std::string installResult = "ok";
    auto result = callbackStub.OnInstallFinished(installResult);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.number: OnInstallFinished_0002
 * Function: OnInstallFinished
 * @tc.name: test OnInstallFinished
 * @tc.desc: test OnInstallFinished success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnInstallFinished_0002, Function | SmallTest | Level0)
{
    const std::weak_ptr<BundleConnectAbilityMgr> server1;
    ServiceCenterStatusCallback callbackStub(server1);
    std::string installResult = "";
    auto result = callbackStub.OnInstallFinished(installResult);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.number: OnInstallFinished_0003
 * Function: OnInstallFinished
 * @tc.name: test OnInstallFinished
 * @tc.desc: test OnInstallFinished success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnInstallFinished_0003, Function | SmallTest | Level0)
{
    std::weak_ptr<BundleConnectAbilityMgr> server = std::make_shared<BundleConnectAbilityMgr>();
    ServiceCenterStatusCallback callbackStub(server);
    std::string installResult = "ok";
    auto result = callbackStub.OnInstallFinished(installResult);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/*
 * @tc.number: IsReachEndAgingThreshold_0100
 * @tc.name: test IsReachEndAgingThreshold
 * @tc.desc: 1.test IsReachEndAgingThreshold of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, IsReachEndAgingThreshold_0100, Function | SmallTest | Level0)
{
    AgingRequest request;
    AgingBundleInfo bundleInfo;
    request.AddAgingBundle(bundleInfo);
    bool ret = request.IsReachStartAgingThreshold();
    EXPECT_EQ(ret, false);
    ret = request.IsReachEndAgingThreshold();
    EXPECT_EQ(ret, true);
}

/*
 * @tc.number: SortAgingBundles_0100
 * @tc.name: test SortAgingBundles
 * @tc.desc: 1.test SortAgingBundles of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, SortAgingBundles_0100, Function | SmallTest | Level0)
{
    AgingRequest request;

    size_t ret = request.SortAgingBundles();
    EXPECT_EQ(ret, request.GetAgingBundles().size());
}

/*
 * @tc.number: SortAgingBundles_0200
 * @tc.name: test SortAgingBundles
 * @tc.desc: 1.test SortAgingBundles of AgingUtil
 */
HWTEST_F(BmsBundleFreeInstallTest, SortAgingBundles_0200, Function | SmallTest | Level0)
{
    AgingUtil util;
    AgingBundleInfo bundleInfo;
    std::vector<AgingBundleInfo> bundles;
    bundles.push_back(bundleInfo);
    bundles.push_back(bundleInfo);
    util.SortAgingBundles(bundles);
    EXPECT_FALSE(bundles.empty());
}

/*
 * @tc.number: ResetRequest_0100
 * @tc.name: test ResetRequest
 * @tc.desc: 1.test ResetRequest of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, ResetRequest_0100, Function | SmallTest | Level0)
{
    AgingRequest request;

    request.Dump();
    request.ResetRequest();
    EXPECT_EQ(request.GetAgingBundles().size(), 0);
}

/*
 * @tc.number: SetTotalDataBytes_0100
 * @tc.name: test SetTotalDataBytes
 * @tc.desc: 1.test SetTotalDataBytes of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, SetTotalDataBytes_0100, Function | SmallTest | Level0)
{
    AgingRequest request;

    int64_t dataBytes = 100;
    request.SetTotalDataBytes(dataBytes);
    EXPECT_EQ(request.GetTotalDataBytes(), dataBytes);

    request.UpdateTotalDataBytesAfterUninstalled(dataBytes);
    EXPECT_EQ(request.GetAgingBundles().size(), 0);
}

/*
 * @tc.number: SetAgingCleanType_0100
 * @tc.name: test SetAgingCleanType
 * @tc.desc: 1.test SetAgingCleanType of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, SetAgingCleanType_0100, Function | SmallTest | Level0)
{
    AgingRequest request;

    request.SetAgingCleanType(AgingCleanType::CLEAN_OTHERS);
    EXPECT_EQ(request.GetAgingCleanType(), AgingCleanType::CLEAN_OTHERS);
}

/*
 * @tc.number: GetTotalDataBytesThreshold_0100
 * @tc.name: test GetTotalDataBytesThreshold
 * @tc.desc: 1.test GetTotalDataBytesThreshold of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, GetTotalDataBytesThreshold_0100, Function | SmallTest | Level0)
{
    EXPECT_EQ(AgingConstants::DEFAULT_AGING_DATA_SIZE_THRESHOLD, AgingRequest::GetTotalDataBytesThreshold());
}

/*
 * @tc.number: GetOneDayTimeMs_0100
 * @tc.name: test GetOneDayTimeMs
 * @tc.desc: 1.test GetOneDayTimeMs of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, GetOneDayTimeMs_0100, Function | SmallTest | Level0)
{
    EXPECT_EQ(AgingConstants::ONE_DAYS_MS, AgingRequest::GetOneDayTimeMs());
}

/**
 * @tc.number: Process_0100
 * @tc.name: test Process
 * @tc.desc: 1.test Process of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, Process_0100, Function | SmallTest | Level0)
{
    AgingRequest request;
    AgingHandlerChain chain;
    chain.AddHandler(nullptr);
    bool ret = chain.Process(request);
    EXPECT_EQ(ret, true);
    request.totalDataBytes_ = AppExecFwk::AgingRequest::totalDataBytesThreshold_ + 1;
    std::shared_ptr<AgingHandler> handler;
    chain.AddHandler(handler);
    ret = chain.Process(request);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.number: Request_0100
 * @tc.name: test Request
 * @tc.desc: 1.test Request of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, Request_0100, Function | SmallTest | Level0)
{
    BundleAgingMgr bundleAgingMgr;
    bool ret = bundleAgingMgr.InitAgingRequest();
    EXPECT_EQ(ret, false);
    bundleAgingMgr.request_.totalDataBytes_ =
        AppExecFwk::AgingRequest::totalDataBytesThreshold_ + 1;
    ret = bundleAgingMgr.InitAgingRequest();
    EXPECT_EQ(ret, false);
    ret = bundleAgingMgr.ResetRequest();
    EXPECT_EQ(ret, true);
    ret = bundleAgingMgr.IsReachStartAgingThreshold();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.number: QueryBundleStatsInfoByInterval_0100
 * @tc.name: test QueryBundleStatsInfoByInterval
 * @tc.desc: 1.test QueryBundleStatsInfoByInterval of AgingRequest
 */
HWTEST_F(BmsBundleFreeInstallTest, QueryBundleStatsInfoByInterval_0100, Function | SmallTest | Level0)
{
    BundleAgingMgr bundleAgingMgr;

    std::vector<DeviceUsageStats::BundleActivePackageStats> results;
    DeviceUsageStats::BundleActivePackageStats stats;
    results.push_back(stats);
    bool ret = bundleAgingMgr.QueryBundleStatsInfoByInterval(results);
    EXPECT_EQ(ret, true);
}

/**
* @tc.number: OnAbilityDisconnectDone_0100
* @tc.name: test OnAbilityDisconnectDone
* @tc.desc: 1.Verify the OnAbilityDisconnectDone function, serviceCenterRemoteObject_& deathRecipient_ instantiation
 */
HWTEST_F(BmsBundleFreeInstallTest, OnAbilityDisconnectDone_0100, Function | SmallTest | Level0)
{
    int32_t connectState = 0;
    ffrt::condition_variable cv;
    const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
    ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
    ElementName element;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    int32_t resultCode = 1;
    EXPECT_TRUE(remoteObject != nullptr);
    connection.serviceCenterRemoteObject_ = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    connection.deathRecipient_ = new (std::nothrow) ServiceCenterDeathRecipient(connection.connectAbilityMgr_);
    EXPECT_TRUE(connection.deathRecipient_ != nullptr);
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ != nullptr);
    connection.OnAbilityDisconnectDone(element, resultCode);
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
    connection.GetRemoteObject();
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
}

/**
* @tc.number: OnAbilityDisconnectDone_0200
* @tc.name: test OnAbilityDisconnectDone
* @tc.desc: 1.Verify the OnAbilityDisconnectDone function, serviceCenterRemoteObject_ instantiation
 */
HWTEST_F(BmsBundleFreeInstallTest, OnAbilityDisconnectDone_0200, Function | SmallTest | Level0)
{
    int32_t connectState = 0;
    ffrt::condition_variable cv;
    const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
    ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
    ElementName element;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    int32_t resultCode = 1;
    EXPECT_TRUE(remoteObject != nullptr);
    connection.serviceCenterRemoteObject_ = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    EXPECT_TRUE(connection.deathRecipient_ == nullptr);
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ != nullptr);
    connection.OnAbilityDisconnectDone(element, resultCode);
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
        connection.GetRemoteObject();
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
}

/**
* @tc.number: OnAbilityDisconnectDone_0300
* @tc.name: test OnAbilityDisconnectDone
* @tc.desc: 1.Verify the OnAbilityDisconnectDone function, deathRecipient_ instantiation
 */
HWTEST_F(BmsBundleFreeInstallTest, OnAbilityDisconnectDone_0300, Function | SmallTest | Level0)
{
    int32_t connectState = 0;
    ffrt::condition_variable cv;
    const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
    ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
    ElementName element;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    int32_t resultCode = 1;
    EXPECT_TRUE(remoteObject != nullptr);
    connection.deathRecipient_ = new (std::nothrow) ServiceCenterDeathRecipient(connection.connectAbilityMgr_);
    EXPECT_TRUE(connection.deathRecipient_ != nullptr);
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
    connection.OnAbilityDisconnectDone(element, resultCode);
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
    connection.GetRemoteObject();
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
}

/**
* @tc.number: OnAbilityDisconnectDone_0400
* @tc.name: test OnAbilityDisconnectDone
* @tc.desc: 1.Verify the OnAbilityDisconnectDone function, serviceCenterRemoteObject_& deathRecipient_ Uninstantiated
*/
HWTEST_F(BmsBundleFreeInstallTest, OnAbilityDisconnectDone_0400, Function | SmallTest | Level0)
{
    int32_t connectState = 0;
    ffrt::condition_variable cv;
    const std::weak_ptr<BundleConnectAbilityMgr> connectAbilityMgr;
    ServiceCenterConnection connection(connectState, cv, connectAbilityMgr);
    ElementName element;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    int32_t resultCode = 1;
    EXPECT_TRUE(remoteObject != nullptr);
    EXPECT_TRUE(connection.deathRecipient_ == nullptr);
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
    connection.OnAbilityDisconnectDone(element, resultCode);
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
    connection.GetRemoteObject();
    EXPECT_TRUE(connection.serviceCenterRemoteObject_ == nullptr);
}

/**
 * @tc.number: CheckEcologicalRule_0001
 * Function: CheckEcologicalRule
 * @tc.name: CheckEcologicalRule
 * @tc.desc: Check Ecological Rule failed
 */
HWTEST_F(BmsBundleFreeInstallTest, CheckEcologicalRule_0001, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    ElementName name;
    name.SetAbilityName("abilityName");
    name.SetBundleName("bundleName");
    want.SetElement(name);
    ErmsCallerInfo callerInfo;
    BmsExperienceRule rule;
    bool ret = connectAbilityMgr->CheckEcologicalRule(want, callerInfo, rule);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: GetEcologicalCallerInfo_0001
 * Function: GetEcologicalCallerInfo
 * @tc.name: test GetEcologicalCallerInfo
 * @tc.desc: test GetEcologicalCallerInfo success
 */
HWTEST_F(BmsBundleFreeInstallTest, GetEcologicalCallerInfo_0001, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    ErmsCallerInfo callerInfo;
    std::string callerBundleName;

    setuid(1);
    connectAbilityMgr->GetEcologicalCallerInfo(want, callerInfo, USERID);
    auto bundleDataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->dataMgr_;
    bool result = bundleDataMgr_->GetNameForUid(IPCSkeleton::GetCallingUid(), callerBundleName);
    EXPECT_EQ(result, true);
}

/**
 * @tc.number: GetEcologicalCallerInfo_0002
 * Function: GetEcologicalCallerInfo
 * @tc.name: test GetEcologicalCallerInfo
 * @tc.desc: test GetEcologicalCallerInfo success
 */
HWTEST_F(BmsBundleFreeInstallTest, GetEcologicalCallerInfo_0002, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    Want want;
    ErmsCallerInfo callerInfo;

    auto bundleDataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->dataMgr_;
    DelayedSingleton<BundleMgrService>::GetInstance()->dataMgr_ = nullptr;
    connectAbilityMgr->GetEcologicalCallerInfo(want, callerInfo, USERID);
    ASSERT_NE(bundleDataMgr_, nullptr);
    DelayedSingleton<BundleMgrService>::GetInstance()->dataMgr_ = bundleDataMgr_;
}

/**
 * @tc.number: GetPreloadFlag_0001
 * Function: GetPreloadFlag
 * @tc.name: test GetPreloadFlag
 * @tc.desc: test GetPreloadFlag success
 */
HWTEST_F(BmsBundleFreeInstallTest, GetPreloadFlag_0001, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    ASSERT_NE(connectAbilityMgr, nullptr);

    int32_t flag = connectAbilityMgr->GetPreloadFlag();
    EXPECT_EQ(flag, 2);
}

/**
 * @tc.number: OnDelayedHeartbeat_0001
 * Function: OnDelayedHeartbeat
 * @tc.name: test OnDelayedHeartbeat
 * @tc.desc: test OnDelayedHeartbeat success
 */
HWTEST_F(BmsBundleFreeInstallTest, OnDelayedHeartbeat_0001, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    FreeInstallParams freeInstallParams;
    connectAbilityMgr->freeInstallParamsMap_.insert(pair<std::string, FreeInstallParams>("1", freeInstallParams));
    std::string transactId = "1";
    connectAbilityMgr->OnDelayedHeartbeat(transactId);
    EXPECT_EQ(transactId, "1");
    EXPECT_EQ(connectAbilityMgr->freeInstallParamsMap_.find("1") != connectAbilityMgr->freeInstallParamsMap_.end(),
        true);
    if (connectAbilityMgr->freeInstallParamsMap_.find("1") != connectAbilityMgr->freeInstallParamsMap_.end()) {
        connectAbilityMgr->freeInstallParamsMap_.erase("1");
    }
}

/*
 * @tc.number: OnDelayedHeartbeat_0100
 * @tc.name: test OnDelayedHeartbeat
 * @tc.desc: 1.test OnDelayedHeartbeat of ServiceCenterStatusCallback
 */
HWTEST_F(BmsBundleFreeInstallTest, OnDelayedHeartbeat_0100, Function | SmallTest | Level0)
{
    std::shared_ptr<BundleConnectAbilityMgr> server = std::make_shared<BundleConnectAbilityMgr>();
    ASSERT_NE(server, nullptr);
    ServiceCenterStatusCallback callback(server);
    auto ret = callback.OnDelayedHeartbeat(DEVICE_ID);
    EXPECT_EQ(ret, ERR_OK);
}

/*
 * @tc.number: OnServiceCenterReceived_0100
 * @tc.name: test OnServiceCenterReceived
 * @tc.desc: 1.test OnServiceCenterReceived of ServiceCenterStatusCallback
 */
HWTEST_F(BmsBundleFreeInstallTest, OnServiceCenterReceived_0100, Function | SmallTest | Level0)
{
    std::shared_ptr<BundleConnectAbilityMgr> server = std::make_shared<BundleConnectAbilityMgr>();
    ASSERT_NE(server, nullptr);
    ServiceCenterStatusCallback callback(server);
    auto ret = callback.OnServiceCenterReceived(DEVICE_ID);
    EXPECT_EQ(ret, ERR_OK);
}

/*
 * @tc.number: CheckSubPackageName_0100
 * @tc.name: test CheckSubPackageName
 * @tc.desc: 1.test CheckSubPackageName of BundleConnectAbilityMgr
 */
HWTEST_F(BmsBundleFreeInstallTest, CheckSubPackageName_0100, Function | SmallTest | Level0)
{
    auto connectAbilityMgr = GetBundleConnectAbilityMgr();
    InnerBundleInfo innerBundleInfo;
    Want want;
    auto ret = connectAbilityMgr->CheckSubPackageName(innerBundleInfo, want);
    EXPECT_TRUE(ret);
}
} // OHOS