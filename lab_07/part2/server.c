#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

#define MSG_SIZE 256
#define LISTENQ 1024
#define HOST_NAME "0.0.0.0"
#define PORT "9998"

int maxi, maxfd;

int new_client(int sock, int client[FD_SETSIZE], int id[FD_SETSIZE], fd_set *allset, fd_set *reset)
{
  int i;
  int connfd;
  char pid[MSG_SIZE];

  if (FD_ISSET(sock, reset))
  {
    int connfd = accept(sock, NULL, NULL);
    if (connfd < 0)
    {
      printf("%s", strerror(errno));
      return errno;
    }
    // Функция accept() возвращает копию дескриптора сокета, открытого
    // для обмена данными с клиентом, запросившим соединение

    for (i = 0; i < FD_SETSIZE; i++)
    {
      if (client[i] < 0)
      {
        client[i] = connfd;
        break;
      }
    }

    if (i == FD_SETSIZE)
    {
      printf("Достигнуто максимальное число клиентов");
      return errno;
    }

    FD_SET(connfd, allset);

    if (connfd > maxfd)
      maxfd = connfd;

    if (i > maxi)
      maxi = i;

    read(connfd, pid, MSG_SIZE);
    id[i] = atoi(pid);
    printf("Клиент %d подключился\n", id[i]);
  }
  return errno;
}

int read_msg(int client[FD_SETSIZE], int id[FD_SETSIZE], fd_set *allset, fd_set *reset)
{
  int n, i;
  int sockfd;
  char msg[MSG_SIZE];

  for (i = 0; i <= maxi; i++)
  {
    if ((sockfd = client[i]) > 0)
    {
      if (FD_ISSET(sockfd, reset))
      {
        if ((n = read(sockfd, msg, MSG_SIZE)) == 0)
        {
          close(sockfd);
          FD_CLR(sockfd, allset);
          client[i] = -1;
        }
        else
        {
          write(sockfd, "OK", 2);
          msg[n] = 0;
          printf("Message from %d: %s", id[i], msg);
        }
      }
    }
  }
  return errno;
}

int main()
{
  int sock;
  int client[FD_SETSIZE];
  int id[FD_SETSIZE];
  fd_set reset, allset;
  struct sockaddr_in server;

  // AF_INET сокет, SOCK_STREAM. Дефолтный протокол
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    printf("%s", strerror(errno));
    return errno;
  }

  server.sin_family = AF_INET;
  // адресс (INADDR_ANY = Все адреса локального хоста 0.0.0.0)
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(atoi(PORT));

  // Присвоить имя
  if (bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
  {
    printf("%s", strerror(errno));
    return errno;
  }

  // Переводим сервер в режим пассивного слушания
  listen(sock, LISTENQ);

  maxfd = sock;
  maxi = -1;
  
  for (int i = 0; i < FD_SETSIZE; i++)
    client[i] = -1; 

  FD_ZERO(&allset);
  FD_SET(sock, &allset);

  struct timespec timesp = {600, 1};

  while(1)
  {
    reset = allset;
    
    // Ожидаем поступление данных или нового соединения
    pselect(maxfd + 1, &reset, NULL, NULL, &timesp, NULL);

    if (new_client(sock, client, id, &allset, &reset) ||
        read_msg(client, id, &allset, &reset))
      return errno;
  }

  // Закрываем сокет
  close(sock);

  return errno;
}