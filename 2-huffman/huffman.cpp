
#include "huffman.hpp"

#include <queue>

namespace huffman {


const code_vnode_t* code_table_t::root() const {
    return nodes_.empty() ? nullptr : &nodes_.back();
}

bool code_table_t::is_leaf(const code_vnode_t* node) const {
    return node->left < 0 && node->right < 0;
}

const code_vnode_t* code_table_t::left(const code_vnode_t* node) const {
    return node->left < 0 ? nullptr : &nodes_[node->left];
}

const code_vnode_t* code_table_t::right(const code_vnode_t* node) const {
    return node->right < 0 ? nullptr : &nodes_[node->right];
}


std::ostream& operator << (std::ostream& ostr, const code_table_t& code_table) {
    constexpr auto indent_spaces = 2;
    auto level = 0;
    auto indent = [&] {
        return std::string(level * indent_spaces, ' ');
    };
    // Use `printer` to call `print_table` recrusively.
    auto print_node = [&](const auto* node, auto& printer) -> void {
        if (!node) return;
        ++level;
        ostr << indent() << "{" << node->symbols << "} " << node->weight << "\n";
        printer(code_table.left(node), printer);
        printer(code_table.right(node), printer);
        --level;
    };
    print_node(code_table.root(), print_node);
    return ostr;
}

code_table_t build(std::vector<symbol_def_t> alphabet) {
    std::vector<code_vnode_t> nodes;
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

} // namespace huffman

int main() {
    std::cout << "Huffman demo\n";

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

    return 0;
}
