#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define HEADER_START(response) response
#define HEADER_END(response) strstr(response, "\r\n\r\n")
#define CONTENT_START(response) HEADER_END(response) + 4

#define EXTRACT_RES_HEADER(from, to) strncpy(to, HEADER_START(from) , HEADER_END(from) - HEADER_START(from));

#define ERR_NO_HDR "No header specified"
#define ERR_NO_URL "No URL specified"
#define ERR_NO_DTA "No data specified"
#define ERR_NO_MTD "No method specified"


#define RESPONSE 0
#define REQUEST 1

typedef enum{False, True} bool;
typedef struct{
    char protocol[8];
    char host[128];
    char path[128];
    char method[8];
    char headers[1024];
    char body[2048];
    int port;
    int fd;
}request;

bool setOption(const char* option, char *next);
void help();
void parseArgs(int parc, char *parv[]);
int createConnection();
void sendRequest();

