#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 200

static int flag = 0;

typedef struct Node
{
    char *word;
    struct Node* left;
    struct Node* right;
} Node;

Node* newNode(char* c)
{
    Node *n = malloc(sizeof(Node));
    n->word = malloc(strlen(c) + 1);
    strcpy(n->word,c);
    n->left = n->right = NULL;
    return n;
}

Node* insertNodeToTree(Node *root, Node *n)
{
    if(root == NULL)
        return newNode(n->word);
    if(strcasecmp(root->word,n->word) < 0)
    {
        root->right = insertNodeToTree(root->right,n);
    }
    else if(strcasecmp(root->word,n->word) > 0)
    {
        root->left = insertNodeToTree(root->left,n);
    }
    return root;
}

int size(Node *root)
{
    if(root == NULL)
        return 0;
    return 1 + size(root->left) + size(root->right);
}

int max(int a, int b)
{
    return a>b?a:b;
}

int height(Node *root)
{
    if(root == NULL)
        return -1;
    return 1 + max(height(root->left),height(root->right));
}

Node* loadDictionary()
{
    FILE *f = fopen("/home/hossam/Workplace/spell-checker/EN-US-Dictionary.txt","r");
    char *temp = malloc(100);
    if(f == NULL)
    {
        printf("null");
        exit(1);
    }
    temp = fgets(temp,100,f);
    temp[strlen(temp) - 1] = '\0';
    Node *root = newNode(temp);
    while(!feof(f))
    {
        temp = fgets(temp,100,f);
        temp[strlen(temp) - 1] = '\0';

        Node *tempN = newNode(temp);

        root = insertNodeToTree(root,tempN);
    }
    if(root != NULL)
    {
        printf("..................................\n");
        printf("Dictionary Loaded Successfully...!\n");
        printf("..................................\n");
        printf("Size = %d\n",size(root));
        printf("..................................\n");
        printf("Height = %d\n",height(root));
        printf("..................................\n");
        return root;
    }
    else
    {
        printf("Error while loading the Dictionary!!");
        exit(1);
    }
}

Node* findMin(Node *n)
{
    if(n == NULL)
        return NULL;
    if(n->left != NULL)
        return findMin(n->left);
    else
        return n;
}
Node* findMax(Node *n)
{
    if(n == NULL)
        return NULL;
    if(n->right != NULL)
        return findMax(n->right);
    else
        return n;
}

Node *search(Node *r, char *c)
{
    if(r == NULL)
        return NULL;
    if(strcasecmp(c,r->word) == 0)
        return r;
    else if(strcasecmp(c,r->word) < 0)
        return search(r->left,c);
    else
        return search(r->right,c);
}

Node* getSuccessor(Node *root, char *c)
{
    Node * current = search(root,c);
    if(current == NULL)
        return NULL;
    if(current->right != NULL)
    {
        return findMin(current->right);
    }
    else
    {
        Node* successor = NULL;
        Node* ancestor = root;
        while(ancestor != current)
        {
            if(strcasecmp(current->word, ancestor->word) < 0)
            {
                successor = ancestor;
                ancestor = ancestor->left;
            }
            else
                ancestor = ancestor->right;
        }
        return successor;
    }
}



Node* getPredecessor(Node *root,char *c)
{
    Node *current = search(root,c);
    if(current == NULL)
        return NULL;
    if(current->left != NULL)
    {
        return findMax(current->left);
    }
    else
    {
        Node* successor = NULL;
        Node* ancestor = root;
        while(ancestor != current)
        {
            if(strcasecmp(current->word, ancestor->word) > 0)
            {
                successor = ancestor;
                ancestor = ancestor->right;
            }
            else
                ancestor = ancestor->left;
        }
        return successor;
    }
}


void checkIsIncorrect(Node *root,char *x,char *c)
{
    Node *pre = getPredecessor(root,x);Node *current = newNode(x);
    Node *suc = getSuccessor(root,x);


    if(pre != NULL && suc != NULL && strcasecmp(c,current->word) != 0  && strcasecmp(c,suc->word) != 0 && strcasecmp(c,pre->word) != 0)
    {
        int a = strcasecmp(c,pre->word);
        int b = strcasecmp(c,current->word);
        int d = strcasecmp(c,suc->word);

        if((a == 1 && b == 1 && d == -1))
        {
            Node *suc2 = getSuccessor(root,suc->word);
            if(suc2 != NULL)
            {
                int e = strcasecmp(c,suc2->word);
                if(e == -1)
                {
                    int indicator = 1;
                    char *c1 = current->word;
                    char *c2 = suc->word;
                    char *cT = c;
                    while((*c1 != 0) && (*cT != 0) && (*c2 != 0))
                    {
                        if((*c1 == *c2) || (*c1 == *c2 - 32) || (*c1 == *c2 + 32))
                        {
                            c1++;
                            c2++;
                            cT++;
                            continue;
                        }
                        else
                        {
                            if(((*c1 == *cT) || (*c1 == *cT - 32) || (*c1 == *cT + 32)) || ((*c2 == *cT) || (*c2 == *cT - 32) || (*c2 == *cT + 32)))
                            {
                                indicator = 2;
                                break;
                            }
                            else
                            {
                                indicator = 1;
                                break;
                            }
                            c1++;
                            c2++;
                            cT++;
                        }
                    }
                    if(indicator == 1)
                    {
                        printf("%s - Incorrect, Suggestions---> Node: %s <----> Successor: %s <----> Predecessor: %s\n",c,current->word,suc->word,pre->word);

                        flag = 1;
                    }
                    else
                    {
                        printf("%s - Incorrect, Suggestions---> Node: %s <----> Successor: %s <----> Predecessor: %s\n",c,suc->word,suc2->word,current->word);

                        flag = 1;
                    }
                }
            }
        }
    }
}

void spellingCheck(Node *root, Node *r, char *c)
{
    if(r != NULL)
    {
        spellingCheck(root,r->left,c);
        if(flag == 0 && strcasecmp(r->word,c) == 0)
        {
            printf("%s - Correct\n",r->word);
            flag = 1;
        }
        else if(flag == 0)
        {
            checkIsIncorrect(root,r->word,c);
        }
        if(flag == 0)
        {
            spellingCheck(root,r->right,c);
        }
    }
}


int main()
{
    Node *root = loadDictionary();
    char sentence[MAX_SIZE];
    printf("Enter a sentence : \n");
    fgets(sentence,MAX_SIZE,stdin);
    char *token = strtok(sentence," ");
    char a[25];
    while(token != NULL)
    {
        flag = 0;
        if(token[strlen(token) - 1] == '\n')
           token[strlen(token) - 1] = '\0';
        spellingCheck(root,root,strcpy(a,token));
        token = strtok(NULL," ");
    }
    return 0;

}
