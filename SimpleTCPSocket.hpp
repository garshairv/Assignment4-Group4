//
// Created by Shizuka Sato on 2021-11-06.
//

#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <resolv.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <thread>

namespace G4 {
    class SimpleTCPSocket {
    private:
       // const int TCP_PROTOCOL{0};
        struct sockaddr_in address;
        int sock;
        int connection;
        std::atomic_bool m_keepRunning{true};
        bool closing;
        pthread_t tid;
        std::mutex g_i_mutex;
        std::thread m_thread;
        void *(*acceptAction)(void*);
        void internal_start();
    public:
        SimpleTCPSocket(int port, int type);
        ~SimpleTCPSocket();
        int connect_to_network(int socket, struct sockaddr_in address);
        void test_connection(int);
        struct sockaddr_in get_address();
        int get_sock();
        int get_connection();
        void set_connection(int con);
        void close_socket();
        void start(void *(*onAccept)(void*) );
    };
}
