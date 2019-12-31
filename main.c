#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#define _ADD(a, b) createNode(a, b, plus)
#define _SUB(a, b) createNode(a, b, minus)
#define _MUL(a, b) createNode(a, b, mul)
#define _DIV(a, b) createNode(a, b, division)

enum {
    bracket = 1,
    symbol = 2,
    number = 3,
    plus = 4,
    minus = 5,
    mul = 6,
    division = 7,
    beginning = 8,
    nothing = 9,
    power = 10
};

typedef int elem_t;

struct Node {

    elem_t data;
    int type;
    struct Node* left;
    struct Node* right;

};



int printBracket(struct Node* from, struct Node* where) {

    if (((from->type == mul || from->type == division) && (where->type == plus || where->type == minus))
    || (from->type == bracket)) {
        return 1;
    } else {
        return 0;
    }

}


int TreeDecrypter(struct Node* tree, FILE* outfile) {

    int bracket_set = 0;
    int frac_set = 0;

    if (tree->type == division) {
        fprintf(outfile, "\\frac{");
        TreeDecrypter(tree->left, outfile);
        fprintf(outfile, "}");
        fprintf(outfile, "{");
        TreeDecrypter(tree->right, outfile);
        fprintf(outfile, "}");
        frac_set = 1;
    }

    if (frac_set == 1) {
        frac_set = 0;
        return 0;
    }


    if (tree->left != NULL) {

        if (printBracket(tree, tree->left) == 1) {
            fprintf(outfile, "(");
            bracket_set = 1;
         }

        TreeDecrypter(tree->left, outfile);

    }

    if (bracket_set == 1) {
        fprintf(outfile, ")");
        bracket_set = 0;
    }

    switch (tree->type) {
        case division:
            fprintf(outfile, "\\frac{");
            TreeDecrypter(tree->left, outfile);
            fprintf(outfile, "}");
            fprintf(outfile, "{");
            TreeDecrypter(tree->right, outfile);
            fprintf(outfile, "}");
            frac_set = 1;
            break;
        case number:
            fprintf(outfile, "%d", tree->data);
            break;
        case symbol:
            fprintf(outfile, "%c", tree->data + 16);
            break;
        case mul:
            fprintf(outfile, "\\cdot ");
            break;
        case plus:
            fprintf(outfile, "+");
            break;
        case minus:
            fprintf(outfile, "-");
            break;
    }

    struct Node* current = NULL;

    if (frac_set == 1) {
        frac_set = 0;
        return 0;
    }

    if (tree->right != NULL) {

        if (printBracket(tree, tree->right) == 1) {
            fprintf(outfile, "(");
            bracket_set = 1;
        }

        TreeDecrypter(tree->right, outfile);
    }


    if (bracket_set == 1) {
        fprintf(outfile, ")");
        bracket_set = 0;
    }

    return 0;
}



void printTree(struct Node* tree, FILE* outfile) {

    if (tree->left != NULL) {
        printTree(tree->left, outfile);
    }

    switch(tree->type) {
        case number:
            fprintf(outfile, "%d", tree->data);
            break;
        case mul:
            if (tree->left->type != number) {
                fprintf(outfile, "\\cdot( ");
            }
            break;
        case division:
            fprintf(outfile, "\\frac {%d}{%d}", (tree->left)->data, (tree->right)->data);
        default: fprintf(outfile, "%d", tree->data);
    }

    if (tree->right != NULL) {
        printTree(tree->right, outfile);
    }

}

void printLaTeX(struct Node* tree, char* output) {

    FILE* outfile = fopen(output, "wb");
    assert(outfile);

    fprintf(outfile, "\\documentclass{article}\n"
                 "\n"
                 "\\title{formula}\n"
                 "\\author{Stasyan}\n"
                 "\n"
                 "\\begin{document}\n"
                 "\\maketitle\n"
                 "\n"
                 "$");

    TreeDecrypter(tree, outfile);

    fprintf(outfile, "$\n"
                 "\n"
                 "\\end{document}");
    assert(outfile);
    fclose(outfile);

}


struct Node* createNodeN(int val) {

