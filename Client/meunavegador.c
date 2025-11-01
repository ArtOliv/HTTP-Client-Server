#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("\nIncorrect use for request, use: %s http://<host>:<port>/<file>\n\n", argv[0]);
        return 1;
    }

    // Parsin URL
    int port = 80;
    char host[256] = "", path[1024] = "";
    char *url_start = argv[1] + 7;
    char *path_ptr = strchr(url_start, '/');
    char *port_ptr = strchr(url_start, ':');

    if(port_ptr && (!path_ptr || port_ptr < path_ptr)){
        int host_len = port_ptr - url_start;
        strncpy(host, url_start, host_len);
        host[host_len] = '\0';
        port = atoi(port_ptr + 1);

        if(path_ptr){
            strcpy(path, path_ptr + 1);
        } else {
            strcpy(path, "");
        }
    } else if(path_ptr){
        int host_len = path_ptr - url_start;
        strncpy(host, url_start, host_len);
        host[host_len] = '\0';
        strcpy(path, path_ptr + 1);
    } else {
        strcpy(host, url_start);
        strcpy(path, "");
    }

    // Create and connect client socket
    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%d", port);
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if(getaddrinfo(host, port_str, &hints, &res) != 0){
        printf("\nError on resolving host\n\n");
        return 1;
    }

    int client_sock = -1;
    for(p = res; p != NULL; p = p->ai_next){
        client_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if(client_sock == -1){
            continue;
        }

        if(connect(client_sock, p->ai_addr, p->ai_addrlen) == -1){
            printf("\nConnection failed\n\n");
            close(client_sock);
        } else {
            printf("\nConnected to server via port %d\n\n", port);
            break;
        }

        client_sock = -1;
    }
    freeaddrinfo(res);
    
    if(client_sock == -1){
        printf("\nConnection failed\n\n");
        return 1;
    }

    // Define client request
    char request[30000];
    if(port == 80){
        snprintf(request, sizeof(request), 
            "GET /%s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "\r\n", path, host
        );
    } else {
        snprintf(request, sizeof(request), 
            "GET /%s HTTP/1.1\r\n"
            "Host: %s:%d\r\n"
            "Connection: close\r\n"
            "\r\n", path, host, port
        );
    }

    size_t request_len = strlen(request);
    int received, sent;

    // Client send the request
    sent = send(client_sock, request, request_len, 0);
    if(sent == -1){
        printf("\nSend message failed\n\n");
        close(client_sock);
        return 1;
    }
    
    // Client receive server response
    char buffer[30000];
    int header_end = 0;
    FILE *file = NULL;

    received = recv(client_sock, buffer, sizeof(buffer)-1, 0);
    if(received == 0){
        printf("\nServer connection closed\n\n");
        close(client_sock);
        return 1;
    } else if(received == -1){
        printf("\nReceive message failed\n\n");
        close(client_sock);
        return 1;
    }
    buffer[received] = '\0';
    
    char *res_body = strstr(buffer, "\r\n\r\n");
    if(res_body != NULL){
        header_end = (res_body - buffer) + 4;

        int status_code = 0;
        if(sscanf(buffer, "HTTP/%*[^ ] %d", &status_code) != 1){
            status_code = 0;
        }
        printf("\n%.*s\n", header_end, buffer);
        
        if(status_code != 200){
            printf("Code %d, aborting file save.\n", status_code);
            close(client_sock);
            return 1;
        }

        char *filename = path;
        if(strlen(filename) == 0){
            filename = "index.html";
        }

        file = fopen(filename, "wb");
        if(file != NULL){
            fwrite(buffer + header_end, 1, received - header_end, file);

            while((received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0){
                fwrite(buffer, 1, received, file);
            }

            fclose(file);
            printf("File '%s' saved!\n\n", filename);
        } else {
            printf("\nError creating file '%s'\n\n", filename);
        }
    }

    // Close client connection
    close(client_sock);
    printf("Socket connection closed\n");

    return 0;
}