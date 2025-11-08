#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server-ip>\n", argv[0]);
        return 1;
    }

    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024];
    char username[50], password[50];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        return 1;
    }

    printf("Connected to %s:%d\n", argv[1], PORT);

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    send(sock, username, strlen(username), 0);
    send(sock, password, strlen(password), 0);

    recv(sock, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer); // LOGIN SUCCESS or FAIL

    while (1) {
        printf("Enter command (UPLOAD <file>, DOWNLOAD <file>, LIST, EXIT):\n> ");
        getchar(); // consume leftover newline
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0; // remove newline

        send(sock, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "UPLOAD ", 7) == 0) {
            int bytes_received = recv(sock, buffer, sizeof(buffer)-1, 0);
            buffer[bytes_received] = '\0';
            printf("Server: %s\n", buffer);
        }
        else if (strncmp(buffer, "DOWNLOAD ", 9) == 0) {
            int bytes_received = recv(sock, buffer, sizeof(buffer)-1, 0);
            buffer[bytes_received] = '\0'; // fix garbage characters
            printf("Server: %s\n", buffer);
        }
        else if (strcmp(buffer, "LIST") == 0) {
            int bytes_received = recv(sock, buffer, sizeof(buffer)-1, 0);
            buffer[bytes_received] = '\0';
            printf("Server: %s\n", buffer);
        }
        else if (strcmp(buffer, "EXIT") == 0) {
            printf("Disconnected from server.\n");
            break;
        }
        else {
            int bytes_received = recv(sock, buffer, sizeof(buffer)-1, 0);
            buffer[bytes_received] = '\0';
            printf("Server: %s\n", buffer);
        }
    }

    close(sock);
    return 0;
}


