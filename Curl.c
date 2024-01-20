#include "curl.h"

int main(int argc, char* argv[]){
    parseArgs(argc - 1, argv + 1);
    createConnection();
    sendRequest();
}
