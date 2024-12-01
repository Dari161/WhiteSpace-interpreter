
#include <iostream>


int main() {
    return 0;
}



// TODO: include error positions

using namespace std;

#include <map>
#include <vector>
#include <algorithm>

// Handle memory errors

/*template <typename T>
class Stack {
private:
    T* stack;
    size_t capacity;
    size_t size;
public:
    Stack() : capacity(1), size(0) {
        stack = new T[capacity];

        if (!newStack) {
            throw exception("Ran out of memory");
        }
    }

    ~Stack() {
        delete[] arr;
    }

    void push(const T& elem) {
        if (size == capacity) {
            T* newStack = new T[2 * capacity];

            if (!newStack) {
                throw exception("Ran out of memory");
            }

            memcpy(newStack, stack, size);

            delete[] stack;
            capacity *= 2;
            stack = newStack;
        }

        stack[size] = data;
        ++size;
    }

    T pop() {
        --size;
        return stack[size];
    }

    T peek() const {
        return stack[size];
    }
};

template <typename T>
class Heap {
private:
    // node struct for binary tree
    struct Node {
        int address;
        T value;
        Node* left;
        Node* right
    };

    Node* root; // root node of binary search tree
    size_t capacity;
    size_t size;
public:
    Heap() : capacity(1), size(0), left(nullptr), right(nullptr) {
        root = new Node[capacity];

        if (!newStack) {
            throw exception("Ran out of memory");
        }
    }

    ~Heap() {
        delete[] heap;
    }

    set(int address, const T& value) {

    }

    get() {

    }
};*/

string intToStr(int num) {
    if (num == 0) return "0";
    string res = "";
    if (num < 0) {
        res += '-';
    }

    while (num != 0) {
        res += (num % 10) + '0';
        num /= 10;
    }
}

// string STCK = " ";
string STCK_PUSH = "  ";
string STCK_DUPLICATEN = " \t ";
string STCK_DISCARDN = " \t\n";
string STCK_DUPLICATETOP = " \n\n";
string STCK_SWAP = " \n\t";
string STCK_DISCARDTOP = " \n\n";
string STCK_PUSH = " \t ";
string ARITH = "\t ";
string HEAP = "\t\t";
string IO = "\t\n";
string CONT = "\n";

// To help with debugging
string unbleach(string s) {
    transform(s.begin(), s.end(), s.begin(), [](char c) { return (c == ' ') ? 's' : ((c == '\n') ? 'n' : 't'); });
    return s;
}

// TokenType
enum TT {
    INT,

};

enum NEED {
    NOTHING,
    NUMBER,
    LABEL
}

class Token {
public:
    Token(TT type, int value) : type(type), value(value) {}
    TT type;
    int value; // maybe not int
};

class Lexer {
private:
    string code;
    int pos = -1;
    char current_char;

    bool advance() {
        do {
            ++pos;
            if (pos >= code.length()) return true;
            current_char = code[pos];
        } while (!(current_char == ' ' || current_char == '\t' || current_char == '\n'));
        return false;
    }

    void eat() {
        if (advance()) throw exception("Unexpected end of file");
    }

    /*char peek() {
        return ' ';
    }*/

    bool expect(char ch) {
        
    }

    TT match() {
        
    }

    NEED getAttr(TT type) {
        switch (type) {
        case TT::STACK_PUSH: return NEED::NUMBER;
        case TT::STACK_DUPLICATE: return NEED::NUMBER;
        case TT::STACK_DISCARD: return NEED::NUMBER;
        case TT::CONTROL_MARK: return NEED::LABEL;
        case TT::CONTROL_CALLSUBROUTINE: return NEED::LABEL;
        case TT::CONTROL_UNCONDITIONALJUMP: return NEED::LABEL;
        case TT::CONTROL_idk: return NEED::LABEL;
        case TT::CONTROL_idk2: return NEED::LABEL;
        }
        return NEED::NOTHING;
    }

    int makeNumber() {
        int sign;
        switch (current_char) {
        case '\t':
            eat();
            sign = -1;
            break;
        case ' ':
            eat();
            sign = 1;
            break;
        default:
            throw exception("Expected a number. Numbers must start with tab or space");
        }

        int num = 0;
        while (current_char != '\n') {
            eat();
            
            num *= 2;
            num += (current_char == '\t'); // ch is either tab -> 1, or space -> 0
        }
        eat(); // eat the newLine
        return sign * num;
    }

