#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
 
#include "my_http.h"
#include "tcp_client.h"
 
#define BUFFER_SIZE 1024

#define HTTP_GET        "GET %s HTTP/1.1\r\n"\
                        "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
                        "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
                        "Host:%s\r\n"\
                        "Connection:close\r\n"\
                        "\r\n"

#define HTTP_POST       "POST /%s HTTP/1.1\r\n"\
                        "Host: %s\r\n"\
                        "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:59.0) Gecko/20100101 Firefox/59.0\r\n"\
                        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"\
                        "Accept-Language: en-US,en;q=0.5\r\n"\
                        "Accept-Encoding: gzip, deflate\r\n"\
                        "Content-Type: multipart/form-data; boundary=%s\r\n"\
                        "Content-Length: %ld\r\n"\
                        "Connection: close\r\n"\
                        "\r\n"

#define UPLOAD_REQUEST	"--%s\r\n"\
						"Content-Disposition: form-data; name=\"file_name\"; filename=\"%s\"\r\n"\
						"Content-Type: image/png\r\n\r\n"

resp_header_def resp;//头信息
upload_buf upload;

static int http_tcpclient_create(const char *host, int port){
    int socket_fd;
    int ret;
    
    socket_fd = create_client_socket();
    ret = create_client_connect(socket_fd,host,port,10);
    if (ret < 0) 
    {
        // if (ret == -2)
        // {
        //     system("udhcpc -i eth0");//自动获取IP地址
        // }
        printf("create_client_connect error: %d\n",ret);
        return ret;
    }

    return socket_fd;
}
 
static void http_tcpclient_close(int socket){
    close(socket);
}
 
static int http_parse_url(const char *url,char *host,char *file,int *port)
{
    char *ptr1,*ptr2,*ptr3;
    char cport[10] = {0};
    char filepath[BUFFER_SIZE] = {'\0'};
    int len = 0;
    int i,j;
    if(!url || !host || !file || !port){
        return -1;
    }
 
    ptr1 = (char *)url;
 
    if(!strncmp(ptr1,"http://",strlen("http://"))){
        ptr1 += strlen("http://");
    }else{
        return -1;
    }

     //get host and ip    
    ptr3 = strchr(ptr1,':');
    ptr3 ++;
    if(ptr3){
        for (i = 0; i < strlen(ptr3); i++)
        {
            if (ptr3[i] == '/')
            {
                break;
            }
        }
        memcpy(cport,ptr3,i);
        cport[i] = '\0';
        *port = atoi(cport);
    }else{
        *port = MY_HTTP_DEFAULT_PORT;
    }

    if (ptr3)
    {
        ptr2 = strchr(ptr1,':');
    }
    else
        ptr2 = strchr(ptr1,'/');
    
    if(ptr2){
        len = strlen(ptr1) - strlen(ptr2);
        memcpy(host,ptr1,len);
        host[len] = '\0';
        if(*(ptr2 + 1)){
            memcpy(filepath,ptr2,strlen(ptr2));
            filepath[strlen(ptr2)] = '\0';
        }
    }else{
        memcpy(host,ptr1,strlen(ptr1));
        host[strlen(ptr1)] = '\0';
    }

    for (i = strlen(filepath) - 1; i >= 0; i--)
    {
        if (filepath[i] == '/')
        {
            break;
        }
    }

    len = strlen(filepath) - (i+1);
    memcpy(file,filepath+i+1,len);
    file[len] = '\0';     
 
    return 0;
}
 
 
static int http_tcpclient_recv(int socket,char *lpbuff,unsigned long bufsize){
    int recvnum = 0;
    int i = 0;
    recvnum = socket_client_recv(socket, lpbuff,bufsize,10,0);
    
    return recvnum;
}
 
static int http_tcpclient_send(int socket,char *buff,unsigned long size){
    int sentnum=0;
    
    sentnum = socket_client_send(socket,buff,size,10,0);
    
    return sentnum;
}

//获取回复头的信息
static int http_get_resp_header(const char *response, resp_header_def *resp)
{
    //查找HTTP/
    char *pos = strstr((char *)response, "HTTP/");
    if (pos)
        sscanf(pos, "%*s %d", &resp->status_code);//返回状态码

    pos = strstr((char *)response, "Content-Type:");//返回内容类型
    if (pos)
        sscanf(pos, "%*s %s", resp->content_type);

    pos = strstr((char *)response, "Content-Length:");//内容的长度(字节)
    if (pos)
        sscanf(pos, "%*s %ld", &resp->content_length);

    return 0;
}

