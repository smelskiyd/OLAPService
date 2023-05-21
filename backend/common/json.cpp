#include "json.h"

namespace {
    bool IsDigit(char c) {
        return ('0' <= c && c <= '9');
    }
}

namespace Json {
    Document::Document(Node root) : root(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root;
    }

    Node LoadNode(std::istream& input);

    Node LoadArray(std::istream& input) {
        std::vector<Node> result;

        for (char c; input >> c && c != ']'; ) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        return Node(std::move(result));
    }

    Node LoadDouble(std::istream& input, bool is_positive = true) {
        double result = 0.;
        bool read_real = false;
        while (true) {
            char c = input.peek();
            if (!IsDigit(c) && c != '.') break;
            if (c == '.') {
                read_real = true;
                input.get();
                break;
            }
            result *= 10;
            result += input.get() - '0';
        }
        if (read_real) {
            double degree = 1;
            while (IsDigit(input.peek())) {
                degree /= 10.;
                result += degree * (double)(input.get() - '0');
            }
        }

        if (!is_positive) result *= -1.;

        if (!read_real) {
            return Node((int)result);
        }
        return Node((double)result);
    }

    Node LoadBool(std::istream& input) {
        std::string field;
        while (islower(input.peek())) {
            field += input.get();
        }
        return (field == "false" ? Node((bool)false) : Node((bool)true));
    }

    Node LoadString(std::istream& input) {
        std::string line;
        char c;
        bool previous_is_slash = false;
        while (input.get(c)) {
            if (c == '\\') {
                if (previous_is_slash) {
                    line += '\\';
                    previous_is_slash = false;
                } else {
                    previous_is_slash = true;
                }
            } else if (c == '"') {
                if (previous_is_slash) {
                    line += '"';
                    previous_is_slash = false;
                } else {
                    break;
                }
            } else {
                previous_is_slash = false;
                line += c;
            }
        }
        return Node(move(line));
    }

    Node LoadDict(std::istream& input) {
        std::map<std::string, Node> result;

        for (char c; input >> c && c != '}'; ) {
            if (c == ',') {
                input >> c;
            }

            std::string key = LoadString(input).AsString();
            input >> c;
            result.emplace(move(key), LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNode(std::istream& input) {
        char c;
        input >> c;
        if (c == '[') {
            return LoadArray(input);
        } else if (c == '{') {
            return LoadDict(input);
        } else if (c == '"') {
            return LoadString(input);
        } else if (IsDigit(c)) {
            input.putback(c);
            return LoadDouble(input);
        } else if (c == '-') {
            return LoadDouble(input, false);
        } else {
            input.putback(c);
            return LoadBool(input);
        }
    }

    Document Load(std::istream& input) {
        return Document{LoadNode(input)};
    }
}  // namespace Json

std::ostream& operator << (std::ostream& out, const Json::Node& node) {
    using Json::Node;

    if (std::holds_alternative<int>(node)) {
        out << std::get<int>(node);
    } else if (std::holds_alternative<double>(node)) {
        out << std::setprecision(10) << std::get<double>(node);
    } else if (std::holds_alternative<std::string>(node)) {
        std::string str = std::get<std::string>(node);
        std::string formatted_str;
        for (auto c : str) {
            if (c == '\\') {
                formatted_str += "\\";
                formatted_str += "\\";
            } else if (c == '"') {
                formatted_str += "\\\"";
            } else {
                formatted_str += c;
            }
        }
        out << "\"" << formatted_str << "\"";
    } else if (std::holds_alternative<std::vector<Node>>(node)) {
        const std::vector<Node>& v = node.AsArray();
        out << "[";
        bool is_first = true;
        for (int i = 0; i < v.size(); ++i) {
            if (!is_first) out << ",\n";
            else out << "\n";
            is_first = false;
            out << v[i];
        }
        out << "\n]";
    } else if (std::holds_alternative<std::map<std::string, Node>>(node)) {
        const std::map<std::string, Node>& m = node.AsMap();
        out << "{";
        bool is_first = true;
        for (const auto& item : m) {
            if (!is_first) out << ",\n";
            else out << "\n";
            is_first = false;
            out << "\"" << item.first << "\": " << item.second;
        }
        out << "\n}";
    }
    return out;
}
