
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    return 0;
}

// Memory alloc errors are handled by the map and vector class themselves

map<int, int> heap;
vector<int> stack;

vector<size_t> callStack;
map<size_t, size_t> labels;

string output = "";

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
    Token(void (*function)(int), int value) : function(function), value(value) {} // function's type is a pinter to a function, with void return type, and an argument with int type
    void(*function)(int);
    int value;
};

// Stack Manipulation
void stack_push(int num) {
    stack.push_back(num);
}

void stack_duplicate(int num) {
    if (num >= stack.size()) throw exception("Runtime error: range out of bounds");
    stack.push_back(stack[stack.size() - num - 1]); //////IDK
}

void stack_discard(int num) {
    if (num < 0 || num >= stack.size()) { // remove everything but the top value
        if (stack.size() < 0) throw exception("Runtime error : stack is empty");
        int top = pop();
        stack.clear();
        stack.push_back(top);
        return;
    }
    int top = pop();
    // Resize the vector to remove the last n elements
    stack.resize(stack.size() - num);
    /*for (int i = 0; i < num; ++i) {
        stack.pop_back();
    }*/
    stack.push_back(top);
}

void stack_duplicateTop(int) {
    stack.push_back(stack[stack.size() - 1]);
}

void stack_swapTop(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to swap them");
    int tmp = stack[stack.size() - 1];
    stack[stack.size() - 1] = stack[stack.size() - 2];
    stack[stack.size() - 2] = tmp;
}

void stack_discardTop(int) {
    if (stack.size() < 1) throw exception("Runtime error: cant pop empty stack");
}

// Arithmetic
void arith_add(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to add them");
    stack.push_back(pop() + pop());
}

void arith_sub(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to subtract them");
    stack.push_back(-pop() + pop());
}

void arith_mult(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to multiply them");
    stack.push_back(pop() * pop());
}

void arith_div(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to divide them");
    int a = pop();
    int b = pop();
    if (a == 0) throw exception("Runtime error: division by 0");
    stack.push_back(b / a); // floor of the division
}

void arith_mod(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to modulo them");
    int a = pop();
    int b = pop();
    int sign = a < 0 ? -1 : 1;
    if (a == 0) throw exception("Runtime error: modulo by 0");
    stack.push_back((b % a) * sign);
}

// Heap access
void heap_set(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to ....... them");
    heap[pop()] = pop();
}

void heap_get(int) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    map<int, int>::iterator it = heap.find(pop());
    if (it == heap.end()) throw exception("Runtime error: No such adress in heap exists");
    stack.push_back(it->second); // it->second is the value
}

// Input/Output
void output_char(int) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    //output as char
    output += static_cast<char>(pop() % 256); // maybe % 256 is not needed
}

void output_num(int) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    //output as num
    output += intToStr(pop());
}

void input_char(int) {

}

void input_num(int) {

}

// Flow Control
void control_call(int label) {
    pc = label;
    // WOrk in progress
}

void control_jump(int label) {
    callStack.push_back(pc);
    pc = label;
}

void control_jumpIfZero(int label) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    if (pop() == 0) {
        pc = label;
    }
}

void control_jumpIfNegative(int label) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    if (pop() < 0) {
        pc = label;
    }
}

void control_return(int label) {
    if (callStack.size() < 1) throw exception("Runtime error: Cannot return, because callstack is empty");
    pc = callStack.back();
    callStack.pop_back();
}

void control_exit(int) {
    // end program
}

// Lex and parse in one go
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

    bool expect(char ch) {
        
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
        advance(); // advane past the newLine
        return sign * num;
    }

    size_t makeLabel() {
        size_t num = 0;
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
    vector<Token> parse() {
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
                    {
                        eat();
                        size_t label = makeLabel();
                        if (labels.find(label) != labels.end()) throw exception("Label already exists"); // check if already exists
                        labels[label] = res.size();
                    }
                        break;
                    case '\t':
                    {
                        eat();
                        map<size_t, size_t>::iterator it = labels.find(makeLabel());
                        if (it == labels.end()) throw exception("No such label exists");
                        res.push_back(Token(control_call, it->second));
                    }
                        break;
                    case '\n':
                    {
                        eat();
                        map<size_t, size_t>::iterator it = labels.find(makeLabel());
                        if (it == labels.end()) throw exception("No such label exists");
                        res.push_back(Token(control_jump, it->second));
                    }
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
                    {
                        eat();
                        map<size_t, size_t>::iterator it = labels.find(makeLabel());
                        if (it == labels.end()) throw exception("No such label exists");
                        res.push_back(Token(control_jumpIfZero, it->second));
                    }
                        break;
                    case '\t':
                    {
                        eat();
                        map<size_t, size_t>::iterator it = labels.find(makeLabel());
                        if (it == labels.end()) throw exception("No such label exists");
                        res.push_back(Token(control_jumpIfNegative, it->second));
                    }
                        break;
                    case '\n':
                        eat();
                        res.push_back(Token(control_return, 0));
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

            // should not reach, because programs should end with control_exit
        return res;
    }
};

class Reader {
public:
    Reader(const string& inp) : inp(inp) {}
    int readNum() {
        // TODO: throw exception if input needed but there are no more left
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

// Solution
string whitespace(const string& code, const string& inp) {

    Parser myparser(code);
    vector<Token> tokens = myparser.parse();

    // Interpreter
    for (Token tok : tokens) {
        void(*funcPtr)(int) = tok.function;
        if (funcPtr == control_exit) {
            return output;
        }
        funcPtr(tok.value);
    }

    throw exception("Programcode ended without control_exit");
}
