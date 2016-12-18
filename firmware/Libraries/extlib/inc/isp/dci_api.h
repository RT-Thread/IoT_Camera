#ifndef _DCI_API_H_
#define _DCI_API_H_

enum DCI_CMD
{
    DCI_NO_COMMAND   = 0,
    DCI_SAVE_DRV_REG = 1,

};  // DCI 命令
enum space_type
{
    userspace    = 0,
    defaultsapce = 1,
};  //存储空间类型

void DCI_Init();        // DCI 初始化
void DCI_DrvCommand();  // DCI 主循环 读取命令并完成相关操作（设置参数）
int API_DCI_GET_PARAM(unsigned char *cmd, unsigned char *cmd_param, unsigned int *param0,
                      unsigned int *param1);  //读取相关参数 cmd为dci命令，后面均为参数

#endif
