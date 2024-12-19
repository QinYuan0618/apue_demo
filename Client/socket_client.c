int main(int argc, char **argv)
{
  int                 sockfd = -1;            //socket返回的描述符
  int                 rv = -1;                //connect返回值
  struct sockaddr_in  servaddr;               //IPv4
  char *servip = "192.168.0.8"; //服务器ip
  int                 port = 12345;           //服务器端口
  char                buf[1024];              //读服务器发来的回应数据（内容）

  //1.建立socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)  // 不成功
  {
    printf("Create socket failure:%s\n", strerror(errono));
    return -1;
  }
  printf("Create socket[%d] successfully!\n", sockfd);    //成功


  //bind()指定 源ip和端口。客户端的源ip和端口一般由操作系统自己调用，不需要bind

  //2.客户端连接服务器connect()
  memset(&servaddr, 0, sizeof(servaddr)); //先将字段清零
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  inet_aton(servip, &servaddr.sin_addr);  //不需要一个返回值？
  rv = connect(sockfd, sockaddr(&servaddr), sizeof(servaddr));
  if (rv < 0)0
  {
    printf("Connect to server[%s:%d] failure: %s\n", servip, port, strerror(errno)); //打印出服务器ip和端口，方便纠错
    return -2;
  }
  printf("Connect to server[%s:%d] successfully!\n", servip, port);    //成功

  while (1)
  {
    //3.给服务器发送内容write()
    write(sockfd, MSG_STR, strlen(MSG_STR));
    if (rv < 0)  //写失败
    {
      printf("write to server by sockfd[%d] failure:%s\n", sockfd, strerror(errno));
      return -3;
    }

    //4.读回应数据read()
    memset(buf, 0, sizeof(buf)); //每次读数据前都要将buf清零
    rv = read(sockfd, buf, sizeof(buf));
    if (rv < 0)  //读失败
    {
      printf("read data from server by sockfd[%d] failure:%s\n", sockfd, strerror(errno));
      return -4;
    }
    else if (rv = 0)    //客户端断开连接
    {
      printf("socket[%d] get disconnected\n", sockfd);
      return -5;
    }
    else if (rv > 0)    //读成功
    {
      printf("Read %d bytes data from Server: %s\n", rv, buf);
    }
  }

  close(fd);
}