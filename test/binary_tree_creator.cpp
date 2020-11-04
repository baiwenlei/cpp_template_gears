#include <iostream>
#include <vector>
#include <queue>
#include <type_traits>
#include <tuple>
#include <utility>

class Node {
public:
    int val = 0;
    Node *left = nullptr, *right = nullptr;
};

///SeqElement
template <class Seq, size_t I>
struct SeqElement;

template <size_t Head, size_t... Others, size_t I>
struct SeqElement<std::index_sequence<Head, Others...>, I> : SeqElement<std::index_sequence<Others...>, I-1>
{};

template <size_t Head, size_t... Others>
struct SeqElement<std::index_sequence<Head, Others...>, 0> : std::integral_constant<size_t, Head>
{};

template <size_t I>
struct SeqElement<std::index_sequence<>, I> : std::integral_constant<size_t, 0>
{};


///IndicatorValue
template <size_t V>
struct IndicatorValue : std::integral_constant<size_t, 1>
{};

template <>
struct IndicatorValue<0> : std::integral_constant<size_t, 0>
{};

///SeqSum
template <class Seq, size_t I>
struct SeqSum;

template <size_t Head, size_t... Others>
struct SeqSum<std::index_sequence<Head, Others...>, 0> : std::integral_constant<size_t, Head>
{};

template <size_t Head, size_t... Others, size_t I>
struct SeqSum<std::index_sequence<Head, Others...>, I> 
{
    static constexpr size_t value = Head + SeqSum<std::index_sequence<Others...>, I-1>::value; 
};


///NodeCreator
template <class Seq, size_t I, size_t V> 
struct NodeCreatorImpl;

template <class Seq, size_t I> 
struct NodeCreatorImpl<Seq, I, 1> {
    static constexpr size_t count = SeqSum<Seq, I>::value;
    static constexpr size_t index_l = 2 * count - 1;
    static constexpr size_t index_r = 2 * count;
    static constexpr size_t value_l = SeqElement<Seq, index_l>::value;
    static constexpr size_t value_r = SeqElement<Seq, index_r>::value;

    using LeftCreator = NodeCreatorImpl<Seq, index_l, value_l>;
    using RightCreator = NodeCreatorImpl<Seq, index_r, value_r>;

    static Node* create() {
        auto node = new Node{count};
        node->left = LeftCreator::create();
        node->right = RightCreator::create();
        return node;
    }
};

template <class Seq, size_t I> 
struct NodeCreatorImpl<Seq, I, 0> {
    static Node* create() {
        return nullptr;
    }
};


template <class Seq> 
struct NodeCreator;

template <size_t H, size_t... Others>
struct NodeCreator<std::index_sequence<H, Others...>> : NodeCreatorImpl<std::index_sequence<H, Others...>, 0, H>
{};



/// visit
void HierarchyVisit(Node *root) {
    if (!root) {
        return;
    }

    std::queue<Node *> q;
    q.push(root);
    while (!q.empty()) {
        auto node = q.front();
        q.pop();
        if (node) {
            std::cout << node->val << " ";
        }
        if (node->left) q.push(node->left);
        if (node->right) q.push(node->right);
    }
}


/// test
using Seq = std::index_sequence<1,0,1,1,1,1,1>;

int main(int argc, char** argv) {
    std::cout << SeqElement<Seq, 6>::value << "\n";
    std::cout << SeqSum<Seq, 6>::value << "\n";
    auto root = NodeCreator<Seq>::create();
    HierarchyVisit(root);
    return 0;
}
