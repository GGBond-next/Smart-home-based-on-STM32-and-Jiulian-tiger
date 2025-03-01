版本号：OpenHarmony4.0

在OpenHarmony4.0中新建的子系统：
新建子系统：my_napi
部件：app_napi
模块：apps_demo

子系统在OpenHarmony4.0的根目录的/build/subsystem_config.json文件中添加子系统描述
在my_napit文件夹中新建app_napi
在这个文件夹中分别新建BULD.gn，bundle.json文件
之后就是编写配置BUILD.gn和bundle.json以及在apps_demo下进行对这个系统的逻辑代码编写
![alt text](NAPI编译构造.jpg)

主要的文件介绍：
apps.c是主要的传感器驱动函数的实现
apps.h是主要的传感器驱动函数的头文件封装，方便调用
apps_demo.cpp是主要的接口封装函数，使用同步或者异步的方式来进行封装，其中对于adc的封装是同步的，其余的都是进行异步的封装。
@ohos.appnapis.d.ts是提供给北向开发的接口函数头文件，在北向环境开发时，可以直接调用这里的函数来对开发板进行控制。

在北向开发时，由于用户的权限问题和系统的原因，需要先对OpenHarmony的外设进行开放：
echo 385 > /sys/class/gpio/export
echo 386 > /sys/class/gpio/export

chmod 666 /sys/class/gpio/gpio385/direction
chmod 666 /sys/class/gpio/gpio386/direction 
chmod 666 /sys/class/gpio/gpio385/value 
chmod 666 /sys/class/gpio/gpio386/value 


echo 0 > /sys/class/pwm/pwmchip0/export
echo 0 > /sys/class/pwm/pwmchip2/export
chmod 666 /sys/class/pwm/pwmchip0/pwm0/duty_cycle
chmod 666 /sys/class/pwm/pwmchip0/pwm0/period
chmod 666 /sys/class/pwm/pwmchip0/pwm0/enable
chmod 666 /sys/class/pwm/pwmchip0/pwm0/polarity



