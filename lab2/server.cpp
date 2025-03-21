#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>


// Обработчик сигнала SIGCHLD для очистки завершённых дочерних процессов (зомби)
void reaper( int sig )
{ int status;
 while( wait3( &status, WNOHANG, (struct rusage *) 0 ) >= 0 ) ;
}


// Функция обработки клиента в дочернем процессе
void handle_client(int client_sock)
{
  int number;
  ssize_t bytes;
  while ((bytes = read(client_sock, &number, sizeof(number))) > 0)
  {
    printf("Получено число: %d\n", number);
    fflush(stdout);
    sleep(number);
  }
  close(client_sock);
  exit(0);
}

int main()
{
  int sockfd, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len;

  // Создаем TCP-сокет SOCK_STREAM
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("Ошибка создания сокета");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = 0;

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("Ошибка привязки (bind)");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  addr_len = sizeof(server_addr);
  if (getsockname(sockfd, (struct sockaddr *)&server_addr, &addr_len) < 0)
  {
    perror("Ошибка getsockname");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Сервер слушает на порту: %d\n", ntohs(server_addr.sin_port));

  if (listen(sockfd, 3) < 0)
  {
    perror("Ошибка listen");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  signal( SIGCHLD, reaper);
  while (1)
  {
    socklen_t client_len = sizeof(client_addr);
    client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0)
    {
      perror("Ошибка accept");
      continue;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
      perror("Ошибка fork");
      close(client_sock);
      continue;
    }
    else if (pid == 0)
    {
      close(sockfd); // Дочернему не нужен слушающий сокет
      handle_client(client_sock);
    }
    else
    {
      close(client_sock); // Родитель закрывает дескриптор подключенного сокета
    }
  }

  close(sockfd);
  return 0;
}
