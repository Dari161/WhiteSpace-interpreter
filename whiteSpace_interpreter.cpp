
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

using my_size_t = int;

int main() {
    return 0;
}

/*6. Undefined Behavior in Labels and Flow Control
Problem: Label definitions (makeLabel) and jump instructions assume labels are pre-defined in labels but this is not guaranteed.
Fix: Process the labels during parsing and store them before evaluating instructions.*/

// Memory alloc errors are handled by the map and vector class themselves

map<int, int> heap;
vector<int> mystack;

vector<my_size_t> callStack;
map<my_size_t, my_size_t> labels;

string output = "";

int pop() {
    int res = mystack.back();
    mystack.pop_back();
    return res;
}

my_size_t pc = 0; // program counter

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

class Reader {
public:
    Reader() {}

    void setInput(string* input) {
        inp = input;
        advance();
    }

    int readNum() {
        if (current_char == '\0') throw runtime_error("Runtime error: Input was empty when trieying to read number from it");
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
        if (current_char == '\0') throw runtime_error("Runtime error: input was empty when trying to read a character");
        char ret = current_char;
        advance();
        return ret;
    }
private:
    string* inp;
    int pos = -1;
    char current_char;

    bool advance() {
        ++pos;
        if (pos >= inp->length()) return true;
        current_char = (*inp)[pos];
        return false;
    }

    void eat() {
        if (advance()) throw runtime_error("Unexpected end of file");
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
            throw runtime_error("Invalid hexadecimal character");
        }
    }
};

Reader myReader;

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
    mystack.push_back(num);
    ++pc;
}

void stack_duplicate(int num) {
    if (num >= mystack.size()) throw runtime_error("Runtime error: range out of bounds");
    mystack.push_back(mystack[mystack.size() - num - 1]); //////IDK
    ++pc;
}

void stack_discard(int num) {
    if (num < 0 || num >= mystack.size()) { // remove everything but the top value
        if (mystack.size() < 0) throw runtime_error("Runtime error : stack is empty");
        int top = pop();
        mystack.clear();
        mystack.push_back(top);
        return;
    }
    int top = pop();
    // Resize the vector to remove the last n elements
    mystack.resize(mystack.size() - num);
    /*for (int i = 0; i < num; ++i) {
        mystack.pop_back();
    }*/
    mystack.push_back(top);
    ++pc;
}

void stack_duplicateTop(int) {
    mystack.push_back(mystack[mystack.size() - 1]);
    ++pc;
}

void stack_swapTop(int) {
    if (mystack.size() < 2) throw runtime_error("Runtime error: must have 2 values on stack to swap them");
    int tmp = mystack[mystack.size() - 1];
    mystack[mystack.size() - 1] = mystack[mystack.size() - 2];
    mystack[mystack.size() - 2] = tmp;
    ++pc;
}

void stack_discardTop(int) {
    if (mystack.size() < 1) throw runtime_error("Runtime error: cant pop empty stack");
    ++pc;
}

// Arithmetic
void arith_add(int) {
    if (mystack.size() < 2) throw runtime_error("Runtime error: must have 2 values on stack to add them");
    mystack.push_back(pop() + pop());
    ++pc;
}

void arith_sub(int) {
    if (mystack.size() < 2) throw runtime_error("Runtime error: must have 2 values on stack to subtract them");
    mystack.push_back(-pop() + pop());
    ++pc;
}

void arith_mult(int) {
    if (mystack.size() < 2) throw runtime_error("Runtime error: must have 2 values on stack to multiply them");
    mystack.push_back(pop() * pop());
    ++pc;
}

void arith_div(int) {
    if (mystack.size() < 2) throw runtime_error("Runtime error: must have 2 values on stack to divide them");
    int a = pop();
    int b = pop();
    if (a == 0) throw runtime_error("Runtime error: division by 0");
    mystack.push_back(b / a); // floor of the division
    ++pc;
}

void arith_mod(int) {
    if (mystack.size() < 2) throw runtime_error("Runtime error: must have 2 values on stack to modulo them");
    int a = pop();
    int b = pop();
    int sign = a < 0 ? -1 : 1;
    if (a == 0) throw runtime_error("Runtime error: modulo by 0");
    mystack.push_back((abs(b) % a) * sign);
    ++pc;
}

// Heap access
void heap_set(int) {
    if (mystack.size() < 2) throw runtime_error("Runtime error: must have 2 values on stack to ....... them");
    int a = pop();
    heap[pop()] = a;
    // in heap[pop()] = pop() the order of function evaluation would depend on the complier, since it is unspecified
    ++pc;
}