static int http_get_response(int socket,char *buff,int size)
{
    int len = 0, lencnt  = 0;
    char *response = (char *)malloc( size*sizeof(char) );

    memset(buff,0,size);
    while((len = http_tcpclient_recv(socket,response, 1)) != 0)
    {
        if((lencnt + len) > size)//判断缓存是否超限
        {
            printf("lencnt %d   size %d\n",lencnt,size);
            return -1;
        }
        strcat(buff, response);
        //找到响应头的头部信息, 两个"\n\r"为分割点
        int flag = 0;
        int i = strlen(buff) - 1;
        for (; buff[i] == '\n' || buff[i] == '\r';i--, flag++);
        {
            if (flag == 4)//最多找4次，没找到
                break;
        }

        lencnt += len;
    }

    free(response);
    return lencnt;
}

static char* http_download_writefile(int socket,int s,int us)
{
    FILE *fp;
    int length = 0;
    int mem_size = 4096;//mem_size might be enlarge, so reset it
    int buf_len = mem_size;//read 4k each time
    int len = 0;
    
    char *filename = (char *)malloc( BUFFER_SIZE*sizeof(char) );
    sprintf(filename,DOWNLOAD_PATH,resp.file_name);

    fp = fopen(filename,"w+");
    if (fp == NULL)
    {
        printf("Create file failed\n");
        return NULL;
    }

    //申请4k缓存
    char *buf = (char *)malloc(mem_size * sizeof(char));
    memset(buf,0,mem_size);

    //读取文件
    while ((len = http_tcpclient_recv(socket, buf ,mem_size)) != 0 && length < resp.content_length)
    {
        fwrite(buf,len,1,fp);
        memset(buf,0,strlen(buf));
        length += len;
    }
    
    if (length == resp.content_length)
    {
        printf("\nDownload successful ^_^\n\n");
    }
    else
    {
        printf("\nFinished length:%d,resp.content_length:%ld\n",
        length, resp.content_length);
        return NULL;
    }

    fclose(fp);

    return filename;
}

static int http_upload_readfile(char *url,char *filename,const char* localfile,upload_buf *upload_test)
{
    char header[1024]={0}; 
    char send_request[1024]={0};
    char send_end[1024]={0};
    char http_boundary[64]={0};

    char *filebuf;
    char *buff;

    long long int timestamp;
	struct timeval tv;

    unsigned long totalsize = 0,filesize = 0,request_len = 0,end_len = 0;
    unsigned long head_len = 0;
    unsigned long read_byte = 0;

    int i;
    //获取毫秒级的时间戳用于boundary的值
	gettimeofday(&tv,NULL);
	timestamp = (long long int)tv.tv_sec * 1000 + tv.tv_usec;
	snprintf(http_boundary,64,"---------------------------%lld",timestamp);

    //打开要上传的图片
    FILE* fp = fopen(localfile, "rb+");							
    if (fp == NULL){  
        printf("open file fail!\r\n");  
        return -1;  
    }

    //文件大小
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    rewind(fp);
    filebuf = (char *)malloc(sizeof(char) * filesize);
    memset(filebuf,0,filesize);
    
    //读取上传的图片信息
    read_byte = fread(filebuf, 1, filesize, fp);
    if (read_byte <= 0)
    {
        printf("reda file fail!\r\n");  
        return -1;
    }
    fclose(fp);

    char localfilename[128] = {'\0'};
    for (i = strlen(localfile) - 1; i >= 0; i--)
    {
        if(localfile[i] == '/')
            break;
    }
    strcpy(localfilename,localfile+i+1);

	request_len = snprintf(send_request,1024,UPLOAD_REQUEST,http_boundary,localfilename); //请求信息
    end_len = snprintf(send_end,1024,"\r\n--%s--\r\n",http_boundary); //结束信息
    totalsize = filesize + request_len + end_len;
    head_len = snprintf(header,1024,HTTP_POST,filename,url,http_boundary,totalsize); //头信息
    totalsize += head_len;
    
    buff = (char *)malloc(totalsize);
    memset(buff,0,totalsize);
    strcat(buff,header);  									//http头信息
    strcat(buff,send_request); 
    memcpy(buff+head_len+request_len,filebuf,read_byte);
    memcpy(buff+head_len+request_len+filesize,send_end,end_len);  //http结束信息
    
    upload_test->buf = (char *)malloc(sizeof(char) * totalsize);
    memset(upload_test->buf,0,totalsize);
    memcpy(upload_test->buf,buff,totalsize);
    upload_test->buf_len = totalsize;
    
    free(buff);
    free(filebuf);

    return 0;
}

