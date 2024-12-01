
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    return 0;
}

map<int, int> heap;
vector<int> stack;

int pop() {
    int res = stack.back();
    stack.pop_back();
    return res;
}

size_t pc = 0; // program counter

// decimal
string intToStr(int num) {
    if (num == 0) return "0";
    
    bool isNegative = num < 0;
    if (isNegative) {
        num = -num;
    }

    string res = "";
    while (num > 0) {
        res += char((num % 10) + '0');
        num /= 10;
    }
    
    if (isNegative) {
        res += '-';
    }

    reverse(res.begin(), res.end());
    return res;
}

// To help with debugging
string unbleach(string s) {
    transform(s.begin(), s.end(), s.begin(), [](char c) { return (c == ' ') ? 's' : ((c == '\n') ? 'n' : 't'); });
    return s;
}

class Token {
public:
    Token(void* function, int value) : function(function), value(value) {}
    void* function;
    int value;
};

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
    stack.push_back(pop() + pop());
}

void arith_sub() {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to subtract them");
    stack.push_back(-pop() + pop());
}

void arith_mult() {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to multiply them");
    stack.push_back(pop() * pop());
}

void arith_div() {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to divide them");
    int a = pop();
    int b = pop();
    if (a == 0) throw exception("Runtime error: division by 0");
    stack.push_back(b / a); // floor of the division
}

void arith_mod() {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to modulo them");
    int a = pop();
    int b = pop();
    int sign = a < 0 ? -1 : 1;
    if (a == 0) throw exception("Runtime error: modulo by 0");
    stack.push_back((b % a) * sign);
}

// Heap access
void heap_set() {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to ....... them");
    int a = pop();
    int b = pop();
    heap.set(a, b);
}

void heap_get() {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    int a = pop();
    int b;
    if (heap.get(a, b)) throw exception("Runtime error: No such adress in heap exists");
    stack.push_back(b);
}

// Input/Output
void output_char() {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    int a = pop();
    //output as char
}

void output_num() {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    int a = pop();
    //output as num
    output += intToStr(a);
}

void input_char() {

}

void input_num() {

}

void control_call(size_t label) {
    pc = label;
}

void control_jump(size_t label) {

}

void control_jumpIfZero(size_t label) {

}

void control_jumpIfNegative(size_t label) {

}

void control_return(size_t label) {

}

// Led and parse in one go
class Parser {
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

    /*TT match() {
        
    }

    NEED getAttr(void* function) {
        switch (function) {
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
    }*/

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
        advance(); // advane past the newLine
        return sign * num;
    }

    int makeLabel() {
        int num = 0;
        while (current_char != '\n') {
            eat();

            num *= 2;
            num += (current_char == '\t'); // ch is either tab -> 1, or space -> 0
        }
        advance(); // advane past the newLine

        return num;
    }

