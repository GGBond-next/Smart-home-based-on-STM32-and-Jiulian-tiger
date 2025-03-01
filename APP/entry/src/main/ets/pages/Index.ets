import router from '@ohos.router';
import appnapis from '@ohos.appnapis';
import promptAction from '@ohos.promptAction';

// 设置LED灯的状态
function setLedStatus(pin: number, status: number): Promise<number> {
  return appnapis.setLedStatusWithPromise(pin, status).then((result: number) => {
    if (result === appnapis.LED_ON) {
      promptAction.showToast({
        message: pin.toString() + ": LED已开启"
      });
    } else if (result === appnapis.LED_OFF) {
      promptAction.showToast({
        message: pin.toString() + ": LED已关闭"
      });
    } else {
      promptAction.showToast({
        message: "设置LED状态失败"
      });
    }
    return result;
  });
}

// 获取ADC值
// function getAdcValue(): number {
//   return appnapis.adc_toget().then((adcValue: number) => {
//     promptAction.showToast({
//       message: "ADC值: " + adcValue.toString()
//     });
//     return adcValue;
//   });
// }

// 设置PWM状态
function setPwm(pwmPin: number, angle: number): Promise<number> {
  return appnapis.setPwmStatus(pwmPin, angle).then((result: number) => {
    promptAction.showToast({
      message: "PWM设置完成，角度：" + angle.toString()
    });
    return result;
  });
}

// 读取USART数据
function readUsartData(): Promise<number> {
  return appnapis.readUsart().then((data: number) => {
    promptAction.showToast({
      message: "从USART读取的数据: " + data.toString()
    });
    return data;
  });
}

// 发送USART数据
function sendUsartData(value: number): Promise<number> {
  return appnapis.sendUsart(value).then((result: number) => {
    promptAction.showToast({
      message: "通过USART发送的数据: " + value.toString() + result.toString()
    });
    return result;
  });
}

// 获取GPIO引脚状态
function getGpioStatus(pin: number): Promise<void> {
  return appnapis.getTouchStatusWithPromise(pin).then((status: number) => {
    let message = `引脚 ${pin} 状态: `;
    if (status === 0) {
      message += "低电平";
    } else if (status === 1) {
      message += "高电平";
    } else {
      message = `获取引脚 ${pin} 状态失败`;
    }
    promptAction.showToast({ message });
  })
}

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  @State ledPin: number = 386; // 假设这是LED引脚编号
  @State pwmPin: number = 1; // 假设这是PWM引脚编号
  @State usartValue: number = 12; // USART发送的值
  @State gpioPin: number = 7; // 假设这是要查询状态的GPIO引脚编号
  @State touch: number = 385;
  @State servoAngle: number = 90; // 初始舵机角度

  private handleSliderChange = (value: number) => {
    this.servoAngle = value;
    setPwm(this.pwmPin, value); // 设置PWM角度
  }

  build() {
    Column({ space: 15 }) { // 设置列内元素间的间距为20px
      Button('关闭蜂鸣器')
        .margin({ top: 5 }) // 给第一个按钮顶部添加额外的边距
        .onClick(() => {
          setLedStatus(this.ledPin, appnapis.LED_ON);
        });

      Button('开启蜂鸣器')
        .onClick(() => {
          setLedStatus(this.ledPin, appnapis.LED_OFF);
        });

      Button('获取ADC值')
        .onClick(() => {
          promptAction.showToast({
            message: "ADC值为：" + appnapis.adc_toget().toString()
          });
          appnapis.adc_toget
        });

      // 圆盘组件用于设置舵机角

      Button('读取USART数据')
        .onClick(() => {
          readUsartData();
        });

      Button('发送USART数据开启风扇f')
        .onClick(() => {
          sendUsartData(65);
        });

      Button('发送USART数据关闭风扇f')
        .onClick(() => {
          sendUsartData(66);
        });

      // 添加获取引脚状态的按钮
      Button('获取引脚状态')
        .onClick(() => {
          getGpioStatus(this.touch);
        });
      Slider({ min: 0, max: 180, value: this.servoAngle })
        .width(100)
        .height(100)
        .onChange(this.handleSliderChange); // 使用箭头函数处理滑动事
      Text(`当前角度：${this.servoAngle}`)
        .fontSize(10)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 10 });


      Text(this.message)
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20 }) // 给文本顶部添加额外的边距
    }
    .width('100%')
    .height('100%')
    .padding(20) // 添加页面整体的内边距，使内容不紧贴边缘
  }
}