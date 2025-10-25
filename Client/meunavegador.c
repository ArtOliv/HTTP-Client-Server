#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(){
    int client_sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrPort;
    addrPort.sin_family = AF_INET;
    addrPort.sin_addr.s_addr = htonl(INADDR_ANY);
    addrPort.sin_port = htons(6969);

    if(connect(client_sock, (struct sockaddr*) &addrPort, sizeof(addrPort)) == -1){
        printf("\nConnect Failed\n\n");
        close(client_sock);
        return 1;
    } else {
        printf("\nConnected via Port 6969\n\n");
    }

    char msg[256];
    int countR;

    while(1){
        countR = recv(client_sock, msg, sizeof(msg)-1, 0);
        if(countR == 0){
            printf("\nServer connection Closed\n\n");
            break;
        } else if(countR == -1){
            printf("\nReceive message failed\n\n");
            break;
        }

        msg[countR] = '\0';
        printf("\nFrom Server: %s\n", msg);

        printf("To server: ");
        fflush(stdout);

        if(!fgets(msg, sizeof(msg), stdin)){
            break;
        }
        
        if(strcmp(msg, "Close\n") == 0){
            break;
        }

        size_t to_send = strlen(msg);
        if(to_send > 0){
            int sent = send(client_sock, msg, to_send, 0);
            if(sent == -1){
                printf("\nSend message Failed\n\n");
                break;
            }
        }


    }

    close(client_sock);
    printf("\nSocket connection Closed\n");

    return 0;
}