#include <iostream>
#include <vector>
#include <sstream>
#include <stack>
#include <cctype>

using namespace std;

enum TokenType
{
    NUMBER,
    OPERATOR,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    END_TOKEN
};

struct Token
{
    string value;
    TokenType type;
};

vector<string> lexer(string expression)
{
    stringstream input(expression);
    vector<string> words;
    char ch;

    while (input.get(ch))
    {
        if (isspace(ch))
            continue;

        if (isdigit(ch))
        {
            string number;
            number += ch;

            while (isdigit(input.peek()))
                number += input.get();

            words.push_back(number);
        }
        else if (string("+-*/()").find(ch) != string::npos)
        {
            words.push_back(string(1, ch));
        }
        else
        {
            throw runtime_error("Invalid input");
        }
    }

    return words;
}

vector<Token> tokenizer(vector<string> words)
{
    vector<Token> tokens;

    for (auto &word : words)
    {
        if (isdigit(word[0]))
            tokens.push_back({word, NUMBER});

        else if (word == "(")
            tokens.push_back({word, OPEN_BRACKET});

        else if (word == ")")
            tokens.push_back({word, CLOSE_BRACKET});

        else
            tokens.push_back({word, OPERATOR});
    }

    tokens.push_back({"", END_TOKEN});

    return tokens;
}

int precedence(char op)
{
    if (op == '+' || op == '-')
        return 1;

    if (op == '*' || op == '/')
        return 2;

    return 0;
}

int applyOperation(int left, int right, char op)
{
    if (op == '+') return left + right;
    if (op == '-') return left - right;
    if (op == '*') return left * right;
    if (op == '/') return left / right;

    return 0;
}

int parser(vector<Token> tokens)
{
    stack<int> numbers;
    stack<char> operators;

    for (auto &token : tokens)
    {
        if (token.type == NUMBER)
        {
            numbers.push(stoi(token.value));
        }

        else if (token.type == OPEN_BRACKET)
        {
            operators.push('(');
        }

        else if (token.type == CLOSE_BRACKET)
        {
            while (operators.top() != '(')
            {
                int right = numbers.top(); numbers.pop();
                int left = numbers.top(); numbers.pop();

                numbers.push(applyOperation(left, right, operators.top()));
                operators.pop();
            }

            operators.pop();
        }

        else if (token.type == OPERATOR)
        {
            char op = token.value[0];

            while (!operators.empty() &&
                   precedence(operators.top()) >= precedence(op))
            {
                int right = numbers.top(); numbers.pop();
                int left = numbers.top(); numbers.pop();

                numbers.push(applyOperation(left, right, operators.top()));
                operators.pop();
            }

            operators.push(op);
        }
    }

    while (!operators.empty())
    {
        int right = numbers.top(); numbers.pop();
        int left = numbers.top(); numbers.pop();

        numbers.push(applyOperation(left, right, operators.top()));
        operators.pop();
    }

    return numbers.top();
}

int main()
{
    string expression;

    cout << "Enter expression: ";
    getline(cin, expression);

    vector<string> words = lexer(expression);
    vector<Token> tokens = tokenizer(words);

    int result = parser(tokens);

    cout << "Result: " << result << endl;
}