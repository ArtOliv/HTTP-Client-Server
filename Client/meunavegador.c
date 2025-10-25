#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(){
    // Create client socket
    int client_sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrPort;
    addrPort.sin_family = AF_INET;
    addrPort.sin_addr.s_addr = htonl(INADDR_ANY);
    addrPort.sin_port = htons(6969);

    // Connect to a server
    if(connect(client_sock, (struct sockaddr*) &addrPort, sizeof(addrPort)) == -1){
        printf("\nConnect failed\n\n");
        close(client_sock);
        return 1;
    } else {
        printf("\nConnected to server via Port 6969\n\n");
    }

    // Define client request
    char request[] = 
    "GET / HTTP/1.1\r\n"
    "Host: 127.0.0.1:6969\r\n"
    "Connection: closed\r\n"
    "\r\n";

    size_t request_len = strlen(request);
    int received, sent;

    // Client send the request
    sent = send(client_sock, request, request_len, 0);
    if(sent == -1){
        printf("\nSend message failed\n\n");
    }
    
    // Client receive server response
    char buffer[30000];
    received = recv(client_sock, buffer, sizeof(buffer)-1, 0);
    if(received == 0){
        printf("\nServer connection closed\n\n");
    } else if(received == -1){
        printf("\nReceive message failed\n\n");
    }
    
    buffer[received] = '\0';
    printf("%s\n\n", buffer);

    // Close client connection
    close(client_sock);
    printf("\nSocket connection closed\n");

    return 0;
}