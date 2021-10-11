#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCK_NAME "socket.soc"
#define MSG_MAX_LEN 256
#define PID_MAX_LEN 10

int sock = -1;
void sigint_catcher(int signum)
{
    close(sock);
    unlink(SOCK_NAME);
    exit(0);
}

int main()
{
    // Unix сокет, Без установки соединения. Дефолтный протокол
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("Error in create socket: %s\n", strerror(errno));
        return -EXIT_FAILURE;
    }
    printf("Sock created\n");
    signal(SIGINT, sigint_catcher);

    struct sockaddr_un srvr_name;
    srvr_name.sun_family = AF_UNIX;
    strcpy(srvr_name.sun_path, SOCK_NAME);

    if (bind(sock, (struct sockaddr *)&srvr_name, strlen(srvr_name.sun_path) + sizeof(srvr_name.sun_family)) < 0)
    {
        printf("Failed bind socket: %s\n", strerror(errno));

        close(sock);
        unlink(SOCK_NAME);
        return -EXIT_FAILURE;
    }

    pid_t client_pid;
    char client_msg[MSG_MAX_LEN];

    int code_error = 0;
    int msg_len = 0;
    while (!code_error)
    {
        if (recvfrom(sock, &client_pid, sizeof(pid_t), 0, NULL, NULL) < 0 ||
            (msg_len = recvfrom(sock, client_msg, MSG_MAX_LEN, 0, NULL, NULL)) < 0)
        {
            printf("Failed receive message: %s\n", strerror(errno));
            code_error = -EXIT_FAILURE;
        }
        else
        {
            client_msg[msg_len] = 0;
            printf("Message received from client %d: %s\n", client_pid, client_msg);
        }
    }
    
    close(sock);
    unlink(SOCK_NAME);
    return code_error;
}