#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main()
{
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    std::cerr << "Ошибка создания сокета" << std::endl;
    return 1;
  }

  // Настройка адреса сервера
  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY; // Любой адрес
  server_addr.sin_port = 0;                 // Система выберет свободный порт

  // Привязка сокета
  if (bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    std::cerr << "Ошибка привязки сокета" << std::endl;
    return 1;
  }

  // Получение номера порта
  socklen_t addr_len = sizeof(server_addr);
  getsockname(sockfd, (sockaddr *)&server_addr, &addr_len);
  std::cout << "Сервер запущен на порту: " << ntohs(server_addr.sin_port) << std::endl;

  // Основной цикл
  char buffer[1024];
  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  while (true)
  {
    // Приём данных от клиента
    int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                  (sockaddr *)&client_addr, &client_len);
    if (bytes_received < 0)
    {
      std::cerr << "Ошибка приёма данных" << std::endl;
      continue;
    }
    buffer[bytes_received] = '\0';

    // Вывод информации о клиенте
    std::cout << "Получено от " << inet_ntoa(client_addr.sin_addr) << ":"
              << ntohs(client_addr.sin_port) << " - " << buffer << std::endl;

    // Преобразование данных (например, умножение на 2)
    int received_num = atoi(buffer);
    int transformed_num = received_num * 2;
    std::string response = std::to_string(transformed_num);

    // Отправка ответа клиенту
    sendto(sockfd, response.c_str(), response.length(), 0,
           (sockaddr *)&client_addr, client_len);
  }

  close(sockfd);
  return 0;
}