#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define PORT 0 // Автоматический выбор свободного порта
#define BUFFER_SIZE 1024

int main()
{
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];
  int port;

  // Создание UDP сокета
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    return 1;
  }
  std::cout << "Socket created successfully" << std::endl;

  // Настройка адреса сервера
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Привязка сокета
  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    std::cerr << "Bind failed: " << strerror(errno) << std::endl;
    return 1;
  }
  std::cout << "Bind successful" << std::endl;

  // Получение назначенного порта
  addr_len = sizeof(server_addr);
  if (getsockname(sockfd, (struct sockaddr *)&server_addr, &addr_len) < 0)
  {
    std::cerr << "Getsockname failed: " << strerror(errno) << std::endl;
    return 1;
  }
  port = ntohs(server_addr.sin_port);
  std::cout << "Server running on port: " << port << std::endl;
  std::cout.flush(); // Принудительный сброс буфера

  while (true)
  {
    // Прием данных от клиента
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                     (struct sockaddr *)&client_addr, &addr_len);
    if (n < 0)
    {
      std::cerr << "Receive failed: " << strerror(errno) << std::endl;
      continue;
    }
    buffer[n] = '\0';

    // Вывод информации о клиенте и полученных данных
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    std::cout << "Received from client " << client_ip << ":"
              << ntohs(client_addr.sin_port) << ": " << buffer << std::endl;
    std::cout.flush();

    // Преобразование строки (добавление "[SERVER]" в конец)
    strcat(buffer, " [SERVER]");

    // Отправка преобразованных данных обратно клиенту
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr *)&client_addr, addr_len);
  }

  close(sockfd);
  return 0;
}