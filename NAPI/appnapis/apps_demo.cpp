/* Copyright 2022 Unionman Technology Co., Ltd.
 *
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
 *
 */

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#ifdef __cplusplus
#include "um_gpio.h"
#include "um_adc.h"
#include "serial_uart.h"
#include "apps.h"
#include "um_pwm.h"
extern "C" {
#endif

//gpio的异步
struct LedAddOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int args[3] = {0};                   // 2个输入参数
    int result = 0;                      // 业务逻辑处理结果（返回值）
};

//pwm的异步
struct PwmOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int args[2] = {0};                   // 2个输入参数
    int result = 0;                      // 业务逻辑处理结果（返回值）
};

//usart的异步
struct UsartOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int args[2] = {0};                   // 2个输入参数
    int result = 0;                      // 业务逻辑处理结果（返回值）
};


// 同步方法实现获取adc值
static napi_value adc_toget(napi_env env, napi_callback_info info)
{
    constexpr int adc_channel = 1;
    int value = 0;
    if (get_adc_data(adc_channel, &value) < 0) {
        value = -1;
    }

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, value, &ret));
    return ret;
}
// 同步方法实现获取权限
static napi_value sync_get_permission(napi_env env, napi_callback_info info)
{
    int result = 0;
    constexpr char path[128] = "/sys/bus/iio/devices/iio:device0/in_voltage2_raw";
    result = open_permission(path);

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, result, &ret));
    return ret;
}




// 业务逻辑处理函数，由worker线程池调度执行。
//设置led灯的亮灭
static void setLedStatusExecuteCB(napi_env env, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    int gpioNum = addOnData->args[0];
    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_OUT;
    int getValue = -1;

    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    UM_GPIO_SetDirection(gpioNum, direction);
    addOnData->result = UM_GPIO_SetValue(gpioNum, addOnData->args[1]);
}

//获取touch的状态
static void getTouchStatusExecuteCB(napi_env env, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    int gpioNum = addOnData->args[2];
    int getValue = -1;

    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_IN;

    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    UM_GPIO_SetDirection(gpioNum, direction);

    UM_GPIO_GetValue(gpioNum, &getValue);
    addOnData->result = getValue;
}


static void completeCBForPromise(napi_env env, napi_status status, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, addOnData->result, &result);
    napi_resolve_deferred(env, addOnData->deferred, result);

    // 删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}

static napi_value setLedStatusWithPromise(napi_env env, napi_callback_info info)
{
    // 获取2个参数，值的类型是js类型(napi_value)
    size_t argc = 2;
    napi_value args[2];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new LedAddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &addonData->args[1]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setLedStatusWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, setLedStatusExecuteCB, completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}


static napi_value getTouchStatusWithPromise(napi_env env, napi_callback_info info)
{
    // 获取1个参数，值的类型是js类型(napi_value)
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new LedAddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[2]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "getTouchStatusWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, getTouchStatusExecuteCB, completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}




// 业务逻辑处理函数，由worker线程池调度执行。
static void PwmexecuteCB(napi_env env, void *data)
{
    PwmOnData *pwmData = (PwmOnData *)data;

    int pwmPex = pwmData->args[0];
    int pwmAngle = pwmData->args[1];

    set_pwm_period(pwmPex, MAX_DUTY);
    double pwmDuty = 1.0 * INIT_DUTY * pwmAngle / 45L + INIT_DUTY;
    set_pwm_dutyCycle(pwmPex, static_cast<int>(pwmDuty));
    set_pwm_enable(pwmPex, 1);

    pwmData->result = is_pwm_enabled(pwmPex);
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void pwmcompleteCBForPromise(napi_env env, napi_status status, void *data)
{
    PwmOnData *pwmData = (PwmOnData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, pwmData->result, &result);
    napi_resolve_deferred(env, pwmData->deferred, result);

    // 删除napi_ref对象
    if (pwmData->callback != nullptr) {
        napi_delete_reference(env, pwmData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, pwmData->asyncWork);
    delete pwmData;
}

static napi_value setPwmStatus(napi_env env, napi_callback_info info)
{
    // 获取2个参数，值的类型是js类型(napi_value)
    size_t argc = 2L;
    napi_value args[2];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto pwmData = new PwmOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &pwmData->args[0]));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &pwmData->args[1]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setPwmStatus", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, PwmexecuteCB, pwmcompleteCBForPromise, (void *)pwmData,
                           &pwmData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, pwmData->asyncWork);

    // 返回promise
    return promise;
}




// 业务逻辑处理函数，由worker线程池调度执行。
static void UsartreadexecuteCB(napi_env env, void *data)
{
    usart_init();

    UsartOnData *usartData = (UsartOnData *)data;

    usartData->result = usart_read();
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void usartcompleteCBForPromise(napi_env env, napi_status status, void *data)
{
    UsartOnData *usartData = (UsartOnData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, usartData->result, &result);
    napi_resolve_deferred(env, usartData->deferred, result);

    // 删除napi_ref对象
    if (usartData->callback != nullptr) {
        napi_delete_reference(env, usartData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, usartData->asyncWork);
    delete usartData;
}

static napi_value readUsart(napi_env env, napi_callback_info info)
{
    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto usartData = new UsartOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "readUsart", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, UsartreadexecuteCB, usartcompleteCBForPromise, (void *)usartData,
                           &usartData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, usartData->asyncWork);

    // 返回promise
    return promise;
}


static void UsartsendexecuteCB(napi_env env, void *data)
{
    usart_init();

    UsartOnData *usartData = (UsartOnData *)data;

    int value = usartData -> args[0];

    usart_send(value);

    usartData->result = 1;
}

static napi_value sendUsart(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));


    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto usartData = new UsartOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    NAPI_CALL(env, napi_get_value_int32(env, args[0], &usartData->args[0]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "sendUsart", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, UsartsendexecuteCB, usartcompleteCBForPromise, (void *)usartData,
                           &usartData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, usartData->asyncWork);

    // 返回promise
    return promise;
}

/*
 * 注册接口
 */
static napi_value registerApps_DemoApis(napi_env env, napi_value exports)
{
    napi_value gpioValHigh = gpioValHigh;
    napi_value gpioValLow = nullptr;
    napi_create_int32(env, UM_GPIO_HIGH_LEVE, &gpioValHigh);
    napi_create_int32(env, UM_GPIO_LOW_LEVE, &gpioValLow);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("adc_toget", adc_toget),
        DECLARE_NAPI_FUNCTION("sync_get_permission", sync_get_permission),
        DECLARE_NAPI_FUNCTION("setLedStatusWithPromise", setLedStatusWithPromise),
        DECLARE_NAPI_FUNCTION("getTouchStatusWithPromise", getTouchStatusWithPromise),
        DECLARE_NAPI_FUNCTION("setPwmStatus", setPwmStatus),
        DECLARE_NAPI_FUNCTION("readUsart", readUsart),
        DECLARE_NAPI_FUNCTION("sendUsart", sendUsart),
        DECLARE_NAPI_STATIC_PROPERTY("LED_ON", gpioValHigh),
        DECLARE_NAPI_STATIC_PROPERTY("LED_OFF", gpioValLow),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
 * 模块定义
 */
static napi_module apps_demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerApps_DemoApis,
    .nm_modname = "apps_demo", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

#ifdef __cplusplus
}
#endif
/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterApps_DemoModule(void)
{
    napi_module_register(&apps_demoModule); // 接口注册函数
}