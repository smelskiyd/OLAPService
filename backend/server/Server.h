//
// Created by daniilsmelskiy on 24.04.21.
//

#pragma once

#include <vector>
#include <memory>
#include <arpa/inet.h>

#include "ChatMessage.h"

#include "service/OLAPService.h"

class Server {
  public:
    void init(std::unique_ptr<OLAPService>&& service);

    [[noreturn]]
    void run(int port);

  private:
    static constexpr int kLimitOfConnection{1};
    static constexpr int kDefaultBufferSize{1025};

    void init(int port);
    void startListen(int limit_of_connections) const;
    int  initializeFDSet(fd_set* read_fds);

    void addNewConnection();
    bool checkConnection(int client, fd_set* read_fds);

    void sendMessageTo(int socket_fd, const ChatMessage& message) const;
    std::optional<ChatMessage> receiveMessageFrom(int socket_fd);
    void processMessageFrom(int client, const ChatMessage& message);

    void addNewClient(int client_data);
    void removeClient(int client_name);

    int master_socket_;
    sockaddr_in address_;
    int address_len_{sizeof(address_)};

    std::vector<int> clients_;

    char buffer_[kDefaultBufferSize];

    std::unique_ptr<OLAPService> service_;
};
