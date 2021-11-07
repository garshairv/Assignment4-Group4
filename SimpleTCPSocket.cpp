//
// Created by Shizuka Sato on 2021-11-06.
//
#include "SimpleTCPSocket.hpp"
#include "time.h"


G4::SimpleTCPSocket::SimpleTCPSocket(int port, int type) {
    //AF_INET => TCP socket
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htons(INADDR_ANY);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    test_connection(sock);
    if(type == 1) {
        //connect
        std::cout << "client " << "\n";
        connection = connect(sock, (struct sockaddr *)&address, sizeof(address));
    } else {
        //server --> bind
        std::cout << "server " << "\n";
        connection = bind(sock, (struct sockaddr *)&address, sizeof(address));
    }
    test_connection(connection);
    std::cout << "socket created" << "\n";
}

void G4::SimpleTCPSocket::test_connection(int item_to_test) {
    if(item_to_test < 0){
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }
}

struct sockaddr_in G4::SimpleTCPSocket::get_address() {
    return address;
}

int G4::SimpleTCPSocket::get_sock() {
    return sock;
}

int G4::SimpleTCPSocket::get_connection() {
    return connection;
}


void G4::SimpleTCPSocket::set_connection(int con) {
    connection = con;
}

void G4::SimpleTCPSocket::close_socket() {
    std::cout << "closing \n";
    m_keepRunning = false;
    closing = true;
}

void G4::SimpleTCPSocket::internal_start() {
    auto threadFunc = [&, this]() {
        while (m_keepRunning) {
            std::lock_guard<std::mutex> lockGuard(this->g_i_mutex);
            // Check the queue and then Accept.
            int msgsock = accept(sock, (struct sockaddr *)0, (socklen_t *)0);
            if (!m_keepRunning) {
                close(msgsock);
            } else {
                if (msgsock == -1) {
                    perror("accept");
                }
                if (msgsock > 0) {
                    printf("before thread create\n");
                    pthread_create(&tid, NULL, acceptAction, (void*) msgsock);
                    printf("after thread create\n");
                }
            }
        }
        close(sock);
        std::cout << "stopped" << "\n";
    };
    m_thread = std::thread(threadFunc);
}

void G4::SimpleTCPSocket::start(void *(*onAccept)(void*) ) {
    //throw error if closing
    acceptAction = onAccept;
    m_keepRunning = true;
    std::cout << "starting" << "\n";
    int l = listen(sock, 3);
    test_connection(l);
    internal_start();
}

G4::SimpleTCPSocket::~SimpleTCPSocket(){
    m_keepRunning = false;
    g_i_mutex.unlock(); // make sure we don't wait in the loop for the lock
    if(m_thread.joinable()) m_thread.join();
}



