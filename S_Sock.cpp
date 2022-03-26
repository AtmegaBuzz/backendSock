#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int main(){

    // create a socket 

    int listening = socket(AF_INET,SOCK_STREAM,0);
    if(listening==-1){
        std::cerr << "failed to create socket!";
        return -1;
    }

    // bind the socket to ip and port 
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons((unsigned short)9009); 
    inet_pton(AF_INET,"127.0.0.1",&hint.sin_addr);

    if(bind(listening,(struct sockaddr*)&hint , sizeof(hint)==-1)){
        std::cerr << "socket binding failed!"<<  strerror(errno);
        return -2;
    }


    // mark the socket for listening 

    if(listen(listening,SOMAXCONN) == -1){
        std::cerr << "Cant listen!";
        return -3;
    }

    // accept a call

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(
                                listening,
                                (sockaddr*)&client,
                                &clientSize
                            );

    if(clientSocket==-1){
        std::cerr << "client connection failed!";
        return -4;
    }

    close(listening);
    memset(host,0,NI_MAXHOST);
    memset(svc,0,NI_MAXSERV);

    int result = getnameinfo(
                                (sockaddr*)&client,
                                sizeof(client),
                                host,
                                NI_MAXHOST,
                                svc,
                                NI_MAXSERV,
                                0    
                            );
    if(result){
        std::cout << host << "connected on " << svc << std::endl;
    }
    else{
        inet_ntop(AF_INET,&client.sin_addr,host,NI_MAXHOST);
        std::cout << host << "connected on" << ntohs(client.sin_port) << std::endl;
    }

    // close the listening socket 
    // while receiving : display message , echo message

    char buf[4096];
    while(true){

        // clear buffer 
        memset(buf,0,4096);
        // wait for the message
        int bytesRecv = recv(clientSocket,buf,4096,0);
        if(bytesRecv == -1){
            std::cerr << "connection problem, Disconnecting!";
            break;
        }
        if(bytesRecv == 0){
            std::cout << "client disconnected!";
            break;
        }

        // display in server
        std::cout << "Recieved: " << std::string(buf,0,bytesRecv) << std::endl;

        // echo back to client
        send(clientSocket,buf, bytesRecv+1,0);

    }

    // close the socket 
    close(clientSocket);



    return 0;
}

