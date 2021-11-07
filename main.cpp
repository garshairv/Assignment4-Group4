#include <iostream>
#include <unistd.h>

#include "SimpleTCPSocket.hpp"

static void *doit(void *);
bool running = true;
bool closed = false;
G4::SimpleTCPSocket* sock;

void closeApp() {
    sock->close_socket();
    delete sock;
}

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum) {
    std::cout << "Caught signal " << signum << std::endl;
    running = false;
    closeApp();
    // Terminate program
    exit(signum);
}

int main() {
    // Register signal and signal handler
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);
    auto m_socket = new G4::SimpleTCPSocket(3052, 0);
    sock = m_socket;
    m_socket->start(doit);
    while(running) {
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
    closeApp();
    return 0;
}

static void * doit(void * arg) {
    char buffer[3000] = {0};
    long clientsock;
    std::cout << "====== Waiting ===== " << std::endl;
    clientsock = (long) (arg);
    read(clientsock, buffer, 30000);
    std::cout << "Buffer read.\n";
    std::cout << buffer;
    char *hello = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><body><p>Hello from Server</p></body></html>";
    std::cout << "Preparing to send." << std::endl;
    auto sent = write(clientsock, hello, strlen(hello));
    std::cout << "Sent bytes: " << sent << std::endl << std::endl;
    shutdown(clientsock, SHUT_RD);
    close(clientsock);
}
