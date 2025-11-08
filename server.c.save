#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

#define PORT 8080

void handle_client(int client_socket) {
    char username[50], password[50];
    char buffer[1024];
    FILE *file;

    // Receive username and password
    recv(client_socket, username, sizeof(username), 0);
    recv(client_socket, password, sizeof(password), 0);

    if (strcmp(username, "user1") == 0 && strcmp(password, "pass123") == 0) {
        send(client_socket, "LOGIN SUCCESS", 13, 0);
    } else {
        send(client_socket, "LOGIN FAILED", 12, 0);
        close(client_socket);
        return;
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Client: %s\n", buffer);

        if (strncmp(buffer, "UPLOAD ", 7) == 0) {
            char filename[100];
            sscanf(buffer + 7, "%s", filename);

            file = fopen(filename, "w");
            send(client_socket, "READY", 5, 0);

            memset(buffer, 0, sizeof(buffer));
            recv(client_socket, buffer, sizeof(buffer), 0);
            fprintf(file, "%s", buffer);
            fclose(file);

            send(client_socket, "UPLOAD OK", 9, 0);
        }
        else if (strcmp(buffer, "LIST") == 0) {
            struct dirent *de;
            DIR *dr = opendir(".");
            char list[1024] = "";
            if (dr == NULL) {
                strcpy(list, "Could not open directory");
            } else {
                while ((de = readdir(dr)) != NULL) {
                    if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
                        strcat(list, de->d_name), strcat(list, "\n");
                }
                closedir(dr);
            }
            send(client_socket, list, strlen(list), 0);
        }
        else if (strncmp(buffer, "DOWNLOAD ", 9) == 0) {
            char filename[100];
            sscanf(buffer + 9, "%s", filename);

            file = fopen(filename, "r");
            if (!file) {
                send(client_socket, "NO FILE", 7, 0);
            } else {
                memset(buffer, 0, sizeof(buffer));
                fread(buffer, 1, sizeof(buffer), file);
                fclose(file);
                send(client_socket, buffer, strlen(buffer), 0);
            }
        }
        else if (strcmp(buffer, "EXIT") == 0) {
            break;
        }
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    listen(server_socket, 5);
    printf("Server Running on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len);
        printf("Client Connected.\n");
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}

