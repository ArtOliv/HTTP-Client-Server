#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(){
    int server_sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrPort;
    addrPort.sin_family = AF_INET;
    addrPort.sin_addr.s_addr = htonl(INADDR_ANY);
    addrPort.sin_port = htons(6969);

    if(bind(server_sock, (struct sockaddr*) &addrPort, sizeof(addrPort)) == -1){
        printf("\nBind Failed to Port 6969\n\n");
        close(server_sock);
        return 1;
    } else {
        printf("\nBind Success to Port 6969\n\n");
    }

    if(listen(server_sock, 10) == -1){
        printf("Listen Faled\n\n");
        close(server_sock);
        return 1;
    } else {
        printf("Listening...\n\n");
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    int connection_sock = accept(server_sock, (struct sockaddr*) &client_addr, &addr_len);
    if(connection_sock == -1){
        printf("\nAccept error\n\n");
        close(server_sock);
        return 1;
    }
    printf("Accepted a connection\n\n");

    char msg[256];
    int countR;

    while(1){
        printf("To Client: ");
        fflush(stdout);

        if(!fgets(msg, sizeof(msg), stdin)){
            break;
        }

        size_t to_send = strlen(msg);
        if(to_send > 0){
            int sent = send(connection_sock, msg, to_send, 0);
            if(sent == -1){
                printf("\nSend message Failed\n\n");
                break;
            }

            if(strcmp(msg, "Close\n") == 0){
                break;
            }

            countR = recv(connection_sock, msg, sizeof(msg)-1, 0);
            if(countR == 0){
                printf("\nClient connection Closed\n\n");
                break;
            } else if(countR == -1){
                printf("\nReceive message failed\n\n");
                break;
            }

            msg[countR] = '\0';
            printf("\nFrom Client: %s\n", msg);
        }
    }

    close(server_sock);
    close(connection_sock);

    printf("\nSocket connection Closed\n");

    return 0;
}