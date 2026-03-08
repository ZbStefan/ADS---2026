#include <stdio.h>
#include <stdlib.h>

// 1 = RED, 0 = BLACK
struct Node
{
    int key;
    int color;
    struct Node *left, *right, *parent;
};
// this is the function for creating the new node which i reused from the BST but added the node its colour
struct Node* CreateNode(int value)
{
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->key = value;
    temp->color = 1; // always when we have a new node it has to be red so 1
    temp->left = temp->right = temp->parent = NULL;
    return temp;
}
//the search function which looks for the target which is the actual value
struct Node* Search(struct Node* root, int target)
{
    if (root == NULL || root->key == target)
    {
        return root;
    }
    if (target < root->key)
    {
        return Search(root->left, target);
    }
    return Search(root->right, target);
}
// the next to functions are for finding the minimum and maximum of the tree
struct Node* Minimum(struct Node* node)
{
    while (node != NULL && node->left != NULL)
    {
        node = node->left;
    }
    return node;
}

struct Node* Maximum(struct Node* node)
{
    while (node != NULL && node->right != NULL)
    {
        node = node->right;
    }
    return node;
}
// LeftRotate and RightRotate functions are for rebalancing the tree
void LeftRotate(struct Node** root, struct Node* x)
{
    struct Node* y = x->right;
    x->right = y->left;
    if (y->left != NULL) { y->left->parent = x; }
    y->parent = x->parent;
    if (x->parent == NULL) { *root = y; }
    else if (x == x->parent->left) { x->parent->left = y; }
    else { x->parent->right = y; }
    y->left = x;
    x->parent = y;
}

void RightRotate(struct Node** root, struct Node* y)
{
    struct Node* x = y->left;
    y->left = x->right;
    if (x->right != NULL)
    {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL)
    {
        *root = x;
    }
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
        {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}
// the first one puts the node in the right spot and the second one is for fixing any rule violations
void FixInsert(struct Node** root, struct Node* z)
{
    while (z != *root && z->parent->color == 1)
    {
        if (z->parent == z->parent->parent->left)
        {
            struct Node* uncle = z->parent->parent->right;
            if (uncle != NULL && uncle->color == 1)
            {
                z->parent->color = 0;
                uncle->color = 0;
                z->parent->parent->color = 1;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    LeftRotate(root, z);
                }
                z->parent->color = 0;
                z->parent->parent->color = 1;
                RightRotate(root, z->parent->parent);
            }
        }
        else
        {
            struct Node* uncle = z->parent->parent->left;
            if (uncle != NULL && uncle->color == 1)
            {
                z->parent->color = 0;
                uncle->color = 0;
                z->parent->parent->color = 1;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    RightRotate(root, z);
                }
                z->parent->color = 0;
                z->parent->parent->color = 1;
                LeftRotate(root, z->parent->parent);
            }
        }
    }
    (*root)->color = 0;
}

void Insert(struct Node** root, int key)
{
    struct Node* z = CreateNode(key);
    struct Node* y = NULL;
    struct Node* x = *root;
    while (x != NULL)
    {
        y = x;
        if (z->key < x->key)
        {
            x = x->left;
        }
        else { x = x->right; }
    }
    z->parent = y;
    if (y == NULL)
    {
        *root = z;
    }
    else if (z->key < y->key)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }
    FixInsert(root, z);
}

void FixDelete(struct Node** root, struct Node* x, struct Node* x_parent)
{
    while (x != *root && (x == NULL || x->color == 0))
    {
        if (x == x_parent->left)
        {
            struct Node* w = x_parent->right;
            if (w->color == 1)
            {
                w->color = 0; x_parent->color = 1;
                LeftRotate(root, x_parent);
                w = x_parent->right;
            }
            if ((w->left == NULL || w->left->color == 0) && (w->right == NULL || w->right->color == 0))
            {
                w->color = 1; x = x_parent; x_parent = x->parent;
            }
            else
            {
                if (w->right == NULL || w->right->color == 0)
                {
                    if (w->left) { w->left->color = 0; }
                    w->color = 1; RightRotate(root, w);
                    w = x_parent->right;
                }
                w->color = x_parent->color; x_parent->color = 0;
                if (w->right) { w->right->color = 0; }
                LeftRotate(root, x_parent);
                x = *root;
            }
        }
        else
        {
            struct Node* w = x_parent->left;
            if (w->color == 1)
            {
                w->color = 0; x_parent->color = 1;
                RightRotate(root, x_parent);
                w = x_parent->left;
            }
            if ((w->right == NULL || w->right->color == 0) && (w->left == NULL || w->left->color == 0))
            {
                w->color = 1; x = x_parent; x_parent = x->parent;
            }
            else
            {
                if (w->left == NULL || w->left->color == 0)
                {
                    if (w->right) { w->right->color = 0; }
                    w->color = 1; LeftRotate(root, w);
                    w = x_parent->left;
                }
                w->color = x_parent->color; x_parent->color = 0;
                if (w->left) { w->left->color = 0; }
                RightRotate(root, x_parent);
                x = *root;
            }
        }
    }
    if (x) { x->color = 0; }
}

void Delete(struct Node** root, int key)
{
    struct Node* z = Search(*root, key);
    if (z == NULL) { return; }
    struct Node *y = z, *x;
    int y_orig_color = y->color;
    struct Node* x_parent;
    if (z->left == NULL)
    {
        x = z->right; x_parent = z->parent;
        if (z->parent == NULL) { *root = x; }
        else if (z == z->parent->left) { z->parent->left = x; }
        else { z->parent->right = x; }
        if (x) { x->parent = z->parent; }
    }
    else if (z->right == NULL)
    {
        x = z->left; x_parent = z->parent;
        if (z->parent == NULL) { *root = x; }
        else if (z == z->parent->left) { z->parent->left = x; }
        else { z->parent->right = x; }
        if (x) { x->parent = z->parent; }
    }
    else
    {
        y = Minimum(z->right); y_orig_color = y->color;
        x = y->right;
        if (y->parent == z) { x_parent = y; }
        else
        {
            x_parent = y->parent; y->parent->left = x;
            if (x) { x->parent = y->parent; }
            y->right = z->right; y->right->parent = y;
        }
        if (z->parent == NULL) { *root = y; }
        else if (z == z->parent->left) { z->parent->left = y; }
        else { z->parent->right = y; }
        y->parent = z->parent; y->left = z->left; y->left->parent = y; y->color = z->color;
    }
    free(z);
    if (y_orig_color == 0) { FixDelete(root, x, x_parent); }
}

void PrintTree(struct Node* root)
{
    if (root != NULL)
    {
        PrintTree(root->left);
        printf("%d(%c) ", root->key, root->color == 1 ? 'R' : 'B');
        PrintTree(root->right);
    }
}

int main()
{
    struct Node* root = NULL;
    int keys[] = {50, 30, 20, 40, 70, 60, 80};

    for (int i = 0; i < 7; i++) { Insert(&root, keys[i]); }

    printf("--- Initial Red-Black Tree State ---\n");
    printf("Min: %d | Max: %d\n", Minimum(root)->key, Maximum(root)->key);
    printf("Tree Structure: ");
    PrintTree(root);
    printf("\n\n");

    int s = 60;
    printf("Searching for %d: %s\n", s, Search(root, s) ? "Found" : "Not Found");

    printf("Deleting 70\n");
    Delete(&root, 70);

    printf("Tree after deletion: ");
    PrintTree(root);
    printf("\n");

    return 0;

}
