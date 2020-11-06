#include <iostream>
#include <vector>
#include <queue>
#include <type_traits>
#include <tuple>
#include <utility>
#include <stack>

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



/// 层次visit
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

std::deque<std::vector<int>> BottomToTopHierarchyVisit(Node *root) {
    std::deque<std::vector<int>> values;
    if (!root) {
        return values;
    }

    std::queue<Node *> q;
    q.push(root);
    while (!q.empty()) {
        std::vector<int> temp;
        auto count = q.size();
        for (size_t i=0; i<count; ++i) {
            auto node = q.front();
            temp.emplace_back(node->val);
            q.pop();
            if(node->left) q.push(node->left);
            if(node->right) q.push(node->right);
        }

        values.push_front(std::move(temp));
    }

    return values;
}

///前序遍历
void PreOrderVisit(Node *root, std::vector<int> &vals) {
    if (!root) {
        return;
    }

    vals.push_back(root->val);
    PreOrderVisit(root->left, vals);
    PreOrderVisit(root->right, vals);
}

std::vector<int> PreOrderVisit(Node *root) {
    std::vector<int> retvals;
    PreOrderVisit(root, retvals);
    return retvals;
}

std::vector<int> PreOrderVisitWithStack(Node *root) {
    std::stack<Node *> stack;
    if (!root) {
        return {};
    }

    std::vector<int> values;

    stack.push(root);
    while (!stack.empty()) {
        auto node = stack.top();
        stack.pop();
        values.push_back(node->val);
        if (node->right) stack.push(node->right);
        if (node->left) stack.push(node->left);
    }

    return values;
}

std::vector<int> PreOrderVisitWithStack2(Node *root) {
    std::stack<Node *> stack;
    if (!root) {
        return {};
    }

    std::vector<int> values;

    auto node = root;
    while (node || !stack.empty()) {
        if (node) {
            values.push_back(node->val);
            stack.push(node);
            node = node->left;
        } else {
            node = stack.top();
            stack.pop();
            node = node->right;
        }
    }

    return values;
}

void MidOrderVisit(Node *root, std::vector<int> &vals) {
    if (!root) {
        return;
    }

    MidOrderVisit(root->left, vals);
    vals.push_back(root->val);
    MidOrderVisit(root->right, vals);
}

std::vector<int> MidOrderVisit(Node *root) {
    std::vector<int> retvals;
    MidOrderVisit(root, retvals);
    return retvals;
}

std::vector<int> MidOrderVisitWithStack(Node *root) {
    std::vector<int> retvals;
    if (!root) {
        return retvals;
    }

    std::stack<Node *>stack;
    auto node = root;
    while (node) {
        stack.push(node);
        node = node->left;
    }

    while (!stack.empty()) {
        node = stack.top();
        stack.pop();
        
        retvals.push_back(node->val);
        node = node->right;
        while (node) {
            stack.push(node);
            node = node->left;
        }
    }

    return retvals;
}

std::vector<int> MidOrderVisitWithStack2(Node *root) {
    std::vector<int> retvals;
    if (!root) {
        return retvals;
    }

    std::stack<Node *>stack;
    auto node = root;

    while (node || !stack.empty()) {
        if (node) {
            stack.push(node);
            node = node->left;
        } else {
            node = stack.top();
            stack.pop();
            retvals.push_back(node->val);
            node = node->right;
        }
    }

    return retvals;
}

void PostOrderVisit(Node *root, std::vector<int> &vals) {
    if (!root) {
        return;
    }

    PostOrderVisit(root->left, vals);
    PostOrderVisit(root->right, vals);
    vals.push_back(root->val);
}

std::vector<int> PostOrderVisit(Node *root) {
    std::vector<int> retvals;
    PostOrderVisit(root, retvals);
    return retvals;
}

std::deque<int> PostOrderVisitWithStack(Node *root) {
    std::deque<int> vals;
    
    Node *node = root;
    std::stack<Node *> stack;
    while (node || !stack.empty()) {
        if (node) {
            vals.push_front(node->val);
            stack.push(node);
            node = node->right;
        } else {
            node = stack.top();
            stack.pop();
            node = node->left;
        }
    }

    return vals;
}

///克隆
Node *Clone(Node *root) {
    if (!root) {
        return nullptr;
    }

    auto clone = new Node{root->val};
    clone->left = Clone(root->left);
    clone->right = Clone(root->right);
    return clone;
}

///相等判断
bool Equals(Node *root1, Node *root2) {
    if (root1 == root2) {
        return true;
    }

    if (!root1 || !root2) {
        return false;
    }

    if (root1->val != root2->val) {
        return false;
    }

    return Equals(root1->left, root2->left) && Equals(root1->right, root2->right);
}

