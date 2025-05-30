/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "json_serializer.h"

#include <benchmark/benchmark.h>

using namespace std;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
constexpr int32_t BENCHMARK_TIMES = 1000;

    /**
     * @tc.name: BenchmarkTestForCustomizeDataToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForCustomizeDataToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        CustomizeData customizeData;
        customizeData.name = "ohos.global.systemres";
        customizeData.value = "1";
        customizeData.extra = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, customizeData);
        }
    }

    /**
     * @tc.name: BenchmarkTestForCustomizeDataFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForCustomizeDataFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["value"] = "1";
        jsonObject["extra"] = "/data/accounts/account_0/applications/ohos.global.systemres";
        CustomizeData customizeData;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, customizeData);
        }
    }

    /**
     * @tc.name: BenchmarkTestForMetaDataToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForMetadataToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        Metadata metadata;
        metadata.name = "ohos.global.systemres";
        metadata.value = "1";
        metadata.resource = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, metadata);
        }
    }

    /**
     * @tc.name: BenchmarkTestForMetaDataFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForMetadataFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["value"] = "1";
        jsonObject["resource"] = "/data/accounts/account_0/applications/ohos.global.systemres";
        Metadata metadata;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, metadata);
        }
    }

    /**
     * @tc.name: BenchmarkTestForAbilityInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForAbilityInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        AbilityInfo abilityInfo;
        abilityInfo.name = "ohos.global.systemres";
        abilityInfo.label = "1";
        abilityInfo.description = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, abilityInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForAbilityInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForAbilityInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["label"] = "1";
        jsonObject["description"] = "/data/accounts/account_0/applications/ohos.global.systemres";
        AbilityInfo abilityInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, abilityInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForExtensionAbilityInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForExtensionAbilityInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        ExtensionAbilityInfo extensionInfo;
        extensionInfo.name = "ohos.global.systemres";
        extensionInfo.icon = "1";
        extensionInfo.description = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, extensionInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForExtensionAbilityInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForExtensionAbilityInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["icon"] = "1";
        jsonObject["description"] = "/data/accounts/account_0/applications/ohos.global.systemres";
        ExtensionAbilityInfo extensionInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, extensionInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForApplicationInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForApplicationInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        ApplicationInfo applicationInfo;
        applicationInfo.name = "ohos.global.systemres";
        applicationInfo.label = "1";
        applicationInfo.iconPath = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, applicationInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForApplicationInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForApplicationInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["label"] = "1";
        jsonObject["iconPath"] = "/data/accounts/account_0/applications/ohos.global.systemres";
        ApplicationInfo applicationInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, applicationInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForBundleInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForBundleInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        BundleInfo bundleInfo;
        bundleInfo.name = "ohos.global.systemres";
        bundleInfo.appId = "1";
        bundleInfo.entryModuleName = "ohos.global.systemres.MainAbility";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, bundleInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForApplicationInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForBundleInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["appId"] = "1";
        jsonObject["entryModuleName"] = "ohos.global.systemres.MainAbility";
        BundleInfo bundleInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, bundleInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForModuleInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForModuleInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        ModuleInfo moduleInfo;
        moduleInfo.moduleName = "ohos.global.systemres";
        moduleInfo.moduleSourceDir = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, moduleInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForModuleInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForModuleInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["moduleName"] = "ohos.global.systemres";
        jsonObject["moduleSourceDir"] = "/data/accounts/account_0/applications/ohos.global.systemres";
        ModuleInfo moduleInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, moduleInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForFormInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForFormInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        FormInfo formInfo;
        formInfo.name = "ohos.global.systemres";
        formInfo.description = "1";
        formInfo.src = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, formInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForFormInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForFormInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        FormInfo formInfo;
        formInfo.name = "ohos.global.systemres";
        formInfo.description = "1";
        formInfo.src = "/data/accounts/account_0/applications/ohos.global.systemres";
        to_json(jsonObject, formInfo);
        FormInfo formInfoTest;

        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, formInfoTest);
        }
    }

    /**
     * @tc.name: BenchmarkTestForShortcutInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForShortcutInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        ShortcutInfo shortcutInfo;
        shortcutInfo.bundleName = "ohos_global_systemres";
        shortcutInfo.id = "1";
        shortcutInfo.disableMessage = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, shortcutInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForShortcutInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForShortcutInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["bundleName"] = "ohos_global_systemres";
        jsonObject["id"] = "1";
        jsonObject["disableMessage"] = "/data/accounts/account_0/applications/ohos.global.systemres";
        ShortcutInfo shortcutInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, shortcutInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForCommonEventInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForCommonEventInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        CommonEventInfo commonEvent;
        commonEvent.name = "ohos.global.systemres";
        commonEvent.permission = "1";
        commonEvent.bundleName = "ohos_global_systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, commonEvent);
        }
    }

    /**
     * @tc.name: BenchmarkTestForCommonEventInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForCommonEventInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        std::vector<std::string> data;
        std::vector<std::string> type;
        std::vector<std::string> events;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["permission"] = "1";
        jsonObject["bundleName"] = "ohos_global_systemres";
        jsonObject["uid"] = -1;
        jsonObject["data"] = data;
        jsonObject["type"] = type;
        jsonObject["events"] = events;
        CommonEventInfo commonEvent;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, commonEvent);
        }
    }

    /**
     * @tc.name: BenchmarkTestForHapModuleInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForHapModuleInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        HapModuleInfo hapModuleInfo;
        hapModuleInfo.name = "ohos.global.systemres";
        hapModuleInfo.label = "1";
        hapModuleInfo.srcPath = "/data/accounts/account_0/applications/ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, hapModuleInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForHapModuleInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForHapModuleInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["label"] = "1";
        jsonObject["srcPath"] = "/data/accounts/account_0/applications/ohos.global.systemres";
        HapModuleInfo hapModuleInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, hapModuleInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForBundleUserInfoToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForBundleUserInfoToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        BundleUserInfo bundleUserInfo;
        bundleUserInfo.disabledAbilities = {"ohos.global.systemres.MainAbility"};
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, bundleUserInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForBundleUserInfoFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForBundleUserInfoFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["abilities"] = {"ohos.global.systemres.MainAbility"};
        jsonObject["disabledAbilities"] = {"ohos.global.systemres.MainAbility"};
        BundleUserInfo bundleUserInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, bundleUserInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForShortcutWantFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForShortcutWantFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["bundleName"] = "ohos_global_systemres";
        jsonObject["abilityName"] = "ohos.global.systemres.MainAbility";
        ShortcutWant shortcutWant;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, shortcutWant);
        }
    }

    /**
     * @tc.name: BenchmarkTestForShortcutFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForShortcutFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["shortcutId"] = "1";
        jsonObject["label"] = "1";
        jsonObject["icon"] = "1";
        Shortcut shortcut;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, shortcut);
        }
    }

    /**
     * @tc.name: BenchmarkTestForShortcutJsonFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForShortcutJsonFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["shortcuts"] = {"ohos_global_systemres"};
        ShortcutJson shortcutJson;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, shortcutJson);
        }
    }

    /**
     * @tc.name: BenchmarkTestForReqPermissionUsedSceToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForReqPermissionUsedSceToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        RequestPermissionUsedScene usedScene;
        usedScene.abilities = {"ohos.global.systemres.MainAbility"};
        usedScene.when = "1";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, usedScene);
        }
    }

    /**
     * @tc.name: BenchmarkTestForReqPermissionUsedSceFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForReqPermissionUsedSceFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["abilities"] = {"ohos.global.systemres.MainAbility"};
        jsonObject["when"] = "1";
        RequestPermissionUsedScene usedScene;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, usedScene);
        }
    }

    /**
     * @tc.name: BenchmarkTestForRequestPermissionToJson
     * @tc.desc: Testcase for testing 'to_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForRequestPermissionToJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        RequestPermission requestPermission;
        requestPermission.name = "ohos.global.systemres";
        requestPermission.reason = "1";
        for (auto _ : state) {
            /* @tc.steps: step1.call to_json in loop */
            to_json(jsonObject, requestPermission);
        }
    }

    /**
     * @tc.name: BenchmarkTestForRequestPermissionFromJson
     * @tc.desc: Testcase for testing 'from_json' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForRequestPermissionFromJson(benchmark::State &state)
    {
        nlohmann::json jsonObject;
        jsonObject["name"] = "ohos.global.systemres";
        jsonObject["reason"] = "1";
        RequestPermission requestPermission;
        for (auto _ : state) {
            /* @tc.steps: step1.call from_json in loop */
            from_json(jsonObject, requestPermission);
        }
    }

    BENCHMARK(BenchmarkTestForCustomizeDataToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForCustomizeDataFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForMetadataToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForMetadataFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForAbilityInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForAbilityInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForExtensionAbilityInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForExtensionAbilityInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForApplicationInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForApplicationInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForBundleInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForBundleInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForModuleInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForModuleInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForFormInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForFormInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForShortcutInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForShortcutInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForCommonEventInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForCommonEventInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForHapModuleInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForHapModuleInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForBundleUserInfoToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForBundleUserInfoFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForShortcutWantFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForShortcutFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForShortcutJsonFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForReqPermissionUsedSceToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForReqPermissionUsedSceFromJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForRequestPermissionToJson)->Iterations(BENCHMARK_TIMES);
    BENCHMARK(BenchmarkTestForRequestPermissionFromJson)->Iterations(BENCHMARK_TIMES);
}

BENCHMARK_MAIN();