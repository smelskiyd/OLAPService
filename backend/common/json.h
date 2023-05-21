#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace Json {
    class Node : public std::variant<std::vector<Node>,
            std::map<std::string, Node>,
            int,
            double,
            bool,
            std::string> {
    public:
        using variant::variant;

        bool IsArray() const {
            return std::holds_alternative<std::vector<Node>>(*this);
        }
        bool IsMap() const {
            return std::holds_alternative<std::map<std::string, Node>>(*this);
        }
        bool IsBool() const {
            return std::holds_alternative<bool>(*this);
        }
        bool IsInt() const {
            return std::holds_alternative<int>(*this);
        }
        bool IsDouble() const {
            return std::holds_alternative<double>(*this);
        }
        bool IsString() const {
            return std::holds_alternative<std::string>(*this);
        }

        const auto& AsArray() const {
            return std::get<std::vector<Node>>(*this);
        }
        const auto& AsMap() const {
            return std::get<std::map<std::string, Node>>(*this);
        }
        bool AsBool() const {
            return std::get<bool>(*this);
        }
        int AsInt() const {
            return std::get<int>(*this);
        }
        double AsDouble() const {
            return std::get<double>(*this);
        }
        const auto& AsString() const {
            return std::get<std::string>(*this);
        }
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root;
    };

    Document Load(std::istream& input);
}  // namespace Json

std::ostream& operator << (std::ostream& out, const Json::Node& node);
