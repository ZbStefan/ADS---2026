#include <stdio.h>
#include <stdlib.h>

#define T 2

typedef struct Node {
    int keys[2*T - 1];
    struct Node *children[2*T];
    int num_keys;
    int is_leaf;
} Node;

Node *new_node(int is_leaf) {
    Node *node = calloc(1, sizeof(Node));
    node->is_leaf = is_leaf;
    return node;
}

int find_index(Node *node, int k) {
    int i = 0;
    while (i < node->num_keys && node->keys[i] < k) i++;
    return i;
}

Node *search(Node *node, int k) {
    int i = find_index(node, k);
    if (i < node->num_keys && node->keys[i] == k) return node;
    if (node->is_leaf) return NULL;
    return search(node->children[i], k);
}

void split_child(Node *parent, int i) {
    Node *full  = parent->children[i];
    Node *right = new_node(full->is_leaf);
    right->num_keys = T - 1;

    for (int j = 0; j < T - 1; j++)
        right->keys[j] = full->keys[j + T];

    if (!full->is_leaf)
        for (int j = 0; j < T; j++)
            right->children[j] = full->children[j + T];

    full->num_keys = T - 1;

    for (int j = parent->num_keys; j > i; j--)
        parent->children[j + 1] = parent->children[j];
    for (int j = parent->num_keys - 1; j >= i; j--)
        parent->keys[j + 1] = parent->keys[j];

    parent->children[i + 1] = right;
    parent->keys[i] = full->keys[T - 1];
    parent->num_keys++;
}

void insert_nonfull(Node *node, int k) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        while (i >= 0 && k < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = k;
        node->num_keys++;
    } else {
        while (i >= 0 && k < node->keys[i]) i--;
        i++;
        if (node->children[i]->num_keys == 2*T - 1) {
            split_child(node, i);
            if (k > node->keys[i]) i++;
        }
        insert_nonfull(node->children[i], k);
    }
}

void insert(Node **root, int k) {
    Node *r = *root;
    if (r->num_keys == 2*T - 1) {
        Node *new_root = new_node(0);
        new_root->children[0] = r;
        split_child(new_root, 0);
        *root = new_root;
    }
    insert_nonfull(*root, k);
}

int get_predecessor(Node *node) {
    while (!node->is_leaf)
        node = node->children[node->num_keys];
    return node->keys[node->num_keys - 1];
}

void merge_children(Node *parent, int i) {
    Node *left  = parent->children[i];
    Node *right = parent->children[i + 1];

    left->keys[T - 1] = parent->keys[i];

    for (int j = 0; j < right->num_keys; j++)
        left->keys[T + j] = right->keys[j];
    if (!left->is_leaf)
        for (int j = 0; j <= right->num_keys; j++)
            left->children[T + j] = right->children[j];

    left->num_keys = 2*T - 1;

    for (int j = i + 1; j < parent->num_keys; j++)
        parent->keys[j - 1] = parent->keys[j];
    for (int j = i + 2; j <= parent->num_keys; j++)
        parent->children[j - 1] = parent->children[j];
    parent->num_keys--;

    free(right);
}

void borrow_from_left(Node *parent, int i) {
    Node *child  = parent->children[i];
    Node *sibling = parent->children[i - 1];

    for (int j = child->num_keys; j > 0; j--)
        child->keys[j] = child->keys[j - 1];
    if (!child->is_leaf)
        for (int j = child->num_keys + 1; j > 0; j--)
            child->children[j] = child->children[j - 1];

    child->keys[0] = parent->keys[i - 1];
    if (!child->is_leaf)
        child->children[0] = sibling->children[sibling->num_keys];

    parent->keys[i - 1] = sibling->keys[sibling->num_keys - 1];
    child->num_keys++;
    sibling->num_keys--;
}

void borrow_from_right(Node *parent, int i) {
    Node *child   = parent->children[i];
    Node *sibling = parent->children[i + 1];

    child->keys[child->num_keys] = parent->keys[i];
    if (!child->is_leaf)
        child->children[child->num_keys + 1] = sibling->children[0];

    parent->keys[i] = sibling->keys[0];

    for (int j = 0; j < sibling->num_keys - 1; j++)
        sibling->keys[j] = sibling->keys[j + 1];
    if (!sibling->is_leaf)
        for (int j = 0; j < sibling->num_keys; j++)
            sibling->children[j] = sibling->children[j + 1];

    child->num_keys++;
    sibling->num_keys--;
}

void delete_key(Node *node, int k);

void fix_child(Node *parent, int i) {
    if (i > 0 && parent->children[i - 1]->num_keys >= T)
        borrow_from_left(parent, i);
    else if (i < parent->num_keys && parent->children[i + 1]->num_keys >= T)
        borrow_from_right(parent, i);
    else {
        if (i < parent->num_keys)
            merge_children(parent, i);
        else
            merge_children(parent, i - 1);
    }
}

void delete_key(Node *node, int k) {
    int i = find_index(node, k);

    if (i < node->num_keys && node->keys[i] == k) {
        if (node->is_leaf) {
            for (int j = i + 1; j < node->num_keys; j++)
                node->keys[j - 1] = node->keys[j];
            node->num_keys--;
        } else {
            int pred_key = get_predecessor(node->children[i]);
            node->keys[i] = pred_key;
            delete_key(node->children[i], pred_key);
        }
    } else {
        if (node->is_leaf) {
            printf("Key %d not found.\n", k);
            return;
        }
        if (node->children[i]->num_keys < T)
            fix_child(node, i);

        i = find_index(node, k);
        if (i < node->num_keys && node->keys[i] == k)
            delete_key(node->children[i], k);  
        else
            delete_key(node->children[i], k);
    }
}

void delete(Node **root, int k) {
    delete_key(*root, k);
    if ((*root)->num_keys == 0 && !(*root)->is_leaf) {
        Node *old = *root;
        *root = (*root)->children[0];
        free(old);
    }
}


void print_tree(Node *node, int depth) {
    if (!node) return;
    for (int i = node->num_keys - 1; i >= 0; i--) {
        if (!node->is_leaf) print_tree(node->children[i + 1], depth + 1);
        for (int d = 0; d < depth; d++) printf("    ");
        printf("%d\n", node->keys[i]);
    }
    if (!node->is_leaf) print_tree(node->children[0], depth + 1);
}

void btree_traverse(Node *node) {
    if (!node) return;
    for (int i = 0; i < node->num_keys; i++) {
        if (!node->is_leaf) btree_traverse(node->children[i]);
        printf("%d ", node->keys[i]);
    }
    if (!node->is_leaf) btree_traverse(node->children[node->num_keys]);
}

int main(void) {
    Node *root = new_node(1);
    int vals[] = {10, 20, 5, 6, 12, 30, 7, 17};

    for (int i = 0; i < 8; i++)
        insert(&root, vals[i]);

    printf("Tree after insertion:\n");
    print_tree(root, 0);

    printf("\nIn-order before: "); btree_traverse(root); printf("\n");

    delete(&root, 6);
    delete(&root, 20);

    printf("In-order after:  "); btree_traverse(root); printf("\n");

    printf("\nTree after deleting 6 and 20:\n");
    print_tree(root, 0);

    printf("\nSearch 12: %s\n", search(root, 12) ? "found" : "not found");
    printf("Search 20: %s\n", search(root, 20) ? "found" : "not found");
}