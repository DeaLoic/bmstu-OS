#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define MSG_SIZE 256
#define HOST_NAME "0.0.0.0"
#define PORT "9998"

int main(int argc, char ** argv)
{
  struct sockaddr_in server;
  struct hostent *host;
  char msg_client[MSG_SIZE], msg_server[MSG_SIZE];
  char id[10];
  sprintf(id, "%d", getpid());
  id[strlen(id)] = 0;

  // AF_INET сокет, SOCK_STREAM. Дефолтный протокол
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    printf("%s", strerror(errno));
    return errno;
  }

  host = gethostbyname(HOST_NAME);
  if (host == NULL)
  {
    printf("%s", strerror(errno));
    return errno;
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(PORT));
  // Адрес хоста
  memcpy(&server.sin_addr, host->h_addr_list[0], host->h_length);

  if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
  {
    printf("%s", strerror(errno));
    return errno;
  }
  write(sock, id, strlen(id));

  while (strcmp(msg_client, "stop\n"))
  {
    memset(msg_client, 0, MSG_SIZE);
    printf("Введите сообщение:\n");
    fgets(msg_client, MSG_SIZE, stdin);

    write(sock, msg_client, strlen(msg_client));

    memset(msg_server, 0, MSG_SIZE);

    read(sock, msg_server, MSG_SIZE);
    printf("%s\n", msg_server);
  }
  // Закрываем сокет
  close(sock);

  return 0;
}