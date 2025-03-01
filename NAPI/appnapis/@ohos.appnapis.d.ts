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

/**
 * unionman a311d
 * @since 8
 * @devices phone, tablet
 * @import import appsnapis from '@ohos.appsnapis';
 * @permission N/A
 */

declare namespace appnapis {
    const LED_ON = 1;
    const LED_OFF = 0;
    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setLedStatusWithPromise(pin: number, status: number): Promise<number>;
    /**
     * 获取Touch引脚的状态
     * @param pin pin口号
     */
    function getTouchStatusWithPromise(pin: number): Promise<number>;
    /**
     * 获取adc （1） 的ad值
     * @param 无
     */
    function adc_toget():number;
    function sync_get_adc_value(): number;
    /**
     * 设置pwm的开关状态
     * @param pwmPex pwm引脚
     * @param pwmAngle pwm角度
     */
    function setPwmStatus(pwmPex: number, pwmAngle: number): Promise<number>;
    /**
     * 接受usart的数据
     * 
     */
    function readUsart(): Promise<number>;
    /**
     * 发送usart的数据
     * 
     */
    function sendUsart(value : number): Promise<number>;
}

export default appnapis;