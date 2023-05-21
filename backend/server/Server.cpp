//
// Created by daniilsmelskiy on 24.04.21.
//

#include "Server.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cmath>
#include <algorithm>

#include "ChatMessage.h"

namespace {
    constexpr int kDefaultBufferSize{1025};

    void ExitWithError(const char* error_message, int exit_status = EXIT_FAILURE) {
        perror(error_message);
        exit(exit_status);
    }

    int CreateMasterSocket() {
        int master_socket = socket(AF_INET , SOCK_STREAM , 0);
        return master_socket;
    }

    void CreateSocketAddress(struct sockaddr_in* address, int port) {
        address->sin_family = AF_INET;
        address->sin_addr.s_addr = INADDR_ANY;
        address->sin_port = htons(port);
    }

    int EnableMultipleConnections(int master_socket) {
        int opt = true;
        return setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    }

    int BindMasterSocket(int master_socket, sockaddr_in* address) {
        return bind(master_socket, (struct sockaddr*)address, sizeof(*address));
    }

    int ListenMasterSocket(int master_socket, int limit_of_connetions = 1) {
        return listen(master_socket, limit_of_connetions);
    }

    int AcceptNewConnection(int master_socket, struct sockaddr_in* address, int* address_len) {
        return accept(master_socket, (struct sockaddr*)address, (socklen_t*)address_len);
    }

    void SendTo(int socket_descriptor, const char* const message, int message_length, int flags = 0) {
        if (send(socket_descriptor, message, message_length, flags) != message_length) {
            ExitWithError("Failed to send message");
        }
    }

    void SendTo(int socket_descriptor, const std::string& message) {
        if (send(socket_descriptor, message.data(), message.size(), 0) != message.size()) {
            ExitWithError("Failed to send message");
        }
    }

    int ReceiveFrom(int socket_descriptor, char* message) {
        int len = recv(socket_descriptor, message, kDefaultBufferSize - 1, 0);
        if (len < 0) {
            ExitWithError("Failed to receive data from user");
        }

        return len;
    }

    int ReceiveFrom(int socket_descriptor, char* message, int length, int flags = 0) {
        int len = recv(socket_descriptor, message, length, flags);
        if (len != length) {
            ExitWithError("Failed to receive enough data from user");
        }

        return len;
    }

    int TryReceiveFrom(int socket_descriptor, char* message, int length, int flags = 0) {
        return recv(socket_descriptor, message, length, flags);
    }

}  // namespace


void Server::init(int port) {
    if ((master_socket_ = CreateMasterSocket()) == 0) {
        ExitWithError("Failed to create Master socket");
    }

    if (EnableMultipleConnections(master_socket_) < 0) {
        ExitWithError("Failed to enable multiple connections");
    }

    CreateSocketAddress(&address_, port);

    if (BindMasterSocket(master_socket_, &address_) < 0) {
        ExitWithError("Bind Master socket failed");
    }

    printf("Listening port %d\n", port);
}

void Server::startListen(int limit_of_connections) const {
    if (ListenMasterSocket(master_socket_, limit_of_connections) < 0) {
        ExitWithError("Failed to start listen connections");
    }
}

int Server::initializeFDSet(fd_set* read_fds) {
    FD_ZERO(read_fds);

    FD_SET(master_socket_, read_fds);
    int max_sd = master_socket_;

    for (const auto& client_fd : clients_) {
        FD_SET(client_fd, read_fds);
        max_sd = std::max(max_sd, client_fd);
    }

    return max_sd;
}

void Server::addNewConnection() {
    int new_socket;
    if ((new_socket = AcceptNewConnection(master_socket_, &address_, &address_len_)) < 0) {
        ExitWithError("Accepting error");
        return;
    }

    printf("New connection accepted, socket fd is %d, ip is: %s, port: %d\n",
           new_socket, inet_ntoa(address_.sin_addr), ntohs(address_.sin_port));

    {   /// Welcome message
        ChatMessage message;

        const std::string hello_message{"Hello from server! You are successfully connected!"};
        message.add_field("response", Json::Node(hello_message));

        sendMessageTo(new_socket, message);
        puts("Welcome message sent successfully");
    }

    addNewClient(new_socket);
}

bool Server::checkConnection(const int client_fd, fd_set* read_fds) {
    int socket_fd = client_fd;

    if (FD_ISSET(socket_fd, read_fds)) {
        auto message = receiveMessageFrom(socket_fd);
        if (!message) {
            getpeername(socket_fd, reinterpret_cast<sockaddr*>(&address_), reinterpret_cast<socklen_t*>(&address_len_));

            printf("Host disconnected, ip %s, port %d\n", inet_ntoa(address_.sin_addr), ntohs(address_.sin_port));
            removeClient(client_fd);

            return false;
        } else {
            processMessageFrom(client_fd, *message);
        }
    }

    return true;
}

void Server::init(std::unique_ptr<OLAPService>&& service) {
    service_ = std::move(service);
}

[[noreturn]]
void Server::run(int port) {
    init(port);
    startListen(kLimitOfConnection);

    address_len_ = sizeof(address_);
    puts("Waiting for connections ...");

    fd_set read_fds;

    while (true) {
        int max_sd = initializeFDSet(&read_fds);

        int activity = select(max_sd + 1, &read_fds, nullptr, nullptr, nullptr);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Select error\n");
        }

        if (FD_ISSET(master_socket_, &read_fds)) {
            addNewConnection();
        }

        for (int i = 0; i < clients_.size(); ++i) {
            if (!checkConnection(clients_[i], &read_fds)) {
                --i;
            }
        }
    }
}

void Server::addNewClient(const int client_fd) {
    clients_.push_back(client_fd);
    printf("New client was successfully added to the network with socked id = %d\n", client_fd);
}

void Server::removeClient(const int client_fd) {
    close(client_fd);
    clients_.erase(std::find(clients_.begin(), clients_.end(), client_fd));

    printf("Client at port %d, was successfully disconnected\n", client_fd);
}

void Server::sendMessageTo(int socket_descriptor, const ChatMessage& message) const {
    SendTo(socket_descriptor, message.get_ready_message());
}

std::optional<ChatMessage> Server::receiveMessageFrom(int socket_fd) {
    int len = TryReceiveFrom(socket_fd, buffer_, kDefaultMessageLength);
    if (len == 0) {
        return std::nullopt;
    }
    if (len != kDefaultMessageLength) {
        ExitWithError("Failed to read message length");
    }
    buffer_[len] = '\0';
    int message_length = std::atoi(buffer_);

    len = ReceiveFrom(socket_fd, buffer_, message_length);
    if (len != message_length) {
        ExitWithError("Failed to read message");
    }

    return ChatMessage(std::string(buffer_, buffer_ + len));
}

void Server::processMessageFrom(const int client_fd, const ChatMessage& message) {
    printf("Server received new request from fd %d\n", client_fd);

    std::string response;
    if (const auto request = message.get_field("request"); request.has_value()) {
        response = service_->handleRequest(request.value().AsString());
    } else {
        response = "ERROR: No request";
    }

    ChatMessage response_message;
    response_message.add_field("response", response);

    sendMessageTo(client_fd, response_message);
}
