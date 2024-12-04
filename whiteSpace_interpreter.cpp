
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

// CodeWarson volt fent ez a feladat

using namespace std;

// Memory alloc errors are handled by the map and vector class themselves

map<int, int> heap;
vector<int> mystack;

vector<size_t> callStack;
map<size_t, size_t> labels;

string output = "";

int pop() {
    int res = mystack.back();
    mystack.pop_back();
    return res;
}

size_t pc = 0; // program counter

string input = "";

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

// invalid character may refer to '\0' (end of string)

class Reader {
private:
    size_t pos;

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
            throw "Runtime error: Invalid hexadecimal character";
        }
    }
public:
    Reader() : pos(0) {}

    // TODO: I think it should support minus numbers, and optionally binary and octal numeral system

    int readNum() {
        if (input[pos] == '\0') throw "Runtime error: Input was empty when trying to read number";
        if (input[pos] == '0') {
            ++pos;
            if (input[pos] == '\n') {
                ++pos;
                return 0;
            }
            if (input[pos] == 'x') {
                ++pos; // eat 'x'
                int num = 0;
                while (input[pos] != '\n') {
                    num *= 16;
                    num += hexCharToInt(input[pos]);
                    ++pos;
                }
                ++pos; // eat '\n'
                return num;
            }
            throw "Runtime error : Input contains unexpected character when trying to read number";
        }
        int num = 0;
        while (input[pos] != '\n') {
            num *= 10;
            if (input[pos] < '0' || input[pos] > '9') throw "Runtime error: Invalid decimal character";
            num += input[pos] - '0';
            ++pos;
        }
        ++pos; // eat '\n'
        return num;
    }

    char readChar() {
        if (input[pos] == '\0') throw "Runtime error : input was empty when trying to read a character";
        char ret = input[pos];
        ++pos;
        return ret;
    }

    void reset() {
        pos = 0;
    }
};

Reader myReader;

struct Token {
    // function's type is a pinter to a function, with void return type, and an argument with int type
    void(*function)();
    int value;
};

int arg = 0;

// Stack Manipulation
void stack_push() {
    mystack.push_back(arg);
    ++pc;
}

void stack_duplicate() {
    if (arg >= mystack.size()) throw "Runtime error: range out of bounds";
    mystack.push_back(mystack[mystack.size() - arg - 1]); //////IDK
    ++pc;
}

void stack_discard() {
    if (arg < 0 || arg >= mystack.size()) { // remove everything but the top value
        if (mystack.size() < 0) throw "Runtime error : stack is empty";
        int top = pop();
        mystack.clear();
        mystack.push_back(top);
        return;
    }
    int top = pop();
    // Resize the vector to remove the last n elements
    mystack.resize(mystack.size() - arg);
    /*for (int i = 0; i < num; ++i) {
        mystack.pop_back();
    }*/
    mystack.push_back(top);
    ++pc;
}

void stack_duplicateTop() {
    mystack.push_back(mystack[mystack.size() - 1]);
    ++pc;
}

void stack_swapTop() {
    if (mystack.size() < 2) throw "Runtime error: must have 2 values on stack to swap them";
    int tmp = mystack[mystack.size() - 1];
    mystack[mystack.size() - 1] = mystack[mystack.size() - 2];
    mystack[mystack.size() - 2] = tmp;
    ++pc;
}

void stack_discardTop() {
    if (mystack.size() < 1) throw "Runtime error: cant pop empty stack";
    ++pc;
}

// Arithmetic
void arith_add() {
    if (mystack.size() < 2) throw "Runtime error: must have 2 values on stack to add them";
    mystack.push_back(pop() + pop());
    ++pc;
}

void arith_sub() {
    if (mystack.size() < 2) throw "Runtime error : must have 2 values on stack to subtract them";
    mystack.push_back(-pop() + pop());
    ++pc;
}

void arith_mult() {
    if (mystack.size() < 2) throw "Runtime error: must have 2 values on stack to multiply them";
    mystack.push_back(pop() * pop());
    ++pc;
}

