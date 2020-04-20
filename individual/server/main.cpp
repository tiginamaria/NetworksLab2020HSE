#include <iostream>
#include "server.h"

int parse_port_arg(const std::string& arg) {
    int port;
    try {
        std::size_t pos;
        port = std::stoi(arg, &pos);
        if (pos < arg.size()) {
            throw std::invalid_argument("Trailing characters after number: " + arg + "\n");
        }
    } catch (std::invalid_argument const &ex) {
        throw std::invalid_argument("Invalid number: " + arg + "\n");
    } catch (std::out_of_range const &ex) {
        throw std::invalid_argument("Number out of range: " + arg + "\n");
    }
    return port;
}

int main(int argc, char *argv[]) {
    int socket_fd;
    sockaddr_in server_addr{};
    int port;

    if (argc < 2) {
        fprintf(stderr, "Usage %s port\n", argv[0]);
        exit(1);
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        fprintf(stderr, "Error opening socket %d", socket_fd);
        exit(1);
    }

    try {
        port = parse_port_arg(argv[1]);
    } catch (std::invalid_argument const &ex) {
        fprintf(stderr, "%s", ex.what());
        exit(1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Error on binding socket_fd: %d port: %d", socket_fd, port);
        exit(1);
    }

    server server(port, socket_fd);

    std::string command;
    while (getline(std::cin, command)) {
        if (command == "exit") {
            server.shutdown();
            break;
        }
    }

    return 0;
}