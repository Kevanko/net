# Makefile для компиляции UDP сервера и клиента на C++

CXX = g++
CXXFLAGS = -Wall -g -std=c++11
LDFLAGS = -lm

# Имена исходных и бинарных файлов
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp
SERVER_BIN = server
CLIENT_BIN = client

# Цели по умолчанию
all: $(SERVER_BIN) $(CLIENT_BIN)

# Компиляция сервера
$(SERVER_BIN): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# Компиляция клиента
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# Очистка скомпилированных файлов
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

# Проверка (опционально)
.PHONY: all clean