///symmetric
void Symmetric(Node *root) {
    if (!root) {
        return;
    }

    std::swap(root->left, root->right);
    Symmetric(root->left);
    Symmetric(root->right);
}

bool IsSymmetric(Node *root1, Node *root2);

bool IsSymmetric(Node *root) {
    if (!root) {
        return true;
    }

    return IsSymmetric(root->left, root->right);
}

bool IsSymmetric(Node *root1, Node *root2) {
    if (root1 == root2) {
       return IsSymmetric(root1);
    }

    if (!root1 || !root2) {
        return false;
    }

    if (root1->val != root2->val) {
        return false;
    }

    return IsSymmetric(root1->left, root2->right) && IsSymmetric(root1->right, root2->left);
}

///重建
struct SeqView {
    const std::vector<int> &seq;
    size_t p=0, q=0;

    int front() const {
        return seq[p];
    }

    int last() const {
        return seq[q-1];
    }

    int operator[](size_t i) const {
        return seq[i];
    }

    size_t size() const {
        return q>p ? (q-p) : 0;
    }

    bool empty() const {
        return size() == 0;
    }

    size_t index(int val) const {
        for (auto i=p; i<q; ++i) {
            if (seq[i] == val) {
                return i;
            }
        }
        throw std::invalid_argument{"wrong sequence input"};
        return static_cast<size_t>(-1);
    }
};

Node *RebuildTree(SeqView &preseq, const SeqView &inseq) {
    if (preseq.empty() || inseq.empty()) {
        return nullptr;
    }

    auto val = preseq.front();
    auto root = new Node{val};
    auto index = inseq.index(val);
    
    preseq.p += 1;
    root->left = RebuildTree(preseq, SeqView{inseq.seq, inseq.p, index});
    root->right = RebuildTree(preseq, SeqView{inseq.seq, index+1, inseq.q});
    return root;
}

Node *RebuildTree(const std::vector<int> &preseq, const std::vector<int> &inseq) {
    SeqView preview{preseq, 0, preseq.size()}, inview{inseq, 0, inseq.size()};
    return RebuildTree(preview, inview);
}

void CalculatePostOrderSequenceImpl(const SeqView &preview, const SeqView &inview, std::deque<int> &vals) {
    if (preview.empty() || inview.empty()) {
        return;
    }

    auto val = preview.front();
    auto index = inview.index(val);
    auto count = inview.q - index - 1;
    vals.push_front(val);
    CalculatePostOrderSequenceImpl(SeqView{preview.seq, preview.q-count, preview.q}, SeqView{inview.seq, index+1, inview.q}, vals);
    CalculatePostOrderSequenceImpl(SeqView{preview.seq, preview.p+1, preview.q-count}, SeqView{inview.seq, inview.p, index}, vals);
}   

std::deque<int> CalculatePostOrderSequence(const std::vector<int> &preseq, const std::vector<int> &inseq) {
    std::deque<int> vals;
    SeqView preview{preseq, 0, preseq.size()}, inview{inseq, 0, inseq.size()};
    CalculatePostOrderSequenceImpl(preview, inview, vals);
    return vals;
}

///后序序列和中序序列作为输入进行重建
class Tree {
public:
    static Node* FromPostAndInOrderSeq(const std::vector<int> &postseq, const std::vector<int> &inseq);
    static Node* FromPostAndInOrderSeq(const SeqView &postview, const SeqView &invew);
};

Node* Tree::FromPostAndInOrderSeq(const std::vector<int> &postseq, const std::vector<int> &inseq) {
    SeqView postview{postseq, 0, postseq.size()}, inview{inseq, 0, inseq.size()};
    return FromPostAndInOrderSeq(postview, inview);
}

Node* Tree::FromPostAndInOrderSeq(const SeqView &postview, const SeqView &invew) {
    if (postview.empty() || invew.empty()) {
        return nullptr;
    }

    auto val = postview.last();
    auto index = invew.index(val);
    auto l = index - invew.p;
    std::cout << "find index: " << index << "\n";

    auto root = new Node{val};
    root->left = FromPostAndInOrderSeq(SeqView{postview.seq, postview.p, postview.p+l}, SeqView{invew.seq, invew.p, index});
    root->right = FromPostAndInOrderSeq(SeqView{postview.seq, postview.p+l, postview.q-1}, SeqView{invew.seq, index+1, invew.q});
    return root;
}

///深度
size_t Depth(Node *root) {
    if (!root) {
        return 0;
    }

    return 1 + std::max(Depth(root->left), Depth(root->right));
}

///最低公共祖先


