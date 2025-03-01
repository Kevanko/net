#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " <server_ip> <server_port>" << std::endl;
    return 1;
  }

  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  int server_port = atoi(argv[2]);

  // Создание UDP сокета
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    std::cerr << "Socket creation failed" << std::endl;
    return 1;
  }

  // Настройка адреса сервера
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
  {
    std::cerr << "Invalid address" << std::endl;
    return 1;
  }

  // Цикл отправки чисел с задержкой
  for (int i = 1; i <= 5; i++)
  { // Отправляем числа 1, 2, 3, 4, 5 с задержкой i сек
    snprintf(buffer, BUFFER_SIZE, "Number: %d", i);
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));
    std::cout << "Sent to server: " << buffer << std::endl;

    // Получение ответа от сервера
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (n < 0)
    {
      std::cerr << "Receive failed" << std::endl;
      continue;
    }
    buffer[n] = '\0';
    std::cout << "Received from server: " << buffer << std::endl;

    sleep(i); // Задержка в i секунд
  }

  close(sockfd);
  return 0;
}