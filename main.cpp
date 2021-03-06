#include <iostream>
#include "garbage_collection.h"
#include "i_smart_object.h"
#include "smart_binary_tree.h"
#include "smart_classes_with_deriving.h"
#include <stdlib.h>

using namespace std;

bool shouldWeCreateAChild()
{
    return rand() % 5;
}

void generateRandomTree(SmartHeapBinaryTree::Node * T, int maxdepth=4, int depth=0)
{
    if (depth >= maxdepth || T == nullptr)
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


void testBinaryTrees()
{
    SmartHeapBinaryTree::Node root(10);
    SmartHeapBinaryTree::Node * root2 = new SmartHeapBinaryTree::Node(2);
    generateRandomTree(root2, 4);
    SmartHeapBinaryTree::Node * root3 = new SmartHeapBinaryTree::Node(3);
    generateRandomTree(root3, 4);
    root.right = root2;
    std::cout << "check trees before collection and after" << std::endl;
    ISmartObject * ptrRoot2 = dynamic_cast<ISmartObject *>(root2);
    ISmartObject * ptrRoot3 = dynamic_cast<ISmartObject *>(root3);
    std::cout << GarbageCollection::getInstance().isPointerSteelAlive(ptrRoot2) << " ";
    std::cout << GarbageCollection::getInstance().isPointerSteelAlive(ptrRoot3) << std::endl;
    GarbageCollection::getInstance().collectGarbage();
    std::cout << GarbageCollection::getInstance().isPointerSteelAlive(ptrRoot2) << " ";
    std::cout << GarbageCollection::getInstance().isPointerSteelAlive(ptrRoot3) << std::endl;
}

void test_bad_alloc()
{
    /*
     * test ONLY IN RELEASE,
     * because memory leak detector and address sanitizer will get crazy
     */
    new test2::B[1000000000LL];
}

int main()
{
//    test_bad_alloc();
    testBinaryTrees();
    test_smart_classes_with_deriving_1();
    test_smart_classes_with_deriving_2();
    test_smart_classes_with_deriving_3();
    test_smart_classes_with_deriving_4();
    test_smart_classes_with_deriving_5();
    /*
     * demonstrate smart singleton that singleton will be destroyed by himself at the end of program
     * also it will collect garbage in the end of his life
     */
    SmartHeapBinaryTree::Node root(10);
    root.left = new SmartHeapBinaryTree::Node(50);
    return 0;
}