//
// Created by daniilsmelskiy on 24.04.21.
//
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>

#include "Client.h"

namespace {
    void ExitWithError(const char* error_message, int exit_status = EXIT_FAILURE) {
        perror(error_message);
        exit(exit_status);
    }

    void CreateSocketAddress(struct sockaddr_in* address, int port) {
        address->sin_family = AF_INET;
        address->sin_addr.s_addr = INADDR_ANY;
        address->sin_port = htons(port);
    }

    int ReadFromServer(int socket_fd_, char* buffer_, int length, int flags = 0) {
        int len = recv(socket_fd_, buffer_, length, flags);
        if (len < 0) {
            ExitWithError("Failed to read data from server.");
        }
        buffer_[len] = '\0';
        return len;
    }

    void SendToServer(int socket_fd_, const char* const buffer_, int length, int flags = 0) {
        if (send(socket_fd_, buffer_, length, flags) != length) {
            ExitWithError("Failed to send message to server");
        }
    }
}  // namespace

void Client::init(int port) {
    if ((socket_fd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ExitWithError("Socket creation failed");
    } else {
        printf("Socket successfully created\n");
    }

    CreateSocketAddress(&server_addr_, port);
}

void Client::connectToServer() {
    if (connect(socket_fd_,  reinterpret_cast<sockaddr*>(&server_addr_), sizeof(server_addr_)) != 0) {
        ExitWithError("Connection with the server failed");
    } else {
        printf("Successfully connected to the server\n");
    }
}

void Client::run(int port) {
    init(port);
    connectToServer();

    /// Get first connection message
    ChatMessage hello_message_from_server = receiveMessage();
    printf("Received message from server: %s\n", hello_message_from_server.get_json_message().c_str());
}

bool Client::hasAnyMessages() {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(socket_fd_, &read_fds);

    struct timeval tv = {0, 0};
    int activity = select(socket_fd_ + 1, &read_fds, nullptr, nullptr, &tv);

    if ((activity < 0) && (errno != EINTR)) {
        ExitWithError("Select error");
    }

    return activity > 0;
}

void Client::send(const char *const message, int length) const {
    SendToServer(socket_fd_, message, length);
}

void Client::send(const std::string& message) const {
    SendToServer(socket_fd_, message.data(), message.size());
}

std::string Client::receive() {
    int len = ReadFromServer(socket_fd_, buffer_, kDefaultBufferSize - 1);

    std::string result(buffer_, buffer_ + len);
    return result;
}

std::string Client::receive(int length) {
    int len = ReadFromServer(socket_fd_, buffer_, length);
    if (len != length) {
        ExitWithError("Failed to receive enough data from server");
    }

    std::string result(buffer_, buffer_ + len);
    return result;
}

ChatMessage Client::receiveMessage() {
    int len = ReadFromServer(socket_fd_, buffer_, kDefaultMessageLength);
    if (len != kDefaultMessageLength) {
        ExitWithError("Failed to read message length");
    }
    buffer_[len] = '\0';
    int message_length = std::atoi(buffer_);

    len = ReadFromServer(socket_fd_, buffer_, message_length);
    if (len != message_length) {
        ExitWithError("Failed to read message");
    }

    return ChatMessage(std::string(buffer_, buffer_ + len));
}

void Client::sendMessage(const ChatMessage& message) const {
    send(message.get_ready_message());
}
