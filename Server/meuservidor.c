#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
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

    // Puts server to listen to a connection
    if(listen(server_sock, 10) == -1){
        printf("Listen failed\n\n");
        close(server_sock);
        return 1;
    } else {
        printf("Listening...\n\n");
    }

    // Create connection socket
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    int connection_sock = accept(server_sock, (struct sockaddr*) &client_addr, &addr_len);
    if(connection_sock == -1){
        printf("\nAccept error\n\n");
        close(server_sock);
        return 1;
    }
    printf("Accepted a connection\n\n");

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
    
    while(1){
        received = recv(connection_sock, buffer, sizeof(buffer)-1, 0);
        if(received == 0){
            printf("\nClient connection closed\n\n");
            break;
        } else if(received == -1){
            printf("\nReceive message failed\n\n");
            break;
        }
        buffer[received] = '\0';
        printf("%s\n", buffer);

        sent = send(connection_sock, response, response_len, 0);
        if(sent == -1){
            printf("\nSend message failed\n\n");
            break;
        }
        printf("%s\n\n", response);
    }

    // Close server connections
    close(server_sock);
    close(connection_sock);

    printf("Socket connection closed\n");

    return 0;
}