#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

vector<int> clients;
mutex clients_mutex;

void handle_client(int client_socket)
{
    cout << "Client connected : " << client_socket << endl;
    char buffer[1024];
    // for now , just keep connection alive

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytes = read(client_socket, buffer, sizeof(buffer));

        if (bytes <= 0)
        {
            cout << "Client disconnected : " << client_socket << endl;
            close(client_socket);

            // Remove client from list
            lock_guard<mutex> lock(clients_mutex);
            clients.erase(remove(clients.begin(), clients.end(), client_socket), clients.end());

            break;
        }

        string messages(buffer, bytes);
        cout << "Received from client " << client_socket << ": " << messages << endl;
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
            clients.push_back(client_socket);
        }

        thread t(handle_client, client_socket);
        t.detach();
    }

    close(server_fd);
    return 0;
}
