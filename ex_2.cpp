#include <iostream>
#include <sstream>
#include <stack>
#include <cctype>

using namespace std;

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
    if (op == '+')
        return left + right;

    if (op == '-')
        return left - right;

    if (op == '*')
        return left * right;

    if (op == '/')
        return left / right;

    return 0;
}

int evaluateExpression(string expression)
{
    stack<int> numbers;
    stack<char> operators;

    stringstream input(expression);
    char ch;

    while (input >> ch)
    {
        if (isdigit(ch))
        {
            input.putback(ch);

            int number;
            input >> number;
            numbers.push(number);
        }
        else if (ch == '(')
        {
            operators.push(ch);
        }
        else if (ch == ')')
        {
            while (operators.top() != '(')
            {
                int right = numbers.top();
                numbers.pop();

                int left = numbers.top();
                numbers.pop();

                numbers.push(applyOperation(left, right, operators.top()));
                operators.pop();
            }

            operators.pop();
        }
        else
        {
            while (!operators.empty() &&
                   precedence(operators.top()) >= precedence(ch))
            {
                int right = numbers.top();
                numbers.pop();

                int left = numbers.top();
                numbers.pop();

                numbers.push(applyOperation(left, right, operators.top()));
                operators.pop();
            }

            operators.push(ch);
        }
    }

    while (!operators.empty())
    {
        int right = numbers.top();
        numbers.pop();

        int left = numbers.top();
        numbers.pop();

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

    cout << "Result: " << evaluateExpression(expression) << endl;
}