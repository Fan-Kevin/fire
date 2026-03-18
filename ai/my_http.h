#ifndef _MY_HTTP_H_
#define _MY_HTTP_H_
 
#define MY_HTTP_DEFAULT_PORT 80

// #define DOWNLOAD_PATH   "/home/xauat/rv1126_download/%s"
#define DOWNLOAD_PATH   "/userdata/rv1126_download/%s"

typedef struct {
    int status_code;//HTTP/1.1 '200' OK
    char content_type[128];//Content-Type: application/gzip
    long content_length;//Content-Length: 11683079
    char file_name[256];
}resp_header_def;

typedef struct {
    unsigned long buf_len;
    char *buf;
}upload_buf;

char* http_get(const char *url);
int http_post(const char *url,const char *localfile);
 
#endif