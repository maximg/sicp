
#include "huffman.hpp"

#include <queue>
#include <stdexcept>

namespace huffman {

bool code_table_t::node_t::is_leaf() const {
    return left < 0 && right < 0;
}

bool code_table_t::node_t::encodes(char c) const {
    return symbols.find(c) != std::string::npos;
}

const code_table_t::node_t* code_table_t::root() const {
    return nodes_.empty() ? nullptr : &nodes_.back();
}

const code_table_t::node_t* code_table_t::left(const code_table_t::node_t* node) const {
    return node->left < 0 ? nullptr : &nodes_[node->left];
}

const code_table_t::node_t* code_table_t::right(const code_table_t::node_t* node) const {
    return node->right < 0 ? nullptr : &nodes_[node->right];
}

void code_table_t::walk(std::function<void(const code_table_t::node_t&, unsigned)> node_fn) const {
    auto level = 0;
    auto walker = [&](const auto* node, auto& walker_self) -> void {
        if (!node) return;
        node_fn(*node, level);
        ++level;
        walker_self(left(node), walker_self);
        walker_self(right(node), walker_self);
        --level;
    };
    walker(root(), walker);
}

std::ostream& operator << (std::ostream& ostr, const code_table_t& code_table) {
    auto indent = [&](const auto level) {
        constexpr auto indent_spaces = 2;
        return std::string(level * indent_spaces, ' ');
    };
    code_table.walk([&](const auto& node, unsigned level){
        ostr << indent(level) << "{" << node.symbols << "} " << node.weight << "\n";
    });
    return ostr;
}

code_table_t build(const std::vector<symbol_def_t>& alphabet) {
    std::vector<code_table_t::node_t> nodes;
    nodes.reserve(2 * alphabet.size());
    auto compare = [&](const int& left, const int& right) {
        return nodes[left].weight > nodes[right].weight;
    };
    std::priority_queue<int, std::vector<int>, decltype(compare)> queue(compare);
    for (const auto& freq : alphabet) {
        nodes.push_back({ std::string(1, freq.symbol), freq.weight, -1, -1});
        queue.push(nodes.size() - 1);
    }
    while (queue.size() > 1) {
        const auto left = queue.top(); queue.pop();
        const auto right = queue.top(); queue.pop();
        const auto symbols = nodes[left].symbols + " " + nodes[right].symbols;
        const auto weight = nodes[left].weight + nodes[right].weight;
        nodes.push_back({ symbols, weight, left, right });
        queue.push(nodes.size() - 1);
    }
    nodes.resize(nodes.size());
    return code_table_t{ std::move(nodes) };
}

bit_vect_t encode(const code_table_t& code_table, std::string_view text) {
    bit_vect_t output;
    for (const char c : text) {
        const code_table_t::node_t* current = code_table.root();
        while (current) {
            if (current->is_leaf()) {
                //output.push_back(' ');
                current = nullptr;
            }
            else if (code_table.left(current)->encodes(c)) {
                output.push_back('0');
                current = code_table.left(current);
            }
            else if (code_table.right(current)->encodes(c)) {
                output.push_back('1');
                current = code_table.right(current);
            }
            else
                throw std::runtime_error("invalid alphabet");
        }
    }
    return output;
}

std::string decode(const code_table_t& code_table, bit_vect_t message) {
    std::string output {};
    const code_table_t::node_t* current = code_table.root();
    for (const char c : message) {
        if (c == '0') {
            current = code_table.left(current);
        }
        else if (c == '1') {
            current = code_table.right(current);
        }
        else if (c == ' ') { // skip
        }
        else
            throw std::runtime_error("invalid message");
        if (current->is_leaf()) {
            output.push_back(current->symbols[0]);
            current = code_table.root();
        }
    }
    return output;
}

} // namespace huffman

int main() {
    std::cout << "Huffman encoding/decoding\n";

    auto code_table = huffman::build({
        {'A', 8},
        {'B', 3},
        {'C', 1},
        {'D', 1},
        {'E', 1},
        {'F', 1},
        {'G', 1},
        {'H', 1}
    });
    std::cout << code_table << "\n";

    const std::string text = "ABABCEDEDFGH";

    const auto encoded = huffman::encode(code_table, text);
    std::cout << encoded << "\n";

    const auto decoded = huffman::decode(code_table, encoded);
    std::cout << decoded << "\n";

    std::cout << (decoded == text ? "OK" : "FAILED") << "\n";

    return 0;
}
