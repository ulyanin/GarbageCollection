#include <iostream>
#include "garbage_collection.h"
#include "i_smart_object.h"
#include "smart_binary_tree.h"
#include <stdlib.h>

using namespace std;

bool shouldWeCreateAChild()
{
    return rand() % 5;
}

void generateRandomTree(SmartHeapBinaryTree::Node * T, int maxdepth=4, int depth=0)
{
    if (depth >= maxdepth)
        return;
    if (shouldWeCreateAChild()) {
        T->left = new SmartHeapBinaryTree::Node(rand() % 100);
        generateRandomTree(T->left, maxdepth, depth + 1);
    }
    if (shouldWeCreateAChild()) {
        T->right = new SmartHeapBinaryTree::Node(rand() % 100);
        generateRandomTree(T->right, maxdepth, depth + 1);
    }
}

void tmp(SmartHeapBinaryTree::Node *T)
{
    T->right = new SmartHeapBinaryTree::Node(11);
}

int main()
{
    cout << "Hello, World!" << endl;
    SmartHeapBinaryTree::Node root(10);
    generateRandomTree(&root, 10);
    //tmp(&root);
    //generateRandomTree(root.right, 2);
    //root.left = new SmartHeapBinaryTree::Node(11);
    SmartHeapBinaryTree::print(&root);
    return 0;
}