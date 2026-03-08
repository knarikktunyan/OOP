#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

enum Type
{
    ADD,
    SUB,
    MUL,
    DIV,
    NUM
};

struct Node
{
    int val;
    Type type;
    Node* left;
    Node* right;

    Node(int v) : val(v), type(NUM), left(nullptr), right(nullptr) {}
    Node(Type t, Node* l, Node* r) : val(0), type(t), left(l), right(r) {}
};

void freeTree(Node* n)
{
    if(!n) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

bool match(char*& p,char c)
{
    if(*p==c)
    {
        p++;
        return true;
    }
    return false;
}

Node* parseExpr(char*& p);

Node* parseFactor(char*& p)
{
    if(isdigit(*p))
    {
        int num=0;
        while(isdigit(*p))
        {
            num=num*10+(*p-'0');
            p++;
        }
        return new Node(num);
    }

    if(match(p,'('))
    {
        Node* n=parseExpr(p);
        if(!match(p,')'))
            throw runtime_error("missing )");
        return n;
    }

    throw runtime_error("wrong input");
}

Node* parseTerm(char*& p)
{
    Node* left=parseFactor(p);

    while(true)
    {
        if(match(p,'*'))
        {
            Node* right=parseFactor(p);
            left=new Node(MUL,left,right);
        }
        else if(match(p,'/'))
        {
            Node* right=parseFactor(p);
            left=new Node(DIV,left,right);
        }
        else break;
    }

    return left;
}

Node* parseExpr(char*& p)
{
    Node* left=parseTerm(p);

    while(true)
    {
        if(match(p,'+'))
        {
            Node* right=parseTerm(p);
            left=new Node(ADD,left,right);
        }
        else if(match(p,'-'))
        {
            Node* right=parseTerm(p);
            left=new Node(SUB,left,right);
        }
        else break;
    }

    return left;
}

int calc(Node* n)
{
    switch(n->type)
    {
        case NUM: return n->val;
        case ADD: return calc(n->left)+calc(n->right);
        case SUB: return calc(n->left)-calc(n->right);
        case MUL: return calc(n->left)*calc(n->right);
        case DIV: return calc(n->left)/calc(n->right);
    }
    return 0;
}

int main()
{
    string input;

    cout<<"Enter expression: ";
    getline(cin,input);

    char* p=&input[0];

    Node* root=parseExpr(p);

    cout<<"Result: "<<calc(root)<<endl;

    freeTree(root);
}