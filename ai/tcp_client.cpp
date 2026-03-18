#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tcp_client.h"

#if defined(LINUX)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <netdb.h>
#elif defined(ARM)/*�?写ARM相关的socket头文�?*/


#endif


/**
  * @name: create_client_socket
  * @description: 创建客户端socket
  * @note
  * @return 套接�? 
  */
int create_client_socket(void)
{
	int mysfd = socket(AF_INET, SOCK_STREAM, 0); 
	if(mysfd < 0){
		printf("create_client_socket error = %d\r\n", mysfd);
		return ERROR;
	}

	return mysfd;
}

/**
  * @name: create_client_connect
  * @description: Tcp连接
  * @note: 传入的必须为ip，没有做域名解析
  * @param sockfd ：网络�?�接�?
  * @param host   ：ip
  * @param port   ：�??�?
  * @param timeout   ：超时时间，按照秒进行�?�算
  * @return -1:错�?? -2超时 大于0正常
  */
int create_client_connect(int sockfd, const char *host, int port, int stimeout)
{
    struct hostent *he;
	struct sockaddr_in sAddr;
	struct timeval timeout;
	fd_set fdr, fdw; 
    int  rc;
    int flags;

    
    // 设置非阻�?
    flags = fcntl(sockfd, F_GETFL, 0);
    if(flags < 0){
        printf("Get flags error:%d\n", errno);
        return ERROR;
    }
    
    flags |= O_NONBLOCK;
    if(fcntl(sockfd, F_SETFL, flags) < 0){
        printf("Set flags error:%d\n", errno);
        return ERROR;
    }

	//建立连接
    if((he = gethostbyname(host))==NULL){
        return -1;
    }

    memset(&sAddr, 0, sizeof(struct sockaddr));
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(port);
    sAddr.sin_addr = *((struct in_addr *)he->h_addr);
     
    // 客户�?连接,如果大于0表示已经连接成功了，此时不需要做超时判断
    rc = connect(sockfd, (struct sockaddr *)&sAddr, sizeof(sAddr));
    if(rc >= 0){
        goto SUCCESS;
    }else{
        if(errno == EINPROGRESS)
        {
            FD_ZERO(&fdr);
            FD_ZERO(&fdw);
            FD_SET(sockfd, &fdr);
            FD_SET(sockfd, &fdw);
            timeout.tv_sec = stimeout; //通常设置�?3
            timeout.tv_usec = 0;
            rc = select(sockfd+1, &fdr, &fdw, NULL, &timeout);
            if(rc > 0)
            {
                //判断套接字是否可�?
                if(!FD_ISSET(sockfd, &fdr) && FD_ISSET(sockfd, &fdw)){
                    //printf("http connect ok\r\n");
                    goto SUCCESS;
                }
                
                else if(FD_ISSET(sockfd, &fdr) && FD_ISSET(sockfd, &fdw))
                {
                    socklen_t len = sizeof(rc);
                    if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &rc, &len) < 0){
                        printf("getsockopt error\r\n");
                        return ERROR;
                    }
                    if(rc == 0){
                        goto SUCCESS;  
                    }
                    else{ 
                        printf("http timeout,rc:%d\r\n", rc);
                        return TIMEOUT;
                    }
                } 
                
            }
            else if(rc == -2){
                //返回-2代表连接超时
                printf("http connect timeout\r\n");
                return TIMEOUT;
            }
            else{
                printf("http select error\r\n");
                return ERROR;
            }
            
            
            
        }
    }
        
SUCCESS:  
    //设置套接字为阻�??
    flags = fcntl(sockfd, F_GETFL, 0);
    if(flags < 0){
        printf("Get flags error:%d\n", errno);
        return ERROR;
    }
    flags &= ~O_NONBLOCK;
    if(fcntl(sockfd, F_SETFL, flags) < 0){
        printf("Set flags error:%d\n", errno);
        return ERROR;
    }    
  
    return rc;
}


/**
  * @name: socket_client_send
  * @description: tcp�?持发送超�?
  * @note: 
  * @param sockfd : socket 套接�?
  * @param data : 需要发送的数据
  * @param len : 需要发送数�?的长�?
  * @param s : 设置超时时间按�??
  * @param us : 设置超时时间按微�?
  * @return 发送了多少�?字节
  */
int socket_client_send(int sockfd, char *data, unsigned long len, int s, int us)
{
    //传参判断

    int rc = 0;
    fd_set write_fds;
    struct timeval timeout;
  
    timeout.tv_sec = s;
    timeout.tv_usec = us;   
    
    FD_ZERO(&write_fds);
    FD_SET(sockfd, &write_fds);
    
    rc = select(sockfd+1, NULL, &write_fds, NULL, &timeout);
    if(rc <= 0)
    {
        if(rc == 0)
        {
            printf("send select timeout\n");
            return TIMEOUT;
        }
        else
        {
            printf("send select error %d\n", rc);
            return ERROR;
        }
    }
    if(rc > 0)
    {
        if(FD_ISSET(sockfd, &write_fds))
        {
            rc = send(sockfd, data, len, 0);
            if(rc < 0)
            {
                printf("socket send error %d\n", rc);
                return ERROR;
            }
        }else{
            printf("FD_ISSET error\n");
            return ERROR;  
        }
    }
    return rc;
}

/**
  * @name: socket_client_recv
  * @description: tcp�?持接收超�?
  * @note: 
  * @param sockfd : socket 套接�?
  * @param data : 接收数据的缓冲区
  * @param len : 接收数据缓冲区的长度
  * @param s : 设置超时时间按�??
  * @param us : 设置超时时间按微�?
  * @return {type} 
  */
int socket_client_recv(int sockfd, char *response, int len ,int s, int us)
{

    int rc = 0;
    fd_set read_fds;
    struct timeval timeout;
   

    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

    
    timeout.tv_sec = s;
    timeout.tv_usec = us;   
    
    rc = select(sockfd+1, &read_fds, NULL, NULL, &timeout);
    if(rc <= 0)
    {
        if(rc == 0)
        {
            printf("recv select timeout\n");
            return TIMEOUT;
        }
        else
        {
            printf("recv select error\n");
            return ERROR;
        }
    }

    if(rc > 0)
    {
        if(FD_ISSET(sockfd, &read_fds)){
            rc = recv(sockfd, response, len, 0);
            if(rc < 0){
                printf("recv error %d\n",rc);
                return ERROR;
            }
            else
                response[rc] = '\0';
        }else{
            printf("FD_ISSET error\n");
            return ERROR;  
        }
    }

    return rc;
}
