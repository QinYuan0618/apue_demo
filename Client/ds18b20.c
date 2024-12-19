/**
 * @Author: QinYuan
 * @Date: 12/18/2024, 8:54:43 PM
 * @LastEditors: QinYuan
 * @LastEditTime: 12/18/2024, 8:54:43 PM
 * Description:
 */

#include "ds18b20.h"

int main(int argc, char **argv)
{
  float                   temp; // 温度数据
  int                     rv;   // 返回值

  // 获取温度值，失败则返回错误
  rv = get_temperature(&temp);
  if (rv < 0)
  {
    printf("get temperature failure, return value: %d", rv);
    return -1;
  }

  // 打印获取到的温度值
  printf("Temperature: %f", temp);
  return 0;
}


/*
 * @brief 获取温度功能函数
 * @param temp 存储获取温度数据的指针
 * @return int 返回0表示成功，否则错误
 */
int get_temperature(float *temp)
{
  int                     fd = -1;        // 文件描述符
  char                    buf[256];       // 存储读取数据的缓冲区
  char                   *ptr = NULL;     // 指向数据中的 "t=" 字符串位置
  DIR                    *dirp = NULL;    // 目录指针
  struct  dirent         *direntp = NULL; // 目录项
  char                    w1_path[64] = "/sys/bus/w1/devices/"; // w1设备路径
  char                    chip_sn[32];    // DS18B20芯片的序列号
  int                     found = 0;      // 是否找到设备的标志

  // 1.打开w1设备目录, dirp指向这个目录
  dirp = opendir(w1_path);
  if (!dirp)
  {
    printf("open folder %s failure: %s\n", w1_path, strerror(errno));
    return -1;
  }

  // 2.查找以"28-"开头的设备名称，表示DS18B20，将设备名称赋值给chip_sn
  while (NULL != (direntp = readdir(dirp)))
  {
    if (strstr(direntp->d_name, "28-"))
    {
      strncpy(chip_sn, direntp->d_name, sizeof(chip_sn));
      found = 1;
    }
  }

  closedir(dirp); //关闭目录

  // 如果没找到该设备，返回错误
  if (!found)
  {
    printf("can not found ds18b20 chipset\n"); 
    return -2;
  }

  // 3.构建设备路径 /sys/bus/w1/devices/28-xxxxxxx/w1_slave
  strncat(w1_path, chip_sn, sizeof(w1_path) - strlen(w1_path));
  strncat(w1_path, "/w1_slave", sizeof(w1_path) - strlen(w1_path));

  // 4.根据设备路径打开设备文件
  if ( (fd=open(w1_path, O_RDONLY)) < 0 )
  {
    // printf("open file failure: %s\n", strerror(errno));
    perror("open file failure");
    return -3;
  }

  // 5.读取数据
  memset(buf, 0, sizeof(buf));
  if (read(fd, buf, sizeof(buf)) < 0)
  {
    printf("read data from fd=%d failure: %s\n", fd, strerror(errno));
    return -4;
  }

  // 6.找到"t="字符串，获取温度值。原为...t=11625
  ptr = strstr(buf, "t=");
  if (!ptr)
  {
    printf("can not find t= string\n");
    return -5;
  }

  ptr += 2; // 跳过"t="

  // 7.温度数据转为浮点数并储存
  *temp = atof(ptr) / 1000;
  printf("[Raspberry Pi Temperature]: %f℃\n", *temp);

  close(fd);

  return 0;
}