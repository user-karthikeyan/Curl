#include "curl.h"

request currentRequest;
bool verbose = False;


//Function to explain syntax and Available Commands
void help(){
    printf("\ncommand -[options]\n");
    printf("Available options:\n\tsyntax [description]\n\t");
    printf("-v [verbose]\n\t-X methodname [Explicitly specify method]\n\t-d dataToBeSent [To send data with request]\n\t-h headerfieldsToBeAdded [To specify additional header fields]\n\t");
}

//Function that prints when verbose is set
void verbosePrint(char content[], int flag){
    content = strtok(content, "\r\n");
    char *format = (flag == REQUEST)?"<%s\n":">%s\n";

    while(content != NULL){
            printf(format, content);
            content = strtok(NULL, "\r\n");
    }
    printf("\n");

}


void error(char *message){
    printf(message);
    help();
    exit(0);
}

//This function parses Args
void parseArgs(int parc, char *parv[]){

    char domain[256];
    char url[2048];
    char firstChar;

    bool urlSet = False;

    //Defaults
    strcpy(currentRequest.method, "GET");
    strcpy(currentRequest.headers, "");
    strcpy(currentRequest.body, "");

    for(int i = 0; i < parc; i++){
        firstChar = parv[i][0];

        switch(firstChar){
            case '-':
                i += setOption(parv[i], (i + 1 < parc)?parv[i + 1]:NULL);
                break;
            default:
                strcpy(url, parv[i]);
                urlSet = True;
        }
    }

    if(!urlSet){
        error(ERR_NO_URL);
    }

    sscanf(url, "%[^:]://%[^/]/%s", currentRequest.protocol, domain, currentRequest.path);
    sscanf(domain, "%[^:]:%d", currentRequest.host, &currentRequest.port);
}

//Function to enable a option
bool setOption(const char *option, char *next){

    bool consumed = False;
    char length[20];

    for(int i = 1; option[i] != '\0'; i++){
            switch(option[i]){
                case 'v':{
                    verbose = True;
                    break;
                }
                case 'X':{
                    if(next){
                        strcpy(currentRequest.method, next);
                        consumed = True;
                        break;
                    }
                    else{
                        error(ERR_NO_MTD);
                    }
                }
                case 'd':{
                    if(next){
                        strcpy(currentRequest.body, next);
                        sprintf(length, "Content-Length: %d\r\n", strlen(next));
                        strcat(currentRequest.headers, length);
                        consumed = True;
                        break;
                    }
                    else{
                        error(ERR_NO_DTA);
                    }
                }

                case 'h':{
                    if(next){
                        strcat(currentRequest.headers, next);
                        strcat(currentRequest.headers, "\r\n");
                        consumed = True;
                        break;
                    }
                    else{
                        error(ERR_NO_HDR);
                    }
                }
            }
    }

    return consumed;

}


int createConnection(){
    struct addrinfo hints, *result = NULL , *resultptr = NULL;
    struct sockaddr *server;
    int socketfd;
    int status;
    bool set = False;

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = 0;


    if ((status = getaddrinfo(currentRequest.host, currentRequest.protocol , &hints, &resultptr)) != 0) {
               fprintf(stderr, "%s\n", gai_strerror(status));
               exit(EXIT_FAILURE);
    }

    for( result = resultptr; result != NULL; result = result -> ai_next){

        socketfd = socket(result -> ai_family, result -> ai_socktype, result -> ai_protocol);

        if(socketfd == -1){
            continue;
        }

        if(connect(socketfd, result -> ai_addr , result -> ai_addrlen) != -1){
            set = True;
            break;
        }

        close(socketfd);
    }

    freeaddrinfo(resultptr);

    if(set){
        currentRequest.fd = socketfd;
    }
    else{
        exit(0);
    }
}


void sendRequest(){
    char *httpFormat = "%s /%s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nConnection: close\r\n%s\r\n%s";
    char *headers = NULL;
    char httpMessage[1024 * 8];
    char httpReceive[1024 * 8];
    char responseHeaders[1024];

    memset(httpMessage, 0, sizeof(httpMessage));
    memset(httpReceive, 0, sizeof(httpReceive));

    sprintf(httpMessage, httpFormat, currentRequest.method, currentRequest.path, currentRequest.host, currentRequest.headers, currentRequest.body);

    send(currentRequest.fd, httpMessage, sizeof(httpMessage), MSG_CONFIRM);

    if(verbose){
        verbosePrint(httpMessage, REQUEST);
    }

    recv(currentRequest.fd, httpReceive, sizeof(httpReceive), MSG_WAITALL);


    if(verbose){
        EXTRACT_RES_HEADER(httpReceive, responseHeaders);
        verbosePrint(responseHeaders, RESPONSE);
    }

    printf("%s\n", CONTENT_START(httpReceive));
}


