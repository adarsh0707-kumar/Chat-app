#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9000);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    cout << "Connected to server!" << endl;

    // Send one message to server
    const string msg = "Hello, Server!";
    send(sock, msg.c_str(), msg.size(), 0);

    sleep(5); // Keep connection alive for a while
    close(sock);

    return 0;
}