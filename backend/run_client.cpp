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

void SendMessage(Client& client, const std::string& receiver, const ChatMessage& message) {
    client.sendMessage(message);

    printf("Message is successfully sent\n");
}

std::optional<ChatMessage> GetUserInput() {
    printf("Print some message or # to stop chatting: \n");

    std::string input_text;
    getline(std::cin, input_text);

    if (input_text.empty()) {
        return std::nullopt;
    }

    ChatMessage message;
    if (input_text == "#") {
        return message;
    }

    std::map<std::string, Json::Node> request_body;
    request_body["request-type"] = Json::Node(input_text);

    if (input_text == "GET_DIAGRAM") {
        printf("Print diagram type: \n");

        std::string diagram_type;
        getline(std::cin, diagram_type);
        request_body["diagram-type"] = Json::Node(diagram_type);
    }

    message.add_field("request", request_body);

    return message;
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
            const auto user_input = GetUserInput();
            if (!user_input.has_value()) {
                last_tp = std::chrono::steady_clock::now();
                continue;
            }

            if (user_input->is_empty()) {
                break;
            }

            SendMessage(client, "server", user_input.value());
            last_tp = std::chrono::steady_clock::now();
        }
    }
}

int main() {
    Client client;
    client.run(DEFAULT_PORT);

    StartChatting(client);
}
