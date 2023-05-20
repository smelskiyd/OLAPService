//
// Created by daniilsmelskiy on 24.04.21.
//

#include <chrono>

#include "server/Client.h"

using namespace std::chrono_literals;

#define DEFAULT_PORT 8888

static std::chrono::steady_clock::duration kWaitDuration{5s};

void ReceiveMessage(Client& client) {
    ChatMessage message = client.receiveMessage();
    std::string text = message.get_field("response")->AsString();

    printf("New response: %s\n", text.c_str());
    fflush(stdout);
}

void SendMessage(Client& client, const std::string& receiver, const std::string& text) {
    ChatMessage message;
    message.add_field("request", text);
    message.add_field("length", Json::Node(static_cast<int>(text.size())));

    client.sendMessage(message);

    printf("Message is successfully sent\n");
}

void StartChatting(Client& client) {
    std::chrono::steady_clock::time_point last_tp = std::chrono::steady_clock::now() - kWaitDuration;

    while (true) {
        while (client.hasAnyMessages()) {
            printf("Server has some messages\n");
            fflush(stdout);

            ReceiveMessage(client);
        }

        std::chrono::steady_clock::time_point cur_tp = std::chrono::steady_clock::now();
        if (cur_tp - last_tp > kWaitDuration) {
            printf("Print some message or # to stop chatting: \n");

            std::string input_text;
            getline(std::cin, input_text);

            if (input_text.empty()) {
                last_tp = std::chrono::steady_clock::now();
                continue;
            }
            if (input_text == "#") {
                break;
            }

            SendMessage(client, "server", input_text);
            last_tp = std::chrono::steady_clock::now();
        }
    }
}

int main() {
    Client client;
    client.run(DEFAULT_PORT);

    StartChatting(client);
}