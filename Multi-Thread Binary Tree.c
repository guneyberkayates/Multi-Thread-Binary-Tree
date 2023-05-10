#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Define the TreeNode struct
typedef struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Define the BinaryTree struct
typedef struct BinaryTree {
    TreeNode* root;
    pthread_mutex_t lock;
    pthread_t search_thread;
    int search_queue[100][2];
    int search_queue_size;
    bool search_results[100];
} BinaryTree;

// Function declarations
void* search_thread_func(void* arg);
bool search(BinaryTree* tree, int val);
void insert(BinaryTree* tree, int val);
void preorder_traversal(TreeNode* node, int* result, int* size);
void inorder_traversal(TreeNode* node, int* result, int* size);
void postorder_traversal(TreeNode* node, int* result, int* size);
void simulate_insertions(BinaryTree* tree, int num_insertions);
void simulate_searches(BinaryTree* tree, int num_searches);

// Define the search_thread_func function
void* search_thread_func(void* arg) {
    BinaryTree* tree = (BinaryTree*) arg;
    while (true) {
        if (tree->search_queue_size > 0) {
            int val = tree->search_queue[0][0];
            int request_id = tree->search_queue[0][1];
            pthread_mutex_lock(&tree->lock);
            bool result = search(tree, val);
            tree->search_results[request_id] = result;
            for (int i = 0; i < tree->search_queue_size - 1; i++) {
                tree->search_queue[i][0] = tree->search_queue[i + 1][0];
                tree->search_queue[i][1] = tree->search_queue[i + 1][1];
            }
            tree->search_queue_size--;
            pthread_mutex_unlock(&tree->lock);
        }
    }
}

// Define the search function
bool search(BinaryTree* tree, int val) {
    TreeNode* curr = tree->root;
    while (curr != NULL) {
        if (val == curr->val) {
            return true;
        } else if (val < curr->val) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    return false;
}

// Define the insert function
void insert(BinaryTree* tree, int val) {
    TreeNode* node = (TreeNode*) malloc(sizeof(TreeNode));
    node->val = val;
    node->left = NULL;
    node->right = NULL;
    if (tree->root == NULL) {
        tree->root = node;
    } else {
        TreeNode* curr = tree->root;
        while (true) {
            if (val < curr->val) {
                if (curr->left == NULL) {
                    curr->left = node;
                    break;
                } else {
                    curr = curr->left;
                }
            } else {
                if (curr->right == NULL) {
                    curr->right = node;
                    break;
                } else {
                    curr = curr->right;
                }
            }
        }
    }
}

// Define the preorder_traversal function
void preorder_traversal(TreeNode* node, int* result, int* size) {
    if (node != NULL) {
        result[(*size)++] = node->val;
        preorder_traversal(node->left, result, size);
        preorder_traversal(node->right, result, size);
    }
}

// Define the inorder_traversal function
void inorder_traversal(TreeNode* node, int* result, int* size) {
    if (node != NULL) {
        inorder_traversal(node->left, result, size);
        result[*size] = node->val;
        (*size)++;
        inorder_traversal(node->right, result, size);
    }
}
void simulate_insertions(BinaryTree* tree, int num_insertions) {
    for (int i = 0; i < num_insertions; i++) {
        int val = rand() % 100;
        pthread_mutex_lock(&tree->lock);
        insert(tree, val);
        pthread_mutex_unlock(&tree->lock);
    }
}

void simulate_searches(BinaryTree* tree, int num_searches) {
    for (int i = 0; i < num_searches; i++) {
        int val = rand() % 1000;
        int request_id = i;
        pthread_mutex_lock(&tree->lock);
        if (tree->search_queue_size < 100) {
            tree->search_queue[tree->search_queue_size][0] = val;
            tree->search_queue[tree->search_queue_size][1] = request_id;
            tree->search_queue_size++;
        } else {
            printf("Error: search queue full\n");
        }
        pthread_mutex_unlock(&tree->lock);

        printf("Searching for %d...\n", val);
        while (true) {
            pthread_mutex_lock(&tree->lock);
            bool result = tree->search_results[request_id];
            pthread_mutex_unlock(&tree->lock);
            if (result != false) {
                printf("%d found\n", val);
                break;
            } else if (result == false && tree->search_queue_size == 0) {
                printf("%d not found\n", val);
                break;
            }
        }
    }
}



int* BinaryTree_inorder_traversal(BinaryTree* tree, int* returnSize) {
    *returnSize = 0;
    int* result = (int*) malloc(sizeof(int) * 100); // assuming max tree 
    inorder_traversal(tree->root, result, returnSize);
    return result;
}
int main() {
    srand(time(NULL)); // Seed random number generator

    // Create a binary tree with a NULL root and a search queue size of 0
    BinaryTree* tree = (BinaryTree*) malloc(sizeof(BinaryTree));
    tree->root = NULL;
    tree->search_queue_size = 0;

    // Initialize the binary tree lock and create the search thread
    pthread_mutex_init(&tree->lock, NULL);
    pthread_create(&tree->search_thread, NULL, search_thread_func, tree);

    // Simulate 10,000 insertions and 1,000 searches
    simulate_insertions(tree, 10000);
    simulate_searches(tree, 1000);

    // Print the inorder traversal of the binary tree
    int returnSize;
    int* result = BinaryTree_inorder_traversal(tree, &returnSize);
    printf("Inorder Traversal: ");
    for (int i = 0; i < returnSize; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    // Clean up
    free(result);
    pthread_cancel(tree->search_thread);
    pthread_mutex_destroy(&tree->lock);
    free(tree);
    return 0;
}



