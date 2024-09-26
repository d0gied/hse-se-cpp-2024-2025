#include <memory>
#include <stack>
#include <utility>

template <class K, class V>
class SearchingTree {
 public:
  SearchingTree() : root_(nullptr) {}

 private:
  enum class Color { RED, BLACK };

  struct Node {
    K key;
    V value;
    Color color;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
  };
  typedef std::shared_ptr<Node> NodePtr;

 public:
  class iterator {
   private:
    friend class SearchingTree;
    iterator(const NodePtr &root) : stack_() { _leftmost_inorder(root); }

   private:
    iterator(const std::stack<NodePtr> &stack) : stack_(stack) {}

   private:
    void _leftmost_inorder(NodePtr node) {
      while (node != nullptr) {
        stack_.push(node);
        node = node->left;
      }
    }

   public:
    iterator &operator++() {
      if (stack_.empty()) {
        return *this;
      }
      NodePtr cur = stack_.top();
      stack_.pop();
      if (cur->right) {
        _leftmost_inorder(cur->right);
      }
      return *this;
    }

    std::pair<const K &, V &> operator*() const {
      return {stack_.top()->key, stack_.top()->value};
    }

    bool operator==(const iterator &other) const {
      if (stack_.empty() && other.stack_.empty()) {
        return true;
      } else if (stack_.empty() || other.stack_.empty()) {
        return false;
      } else {
        return stack_.top() == other.stack_.top();
      }
    }

    bool has_next() const { return !stack_.empty(); }

   private:
    std::stack<NodePtr> stack_;
  };

  iterator begin() const { return iterator(root_); }

  iterator end() const { return iterator(nullptr); }

 private:
  NodePtr _rotate_left(NodePtr node) {
    NodePtr right = node->right;
    node->right = right->left;
    right->left = node;
    right->color = node->color;
    node->color = Color::RED;
    return right;
  }

  NodePtr _rotate_right(NodePtr node) {
    NodePtr left = node->left;
    node->left = left->right;
    left->right = node;
    left->color = node->color;
    node->color = Color::RED;
    return left;
  }

  void _flip_colors(NodePtr node) {
    node->color = Color::RED;
    if (node->left) node->left->color = Color::BLACK;
    if (node->right) node->right->color = Color::BLACK;
  }

  NodePtr _fix_violation(NodePtr node) {
    if (node->left && node->left->color == Color::RED) {
      if (node->right && node->right->color == Color::BLACK) {
        node = _rotate_right(node);
      }
    }

    if (node->right && node->right->color == Color::RED && node->right->right &&
        node->right->right->color == Color::RED) {
      node = _rotate_left(node);
    }

    if (node->left && node->left->color == Color::RED && node->right &&
        node->right->color == Color::RED) {
      _flip_colors(node);
    }

    return node;
  }

  NodePtr _insert(NodePtr root, NodePtr node) {
    if (root == nullptr) {
      return node;
    }
    if (node->key < root->key) {
      root->left = _insert(root->left, node);
    } else if (node->key > root->key) {
      root->right = _insert(root->right, node);
    }
    return _fix_violation(root);
  }

 public:
  void insert(const K &key, const V &value) {
    NodePtr node =
        std::make_shared<Node>(key, value, Color::RED, nullptr, nullptr);
    root_ = _insert(root_, node);
    root_->color = Color::BLACK;
  }

 private:
  NodePtr _fix_delete(NodePtr node) {
    if (node->left && node->left->color == Color::RED) {
      node = _rotate_right(node);
    }

    if (node->left && node->left->color == Color::BLACK && node->left->left &&
        node->left->left->color == Color::BLACK) {
      _flip_colors(node);
    }

    if (node->left && node->left->color == Color::BLACK && node->left->right &&
        node->left->right->color == Color::BLACK) {
      node->left = _rotate_left(node->left);
    }

    if (node->left && node->left->color == Color::RED && node->left->right &&
        node->left->right->color == Color::RED) {
      node->left = _rotate_left(node->left);
    }

    return node;
  }

  NodePtr _erase(NodePtr node, const K &key) {
    if (node == nullptr) {
      return nullptr;
    }
    if (key < node->key) {
      node->left = _erase(node->left, key);
    } else if (key > node->key) {
      node->right = _erase(node->right, key);
    } else {
      if (node->left == nullptr) return node->right;
      if (node->right == nullptr) return node->left;

      NodePtr cur = node->right;
      while (cur->left != nullptr) {
        cur = cur->left;
      }
      node->key = cur->key;
      node->value = cur->value;
      node->right = _erase(node->right, cur->key);
    }
    if (!node) return node;
    return _fix_delete(node);
  }

 public:
  void erase(const K &key) {
    if (root_ == nullptr) return;
    root_ = _erase(root_, key);
    if (root_) root_->color = Color::BLACK;
  }

  iterator find(const K &key) const {
    std::stack<NodePtr> stack;
    NodePtr cur = root_;
    while (cur != nullptr) {
      stack.push(cur);
      if (key < cur->key) {
        cur = cur->left;
      } else if (key > cur->key) {
        cur = cur->right;
      } else {
        return iterator(stack);
      }
    }
    return end();
  }

 private:
  class tree_range {
   public:
    class iterator : SearchingTree::iterator {
     private:
      friend class SearchingTree;
      iterator(const std::stack<NodePtr> &stack, const K &high)
          : SearchingTree::iterator(stack), high_(high) {}

     public:
      iterator &operator++() {
        this->SearchingTree::iterator::operator++();
        if (!this->stack_.empty() && this->stack_.top()->key >= high_) {
          this->stack_ = std::stack<NodePtr>();
        }
        return *this;
      }

      bool operator==(const iterator &other) const {
        return this->SearchingTree::iterator::operator==(other) &&
               high_ == other.high_;
      }

      std::pair<const K &, V &> operator*() const {
        return this->SearchingTree::iterator::operator*();
      }

     private:
      K high_;
    };

   private:
    friend class SearchingTree;
    tree_range(const std::stack<NodePtr> &stack, const K &high)
        : stack_(stack), high_(high) {}

   public:
    iterator begin() const { return iterator(stack_, high_); }

    iterator end() const { return iterator(std::stack<NodePtr>(), high_); }

   private:
    const std::stack<NodePtr> stack_;
    const K high_;
  };

 public:
  tree_range range(const K &low, const K &high) const {
    std::stack<NodePtr> stack;
    NodePtr cur = root_;
    while (cur != nullptr) {
      if (cur->key >= low) {
        stack.push(cur);
      }
      if (low < cur->key) {
        cur = cur->left;
      } else if (low > cur->key) {
        cur = cur->right;
      } else {
        break;
      }
    }
    return tree_range(stack, high);
  }

 private:
  NodePtr root_;
};
