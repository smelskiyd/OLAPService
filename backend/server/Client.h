//
// Created by daniilsmelskiy on 24.04.21.
//

#pragma once

#include <unistd.h>
#include <arpa/inet.h>
#include <string>

#include "ChatMessage.h"

class Client {
  public:
    Client() = default;
    ~Client() {
        close(socket_fd_);
    }

    void run(int port);
    bool hasAnyMessages();

    void send(const char* const message, int length) const;
    void send(const std::string& message) const;

    std::string receive();
    std::string receive(int length);

    ChatMessage receiveMessage();
    void        sendMessage(const ChatMessage& message) const;

  private:
    static constexpr int kDefaultBufferSize{1025};

    void init(int port);
    void connectToServer();

    int socket_fd_;
    sockaddr_in server_addr_;

    char buffer_[kDefaultBufferSize];
};
