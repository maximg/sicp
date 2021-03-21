
#include "huffman.hpp"

#include <cassert>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace huffman {

template <typename T>
int sign(T value) {
    return (value < T()) ? -1 : 1;
}

bool code_table_t::node_t::is_leaf() const {
    assert(sign(left) == sign(right) && "unbalanced node");
    return left < 0 && right < 0;
}

bool code_table_t::node_t::encodes(char c) const {
    return symbols.find(c) != std::string::npos;
}

char code_table_t::node_t::symbol() const {
    assert(is_leaf() && "not a leaf node");
    return symbols.front();
}

const code_table_t::node_t* code_table_t::root() const {
    assert(!nodes_.empty() && "empty table");
    return std::addressof(nodes_.back());
}

const code_table_t::node_t* code_table_t::left(const code_table_t::node_t* node) const {
    assert(!node->is_leaf() && "at leaf node");
    return std::addressof(nodes_[node->left]);
}

const code_table_t::node_t* code_table_t::right(const code_table_t::node_t* node) const {
    assert(!node->is_leaf() && "at leaf node");
    return std::addressof(nodes_[node->right]);
}

void code_table_t::walk(std::function<void(const code_table_t::node_t&, unsigned)> node_fn) const {
    auto level = 0;
    auto walker = [&](const auto& node, auto& walker_self) -> void {
        node_fn(node, level);
        if (node.is_leaf()) return;
        ++level;
        walker_self(*left(&node), walker_self);
        walker_self(*right(&node), walker_self);
        --level;
    };
    walker(*root(), walker);
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

class stringstream_writer {
public:
    explicit stringstream_writer(std::stringstream& str_p) : str(str_p) {}

    void emit(int bit) { str << (bit & 1); }

private:
    std::stringstream& str;
};

struct string_reader : public std::string {
    explicit string_reader(const std::string& message) : std::string(message) {}

    static bool is_set(char c) {
        if (c == '0') return false;
        if (c == '1') return true;
        throw std::runtime_error("decode: invalid input");
    }
};

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

    auto encodeDecode = [&code_table](const std::string& text) {
        std::stringstream encoded;
        auto writer = huffman::stringstream_writer(encoded);
        huffman::encode(code_table, text, writer);
        std::cout << encoded.str() << "\n";

        const auto decoded = huffman::decode(code_table, huffman::string_reader(encoded.str()));
        std::cout << decoded << "\n";

        std::cout << (decoded == text ? "OK" : "FAILED") << "\n";
    };

    encodeDecode("");
    encodeDecode("A");
    encodeDecode("H");
    encodeDecode("ABABCEDEDFGH");

    return 0;
}
