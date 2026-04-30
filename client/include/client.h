#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <atomic>

using namespace std;

extern atomic<bool> running;

void receive_messages(int sock);

#endif // CLIENT_H