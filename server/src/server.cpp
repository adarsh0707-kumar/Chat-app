#include "../include/server.h"

map<int, string> clients;
mutex clients_mutex;

void broadcast_message(const string &message, int sender_socket)
{
    lock_guard<mutex> lock(clients_mutex);
    for (auto &[client_socket, _] : clients)
    {
        if (client_socket != sender_socket)
        {
            if (send(client_socket, message.c_str(), message.size(), 0) < 0)
            {
                // Handle send error, perhaps remove client
                clients.erase(client_socket);
            }
        }
    }
}

void handle_private_message(const std::string &input, int sender_socket)
{
    std::istringstream iss(input);
    std::string cmd, target_user, msg;
    iss >> cmd >> target_user;
    std::getline(iss, msg); // rest is message
    std::string sender_name;
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        sender_name = clients[sender_socket];
    }
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto &[sock, name] : clients)
    {
        if (name == target_user)
        {
            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            char time_str[6];
            std::strftime(time_str, sizeof(time_str), "%H:%M", local_time);
            std::string full_msg = "[" + std::string(time_str) + "] [PRIVATE] " + sender_name + ": " + msg;
            if (send(sock, full_msg.c_str(), full_msg.size(), 0) < 0)
            {
                // Handle error
            }
            return;
        }
    }
    // user not found
    std::string error = "User not found\n";
    if (send(sender_socket, error.c_str(), error.size(), 0) < 0)
    {
        // Handle error
    }
}

void handle_client(int client_socket)
{
    cout << "Client connected : " << client_socket << endl;
    char buffer[1024];

    char name_buffer[1024] = {0};

    int bytes = read(client_socket, name_buffer, sizeof(name_buffer));
    if (bytes <= 0)
    {
        close(client_socket);
        return;
    }

    string username(name_buffer);
    // Trim whitespace
    username.erase(username.begin(), find_if(username.begin(), username.end(), [](unsigned char ch)
                                             { return !isspace(ch); }));
    username.erase(find_if(username.rbegin(), username.rend(), [](unsigned char ch)
                           { return !isspace(ch); })
                       .base(),
                   username.end());

    if (username.empty())
    {
        std::string error = "Username cannot be empty\n";
        send(client_socket, error.c_str(), error.size(), 0);
        close(client_socket);
        return;
    }

    {
        lock_guard<mutex> lock(clients_mutex);
        bool exists = false;
        for (auto &[sock, name] : clients)
        {
            if (name == username)
            {
                exists = true;
                break;
            }
        }
        if (exists)
        {
            std::string error = "Username already taken\n";
            if (send(client_socket, error.c_str(), error.size(), 0) < 0)
            {
                // Handle error
            }
            close(client_socket);
            return;
        }
        clients[client_socket] = username;
    }

    cout << username << " joined the chat!" << endl;

    // Broadcast join message
    std::time_t now = std::time(nullptr);
    std::tm *local_time = std::localtime(&now);
    char time_str[6];
    std::strftime(time_str, sizeof(time_str), "%H:%M", local_time);
    std::string join_msg = "[" + std::string(time_str) + "] " + username + " joined the chat";
    broadcast_message(join_msg, client_socket);

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

            // Broadcast leave message
            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            char time_str[6];
            std::strftime(time_str, sizeof(time_str), "%H:%M", local_time);
            std::string leave_msg = "[" + std::string(time_str) + "] " + username + " left the chat";
            broadcast_message(leave_msg, -1); // -1 to send to all

            break;
        }

        std::string username;

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            username = clients[client_socket];
        }

        std::string input(buffer, bytes);
        if (input.rfind("/msg ", 0) == 0)
        {
            // private message
            handle_private_message(input, client_socket);
        }
        else if (input == "/help")
        {
            std::string help =
                "Commands:\n"
                "/msg <user> <message> - private message\n";
            if (send(client_socket, help.c_str(), help.size(), 0) < 0)
            {
                // Handle error
            }
        }
        else
        {
            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            char time_str[6];
            std::strftime(time_str, sizeof(time_str), "%H:%M", local_time);
            std::string message = "[" + std::string(time_str) + "] " + username + ": " + input;
            cout << message << std::endl;
            broadcast_message(message, client_socket);
        }
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
