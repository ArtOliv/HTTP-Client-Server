#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>

int main(){
    // Create server socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrPort;
    addrPort.sin_family = AF_INET;
    addrPort.sin_addr.s_addr = htonl(INADDR_ANY);
    addrPort.sin_port = htons(6969);

    // Bind server to the port
    if(bind(server_sock, (struct sockaddr*) &addrPort, sizeof(addrPort)) == -1){
        printf("\nBind failed to port 6969\n\n");
        close(server_sock);
        return 1;
    } else {
        printf("\nBind success to port 6969\n\n");
    }

    // Put server to listen to a connection
    if(listen(server_sock, 10) == -1){
        printf("Listen failed\n\n");
        close(server_sock);
        return 1;
    }

    // Define server response
    char response[30000];
    char body[] = "<html><body><h1>Hello World!</h1></body></html>";
    
    snprintf(response, sizeof(response), 
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: text/html\r\n"
        //"Connection: close\r\n"
        "\r\n"
        "%s", 
        strlen(body), body
    );

    size_t response_len  = strlen(response);

    char buffer[30000];
    int received, sent;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while(1){
        printf("Waiting for connection...\n\n");

        // Create connection socket
        int connection_sock = accept(server_sock, (struct sockaddr*) &client_addr, &addr_len);
        if(connection_sock == -1){
            printf("\nAccept error\n\n");
            close(server_sock);
            return 1;
        }
        printf("Accepted a connection\n\n");

        // Set connection timeout
        struct timeval timeout;
        timeout.tv_sec = 120;
        timeout.tv_usec = 0;
        if(setsockopt(connection_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0){
            printf("\nError on setting timeout(not fatal)\n\n");
        }

        // Server receive the request
        while(1){
            received = recv(connection_sock, buffer, sizeof(buffer)-1, 0);

            if(received == 0){
                printf("\nClient connection closed\n\n");
                break;
            } else if(received == -1){
                if(errno == EWOULDBLOCK || errno == EAGAIN){
                    printf("\nConnection timed out\n");
                    break;
                } else {
                    printf("\nReceive message failed\n\n");
                    break;
                }
            }
            buffer[received] = '\0';
            printf("%s\n", buffer);

            // Server sends the response
            sent = send(connection_sock, response, response_len, 0);
            if(sent == -1){
                printf("\nSend message failed\n\n");
                break;
            }
            printf("%s\n", response);
        }

        printf("\nClient disconnected\n\n");
        close(connection_sock);
    }

    // Close server connections
    close(server_sock);

    printf("Socket connection closed\n");

    return 0;
}