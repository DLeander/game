#include<iostream>
#include "server/server.h"

int main(int argc, char *argv[]){   
    CSERVER server(2);
    server.run();
    return 0;
}