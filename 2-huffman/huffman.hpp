
#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// Huffman code builder, encoder and decoder.
/// Code table is a vector, linked via indices.

namespace huffman {

class code_table_t {
public:
    struct node_t {
        std::string symbols {};
        unsigned weight {};
        int left {-1};
        int right {-1};

        bool is_leaf() const;
        bool encodes(char c) const;
        char symbol() const;
    };

    explicit code_table_t(std::vector<node_t>&& nodes) : nodes_(std::move(nodes)) {}

    const node_t* root() const;
    const node_t* left(const node_t* node) const;
    const node_t* right(const node_t* node) const;

    void walk(std::function<void(const node_t&, unsigned /*level*/)> node_fn) const;

private:
    std::vector<node_t> nodes_;
};

std::ostream& operator << (std::ostream& ostr, const code_table_t& code_table);

struct symbol_def_t {
    char symbol;
    unsigned weight;
};
code_table_t build(const std::vector<symbol_def_t>& alphabet);

/// TWriter must provide `emit_0` and `emit_1` methods.
template <typename TWriter>
void encode(const code_table_t& code_table, const char sym, TWriter& writer) {
    const auto* current = code_table.root();
    while (!current->is_leaf()) {
        if (const auto* next = code_table.left(current); next->encodes(sym)) {
            writer.emit_0();
            current = next;
        }
        else if (const auto* next = code_table.right(current); next->encodes(sym)) {
            writer.emit_1();
            current = next;
        }
        else
            throw std::runtime_error("encode: unknown symbol on input");
    }
}

/// TWriter must provide `emit_0` and `emit_1` methods.
template <typename TInput, typename TWriter>
void encode(const code_table_t& code_table, const TInput& input, TWriter& writer) {
    for (const auto& sym : input)
        encode(code_table, sym, writer);
}

/// TReader must provide `cbegin`, `cend` and `is_set` methods.
template <typename TReader>
std::string decode(const code_table_t& code_table, const TReader& reader) {
    std::string output {};
    const auto* current = code_table.root();
    for (const auto bit : reader) {
        current = reader.is_set(bit)
            ? code_table.right(current)
            : code_table.left(current);
        if (current->is_leaf()) {
            output.push_back(current->symbol());
            current = code_table.root();
        }
    }
    if (current != code_table.root())
        throw std::invalid_argument("decode: incomplete input");
    return output;
}

} // namespace huffman
