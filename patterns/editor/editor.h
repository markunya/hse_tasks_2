#pragma once
#include <string>
#include <vector>

class Editor {
public:
    const std::string &GetText() {
        buffer_ = text_.substr(left_, right_ - left_) + buffer_;
        for (size_t i = 0; i + right_ < text_.size(); ++i) {
            text_[left_ + i] = text_[right_ + i];
        }
        text_.resize(text_.size() - (right_ - left_));
        right_ = left_;
        return text_;
    }

    void Type(char c) {
        Grow();
        text_[left_] = c;
        ++left_;
        to_undo_.emplace_back(Command::Type);
        to_redo_.clear();
    }

    void ShiftLeft() {
        if (left_ == 0) {
            return;
        }
        text_[right_ - 1] = text_[left_ - 1];
        --left_;
        --right_;
        to_undo_.emplace_back(Command::ShiftLeft);
        to_redo_.clear();
    }

    void ShiftRight() {
        if (right_ == text_.size()) {
            return;
        }
        text_[left_] = text_[right_];
        ++right_;
        ++left_;
        to_undo_.emplace_back(Command::ShiftRight);
        to_redo_.clear();
    }

    void Backspace() {
        if (left_ == 0) {
            return;
        }
        --left_;
        to_undo_.emplace_back(Command::Backspace);
        to_redo_.clear();
    }

    void Undo() {
        if (to_undo_.empty()) {
            return;
        }
        Command command = to_undo_.back();
        to_undo_.pop_back();
        switch (command) {
            case Command::Type:
                --left_;
                break;
            case Command::ShiftRight:
                text_[right_ - 1] = text_[left_ - 1];
                --left_;
                --right_;
                break;
            case Command::ShiftLeft:
                text_[left_] = text_[right_];
                ++right_;
                ++left_;
                break;
            case Command::Backspace:
                ++left_;
                break;
        }
        to_redo_.emplace_back(command);
    }

    void Redo() {
        Grow();
        if (to_redo_.empty()) {
            return;
        }
        Command command = to_redo_.back();
        to_redo_.pop_back();
        switch (command) {
            case Command::Type:
                ++left_;
                break;
            case Command::ShiftRight:
                text_[left_] = text_[right_];
                ++right_;
                ++left_;
                break;
            case Command::ShiftLeft:
                text_[right_ - 1] = text_[left_ - 1];
                --left_;
                --right_;
                break;
            case Command::Backspace:
                --left_;
                break;
        }
    }

private:
    inline static const size_t kGapBufferSize = 16;
    enum class Command { ShiftRight, ShiftLeft, Backspace, Type };
    std::vector<Command> to_undo_;
    std::vector<Command> to_redo_;
    size_t left_ = 0;
    size_t right_ = kGapBufferSize;
    std::string buffer_;
    std::string text_ = std::string(kGapBufferSize, 'a');
    void Grow() {
        if (left_ == right_) {
            size_t buffer_size = buffer_.empty() ? kGapBufferSize : buffer_.size();
            text_.resize(text_.size() + buffer_size);
            right_ += buffer_size;
            for (size_t r = text_.size() - 1; r >= right_; --r) {
                text_[r] = text_[r - buffer_size];
            }
            for (size_t i = 0; i < buffer_.size(); ++i) {
                text_[left_ + i] = buffer_[i];
            }
            buffer_.clear();
        }
    }
};
