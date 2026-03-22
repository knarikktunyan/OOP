#include <iostream>
#include <stack>
#include <string>
#include <cctype>

int check_priority(char operand) {
    switch(operand) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        default:  return 0;
    }
}

int evaluate(std::string &text) {
    std::stack<int> st;

    for(int i = 0; i < text.size(); ++i) {
        if (isdigit(text[i])) {
            st.push(text[i] - '0');
        } 
        else {
            // Check if stack has at least 2 elements to prevent Seg Fault
            if (st.size() < 2) continue; 

            int right = st.top(); st.pop(); 
            int left = st.top(); st.pop(); 
            switch(text[i]) {
                case '+': st.push(left + right); break;
                case '-': st.push(left - right); break;
                case '*': st.push(left * right); break;
                case '/': 
                    if (right == 0) return 0; 
                    st.push(left / right); 
                    break;
            }
        }
    }
    return st.empty() ? 0 : st.top();
}

void postfix_eval(std::string &math_exp, std::string &text) {
    std::stack<char> st;
    
    for(int i = 0; i < math_exp.size(); ++i) {
        char c = math_exp[i];

        if (isdigit(c)) {
            text.push_back(c);
        }
        else if (c == '(') {
            st.push(c);
        }
        else if (c == ')') {
            while(!st.empty() && st.top() != '(') {
                text.push_back(st.top());
                st.pop();
            }
            if (!st.empty()) st.pop(); // Remove '('
        }
        else {
            int current_prior = check_priority(c);
            while(!st.empty() && st.top() != '(' && 
                  check_priority(st.top()) >= current_prior) {
                text.push_back(st.top());
                st.pop();
            }
            st.push(c);
        }
    }

    while(!st.empty()) {
        if (st.top() != '(') text.push_back(st.top());
        st.pop();
    }
}

int main() {
    std::string math_exp;
    std::string text = "";
    
    std::cout << "Input the expression ";
    std::cin >> math_exp;
    
    postfix_eval(math_exp, text);
    std::cout << "Postfix notation: " << text << "\n";
    std::cout << "The result is: " << evaluate(text) << "\n";

    return 0;
}
