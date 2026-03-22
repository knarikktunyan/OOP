#include <iostream>
#include <stack>
#include <string>

int getPrecedence(char op) {
    switch(op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        default:  return 0;
    }
}

int runPostfix(std::string &postfixExpr) {
    std::stack<int> operands;
    int length = postfixExpr.size();

    for(int i = 0; i < length; ++i) {
        if (isdigit(postfixExpr[i])) {
            operands.push(postfixExpr[i] - '0');
        }
        else {
            int val2 = operands.top();
            operands.pop();
            int val1 = operands.top();
            operands.pop();

            switch(postfixExpr[i]) {
                case '+': operands.push(val1 + val2); break;
                case '-': operands.push(val1 - val2); break;
                case '*': operands.push(val1 * val2); break;
                case '/': operands.push(val1 / val2); break;
            }
        }
    }
    return operands.top();
}

void convertToPostfix(std::string &infixIn, std::string &postfixOut) {
    std::stack<char> opStack;
    int limit = infixIn.size();

    for(int i = 0; i < limit; ++i) {
        char current = infixIn[i];

        if(current == '(') {
            opStack.push(current);
        }
        else if (isdigit(current)) {
            postfixOut.push_back(current);
        }
        else if (current == ')') {
            while(!opStack.empty() && opStack.top() != '(') {
                char topOp = opStack.top();
                opStack.pop();
                postfixOut.push_back(topOp);
            }
            if(!opStack.empty()) opStack.pop();
        }
        else {
            if (opStack.empty()) {
                opStack.push(current);
            } else {
                int p_current = getPrecedence(current);
                while(!opStack.empty() && opStack.top() != '(' && 
                      p_current <= getPrecedence(opStack.top())) {
                    postfixOut.push_back(opStack.top());
                    opStack.pop();
                }
                opStack.push(current);
            }
        }
    }
    
    while(!opStack.empty()) {
        postfixOut.push_back(opStack.top());
        opStack.pop();
    }
}

int main() {
    std::string rawInput;
    std::string processedOutput = "";
    
    std::cout << "Enter expression: ";
    std::cin >> rawInput;
    
    convertToPostfix(rawInput, processedOutput);
    std::cout << "Result: " << runPostfix(processedOutput) << "\n";
    
    return 0;
}