void heap_get(int) {
    if (mystack.size() < 1) throw runtime_error("Runtime error: must have a values on stack to .......");
    map<int, int>::iterator it = heap.find(pop());
    if (it == heap.end()) throw runtime_error("Runtime error: No such adress in heap exists");
    mystack.push_back(it->second); // it->second is the value
    ++pc;
}

// Input/Output
void output_char(int) {
    if (mystack.size() < 1) throw runtime_error("Runtime error: must have a values on stack to .......");
    //output as char
    output += static_cast<char>(pop() % 256); // maybe % 256 is not needed
    ++pc;
}

void output_num(int) {
    if (mystack.size() < 1) throw runtime_error("Runtime error: must have a values on stack to .......");
    //output as num
    output += intToStr(pop());
    ++pc;
}

void input_char(int) {
    if (mystack.size() < 1) throw runtime_error("Runtime error: must have a values on stack to .......");
    heap[pop()] = myReader.readChar();
    ++pc;
}

void input_num(int) {
    if (mystack.size() < 1) throw runtime_error("Runtime error: must have a values on stack to .......");
    heap[pop()] = myReader.readNum();
    ++pc;
}

// Flow Control
void control_call(int label) {
    callStack.push_back(pc);
    pc = label;
}

void control_jump(int label) {
    pc = label;
}

void control_jumpIfZero(int label) {
    if (mystack.size() < 1) throw runtime_error("Runtime error: must have a values on stack to .......");
    if (pop() == 0) {
        pc = label;
    } else {
        ++pc;
    }
}

void control_jumpIfNegative(int label) {
    if (mystack.size() < 1) throw runtime_error("Runtime error: must have a values on stack to .......");
    if (pop() < 0) {
        pc = label;
    } else {
        ++pc;
    }
}

void control_return(int label) {
    if (callStack.size() < 1) throw runtime_error("Runtime error: Cannot return, because callstack is empty");
    pc = callStack.back() + 1; // +1 because it should not call the subroutine again but progress forward
    callStack.pop_back();
}

void control_exit(int) {} // end program

void unexpected_endOfProgram(int) {
    throw runtime_error("Programcode ended unexpectedly (without end of program statement)");
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
        if (advance()) throw runtime_error("Unexpected end of file");
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
            throw runtime_error("Expected a number. Numbers must start with tab or space");
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

    my_size_t makeLabel() {
        my_size_t num = 0;
        while (current_char != '\n') {
            eat();

            num *= 2;
            num += (current_char == '\t'); // ch is either tab -> 1, or space -> 0
        }
        advance(); // advane past the newLine

        return num;
    }

public:
    Parser(const string& code) : code(code) {
        advance();
    }

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
                        my_size_t label = makeLabel();
                        if (labels.find(label) != labels.end()) throw runtime_error("Label already exists"); // check if already exists
                        labels[label] = res.size();
                    }
                        break;
                    case '\t':
                    {
                        eat();
                        res.push_back(Token(control_call, makeLabel()));
                    }
                        break;
                    case '\n':
                    {
                        eat();
                        res.push_back(Token(control_jump, makeLabel()));
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
                        res.push_back(Token(control_jumpIfZero, makeLabel()));
                    }
                        break;
                    case '\t':
                    {
                        eat();
                        res.push_back(Token(control_jumpIfNegative, makeLabel()));
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

        // Label postprocessing
        for (Token& tok : res) {
            if (tok.function == control_call ||
                tok.function == control_jump ||
                tok.function == control_jumpIfZero ||
                tok.function == control_jumpIfNegative) {
                map<my_size_t, my_size_t>::iterator it = labels.find(tok.value);
                if (it == labels.end()) throw runtime_error("No such label exists");
                tok.value = it->second;
            }
        }

        // should not reach, because programs should end with control_exit
        res.push_back(Token(unexpected_endOfProgram, 0));
        return res;
    }
};

// Solution
string whitespace(const string& code, string& inp) {

    Parser myparser(code);
    vector<Token> tokens = myparser.parse();

    if (tokens.size() == 0) throw runtime_error("Tokens are empty, meaning there was no end of program statement");

    myReader.setInput(&inp);

    // Interpreter
    for (;;) {
        Token tok = tokens[pc];
        void(*funcPtr)(int) = tok.function;
        if (funcPtr == control_exit) { // it needs to be in every correct program, if there is none the loop will break, beacuse the unexpected_endOfProgram function would be called resulting in throwing an error
            return output;
        }
        funcPtr(tok.value);
    }

    throw runtime_error("Programcode ended without control_exit");
}