    struct Node* node = (struct Node*) calloc(1, sizeof(struct Node));

    node->data = val;
    node->left = NULL;
    node->right = NULL;

    if (isalpha(node->data)) {
        node->type = symbol;
    } else {
        node->type = number;
    }

    return node;
}

struct Node* createNodeG(struct Node* val) {

    struct Node* node = (struct Node*) calloc(1, sizeof(struct Node));

    node->data = -1;
    node->left = val;
    node->right = NULL;
    node->type = beginning;

    return node;
}

struct Node* createNodeP(struct Node* val) {

    struct Node* node = (struct Node*) calloc(1, sizeof(struct Node));

    node->data = -1;
    node->left = val;
    node->right = NULL;
    node->type = bracket;

    return node;
}

struct Node* createNode(struct Node* val, struct Node* val2, int retval) {

    struct Node* node = (struct Node*) calloc(1, sizeof(struct Node));

    if (retval == mul) {
       // node->data = '*';
        node->type = mul;
    }
    else if (retval == minus) {
        node->type = minus;
    }
    else if (retval == plus) {
       // node->data = '+';
        node->type = plus;
    }
    else if (retval == division) {
       // node->data = '/';
         node->type = division;
    }
    else if (retval == power) {
       // node->data = '^';
        node->type = power;
    }
    else {
        node->data = -1;
        node->type = nothing;
    }

    node->left = val;
    node->right = val2;

    return node;
}

int calculate(struct Node* tree) {

    int res1 = 0;
    int res2 = 0;

    if (tree->left != NULL) {
        res1 = calculate(tree->left);
    }
    if (tree->right != NULL) {
        res2 = calculate(tree->right);
    }

    if (tree->type == number) {

        return tree->data;
    } else {

        switch(tree->type) {

            case plus:
                return res1 + res2;

            case minus:
                return res1 - res2;

            case mul:
                return res1 * res2;

            case division:
                return res1 / res2;
        }
    }
    return res1;
}

char* s = NULL;

char* func = NULL;

struct Node* getE();

struct Node* getT();

struct Node* getN() {

    struct Node* val = calloc(1, sizeof(struct Node));

    if (isdigit(*s)) {
        val->type = number;
    } else {
        val->type = symbol;
    }

        do {
            val->data = val->data * 10 + (*s - '0');
            s++;
        } while (*s >= '0' && *s <= '9');

    return val;
}

struct Node* getP() {

    struct Node* val1 = NULL;

    if (*s == '(') {
        ++s;
        val1 = getE();
        assert(*s == ')');
        s++;
        return val1;
    }

    return getN();
}

struct Node* getL() {

    struct Node *val1 = NULL;
    struct Node *val2 = NULL;
    struct Node *val3 = NULL;



    val1 = getP();


    if (*s == '^') {

        val2 = getN();

        val3 = calloc(1, sizeof(struct Node));

        val3->type = power;

        val3->left = val1;
        val3->right = val2;

        return val3;
    }

    return getT();
}

struct Node* getT() {

    struct Node* val1 = NULL;
    struct Node* val2 = NULL;
    struct Node* val3 = NULL;


    val1 = getP();

    while (*s == '*' || *s == '/') {

        char op = *s;
        s++;
        val2 = getP();

        val3 = (struct Node*) calloc(1, sizeof(struct Node));

        if (op == '*') {
            val3->type = mul;
        } else {
            val3->type = division;
        }

        val3->data = 0;

        struct Node *temp = val1;
        val1 = val3;
        val3 = temp;

        val1->right = val2;

        val1->left = val3;
    }

    return val1;
}
struct Node* getE() {

    struct Node* val1 = NULL;
    struct Node* val2 = NULL;
    struct Node* val3 = NULL;


    val1 = getT();

    while (*s == '+' || *s == '-') {

        char op = *s;
        s++;
        val2 = getT();

        val3 = (struct Node*) calloc(1, sizeof(struct Node));

        if (op == '+') {
            val3->type = plus;
        } else {
            val3->type = minus;
        }

        val3->data = 0;

        struct Node *temp = val1;
        val1 = val3;
        val3 = temp;

        val1->right = val2;

        val1->left = val3;
    }

