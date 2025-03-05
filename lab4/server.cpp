#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BACKLOG 5
#define BUFFER_SIZE 256

int main(void)
{
  int msock, newfd, maxfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addrlen;
  fd_set master_set, read_set;

  msock = socket(AF_INET, SOCK_STREAM, 0);
  if (msock < 0)
  {
    perror("socket error");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = 0;

  if (bind(msock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("bind error");
    close(msock);
    exit(EXIT_FAILURE);
  }

  addrlen = sizeof(server_addr);
  if (getsockname(msock, (struct sockaddr *)&server_addr, &addrlen) < 0)
  {
    perror("getsockname error");
    close(msock);
    exit(EXIT_FAILURE);
  }
  printf("Сервер слушает на порту: %d\n", ntohs(server_addr.sin_port));

  // Переводим сокет в режим прослушивания
  if (listen(msock, BACKLOG) < 0)
  {
    perror("listen error");
    close(msock);
    exit(EXIT_FAILURE);
  }

  // Инициализируем множество дескрипторов и добавляем слушающий сокет
  FD_ZERO(&master_set);
  FD_SET(msock, &master_set);
  maxfd = msock;

  // Открываем общий файл для логирования сообщений (режим добавления)
  FILE *logfile = fopen("data.txt", "a");
  if (!logfile)
  {
    perror("Ошибка открытия файла");
    close(msock);
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    read_set = master_set; // делаем копию множества дескрипторов для select
    int activity = select(maxfd + 1, &read_set, NULL, NULL, NULL);
    if (activity < 0)
    {
      perror("select error");
      break;
    }

    // Проходим по всем дескрипторам от 0 до maxfd
    for (int fd = 0; fd <= maxfd; fd++)
    {
      if (FD_ISSET(fd, &read_set))
      {
        if (fd == msock)
        {
          // Новое подключение
          addrlen = sizeof(client_addr);
          newfd = accept(msock, (struct sockaddr *)&client_addr, &addrlen);
          if (newfd < 0)
          {
            perror("accept error");
            continue;
          }
          FD_SET(newfd, &master_set);
          if (newfd > maxfd)
            maxfd = newfd;
          printf("Новое подключение: %s, сокет %d\n",
                 inet_ntoa(client_addr.sin_addr), newfd);
        }
        else
        {
          // Данные от существующего клиента
          int number;
          int n = read(fd, &number, sizeof(number));
          if (n <= 0)
          {
            if (n == 0)
              printf("Сокет %d: соединение закрыто клиентом\n", fd);
            else
              perror("read error");
            close(fd);
            FD_CLR(fd, &master_set);
          }
          else
          {
            // Вывод полученного числа на экран
            printf("Получено число: %d от сокета %d\n", number, fd);
            // Запись в лог-файл
            fprintf(logfile, "Сокет %d: %d\n", fd, number);
            fflush(logfile);
          }
        }
      }
    }
  }

  fclose(logfile);
  close(msock);
  return 0;
}