int http_post(const char *url,const char *localfile)
{ 
    char post[BUFFER_SIZE] = {'\0'};
    int socket_fd = -1;
    char lpbuf[BUFFER_SIZE*4] = {'\0'};
    char host_addr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE] = {'\0'};
    unsigned long uploadlen = 0;
    int port = 0;
    int len=0;
	char *response = NULL;
 
    if(!url || !localfile){
        printf("failed!\n");
        return -1;
    }
 
    if(http_parse_url(url,host_addr,file,&port)){
        printf("http_parse_url failed!\n");
        return -1;
    }
 
    socket_fd = http_tcpclient_create(host_addr,port);
    if(socket_fd < 0){
        printf("http_tcpclient_create failed\n");
        return -1;
    }
    
    upload_buf upload_test;
    if(http_upload_readfile(host_addr,file,localfile,&upload_test) < 0){
        printf("http_upload_readfile failed..\n");
        return -1;
    }

    if(http_tcpclient_send(socket_fd,upload_test.buf,upload_test.buf_len) < 0){
        printf("http_tcpclient_send failed..\n");
        return -1;
    }
    
    if(http_get_response(socket_fd,lpbuf,BUFFER_SIZE*4) <= 0){
        printf("http_get_response failed\n");
        return -1;
    }

    http_get_resp_header(lpbuf,&resp);
    strcpy(resp.file_name, file);
    
    http_tcpclient_close(socket_fd);
 
    return 0;
}
 
char* http_get(const char *url)
{
    int socket_fd = -1;
    char lpbuf[BUFFER_SIZE*4] = {'\0'};
    char host_addr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE] = {'\0'};
    int port = 0;
 
    if(!url){
        printf("url error!\n");
        return NULL;
    }
 
    if(http_parse_url(url,host_addr,file,&port)){
        printf("http_parse_url failed!\n");
        return NULL;
    }
 
    socket_fd =  http_tcpclient_create(host_addr,port);
    if(socket_fd < 0){
        printf("http_tcpclient_create failed\n");
        return NULL;
    }

    sprintf(lpbuf,HTTP_GET,url,host_addr);
 
    if(http_tcpclient_send(socket_fd,lpbuf,strlen(lpbuf)) < 0){
        printf("http_tcpclient_send failed..\n");
        return NULL;
    }
    
    if(http_get_response(socket_fd,lpbuf,BUFFER_SIZE*4) <= 0){
        printf("http_get_response failed\n");
        return NULL;
    }

    http_get_resp_header(lpbuf,&resp);
    if (resp.status_code != 200)
    {
        printf("resp status error code:%d\n",resp.status_code);
        return NULL;
    }
    
    strcpy(resp.file_name, file);

    char *filepath = (char *)malloc( BUFFER_SIZE*sizeof(char) );
    filepath = http_download_writefile(socket_fd,10,0);
    if (filepath == NULL)
    {
        printf("http_download_writefile failed\n");
        return NULL;
    }
    
    http_tcpclient_close(socket_fd);
    
    
    return filepath;
}

int test(void)
{
    // char *buf;
    // unsigned long bufsize;
    // FILE* fp = fopen(picname, "rb+");							//打开要上传的图片
    // if (fp == NULL){  
    //     printf("open file fail!\r\n");  
    //     return -1;  
    // }

    // fseek(fp, 0, SEEK_END);
    // bufsize = ftell(fp);
    // rewind(fp);
    // printf("get the size is: %ld\n", bufsize);

    
    // buf = malloc(bufsize);
    // memset(buf, 0, bufsize);
    // fread(buf, bufsize , 1, fp);

    // int ret = http_get("http://xzd.197946.com/sscom32.zip");
    int ret = http_post("http://120.48.16.252/predict:32888","/home/xauat/predict/test.png");
    // remove(picname);

    return 0;
    
}