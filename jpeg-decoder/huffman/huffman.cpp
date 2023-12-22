#include <huffman.h>

class HuffmanTree::Impl {
public:
    void Build(const std::vector<uint8_t> &code_lengths, const std::vector<uint8_t> &values) {
        std::vector<uint8_t> code_lengths_modified = code_lengths;
        while (!code_lengths_modified.empty() && code_lengths_modified.back() == 0) {
            code_lengths_modified.pop_back();
        }
        if (code_lengths_modified.size() > 16) {
            throw std::invalid_argument("Size should be no bigger than 16");
        }
        size_t sum = 0;
        for (size_t i = 0; i < code_lengths_modified.size(); ++i) {
            sum += code_lengths_modified[i];
            if (code_lengths_modified[i] > (1 << (i + 1))) {
                throw std::invalid_argument("Too many leaves for layer");
            }
        }
        if (values.size() != sum) {
            throw std::invalid_argument("Too few values");
        }
        if (code_lengths_modified.empty()) {
            return;
        }
        size_t cl_i = 0;
        while (code_lengths_modified[cl_i] == 0) {
            ++cl_i;
        }
        size_t v_i = 0;
        root_ = Builder(code_lengths_modified, values, cl_i, v_i);
        position_ = root_.get();
    }

    bool Move(bool bit, int &value) {
        if (!position_) {
            throw std::invalid_argument("Huffman tree has not been built or it is incorrect");
        }
        if (bit) {
            position_ = position_->right.get();
        } else {
            position_ = position_->left.get();
        }
        if (!position_) {
            throw std::invalid_argument("Huffman tree has been built incorrect");
        }
        if (!position_->left && !position_->right) {
            value = position_->value;
            position_ = root_.get();
            return true;
        }
        return false;
    }

private:
    struct Node {
        uint8_t value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    std::unique_ptr<Node> Builder(std::vector<uint8_t> &code_lengths,
                                  const std::vector<uint8_t> &values, size_t &current_cl_index,
                                  size_t &current_value_index, size_t cl = 0) {
        if (current_cl_index == code_lengths.size() || current_value_index == values.size()) {
            return nullptr;
        }
        std::unique_ptr<Node> result = std::make_unique<Node>();
        if (cl == current_cl_index + 1) {
            --code_lengths[current_cl_index];
            while (code_lengths[current_cl_index] == 0) {
                ++current_cl_index;
            }
            result->value = values[current_value_index];
            ++current_value_index;
            return result;
        }
        result->left = Builder(code_lengths, values, current_cl_index, current_value_index, cl + 1);
        result->right =
            Builder(code_lengths, values, current_cl_index, current_value_index, cl + 1);
        return result;
    }
    std::unique_ptr<Node> root_;
    Node *position_ = nullptr;
};

HuffmanTree::HuffmanTree() : impl_(std::make_unique<Impl>()) {
}

void HuffmanTree::Build(const std::vector<uint8_t> &code_lengths,
                        const std::vector<uint8_t> &values) {
    impl_->Build(code_lengths, values);
}

bool HuffmanTree::Move(bool bit, int &value) {
    return impl_->Move(bit, value);
}

HuffmanTree::HuffmanTree(HuffmanTree &&) = default;

HuffmanTree &HuffmanTree::operator=(HuffmanTree &&) = default;

HuffmanTree::~HuffmanTree() = default;
