#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <atomic>

using namespace std;

atomic<bool> running(true);

void receive_messages(int sock)
{
    char buffer[1024];
    while (running)
    {
        int bytes = read(sock, buffer, sizeof(buffer));
        if (bytes > 0)
        {
            cout << "\n"
                 << string(buffer, bytes) << endl;
            cout.flush();
        }
        else
        {
            running = false;
            break;
        }
    }
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9000);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    cout << "Connected to server!" << endl;

    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin, username);

    send(sock, username.c_str(), username.size(), 0);

    // Send multiple messages to the server
    thread receiver(receive_messages, sock);

    while (true)
    {
        string message;
        cout << "Enter message to send (type 'exit' to quit): ";
        getline(cin, message);

        if (message == "exit")
        {
            running = false;
            shutdown(sock, SHUT_RDWR);
            close(sock);
            break;
        }

        send(sock, message.c_str(), message.size(), 0);
    }

    if (receiver.joinable())
    {
        receiver.join();
    }

    return 0;
}