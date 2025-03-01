# PWM舵机调试

## **什么是PWM？**

PWM（Paulse Width Modulation）,即脉冲宽度调制，也就是占空比可变的脉冲波形。

占空比：指的是在一个周期内，信号处于高电平的时间占据整个信号周期的百分比。

pwm的占空比，就是指高电平保持的时间，与该pwm时钟周期时间之比。如：舵机的转角就是通过调节pwm占空比来控制，也就是一个周期中高电平所占的百分比来控制的。详细例子参考：sample/pwm

## 设置及打开PWM

九联A311D开发板有两个pwm引脚分别对应目录为

```
 PWM_1: /sys/class/pwm/pwmchip0/pwm0
 PWM_2: /sys/class/pwm/pwmchip2/pwm0
```

导出PWM_1、PWM_2引脚目录

```
$ echo 0 > /sys/class/pwm/pwmchip0
$ echo 0 > /sys/class/pwm/pwmchip2
```

此时就会生成对应的引脚目录pwm0

### 打开或者关闭PWM(以pwm_1为例）

```
$ echo 1 > /sys/class/pwm/pwmchip0/pwm0/enabled
$ echo 0 > /sys/class/pwm/pwmchip0/pwm0/enabled
```

### 设置pwm的溢出值（单位 ns）

```
$ echo 2600000 > /sys/class/pwm/pwmchip0/pwm0/period
```

### 设置pwm一个周期高电平的时间

```
$ echo 5000000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
```

###  设置pwm的极性

```
$ echo normal > /sys/class/pwm/pwmchip0/pwm0/polarity
$ echo inversed > /sys/class/pwm/pwmchip0/pwm0/polarity
```

###  pwm接口函数

```
#define PWM1	1
#define PWM2	2

#define PWM1_PEX "/sys/class/pwm/pwmchip0" 
#define PWM2_PEX "/sys/class/pwm/pwmchip2"

int init_pmw(int pwmChannel);
int set_pwm_period(int pwmChannel, long period);
int set_pwm_dutyCycle(int pwmChannel, long dutyCycle);
int set_pwm_polarity(int pwmChannel, int polarity);
int set_pwm_enable(int pwmChannel, int isEnable);
long get_pwm_period(int pwmChannel);
long get_pwm_dutyCycle(int pwmChannel);
int get_pwm_polarity(int pwmChannel);
int is_pwm_enabled(int pwmChannel);
```

## 举例

## 九联A311D开发板驱动TIANKONGRC MG996R舵机

九联A311D开发板一共有两路pwm输出，根据开发板标识从左到右依次为（3.3V，PWM_1，PWM_2，GND），舵机的连线根据舵机的说明进行连接。

通信接口：pwm接口

接线及组装方式：使用杜邦线，将舵机连接到开发板的pwm接口

### MG996R舵机

- MG996R 舵机是一款单线驱动180°舵机
- 信号线（黄）电源线（红）地线（棕）
- 工作死区：4微秒
- 工作电压：3.0V-7.2V
- 适用温度：-10°~60°
- 操作速度  0.17秒/60度(4.8V)；0.13秒/60度(6.0V) 

舵机的控制一般需要一个20ms（50hz）的时基脉冲，该脉冲的高电平部分一般为0.5ms~2.5ms范围内的角度控制脉冲部分。以180度角度舵机为例，那么对应的控制关系是这样的： 

- 0.5ms————–0度；
- 1.0ms————45度； 
- 1.5ms————90度； 
- 2.0ms———–135度； 
- 2.5ms———–180度； 

### 连接方式

舵机连接A311D开发板： 舵机**信号线**连接开发板pwm_1或pwm_2，**电源线**连接开发板5v供电pin脚，**地线**连接开发板GND接地pin脚，这里是以MG996R为例，连接如图所示（这里电源连接的是板子上的5.0V电源）。
注意：该代码是以MG996R舵机为例。

![](../figures/pwm_demo.png)
图1	pwm连接实物图

### 使用方法

烧录成功后，连接开发板，在串口调试窗口下直接执行该程序。

```
# pwm_test 1 1500000
```

1 是选择的引脚，1500000 是设置的 duty_cycle