#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

/**************************Macro definition***********************************/
/*宏定义,如果是linux平台，定义linux，否则定义ARM*/
#define LINUX
//#define ARM


typedef enum
{
    TIMEOUT = -2, //超时
    ERROR   = -1, //错误
    NORMAL  = 0  //正常
}HttpState_e;


/*************************extern declaration ********************************/
	


/**************************Global variable***********************************/



/**************************Function declaration******************************/

/**
  * @name: create_socket
  * @description: 创建socket
  * @note
  * @return 套接字
  */
int create_client_socket(void);

/**
  * @name: create_connect
  * @description: Tcp连接
  * @note: 传入的必须为ip，没有做域名解析
  * @param sockfd ：网络套接字
  * @param host   ：ip
  * @param port   ：端口
  * @param timeout   ：超时时间，按照秒进行运算
  * @return -1:错误 -2超时 大于0正常
  */
int create_client_connect(int sockfd, const char *host, int port, int stimeout);


/**
  * @name: socket_send
  * @description: tcp�?持发送超�?
  * @note: 
  * @param sockfd : socket 套接�?
  * @param data : 需要发送的数据
  * @param len : 需要发送数�?的长�?
  * @param s : 设置超时时间按�??
  * @param us : 设置超时时间按微�?
  * @return 发送了多少�?字节
  */
int socket_client_send(int sockfd, char *data, unsigned long len, int s, int us);

/**
  * @name: socket_recv
  * @description: tcp�?持接收超�?
  * @note: 
  * @param sockfd : socket 套接�?
  * @param data : 接收数据的缓冲区
  * @param len : 接收数据缓冲区的长度
  * @param s : 设置超时时间按�??
  * @param us : 设置超时时间按微�?
  * @return {type} 
  */
int socket_client_recv(int sockfd, char *response, int len ,int s, int us);




#endif