    return val1;

}

struct Node* GetG(char* const str, struct Node* tree) {

    struct Node* val1 = NULL;
    struct Node* val2 = NULL;

    s = str;

    val1 = getE();

    val2 = (struct Node*) calloc(1, sizeof(struct Node));


    val2->type = beginning;
    val2->data = 0;
    val2->left = val1;

    assert(*s == '#');
    return val2;
}

struct Node* copy(struct Node* node) {


    struct Node* copy = (struct Node*) calloc(1, sizeof(struct Node));

    copy->data = node->data;
    copy->type = node->type;
    copy->left = node->left;
    copy->right = node->right;

    return copy;

}
struct Node* simplifier(struct Node* node) {

    if (node->left->left != NULL || node->left->right != NULL) {
        node->left = simplifier(node->left);

    }

    if (node->left->type == number && node->right->type == number && (node->left->data == 0 || node->right->data == 0)) {

        struct Node* new_node = (struct Node *) calloc(1, sizeof(struct Node));

        if (node->type == mul) {
            new_node->data = 0;
        }

        if (node->type == plus) {
            if (node->left->data != 0) {
                new_node->data = node->left->data;
            }
            if (node->right->data != 0) {
                new_node->data = node->right->data;
            }
        }

        new_node->type = number;
        new_node->left = NULL;
        new_node->right = NULL;

        node = new_node;
        return node;
    }
    if ((node->left->type == symbol && node->right->type == number && node->right->data == 1) ||
            (node->left->type == number && node->right->type == symbol && node->left->data == 1)) {

        struct Node* new_node = (struct Node *) calloc(1, sizeof(struct Node));

        if (node->right->type == symbol) {
            new_node->data = node->right->data;
        }

        if (node->left->type == symbol) {
            new_node->data = node->left->data;
        }

//        if (node->type == plus) {
//            new_node->data = node->left->data;
//        }

        new_node->type = symbol;
        new_node->left = NULL;
        new_node->right = NULL;

        node = new_node;
        return node;
    }

    if ((node->left->type == symbol && node->right->type == number && node->right->data == 0) ||
            (node->left->type == number && node->right->type == symbol && node->left->data == 0)) {

        struct Node* new_node = (struct Node *) calloc(1, sizeof(struct Node));

        if (node->type == mul) {
            new_node->data = 0;
        }

        if (node->type == plus) {

            if (node->left->type == symbol) {
                new_node->data = node->left->data;
            }

            if (node->right->type == symbol) {
                new_node->data = node->right->data;
            }
        }

        new_node->type = symbol;
        new_node->left = NULL;
        new_node->right = NULL;

        node = new_node;
        return node;
    }



    if (node->right != NULL) {

        if (node->right->left != NULL || node->right->right != NULL) {
            node->right = simplifier(node->right);
        }

    }

    return node;
}



struct Node* differentiate(struct Node* node) {

    switch(node->type) {

        case plus:
            return _ADD(differentiate(node->left), differentiate(node->right));

        case minus:
            return _SUB(differentiate(node->left), differentiate(node->right));

        case mul:
            return _ADD(_MUL(differentiate(node->right), copy(node->left)), _MUL(copy(node->right), differentiate(node->left)));//_ADD(_MUL(differentiate(node->right), copy(node->left)), _MUL(copy(node->right), differentiate(node->left)));

        case division:
            return _DIV(_SUB(_MUL(differentiate(node->left), copy(node->right)), _MUL(copy(node->left), differentiate(node->right))), _MUL(copy(node->right), copy(node->right)));
        case number:
            return createNodeN(0);

        case symbol:
            return createNodeN(1);

        default:
            return createNode(differentiate(node->left), NULL, beginning);

    }
}


int main() {
    struct Node diff_tree = {0};
    struct Node new_tree = {0};

    diff_tree = *(GetG("(x+1)*(x+1)/(x+2)#", &diff_tree));

    new_tree = *differentiate(&diff_tree);
    simplifier(&new_tree);
    printLaTeX(&new_tree, "output.tex");

    return 0;
}