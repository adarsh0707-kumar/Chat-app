#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <sstream>
#include <ctime>

using namespace std;

extern map<int, string> clients;
extern mutex clients_mutex;

void broadcast_message(const string &message, int sender_socket);
void handle_private_message(const std::string &input, int sender_socket);
void handle_client(int client_socket);

#endif // SERVER_H