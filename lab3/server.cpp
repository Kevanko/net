#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>

// Максимальное число ожидающих подключений
#define BACKLOG 5

// Глобальный мьютекс для защиты общего файла
pthread_mutex_t file_mutex;
// Глобальный указатель на файл для записи данных
FILE *fp;

// Функция обработки клиента в потоке
void *handle_client(void *arg)
{
  int client_sock = *((int *)arg);
  free(arg);
  int number;
  ssize_t bytes_read;

  while ((bytes_read = read(client_sock, &number, sizeof(number))) > 0)
  {
    printf("Получено число: %d\n", number);
    fflush(stdout);

    // Сохраняем данные в общий файл с защитой мьютексом
    pthread_mutex_lock(&file_mutex);
    fprintf(fp, "Получено число: %d\n", number);
    fflush(fp);
    pthread_mutex_unlock(&file_mutex);
  }

  close(client_sock);
  return NULL;
}

int main()
{
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(server_addr);

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

  if (getsockname(sockfd, (struct sockaddr *)&server_addr, &addr_len) < 0)
  {
    perror("Ошибка getsockname");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Сервер слушает на порту: %d\n", ntohs(server_addr.sin_port));

  // Переводим сокет в режим прослушивания
  if (listen(sockfd, BACKLOG) < 0)
  {
    perror("Ошибка listen");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Открываем общий файл для записи [режим добавления]
  fp = fopen("data.txt", "a");
  if (!fp)
  {
    perror("Ошибка открытия файла");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Инициализируем мьютекс для синхронизации доступа к файлу
  pthread_mutex_init(&file_mutex, NULL);

  while (1)
  {
    socklen_t client_len = sizeof(client_addr);
    int *client_sock = new int;

    if (!client_sock)
    {
      perror("Ошибка выделения памяти");
      continue;
    }
    *client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (*client_sock < 0)
    {
      perror("Ошибка accept");
      delete client_sock;
      continue;
    }

    // Создаем новый поток для обслуживания клиента
    pthread_t tid;
    if (pthread_create(&tid, NULL, handle_client, client_sock) != 0)
    {
      perror("Ошибка создания потока");
      close(*client_sock);
      delete client_sock;
      continue;
    }
    // Отсоединяем поток, чтобы его ресурсы освобождались автоматически после завершения
    pthread_detach(tid);
  }

  fclose(fp);
  pthread_mutex_destroy(&file_mutex);
  close(sockfd);
  return 0;
}