void arith_div() {
    if (mystack.size() < 2) throw "Runtime error: must have 2 values on stack to divide them";
    int a = pop();
    int b = pop();
    if (a == 0) throw "Runtime error: division by 0";
    mystack.push_back(b / a); // floor of the division
    ++pc;
}

void arith_mod() {
    if (mystack.size() < 2) throw "Runtime error: must have 2 values on stack to modulo them";
    int a = pop();
    int b = pop();
    int sign = a < 0 ? -1 : 1;
    if (a == 0) throw "Runtime error: modulo by 0";
    mystack.push_back((abs(b) % a) * sign);
    ++pc;
}

// Heap access
void heap_set() {
    if (mystack.size() < 2) throw "Runtime error: must have 2 values on stack to ....... them";
    int a = pop();
    heap[pop()] = a;
    // in heap[pop()] = pop() the order of function evaluation would depend on the complier, since it is unspecified
    ++pc;
}

void heap_get() {
    if (mystack.size() < 1) throw "Runtime error: must have a values on stack to .......";
    map<int, int>::iterator it = heap.find(pop());
    if (it == heap.end()) throw "Runtime error: No such adress in heap exists";
    mystack.push_back(it->second); // it->second is the value
    ++pc;
}

// Input/Output
void output_char() {
    if (mystack.size() < 1) throw "Runtime error: must have a values on stack to .......";
    //output as char
    output += static_cast<char>(pop() % 256); // maybe % 256 is not needed
    ++pc;
}

void output_num() {
    if (mystack.size() < 1) throw "Runtime error: must have a values on stack to .......";
    //output as num
    output += intToStr(pop());
    ++pc;
}

void input_char() {
    if (mystack.size() < 1) throw "Runtime error: must have a values on stack to .......";
    heap[pop()] = myReader.readChar();
    ++pc;
}

void input_num() {
    if (mystack.size() < 1) throw "Runtime error: must have a values on stack to .......";
    heap[pop()] = myReader.readNum();
    ++pc;
}

// Flow Control
void control_call() {
    callStack.push_back(pc);
    pc = arg;
}

void control_jump() {
    pc = arg;
}

void control_jumpIfZero() {
    if (mystack.size() < 1) throw "Runtime error: must have a values on stack to .......";
    if (pop() == 0) {
        pc = arg;
    } else {
        ++pc;
    }
}

void control_jumpIfNegative() {
    if (mystack.size() < 1) throw "Runtime error: must have a values on stack to .......";
    if (pop() < 0) {
        pc = arg;
    } else {
        ++pc;
    }
}

void control_return() {
    if (callStack.size() < 1) throw "Runtime error: Cannot return, because callstack is empty";
    pc = callStack.back() + 1; // +1 because it should not call the subroutine again but progress forward
    callStack.pop_back();
}

void control_exit() {} // end program

void unexpected_endOfProgram() {
    throw "Programcode ended unexpectedly (without end of program statement)";
}

// Lex and parse in one go
class Parser {
private:
    string code;
    size_t pos;

    int makeNumber() {
        int sign;
        switch (code[pos]) {
        case '\t': sign = -1; ++pos; break;
        case ' ' : sign =  1; ++pos; break;
        default:
            throw "Expected a number. Numbers must start with tab or space";
        }

        int num = 0;
        while (code[pos] != '\n') {
            if (code[pos] == '\0') throw "Unexpected end of code";
            num *= 2;
            num += (code[pos] == '\t'); // ch is either tab -> 1, or space -> 0
            ++pos;
        }
        ++pos; // advance past the newLine
        return sign * num;
    }

    int makeLabel() {
        size_t num = 0;
        while (code[pos] != '\n') {
            if (code[pos] == '\0') throw "Unexpected end of code";
            num *= 2;
            num += (code[pos] == '\t'); // ch is either tab -> 1, or space -> 0
            ++pos;
        }
        ++pos; // advance past the newLine

        return num;
    }

public:
    Parser(const string& code_) : code(code_), pos(0) {}