bool FindPathImpl(Node *root, int val, std::vector<Node *>& path) {
    if (!root) {
        return false;
    }

    path.push_back(root);

    if (root->val == val) {
        return true;
    }

    if (FindPathImpl(root->left, val, path)) {
        return true;
    }

    if (FindPathImpl(root->right, val, path)) {
        return true;
    }

    path.pop_back();
    return false;
}

std::vector<Node *> FindPath(Node *root, int val) {
    std::vector<Node *> path;
    FindPathImpl(root, val, path);
    return path;
}

int FindLeastAncient(Node *root, int a, int b) {
    auto path1 = FindPath(root, a);
    auto path2 = FindPath(root, b);
    if (path1.empty() || path2.empty()) {
        throw std::invalid_argument{"wrong input for FindLeastAncient"};
    }

    Node *ancient = root;
    auto size = std::min(path1.size(), path2.size());
    for (size_t i=0; i<size; ++i) {
        if (path1[i] == path2[i]) {
            ancient = path1[i];
        } else {
            break;
        }
    }
    return ancient->val;
}

size_t Distance(Node *root, int a, int b) {
    auto path1 = FindPath(root, a);
    auto path2 = FindPath(root, b);
    if (path1.empty() || path2.empty()) {
        throw std::invalid_argument{"wrong input for FindLeastAncient"};
    }

    size_t i = 0;
    auto size = std::min(path1.size(), path2.size());
    for (; i<size; ++i) {
        if (path1[i] != path2[i]) {
            break;
        }
    }
    return path1.size() + path2.size() - 2*i;
}


/// test
using Seq = std::index_sequence<1,0,1,1,1,0,0,1,1>;

int main(int argc, char** argv) {
    std::cout << SeqElement<Seq, 6>::value << "\n";
    std::cout << SeqSum<Seq, 6>::value << "\n";
    auto root1 = NodeCreator<Seq>::create();
    auto root2 = NodeCreator<Seq>::create();

    std::cout << std::boolalpha;
    std::cout << Equals(root1, root2) << "\n";

    std::cout <<  "\nHierarchy visit:\n------------------\n";
    HierarchyVisit(root1);

    std::cout <<  "\n\npre order visit:\n------------------\n";
    auto preseq = PreOrderVisit(root1);
    for (auto val : preseq) {
        std::cout << val << " ";
    }

    std::cout << "\n";
    auto preseq2 = PreOrderVisitWithStack2(root1);
    for (auto val : preseq2) {
        std::cout << val << " ";
    }

    std::cout <<  "\n\nin order visit:\n------------------\n";
    auto midseq = MidOrderVisit(root1);
    for (auto val : midseq) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    auto midseq2 = MidOrderVisitWithStack2(root1);
    for (auto val : midseq2) {
        std::cout << val << " ";
    }

    std::cout <<  "\n\npost order visit:\n------------------\n";
    auto postseq = PostOrderVisit(root1);
    for (auto val : postseq) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    auto postseq2 = PostOrderVisitWithStack(root1);
    for (auto val : postseq2) {
        std::cout << val << " ";
    }

    // auto rebuild_t1 = RebuildTree(preseq, midseq);
    // std::cout <<  "\nHierarchy visit rebuild_t1:\n------------------\n";
    // HierarchyVisit(rebuild_t1);
    // std::cout << Equals(root1, rebuild_t1) << "\n";


    // auto rebuild_t2 = Tree::FromPostAndInOrderSeq(postseq, midseq);
    // std::cout <<  "\n\nHierarchy visit rebuild_t2:\n------------------\n";
    // HierarchyVisit(rebuild_t2);
    // std::cout << Equals(root1, rebuild_t2) << "\n";

    // std::cout <<"\n\n";
    // auto clone_root = Clone(root1);
    // std::cout << Equals(root1, clone_root) << "\n";

    // Symmetric(root2);
    // std::cout << Equals(root1, root2) << "\n";

    // std::cout << IsSymmetric(root1, root2) << "\n";

    // std::cout <<  "\nHierarchy visit root2:\n------------------\n";
    // HierarchyVisit(root2);

    std::cout <<  "\n\ndepth test:\n------------------\n";
    std::cout << Depth(root1) << "\n";

    std::cout <<  "\n\n least ancient test:\n------------------\n";
    std::cout << FindLeastAncient(root1, 5, 1) << "\n";

    std::cout <<  "\n\n least ancient test:\n------------------\n";
    std::cout << Distance(root1, 5, 1) << "\n";


    auto bottomup = BottomToTopHierarchyVisit(root1);
    for (auto const &vec : bottomup) {
        for (auto val : vec) {
            std::cout << val << " ";
        }

        std::cout << "\n";
    }

    return 0;
}
