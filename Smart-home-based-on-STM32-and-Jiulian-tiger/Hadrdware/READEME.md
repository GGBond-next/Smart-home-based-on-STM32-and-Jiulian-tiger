版本号：OpenHarmony4.0

在OpenHarmony4.0中新建的子系统：
新建子系统：my_project
部件：app_test
模块：demo_test

子系统在OpenHarmony4.0的根目录的/build/subsystem_config.json文件中添加子系统描述
在my_project文件夹中新建app_test
在这个文件夹中分别新建BULD.gn，bundle.json文件
之后就是编写配置BUILD.gn和bundle.json以及在demo_test下进行对这个系统的逻辑代码编写
![alt text](image.png)

主要的文件介绍：
apps.c是主要的线程函数的实现
apps.h是主要的线程函数的头文件封装，方便调用
main.c是主要的逻辑函数，实现线程并实现对线程的调用
其余的是主要的用到的芯片外设的各种头文件和源代码的实现

在cmd输入下列代码时
hdc shell mount -o remount,rw /

hdc shell

cd  /system/lib/module/（可执行文件存放目录）

./demo_test

输出对应的打印信息：
![alt text](HDC调试图1.png)
![alt text](HDC调试图2.png)