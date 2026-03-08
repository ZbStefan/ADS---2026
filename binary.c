#include <stdio.h>
#include <stdlib.h>

struct Node
{
    int key;
    struct Node *left, *right;
};

// the function is for creating the first node
struct Node *CreateNewNode(int value)
{
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->key = value;
    temp->left = temp->right = NULL;
    return temp;
}
// recursively navigates the tree to place a new value in its correct sorted position
struct Node *InsertNode(struct Node *node, int value)
{
    if (node == NULL)
    {
        return CreateNewNode(value);
    }
    if (value < node->key)
    {
        node->left = InsertNode(node->left, value);
    }
    else if (value > node->key)
    {
        node->right = InsertNode(node->right, value);
    }
    return node;
}
// searches the tree by going left if its smaller or right if its greater
struct Node *SearchNode(struct Node *root, int s_value)
{
    if (root == NULL || root->key == s_value)
    {
        return root;
    }
    if (root->key < s_value)
    {
        return SearchNode(root->right, s_value);
    }
    return SearchNode(root->left, s_value);
}


struct Node *FindMin(struct Node *root)
{
    while (root && root->left != NULL)
    {
        root = root->left;
    }
    return root;
}
//
struct Node *FindMax(struct Node *root)
{
    while (root && root->right != NULL)
    {
        root = root->right;
    }
    return root;
}


struct Node *DeleteNode(struct Node *root, int x)
{
    if (root == NULL)
    {
        return NULL;
    }

    if (x < root->key)
    {
        root->left = DeleteNode(root->left, x);
    }
    else if (x > root->key)
    {
        root->right = DeleteNode(root->right, x);
    }
    else
    {
        // case 1 and 2 for no child or one child
        if (root->left == NULL)
        {
            struct Node *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            struct Node *temp = root->left;
            free(root);
            return temp;
        }

        struct Node *temp = FindMin(root->right);
        root->key = temp->key;
        root->right = DeleteNode(root->right, temp->key);
    }
    return root;
}

void Order(struct Node *root)
{
    if (root != NULL)
    {
        Order(root->left);
        printf("%d ", root->key);
        Order(root->right);
    }
}

int main()
{
    struct Node *root = NULL;
    int keys[] = {50, 30, 20, 40, 70, 60, 80};

    for(int i = 0; i < 7; i++)
    {
        root = InsertNode(root, keys[i]);
    }

    printf("Initial Tree (Sorted): ");
    Order(root);
    printf("\n");

    struct Node* min = FindMin(root);
    struct Node* max = FindMax(root);
    printf("Min Value: %d | Max Value: %d\n", min->key, max->key);

    int target = 60;
    if (SearchNode(root, target)) printf("Search: %d found!\n", target);
    else printf("Search: %d not found.\n", target);

    printf("Deleting 70...\n");
    root = DeleteNode(root, 70);

    printf("Tree after deletion: ");
    Order(root);
    printf("\n");

    return 0;
}