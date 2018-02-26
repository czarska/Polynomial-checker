//
//  main.cpp
//  Parser
//
//  Created by Gabrysia Czarska on 14.06.2017.
//  Copyright © 2017 Gabrysia Czarska. All rights reserved.
//

#include <iostream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace std;
struct Node {
    enum class Type { CONS, VAR, SUM, TIMES, POW };
    Type type;
    Node * left;
    Node * right;
    unsigned long long int number;
    Node(Type type, Node *left = nullptr, Node *right = nullptr, unsigned long long int number = 0) : type(type), left(left), right(right), number(number) {}
    ~Node() {
        if(left != nullptr) delete left;
        if(right != nullptr) delete right;
        
    }
    
};
class PParser {
public:
    Node* root = nullptr;
    PParser(char * input) : input(input) {}
    Node* Parse() {
        root = ParseE();
        except('\0');
        return root;
    }
    unsigned long long int exponentiation (unsigned long long int b, unsigned long long int s, unsigned long long int p ) {
        __int128 w = 1;
        __int128 m = b;
        for (unsigned long long int j=s; j>0; j>>=1) {
            if(j%2 == 1)
                w = (w * m) % p;
            m = (m * m) % p;
        }
        return w;
    }
    pair<unsigned long long int, unsigned long long int> evaluate(Node *n, unsigned long long int value, unsigned long long int mod) {
        pair<unsigned long long int, unsigned long long int> p;
        pair<unsigned long long int, unsigned long long int> c1;
        pair<unsigned long long int, unsigned long long int> c2;
        switch (n->type) {
            case Node::Type::CONS:
                p.first = (n->number);
                p.second = 0;
                break;
            case Node::Type::VAR:
                p.first = value%mod;
                p.second = 1;
                break;
            case Node::Type::SUM:
                c1 = evaluate(n->left, value, mod);
                c2 = evaluate(n->right, value, mod);
                p.first = ((c1.first%mod)+(c2.first%mod))%mod;
                p.second = ((c1.second%mod)+(c2.second%mod))%mod;
                break;
            case Node::Type::TIMES:
                c1 = evaluate(n->left, value, mod);
                c2 = evaluate(n->right, value, mod);
                p.first = ((c1.first%mod)*(c2.first%mod))%mod;
                p.second = (((c1.first)%mod*(c2.second)%mod)%mod + ((c1.second)%mod*(c2.first)%mod)%mod)%mod;
                break;
            case Node::Type::POW:
                c1 = evaluate(n->left, value, mod);
                c2 = evaluate(n->right, value, mod);
                p.first = exponentiation(c1.first%mod, c2.first, mod)%mod;
                p.second = ( ((c1.second%mod)*(c2.first%mod))%mod * exponentiation(c1.first%mod, c2.first-1, mod)%mod )%mod;
                break;
        }
        return p;
    }
    
private:
    bool debug = 0;
    char * input;
    bool accept(char c) {
        while (*input == ' ') ++input;
        if (*input == c) {
            ++input;
            return true;
        }
        return false;
    }
    void except(char c) {
        while (*input == ' ') ++input;
        if (*input == c) {
            ++input;
        } else {
            throw "Syntax error";
        }
    }
    Node* ParseC() {
        if(debug) cout<<"parseC input:"<<*input<<endl;
        while(*input == ' ') input++;
        unsigned long long int counter = 0;
        char *number = input;
        while (*input > 47 && *input < 58) {
            counter++;
            input++;
        }
        if (counter == 0) {
            throw "Syntax error";
        }
        unsigned long long int nr = stoul(number);
        return new Node(Node::Type::CONS, nullptr, nullptr, nr);
    }
    Node* ParseE() {
        if(debug) cout<<"parseE input:"<<*input<<endl;
        Node* n = ParseT();
        if (accept('+')) {
            try {
                Node* m = ParseE();
                n = new Node(Node::Type::SUM, n, m);
            } catch(...) {
                delete n;
                throw "Syntax error";
            }
        }
        return n;
    }
    Node* ParseT() {
        if(debug) cout<<"parseT input:"<<*input<<endl;
        Node* n = ParseP();
        char *temp = input;
        try {
            Node* m = ParseT();
            n = new Node(Node::Type::TIMES, n, m);
        }
        catch (...) {
            input = temp;
        }
        return n;
    }
    Node* ParseP() {
        if(debug) cout<<"parseP input:"<<*input<<endl;
        Node* n = ParseF();
        if (accept('^')) {
            try{
                Node* m = ParseC();
                n = new Node(Node::Type::POW, n, m);
            } catch(...) {
                delete n;
                throw "Syntax error";
            }
        }
        return n;
    }
    Node* ParseF() {
        if(debug) cout<<"parseF input:"<<*input<<endl;
        if (accept('(')) {
            Node* n = ParseE();
            try{ except(')'); }
            catch(...) {
                delete n;
                throw "Syntax error";
            }
            return n;
        }
        if (accept('x')) {
            return new Node(Node::Type::VAR);
        }
        Node* n = ParseC();
        return n;
        
    }
};
int main() {
    unsigned long long int z;
    string s, ss;
    cin>>z;
    getline(cin, s);
    for (unsigned long long int i=0; i<z; i++) {
        getline(cin, s);
        char * c = new char[s.length()+1];
        strcpy(c, s.c_str());
        PParser *p = new PParser(c);
        try {
            Node* n = (*p).Parse();
            unsigned long long int k;
            cin>>k;
            for (unsigned long long int j=0; j<k; j++) {
                unsigned long long int a, m;
                cin>>a>>m;
                pair<unsigned long long int, unsigned long long int> c = (*p).evaluate(n, a%m, m);
                cout<<c.second%m<<" ";
            }
            delete n;
            cout<<endl;
            getline(cin, ss);
        }
        catch(...) {
            if ((*p).root != nullptr) delete (*p).root;
            getline(cin, ss);
            cout<<"SYNTAX ERROR"<<endl;
        }
        delete[]c;
        delete p;
    }
    return 0;
}
