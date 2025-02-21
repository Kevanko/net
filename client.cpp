#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    std::cerr << "Использование: " << argv[0] << " <IP сервера> <порт> <число i>" << std::endl;
    return 1;
  }

  const char *server_ip = argv[1];
  int server_port = atoi(argv[2]);
  int i = atoi(argv[3]);

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
  server_addr.sin_addr.s_addr = inet_addr(server_ip);
  server_addr.sin_port = htons(server_port);

  // Цикл отправки
  std::string message = std::to_string(i);
  char buffer[1024];
  for (int count = 0; count < 5; ++count)
  { // Отправляем 5 раз
    // Отправка числа i
    sendto(sockfd, message.c_str(), message.length(), 0,
           (sockaddr *)&server_addr, sizeof(server_addr));

    // Получение ответа
    socklen_t addr_len = sizeof(server_addr);
    int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);
    if (bytes_received < 0)
    {
      std::cerr << "Ошибка получения данных" << std::endl;
      continue;
    }
    buffer[bytes_received] = '\0';

    std::cout << "Получено от сервера: " << buffer << std::endl;

    // Задержка i секунд
    sleep(i);
  }

  close(sockfd);
  return 0;
}