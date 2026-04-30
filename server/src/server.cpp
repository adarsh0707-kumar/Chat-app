#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>

using namespace std;

map<int, string> clients;
mutex clients_mutex;

void broadcast_message(const string &message, int sender_socket)
{
    lock_guard<mutex> lock(clients_mutex);
    for (auto &[client_socket, _] : clients)
    {
        if (client_socket != sender_socket)
        {
            send(client_socket, message.c_str(), message.size(), 0);
        }
    }
}

void handle_client(int client_socket)
{
    cout << "Client connected : " << client_socket << endl;
    char buffer[1024];

    char name_buffer[1024] = {0};

    read(client_socket, name_buffer, sizeof(name_buffer));

    string username(name_buffer);

    {
        lock_guard<mutex> lock(clients_mutex);
        clients[client_socket] = username;
    }

    cout << username << " joined the chat!" << endl;

    // for now , just keep connection alive

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytes = read(client_socket, buffer, sizeof(buffer));

        if (bytes <= 0)
        {
            cout << "Client disconnected : " << client_socket << endl;
            close(client_socket);

            std::string username;

            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                username = clients[client_socket];
                clients.erase(client_socket);
            }

            std::cout << username << " left the chat\n";

            break;
        }

        std::string username;

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            username = clients[client_socket];
        }

        std::string message = username + ": " + std::string(buffer, bytes);

        cout << message << std::endl;

        broadcast_message(message, client_socket);
    }
}

int main()
{
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9000);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    cout << "Server is listening on port 9000..." << endl;

    while (true)
    {
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        {
            lock_guard<mutex> lock(clients_mutex);
            clients[client_socket] = "Unknown";
        }

        thread t(handle_client, client_socket);
        t.detach();
    }

    close(server_fd);
    return 0;
}
