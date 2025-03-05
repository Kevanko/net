#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    fprintf(stderr, "Использование: %s <IP_сервера> <порт_сервера> <число>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const char *server_ip = argv[1];
  int server_port = atoi(argv[2]);
  int number = atoi(argv[3]);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("Ошибка создания сокета");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(server_port);

  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0)
  {
    perror("Неверный адрес сервера");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Ошибка подключения");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Подключено к серверу %s:%d\n", server_ip, server_port);

  while (1)
  {
    if (write(sockfd, &number, sizeof(number)) < 0)
    {
      perror("Ошибка отправки данных");
      close(sockfd);
      exit(EXIT_FAILURE);
    }
    printf("Отправлено число: %d\n", number);
    sleep(number);

    close(sockfd);
    return 0;
  }
}
