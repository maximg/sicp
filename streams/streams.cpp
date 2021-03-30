
#include <functional>
#include <iostream>

template <typename T>
struct Stream {
    using value_type = T;
    value_type head;
    std::function<Stream()> stored_tail;
};

template <typename S>
typename S::value_type head(const S& stream) { return stream.head; }

template <typename S>
S tail(const S& stream) { return stream.stored_tail(); }

using IntStream = Stream<int>;
IntStream integersFrom(int n) {
    return IntStream{
        n,
        [n](){ return integersFrom(n + 1); }
    };
};

template <typename T>
std::vector<T> take(const Stream<T>& stream, size_t n) {
    std::vector<T> v;
    auto s = stream;
    while (n-- > 0) {
        v.push_back(head(s));
        s = tail(s);
    }
    return v;
}

int main() {
    auto s = integersFrom(5);
    std::cout << head(s) << std::endl;
    std::cout << head(tail(s)) << std::endl;
    auto v5 = take(s, 5);
    for (const auto val: v5)
        std::cout << val << std::endl;
}
