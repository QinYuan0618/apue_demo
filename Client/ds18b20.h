/*
 * @Author: QinYuan
 * @Date: 2024-12-18 19:12:13
 * @LastEditTime: 2024-12-18 20:17:33
 * @FilePath: /qinyuan/apue_demo/Client/ds18b20.h
 * @Description:
 */
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int get_temperature(float *temp);