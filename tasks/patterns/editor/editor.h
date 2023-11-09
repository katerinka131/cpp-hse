#pragma once
#include <stack>

#include <iostream>
// class Editor {
//     std::string text_;
//     int64_t cursor_;            /// cursor is cursor_ position
//
//     std::stack<std::string> undo_;
//     std::stack<std::string> redo_;
//
// public:
//     Editor() {
//         cursor_ = -1;
//         text_ = "";
//     }
//
//     const std::string &GetText() const {
//         return text_;
//     }
//
//     void Type(char c) {
//         if (cursor_ + 1 == static_cast<int64_t>(text_.size())) {
//             text_.push_back(c);
//         } else {
//             text_.insert(cursor_ + 1, 1, c);
//         }
//         auto s = std::string("t");
//         s.push_back(c);
//         undo_.push(s);
//         cursor_ ++;
//     }
//
//     void ShiftLeft() {
//         if (cursor_ == -1) {
//             return;
//         }
//         cursor_--;
//         undo_.push("l");
//     }
//
//     void ShiftRight() {
//         if (cursor_ == static_cast<int64_t>(text_.size() - 1)) {
//             return;
//         }
//         cursor_++;
//         undo_.push("r");
//     }
//
//     void Backspace() {
//         if (cursor_ == -1) {
//             return;
//         }
//         auto s = std::string("e");
//         s.push_back(text_[cursor_]);
//         undo_.push(s);
//         text_.erase(cursor_, 1);
//         cursor_ --;
//
//     }
//
//     void Undo() {
//         if (undo_.empty()) {
//             return;
//         }
//         std::string prev = undo_.top();
//         if (prev[0] == 't') {
//             auto s = std::string("e");
//             s.push_back(text_[cursor_]);
//             redo_.push(s);
//             text_.erase(cursor_, 1);
//             cursor_ --;
//         }
//         if (prev[0] == 'l') {
//             cursor_++;
//             redo_.push("r");
//         }
//         if (prev[0] == 'r') {
//             cursor_--;
//             redo_.push("l");
//         }
//         if (prev[0] == 'e') {
//             if (cursor_ + 1 == static_cast<int64_t>(text_.size())) {
//                 text_.push_back(prev[1]);
//             } else {
//                 text_.insert(cursor_ + 1, 1, prev[1]);
//             }
//             auto s = std::string("t");
//             s.push_back(prev[1]);
//             redo_.push(s);
//         }
//     }
//
//     void Redo() {
//         if (redo_.empty()) {
//             return;
//         }
//         std::string prev = redo_.top();
//         if (prev[0] == 't') {
//             auto s = std::string("e");
//             s.push_back(text_[cursor_]);
//             undo_.push(s);
//             text_.insert(cursor_ + 1,1, prev[1]);
//             redo_.pop();
//             cursor_ ++;
//         }
//         if (prev[0] == 'l') {
//             cursor_++;
//             undo_.push("r");
//             redo_.pop();
//         }
//         if (prev[0] == 'r') {
//             cursor_--;
//             undo_.push("l");
//             redo_.pop();
//         }
//         if (prev[0] == 'e') {
//             if (cursor_ + 1 == static_cast<int64_t>(text_.size())) {
//                 text_.push_back(prev[1]);
//             } else {
//                 text_.insert(cursor_ + 1,1, prev[1]);
//             }
//             auto s = std::string("t");
//             s.push_back(prev[1]);
//             undo_.push(s);
//             redo_.pop();
//         }
//     }
// };

class Worker {
    std::stack<std::string>& write_;
    std::stack<std::string>& read_;

    std::string& text_;
    int64_t& cursor_;

public:
    Worker(std::stack<std::string>& w, std::stack<std::string>& r, std::string& text,
           int64_t& cursor)
        : write_(w), read_(r), text_(text), cursor_(cursor) {
        //...
    }

    void ProcessCommand() {
        if (read_.empty()) {
            return;
        }
        auto cmd = read_.top();
        if (cmd[0] == 'l') {
            ShiftLeft();
        }
        if (cmd[0] == 'r') {
            ShiftRight();
        }
        if (cmd[0] == 'b') {
            Backspace();
        }
        if (cmd[0] == 't') {
            Type(cmd[1]);
        }
        read_.pop();
    }

private:
    void ShiftLeft() {
        if (cursor_ == -1) {
            return;
        }
        cursor_--;
        write_.push("r");
    }
    void ShiftRight() {
        if (cursor_ == static_cast<int64_t>(text_.size() - 1)) {
            return;
        }
        cursor_++;
        write_.push("l");
    }
    void Type(char c) {
        text_.insert(cursor_ + 1, 1, c);
        cursor_++;
        auto s = std::string("b");
        s.push_back(c);
        write_.push(s);
    }
    void Backspace() {
        if (cursor_ == -1) {
            return;
        }
        auto s = std::string("t");
        s.push_back((text_[cursor_]));
        write_.push(s);
        text_.erase(cursor_, 1);
        cursor_--;
    }
};

class Editor {
    std::string text_;
    int64_t cursor_;  /// cursor is cursor_ position

    Worker doer_;
    Worker undoer_;

    std::stack<std::string> commands_for_doer_ = {};
    std::stack<std::string> commands_for_undoer_ = {};

public:
    Editor()
        : doer_(commands_for_undoer_, commands_for_doer_, text_, cursor_),
          undoer_(commands_for_doer_, commands_for_undoer_, text_, cursor_) {
        cursor_ = -1;
        text_ = "";
    }

    void ShiftRight() {
        ClearDoerCommands();
        commands_for_doer_.push("r");
        doer_.ProcessCommand();
    }

    void ShiftLeft() {
        ClearDoerCommands();
        commands_for_doer_.push("l");
        doer_.ProcessCommand();
    }

    void Type(char c) {
        ClearDoerCommands();
        auto s = std::string("t");
        s.push_back(c);
        commands_for_doer_.push(s);

        doer_.ProcessCommand();
    }

    void Backspace() {
        ClearDoerCommands();
        commands_for_doer_.push("b");
        doer_.ProcessCommand();
    }

    const std::string& GetText() const {
        return text_;
    }

    void Undo() {
        undoer_.ProcessCommand();
    }

    void Redo() {
        doer_.ProcessCommand();
    }

private:
    void ClearDoerCommands() {
        commands_for_doer_ = std::stack<std::string>();
    }
};