public:
    Parser(const string& code) : code(code) {}
    vector<Token> parse(const string& code) {
        vector<Token> res;
        while (current_char != '\0') {
            switch (current_char) {
            case ' ':
                // Stack manipulation
                eat();
                switch (current_char) {
                case ' ':
                    eat();
                    res.push_back(Token(stack_push, makeNumber()));
                    break;
                case '\t':
                    eat();
                    switch (current_char) {
                    case ' ':
                        eat();
                        res.push_back(Token(stack_duplicate, makeNumber()));
                        break;
                    case '\n':
                        eat();
                        res.push_back(Token(stack_discard, makeNumber()));
                        break;
                    default:
                        throw exception();
                    }
                case '\n':
                    eat();
                    switch (current_char) {
                    case ' ':
                        advance();
                        res.push_back(Token(stack_duplicateTop, 0));
                        break;
                    case '\t':
                        advance();
                        res.push_back(Token(stack_swapTop, 0));
                        break;
                    case '\n':
                        advance();
                        res.push_back(Token(stack_discardTop, 0));
                        break;
                    default:
                        // unreachable
                        throw exception();
                    }
                default:
                    // unreachable
                    throw exception();
                }
                break;
            case '\t':
                eat();
                switch (current_char) {
                case ' ':
                    // Arithmetic
                    eat();
                    switch (current_char) {
                    case ' ':
                        eat();
                        switch (current_char) {
                        case ' ':
                            advance();
                            res.push_back(Token(arith_add, 0));
                            break;
                        case '\t':
                            advance();
                            res.push_back(Token(arith_sub, 0));
                            break;
                        case '\n':
                            advance();
                            res.push_back(Token(arith_mult, 0));
                            break;
                        default:
                            // unreachable
                            throw exception();
                        }
                        break;
                    case '\t':
                        eat();
                        switch (current_char) {
                        case ' ':
                            advance();
                            res.push_back(Token(arith_div, 0));
                            break;
                        case '\t':
                            advance();
                            res.push_back(Token(arith_mod, 0));
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
                    eat();
                    switch (current_char) {
                    case ' ':
                        advance();
                        res.push_back(Token(heap_set, 0));
                        break;
                    case '\t':
                        advance();
                        res.push_back(Token(heap_get, 0));
                        break;
                    default:
                        throw exception();
                    }
                    break;
                case '\n':
                    // Input/Output
                    eat();
                    switch (current_char) {
                    case ' ':
                        eat();
                        switch (current_char) {
                        case ' ':
                            advance();
                            res.push_back(Token(output_char, 0));
                            break;
                        case '\t':
                            advance();
                            res.push_back(Token(output_num, 0));
                            break;
                        default:
                            throw exception();
                        }
                        break;
                    case '\t':
                        eat();
                        switch (current_char) {
                        case ' ':
                            advance();
                            res.push_back(Token(input_char, 0));
                            break;
                        case '\t':
                            advance();
                            res.push_back(Token(input_num, 0));
                            break;
                        default:
                            throw exception();
                        }
                        break;
                    default:
                        throw exception();
                    }
                    break;
                default:
                    throw exception();
                }
                break;
            case '\n':
                // Flow-Control
                eat();
                switch (current_char) {
                case ' ':
                    eat();
                    switch (current_char) {
                    case ' ':
                        eat();
                        labels.insert(makeLabel(), res.size()); // check if already exists
                        //res.push_back(Token(, );
                        break;
                    case '\t':
                        eat();
                        int nthCommand = labels.get(makeLabel());
                        res.push_back(Token(control_call, nthCommand));
                        break;
                    case '\n':
                        eat();
                        int nthCommand = labels.get(makeLabel());
                        res.push_back(Token(control_jump, nthCommand));
                        break;
                    default:
                        // unreachable
                        throw exception();
                    }
                    break;
                case '\t':
                    eat();
                    switch (current_char) {
                    case ' ':
                        eat();
                        int nthCommand = labels.get(makeLabel());
                        res.push_back(Token(control_jumpIfZero, nthCommand));
                        break;
                    case '\t':
                        eat();
                        int nthCommand = labels.get(makeLabel());
                        res.push_back(Token(control_jumpIfNegative, nthCommand));
                        break;
                    case '\n':
                        eat();
                        int nthCommand = labels.get(makeLabel());
                        res.push_back(Token(control_return, nthCommand));
                        break;
                    default:
                        // unreachable
                        throw exception();
                    }
                    break;
                case '\n':
                    eat();
                    switch (current_char) {
                    case '\n':
                        advance();
                        res.push_back(Token(control_exit, 0));
                        break;
                    default:
                        throw exception();
                    }
                    break;
                default:
                    // unreachable
                    throw exception();
                }
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
        res.push_back(Token(endOfProgram, 0)); // should not reach, because programs should end with control_exit

        return res;
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
};

class Interpreter {
    
    // size_t stackPointer = 0;

    // Stack manipulation
    

    // Flow Control
    /*void control_mark(int label) {
        
    }*/
};

// Solution
string whitespace(const string& code, const string& inp) {


    map<int, int> heap;
    vector<int> stack;
    string output;
    /// ...
    return output;
}