    vector<Token> parse() {
        vector<Token> res;
        while (code[pos] != '\0') {
            switch (code[pos]) {
            case ' ':
                // Stack manipulation
                ++pos;
                switch (code[pos]) {
                case ' ':
                    ++pos;
                    res.push_back(Token{ stack_push, makeNumber() });
                    break;
                case '\t':
                    ++pos;
                    switch (code[pos]) {
                    case ' ':
                        ++pos;
                        res.push_back(Token{ stack_duplicate, makeNumber() });
                        break;
                    case '\n':
                        ++pos;
                        res.push_back(Token{ stack_discard, makeNumber() });
                        break;
                    default:
                        throw "unexpected character";
                    }
                case '\n':
                    ++pos;
                    switch (code[pos]) {
                    case ' ':
                        res.push_back(Token{ stack_duplicateTop, 0 });
                        ++pos; break;
                    case '\t':
                        res.push_back(Token{ stack_swapTop, 0 });
                        ++pos; break;
                    case '\n':
                        res.push_back(Token{ stack_discardTop, 0 });
                        ++pos; break;
                    default:
                        throw "unexpected character";
                    }
                default:
                    throw "unexpected character";
                }
                break;
            case '\t':
                ++pos;
                switch (code[pos]) {
                case ' ':
                    // Arithmetic
                    ++pos;
                    switch (code[pos]) {
                    case ' ':
                        ++pos;
                        switch (code[pos]) {
                        case ' ':
                            res.push_back(Token{ arith_add, 0 });
                            ++pos; break;
                        case '\t':
                            res.push_back(Token{ arith_sub, 0 });
                            ++pos; break;
                        case '\n':
                            res.push_back(Token{ arith_mult, 0 });
                            ++pos; break;
                        default:
                            throw "unexpected character";
                        }
                        break;
                    case '\t':
                        ++pos;
                        switch (code[pos]) {
                        case ' ':
                            res.push_back(Token{ arith_div, 0 });
                            ++pos; break;
                        case '\t':
                            res.push_back(Token{ arith_mod, 0 });
                            ++pos; break;
                        default:
                            throw "unexpected character";
                        }
                        break;
                    default:
                        throw "unexpected character";
                    }
                    break;
                case '\t':
                    // Heap Access
                    ++pos;
                    switch (code[pos]) {
                    case ' ':
                        res.push_back(Token{ heap_set, 0 });
                        ++pos; break;
                    case '\t':
                        res.push_back(Token{ heap_get, 0 });
                        ++pos; break;
                    default:
                        throw "unexpected character";
                    }
                    break;
                case '\n':
                    // Input/Output
                    ++pos;
                    switch (code[pos]) {
                    case ' ':
                        ++pos;
                        switch (code[pos]) {
                        case ' ':
                            res.push_back(Token{ output_char, 0 });
                            ++pos; break;
                        case '\t':
                            res.push_back(Token{ output_num, 0 });
                            ++pos; break;
                        default:
                            throw "unexpected character";
                        }
                        break;
                    case '\t':
                        ++pos;
                        switch (code[pos]) {
                        case ' ':
                            res.push_back(Token{ input_char, 0 });
                            ++pos; break;
                        case '\t':
                            res.push_back(Token{ input_num, 0 });
                            ++pos; break;
                        default:
                            throw "unexpected character";
                        }
                        break;
                    default:
                        throw "unexpected character";
                    }
                    break;
                default:
                    throw "unexpected character";
                }
                break;
            case '\n':
                // Flow-Control
                ++pos;
                switch (code[pos]) {
                case ' ':
                    ++pos;
                    switch (code[pos]) {
                    case ' ':
                        ++pos;
                    {
                        size_t label = makeLabel();
                        if (labels.find(label) != labels.end()) throw "Label already exists"; // check if already exists
                        labels[label] = res.size();
                    }
                        break;
                    case '\t':
                        ++pos;
                        res.push_back(Token{ control_call, makeLabel() });
                        break;
                    case '\n':
                        ++pos;
                        res.push_back(Token{ control_jump, makeLabel() });
                        break;
                    default:
                        throw "unexpected character";
                    }
                    break;
                case '\t':
                    ++pos;
                    switch (code[pos]) {
                    case ' ':
                        ++pos;
                        res.push_back(Token{ control_jumpIfZero, makeLabel() });
                        break;
                    case '\t':
                        ++pos;
                        res.push_back(Token{ control_jumpIfNegative, makeLabel() });
                        break;
                    case '\n':
                        res.push_back(Token{ control_return, 0 });
                        ++pos; break;
                    default:
                        throw "unexpected character";
                    }
                    break;
                case '\n':
                    ++pos;
                    switch (code[pos]) {
                    case '\n':
                        res.push_back(Token{ control_exit, 0 });
                        ++pos; break;
                    default:
                        throw "unexpected character";
                    }
                    break;
                default:
                    throw "unexpected character";
                }
                break;
            default:
                throw "unexpected character";
            }

            if (pos >= code.size() - 1) break;

        }

        // Label postprocessing
        for (Token& tok : res) {
            if (tok.function == control_call ||
                tok.function == control_jump ||
                tok.function == control_jumpIfZero ||
                tok.function == control_jumpIfNegative) {
                map<size_t, size_t>::iterator it = labels.find(tok.value);
                if (it == labels.end()) throw "No such label exists";
                tok.value = it->second;
            }
        }

        // should not reach, because programs should end with control_exit
        res.push_back(Token{ unexpected_endOfProgram, 0 });
        return res;
    }
};

