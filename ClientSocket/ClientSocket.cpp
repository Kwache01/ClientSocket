// Указывает на использование только основной части заголовочных файлов Windows
#define WIN32_LEAN_AND_MEAN

// Подключение необходимых заголовочных файлов для работы с Windows и сокетами
#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

// Использование пространства имен std для упрощения написания кода
using namespace std;

int main() {
    WSADATA wsaData;  // Структура для хранения информации о реализации Windows Sockets
    ADDRINFO hints;  // Структура для указания критериев для getaddrinfo
    ADDRINFO* addrResult;  // Указатель для хранения результатов getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET;  // Сокет для работы с соединением
    char recvBuffer[512];  // Буфер для приема данных от сервера

    const char* sendBuffer1 = "Hello from client 1";  // Данные для отправки серверу от клиента 1
    const char* sendBuffer2 = "Hello from client 2";  // Данные для отправки серверу от клиента 2

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // Обнуление структуры hints и настройка параметров для создания сокета
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  // Использовать IPv4
    hints.ai_socktype = SOCK_STREAM;  // Использовать потоковый сокет (TCP)
    hints.ai_protocol = IPPROTO_TCP;  // Протокол TCP

    // Получение информации о локальном адресе и порте сервера
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета для соединения с сервером
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Установка соединения с сервером
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Отправка данных серверу от клиента 1
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // Отправка данных серверу от клиента 2
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // Завершение отправки данных и закрытие отправки на запись
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Получение данных от сервера
    do {
        ZeroMemory(recvBuffer, 512);  // Обнуление буфера для приема данных
        result = recv(ConnectSocket, recvBuffer, 512, 0);  // Получение данных от сервера
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    // Закрытие сокета и освобождение ресурсов
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
