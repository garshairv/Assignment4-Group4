#include <iostream>
#include <unistd.h>
#include <string.h>

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
    auto m_socket = new G4::SimpleTCPSocket(3002, 0);
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
    char *hello = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\"<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n  <meta charset=\"UTF-8\">\n  <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n  <title>Document</title>\n</head>\n<body>\n  <h2>Upload Image</h2>\n  <form action=\"http://localhost:8086\" method=\"POST\">\n    <label for=\"file\">file:</label><br>\n    <input type=\"file\" id=\"file\" name=\"file\"><br>\n    <label for=\"caption\">caption:</label><br>\n    <input type=\"text\" id=\"caption\" name=\"caption\"><br>\n    <label for=\"date\">date:</label><br>\n    <input type=\"date\" id=\"date\" name=\"date\"><br>\n    <input type=\"submit\" id=\"submit\" name=\"submit\"><br>\n  </form>\n</body>\n</html>";
    std::cout << "Preparing to send." << std::endl;
    auto sent = write(clientsock, hello, strlen(hello));
    std::cout << "Sent bytes: " << sent << std::endl << std::endl;
    shutdown(clientsock, SHUT_RD);
    close(clientsock);
}
