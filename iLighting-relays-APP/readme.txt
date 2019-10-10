freertos v10.2.1
STM32Cube FW_F1 V1.8.0

编译之前请执行  。/version.sh 
该脚本在core/inc目录内新建build_def.h文件。
该文件内包含git仓库的部分信息，可以用于固件号码字段。

此工程为6/12路继电器模块统一工程，通过在编译时设定 MODULE_TYPE_RELAYS_6|MODULE_TYPE_RELAYS_12 宏来
确定编译出的固件