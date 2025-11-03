#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <arpa/inet.h>

const char *file_type(const char *path){ // Return the Content-Type based on file type
    const char *ext = strrchr(path, '.');
    if(!ext) return "application/octet-stream";
    if(strcasecmp(ext, ".html") == 0 || strcasecmp(ext, ".htm") == 0) return "text/html";
    if(strcasecmp(ext, ".css") == 0) return "text/css";
    if(strcasecmp(ext, ".js") == 0) return "application/javascript";
    if(strcasecmp(ext, ".png") == 0) return "image/png";
    if(strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0) return "image/jpeg";
    if(strcasecmp(ext, ".gif") == 0) return "image/gif";
    if(strcasecmp(ext, ".pdf") == 0) return "application/pdf";
    if(strcasecmp(ext, ".txt") == 0) return "text/plain";
    return "application/octet-stream";
}

void send_error(int socket, int code, const char *msg){
    char response[256];

    snprintf(response, sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Connection: close\r\n"
        "Content-Length: 0\r\n"
        "\r\n",
        code, msg
    );
    size_t res_len = strlen(response);

    if(send(socket, response, res_len, 0) == -1){        
        printf("\nSend message failed\n\n");
        return;
    }
    printf("%.*s\n", (int)res_len, response);
}

void send_file(int socket, const char *path, const char *ftype){
    FILE *file = fopen(path, "rb");
    if(!file){
        send_error(socket, 404, "Not Found");
        return;
    }

    if(fseek(file, 0, SEEK_END) != 0){ // Points to EOF
        fclose(file);
        send_error(socket, 500, "Internal Server Error");
        return;
    }

    long int file_len = ftell(file); // Returns file length
    if(file_len < 0){
        fclose(file);
        send_error(socket, 500, "Internal Server Error");
        return;
    }
    rewind(file);

    char response[256];
    if(strcmp(ftype, "text/html") == 0){ // If it is a html file, use encoding
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s; charset=UTF-8\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n"
            "\r\n",
            ftype,
            file_len
        );
    } else {
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n"
            "\r\n",
            ftype,
            file_len
        );
    }
    size_t res_len = strlen(response);

    if(send(socket, response, res_len, 0) == -1){
        printf("\nSend message failed (header)\n\n");
        fclose(file);
        return;
    }

    char buffer[30000];
    size_t read_bytes;

    while((read_bytes = fread(buffer, 1, sizeof(buffer), file)) > 0){ 
        if(send(socket, buffer, read_bytes, 0) == -1){
            printf("\nSend message failed\n\n");
            fclose(file);
            return;
        }
    }
    
    fclose(file);
}

void send_file_list(int socket, const char *directory, const char *msg){
    DIR *dir = opendir(directory);
    if(!dir){
        send_error(socket, 404, "Not Found");
        return;
    }

    char response[30000];
    size_t off = 0;
    off += snprintf(response + off, sizeof(response) - off,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html><body><h1>%s</h1><ul>",
        msg
    );

    struct dirent *ent;
    while((ent = readdir(dir)) != NULL){
        if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0){
            continue;
        }

        off += snprintf(response + off, sizeof(response) - off, "<li><a href=\"%s\">%s</a></li>", ent->d_name, ent->d_name);

        if(off > sizeof(response) - 256){
            break;
        }
    }
    closedir(dir);

    off += snprintf(response + off, sizeof(response) - off, "</ul></body></html>");

    if(send(socket, response, off, 0) == -1){        
        printf("\nSend message failed\n\n");
        return;
    }
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("\nError on inicializing the server, use: %s path/to/folder\n\n", argv[0]);
        return 1;
    }

    char *directory = argv[1];

    // Create server socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1){
        printf("\nError on creating socket\n\n");
        return 1;
    }

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

    // Receive a request and send response
    char buffer[30000];
    int received;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while(1){
        printf("Waiting for connection...\n\n");

        // Create connection socket
        int connection_sock = accept(server_sock, (struct sockaddr*) &client_addr, &addr_len);
        if(connection_sock == -1){
            printf("\nError on accepting a connection\n\n");
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

            char method[8], file[512];
            file[0] = '\0';
            sscanf(buffer, "%7s %511s", method, file);
            if(strcmp(method, "GET") != 0){
                send_error(connection_sock, 405, "Method Not Allowed");
                close(connection_sock);
                continue;
            }
            printf("%s\n", buffer);

            const char *ftype = file_type(file);
            char full_path[1024];

            if(strcmp(file, "/") == 0){ // Just to avoid double slashes
                snprintf(full_path, sizeof(full_path), "%s", directory);
            } else {
                snprintf(full_path, sizeof(full_path), "%s%s", directory, file); // Concatenates the directory with the requested file
            }
            
            if(strlen(full_path) >= sizeof(full_path) - 1){
                send_error(connection_sock, 414, "Request-URI Too Long");
                break;
            }

            // Server sends the response
            struct stat st_full;
            if(stat(full_path, &st_full) == -1){ // If index.html does not exists, return file list
                if(strcmp(file, "/index.html") == 0){
                    send_file_list(connection_sock, directory, "Directory listing - index.html not found");
                } else {
                    send_error(connection_sock, 404, "Not Found");
                }
                break;
            }
            
            if(S_ISDIR(st_full.st_mode)){ // If it's just a directory
                char index_file[2048];

                snprintf(index_file, sizeof(index_file), "%s/index.html", full_path); // Concatenate index.html to check if it exists
                
                if(strlen(index_file) >= sizeof(index_file)){
                    send_error(connection_sock, 414, "Request-URI Too Long");
                    break;
                }
                
                struct stat st_index;
                if(stat(index_file, &st_index) == 0 && S_ISREG(st_index.st_mode)){   // If index.html exists, send it
                    ftype = file_type(index_file);
                    send_file(connection_sock, index_file, ftype);
                } else {                                                             // If not, send file list
                    send_file_list(connection_sock, full_path, "Directory listing");
                }
            } else { // If not, send file
                send_file(connection_sock, full_path, ftype);
            }

            printf("File sent successfully\n\n");
            break;
        }

        close(connection_sock);
    }

    // Close server connections
    close(server_sock);

    printf("Socket connection closed\n");

    return 0;
}