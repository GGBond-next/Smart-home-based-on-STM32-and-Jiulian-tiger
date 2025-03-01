在Index组件中，提供了多个按钮和滑动条来交互式地测试上述功能。具体如下：

关闭蜂鸣器/开启蜂鸣器：点击按钮可以控制连接到特定引脚的蜂鸣器。

获取ADC值：点击按钮读取光敏电阻或其他模拟传感器的ADC值。

读取USART数据/发送USART数据开启风扇/发送USART数据关闭风扇：用于读写串行接口的数据，控制风扇等设备。

获取引脚状态：点击按钮查询金属触摸传感器的状态。

调整舵机角度：使用滑动条调整PWM引脚输出的角度，进而控制舵机的位置。

功能说明
控制LED灯状态：
function setLedStatus(pin: number, status: number): Promise<number>
该函数用于设置指定引脚上的LED灯的状态。成功时会弹出提示框显示当前操作（开启或关闭），失败则提示设置失

设置PWM状态：
function setPwm(pwmPin: number, angle: number): Promise<number>
此函数用于设定PWM引脚的角度值，适用于舵机等需要角度控制的设备。完成后会弹出提示框通知用户PWM设置完成及设定的角度。

读取USART数据：
function readUsartData(): Promise<number>
从串行接口读取数据，并以弹窗形式展示给用户。

发送USART数据：
function sendUsartData(value: number): Promise<number>
向串行接口发送指定数值的数据，用于与外部设备通信，如发送命令开启或关闭风扇。

获取GPIO引脚状态：
function getGpioStatus(pin: number): Promise<void>
查询并显示指定GPIO引脚的电平状态（高/低）