    int makeLabel() {
        int num = 0;
        while (current_char) {
            eat();

            num *= 2;
            num += (current_char == '\t'); // ch is either tab -> 1, or space -> 0
        }
        eat();

        // make sure label is unique
        return num;
    }

public:
    Lexer(const string& code) : code(code) {}
    vector<Token> lex(const string& code) {
        vector<Token> res;
        while (current_char != '\0') {
            switch (current_char) {
            case ' ':
                // Stack manipulation
                if (advance()) throw exception();
                switch (current_char) {
                case ' ':
                    if (advance()) throw exception();
                    res.push_back(Token(TT::INT, makeNumber()));
                    break;
                case '\t':
                    if (advance()) throw exception();
                    switch (current_char) {
                    case ' ':
                        if (advance()) throw exception();
                        res.push_back(Token(TT::INT, createNumber()));
                        break;
                    case '\n':
                        if (advance()) throw exception();
                        res.push_back(Token(TT::INT, createNumber()));
                        break;
                    default:
                        throw exception();
                    }
                }
                break;
            case '\t':
                if (advance()) throw exception();
                switch (current_char) {
                case ' ':
                    // Arithmetic
                    if (advance()) throw exception();
                    switch (current_char) {
                    case ' ':
                        if (advance()) throw exception();
                        switch (current_char) {
                        case ' ':
                            break;
                        case '\t':
                            break;
                        case '\n':
                            break;
                        default:
                            throw exception();
                        }
                        break;
                    case '\t':
                        if (advance()) throw exception();
                        switch (current_char) {
                        case ' ':
                            break;
                        case '\t':
                            break;
                        default:
                            throw exception();
                        }
                        break;
                    default:
                        throw exception();
                    }
                    break;
                case '\t':
                    // Heap Access
                    break;
                case '\n':
                    break;
                default:
                    throw exception();
                }
            case '\n':
                break;
            default:
                throw exception();
            }
        }
            /*if (isKeyword()) {
                TT type match(KEYWORDS);
                NEED attributeNeed = getAttrs(type);
                switch (attributeNeed) {
                case NEED::NOTHING:
                    res.push_back(Token(type, -1));
                    break;
                case NEED::NUMBER:
                    res.push_back(Token(type, makeNumber()));
                    break;
                case NEED::LABEL:
                    res.push_back(Token(type, makeLabel()));
                    break;
                default:
                    throw exception();
                }

                if ((attributeNeed == NEED::NOTHING)) {
                    res.push_back(Token(type, -1));
                } else if (attributeNeed == NEED::NUMBER) {
                    res.push_back(Token(type, makeNumber()));
                } else if (attributeNeed == NEED::LABEL) {
                    res.push_back(Token(type, makeLabel()));
                }
                
            } else {
                throw exception();
            }*/

    }
};

class Reader {
public:
    Reader(const string& inp) : inp(inp) {}
    int readNum() {
        if (current_char == '0') {
            if (advance()) return 0;
            if (current_char == '\n') return 0;
            if (current_char == 'x') {
                eat();
                int num = 0;
                while (current_char != '\n') {
                    num *= 16;
                    num += current_char - '0'; // May not work for ABCDEF
                    if (advance()) return num;
                }
                eat();
                return num;
            }
        }

        int num = 0;
        while (current_char != '\n') {
            num *= 10;
            num += current_char - '0';
            if (advance()) return num;
        }
        eat();
        return num;
    }
private:
    const string& inp;
    int pos = -1;
    char current_char;

    bool advance() {
        ++pos;
        if (pos >= inp.length()) return true;
        current_char = inp[pos];
        return false;
    }

    void eat() {
        if (advance()) throw exception("Unexpected end of file");
    }
}

class Runtime {
    map<int, int > heap;
    vector<int> stack;
    size_t stackPointer = 0;

    // Stack manipulation
    void stack_push(int num) {
        stack.push_back(num);
    }

    void stack_duplicate(int num) {
        if (num >= stack.size()) throw exception("Runtime error: range out of bounds");
        stack.push_back(stack[num]);
    }

    void stack_discard(int num) {
        
    }

    void stack_duplicateTop() {
        stack.push_back(stack[stack.size() - 1]);
    }

    void stack_swapTop() {
        if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to swap them");
        int tmp = stack[stack.size() - 1];
        stack[stack.size() - 1] = stack[stack.size() - 2];
        stack[stack.size() - 2] = tmp;
    }

    void stack_discardTop() {
        if (stack.size() < 1) throw exception("Runtime error: cant pop empty stack");
    }

    // Arithmetic
    void arith_add() {
        if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to add them");
        stack.push_back(stack.pop_back() + stack.pop_back());
    }

    void arith_sub() {
        if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to subtract them");
        stack.push_back(-stack.pop_back() + stack.pop_back());
    }

    void arith_mult() {
        if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to multiply them");
        stack.push_back(stack.pop_back() * stack.pop_back());
    }

    void arith_div() {
        if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to divide them");
        int a = stack.pop_back();
        int b = stack.pop_back();
        if (b == 0) throw exception("Runtime error: division by 0");
        stack.push_back(b / a); // floor of the division
    }

    void arith_div() {
        if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to modulo them");
        int a = stack.pop_back();
        int b = stack.pop_back();
        if (b == 0) throw exception("Runtime error: modulo by 0");
        stack.push_back(b / a); // floor of the division
        // TODO: sign is needed
    }

    // Heap access
    void heap_set() {
        if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to ....... them");
        int a = stack.pop_back();
        int b = stack.pop_back();
        heap.set(a, b);
    }

    void heap_get() {
        if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
        int a = stack.pop_back();
        int b;
        if (heap.get(a, b)) throw exception("Runtime error: No such adress in heap exists");
        stack.push_back(b);
    }

    // Input/Output
    void output_char() {
        if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
        int a = stack.pop_back();
        //output as char
    }

    void output_num() {
        if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
        int a = stack.pop_back();
        //output as num
        output += intToStr(a);
    }

    void input_char() {
    
    }

    void input_number() {
    
    }

    // Flow Control
    /*void control_mark(int label) {
        
    }*/
}

// Solution
string whitespace(const string& code, const string& inp = string())
{


    map<int, int> heap;
    vector<int> stack;
    string output;
    /// ...
    return output;
}
