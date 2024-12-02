
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

const char* hexDigits = "0123456789ABCDEF";
class Reader {
public:
    Reader(const string& inp) : inp(inp) {}

    int readNum() {
        if (current_char == '\0') throw exception("Runtime error: Input was empty when trieying to read number from it");
        if (current_char == '0') {
            if (advance()) return 0;
            if (current_char == '\n') return 0;
            if (current_char == 'x') {
                eat(); // eat 'x'
                int num = 0;
                while (current_char != '\n') {
                    num *= 16;
                    num += hexCharToInt(current_char);
                    eat();
                }
                eat(); // eat '\n'
                return num;
            }
        }

        int num = 0;
        while (current_char != '\n') {
            num *= 10;
            num += current_char - '0';
            eat();
        }
        eat(); // eat '\n'
        return num;
    }

    char readChar() {
        if (pos >= inp.length()) throw exception("Runtime error: input was empty when trying to read a character");
        advance();
    }
private:
    const string& inp;
    int pos = 0;
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

    int hexCharToInt(char hexChar) {
        switch (hexChar) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'A': case 'a': return 10;
        case 'B': case 'b': return 11;
        case 'C': case 'c': return 12;
        case 'D': case 'd': return 13;
        case 'E': case 'e': return 14;
        case 'F': case 'f': return 15;
        default:
            throw std::invalid_argument("Invalid hexadecimal character");
        }
    }
};

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
    ++pc;
}

void stack_duplicate(int num) {
    if (num >= stack.size()) throw exception("Runtime error: range out of bounds");
    stack.push_back(stack[stack.size() - num - 1]); //////IDK
    ++pc;
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
    ++pc;
}

void stack_duplicateTop(int) {
    stack.push_back(stack[stack.size() - 1]);
    ++pc;
}

void stack_swapTop(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to swap them");
    int tmp = stack[stack.size() - 1];
    stack[stack.size() - 1] = stack[stack.size() - 2];
    stack[stack.size() - 2] = tmp;
    ++pc;
}

void stack_discardTop(int) {
    if (stack.size() < 1) throw exception("Runtime error: cant pop empty stack");
    ++pc;
}

// Arithmetic
void arith_add(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to add them");
    stack.push_back(pop() + pop());
    ++pc;
}

void arith_sub(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to subtract them");
    stack.push_back(-pop() + pop());
    ++pc;
}

void arith_mult(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to multiply them");
    stack.push_back(pop() * pop());
    ++pc;
}

void arith_div(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to divide them");
    int a = pop();
    int b = pop();
    if (a == 0) throw exception("Runtime error: division by 0");
    stack.push_back(b / a); // floor of the division
    ++pc;
}

void arith_mod(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to modulo them");
    int a = pop();
    int b = pop();
    int sign = a < 0 ? -1 : 1;
    if (a == 0) throw exception("Runtime error: modulo by 0");
    stack.push_back((b % a) * sign);
    ++pc;
}

// Heap access
void heap_set(int) {
    if (stack.size() < 2) throw exception("Runtime error: must have 2 values on stack to ....... them");
    heap[pop()] = pop();
    ++pc;
}

void heap_get(int) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    map<int, int>::iterator it = heap.find(pop());
    if (it == heap.end()) throw exception("Runtime error: No such adress in heap exists");
    stack.push_back(it->second); // it->second is the value
    ++pc;
}

// Input/Output
void output_char(int) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    //output as char
    output += static_cast<char>(pop() % 256); // maybe % 256 is not needed
    ++pc;
}

void output_num(int) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    //output as num
    output += intToStr(pop());
    ++pc;
}

void input_char(int) {
    //Reader myReader("aba");
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    heap[pop()] = myReader.readChar();
    ++pc;
}

void input_num(int) {
    //Reader myReader("aba");
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    heap[pop()] = myReader.readNum();
    ++pc;
}

// Flow Control
void control_call(int label) {
    pc = label;
}

void control_jump(int label) {
    callStack.push_back(pc);
    pc = label;
}

void control_jumpIfZero(int label) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    if (pop() == 0) {
        pc = label;
    } else {
        ++pc;
    }
}

void control_jumpIfNegative(int label) {
    if (stack.size() < 1) throw exception("Runtime error: must have a values on stack to .......");
    if (pop() < 0) {
        pc = label;
    } else {
        ++pc;
    }
}

void control_return(int label) {
    if (callStack.size() < 1) throw exception("Runtime error: Cannot return, because callstack is empty");
    pc = callStack.back() + 1; // +1 because it should not call the subroutine again but progress forward
    callStack.pop_back();
}

void control_exit(int) {} // end program

void unexpected_endOfProgram(int) {
    throw exception("Programcode ended unexpectedly (without end of program statement)");
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

        // should not reach, because programs should end with control_exit
        res.push_back(Token(unexpected_endOfProgram, 0));
        return res;
    }
};

// Solution
string whitespace(const string& code, const string& inp) {

    Parser myparser(code);
    vector<Token> tokens = myparser.parse();

    if (tokens.size() == 0) throw exception("Tokens are empty, meaning there was no end of program statement");

    // Interpreter
    /*for (Token tok : tokens) {
        void(*funcPtr)(int) = tok.function;
        if (funcPtr == control_exit) {
            return output;
        }
        funcPtr(tok.value);
    }*/

    for (;;) {
        Token tok = tokens[pc];
        void(*funcPtr)(int) = tok.function;
        if (funcPtr == control_exit) { // it needs to be in every correct program, if there is none the loop will break, beacuse the unexpected_endOfProgram function would be called resulting in throwing an error
            return output;
        }
        funcPtr(tok.value);
    }

    throw exception("Programcode ended without control_exit");
}
