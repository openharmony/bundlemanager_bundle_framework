/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "verifymanagerhost_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "verify_manager_stub.h"
#include "securec.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
constexpr size_t U32_AT_SIZE = 4;
constexpr size_t MESSAGE_SIZE = 3;
constexpr size_t DCAMERA_SHIFT_24 = 24;
constexpr size_t DCAMERA_SHIFT_16 = 16;
constexpr size_t DCAMERA_SHIFT_8 = 8;

uint32_t GetU32Data(const char* ptr)
{
    return (ptr[0] << DCAMERA_SHIFT_24) | (ptr[1] << DCAMERA_SHIFT_16) | (ptr[2] << DCAMERA_SHIFT_8) | (ptr[3]);
}
class MockVerifyManagerStub : public VerifyManagerStub {
public:
    int32_t CallbackEnter(uint32_t code) override
    {
        return 0;
    }
    
    int32_t CallbackExit(uint32_t code, int32_t result) override
    {
        return 0;
    }
    
    ErrCode Verify(const std::vector<std::string>& abcPaths, int32_t& funcResult) override
    {
        return ERR_OK;
    }
        
    ErrCode DeleteAbc(const std::string& path, int32_t& funcResult) override
    {
        return ERR_OK;
    }
};

bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
{
    uint32_t code = (GetU32Data(data) % MESSAGE_SIZE);
    MessageParcel datas;
    std::u16string descriptor = MockVerifyManagerStub::GetDescriptor();
    datas.WriteInterfaceToken(descriptor);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    MockVerifyManagerStub verifyManagerStub;
    verifyManagerStub.OnRemoteRequest(code, datas, reply, option);
    return true;
}
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::U32_AT_SIZE) {
        return 0;
    }

    char* ch = static_cast<char*>(malloc(size + 1));
    if (ch == nullptr) {
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        free(ch);
        ch = nullptr;
        return 0;
    }
    OHOS::DoSomethingInterestingWithMyAPI(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}