// Solution
string whitespace(const string& code, const string& inp = string()) {


    mystack.clear();
    heap.clear();
    callStack.clear();
    labels.clear();
    output = "";
    pc = 0;
    myReader.reset();

    // strip everything but the 3 whitespaces
    string code2 = "";
    for (const char ch : code) {
        if (ch == ' ' || ch == '\t' || ch == '\n') {
            code2 += ch;
        }
    }

    Parser myparser(code2);
    vector<Token> tokens = myparser.parse();

    input = inp;

    // Interpreter
    for (;;) {
        Token tok = tokens[pc];
        void(*funcPtr)() = tok.function;
        if (funcPtr == control_exit) { // it needs to be in every correct program, if there is none the loop will break, beacuse the unexpected_endOfProgram function would be called resulting in throwing an error
            return output;
        }
        arg = tok.value;
        funcPtr();
    }

    throw "Programcode ended without control_exit";
}

int main() {
    // Tests

    // test Reader

    //tes1
    /*input = "asd";
    cout << myReader.readChar();
    cout << myReader.readChar();
    cout << myReader.readChar();
    cout << myReader.readChar();*/

    //test2
    /*input = "12\nads0xAB1\n32";
    cout << myReader.readNum();
    //cout << myReader.readNum();
    cout << myReader.readChar();
    cout << myReader.readChar();
    cout << myReader.readChar();
    //cout << myReader.readChar();
    cout << myReader.readNum();
    //cout << myReader.readNum();
    cout << myReader.readChar();
    cout << myReader.readChar();*/

    //test3
    /*input = "0x77";
    //cout << myReader.readNum();*/

    // test makeNumber
    //cout << whitespace("push  num \t\t\t \noutputNum\t\n \tend\n\n\n");
    //cout << whitespace("push  num\t\t \t \noutputNum\t\n \tend\n\n\n");
    cout << whitespace("   \t\n\t\n \t\n\n\n");
    cout << whitespace("   \t \n\t\n \t\n\n\n");
    cout << whitespace("   \t\t\n\t\n \t\n\n\n");
    cout << whitespace("    \n\t\n \t\n\n\n");

    // test makeLabel
    //cout << whitespace("push  num   \npush  num\t\t\t\njump\n \nlabel \t\t\noutputNum\t\n \tMarkWithLabel\n  label \t\t\noutputNum\t\n \tend\n\n\n");



    //cout << whitespace("   \t      \t \n   \t\t\t \t\t   \t\t  \t \n    \n\t\t    \t  \t   \n\t\n     \t\t  \t \t\n\t\n     \t\t \t\t  \n \n \t\n  \t\n     \t\t \t\t\t\t\n\t\n     \t     \n\t\n     \t \t \t\t\t\n\t\n     \t\t \t\t\t\t\n\t\n     \t\t\t  \t \n\t\n     \t\t \t\t  \n\t\n     \t\t  \t  \n\t\n     \t    \t\n\t\n     \t \t \n\t\n   !", "");

    return 0;
}
