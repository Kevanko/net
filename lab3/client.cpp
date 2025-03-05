#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 64

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " <server_ip> <port>\n";
    return 1;
  }

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    std::cerr << "Socket failed\n";
    return 1;
  }

  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  for (int i = 1; i <= 5; i++)
  {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Number: %d", i);
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));
    std::cout << "Sent: " << buffer << "\n";

    int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, nullptr, nullptr);
    if (n > 0)
    {
      buffer[n] = '\0';
      std::cout << "Received: " << buffer << "\n";
    }

    sleep(i); // Задержка i секунд
  }

  close(sockfd);
  return 0;
}