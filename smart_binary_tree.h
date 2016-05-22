#ifndef GARBAGECOLLECTION_SMART_BINARY_TREE_H
#define GARBAGECOLLECTION_SMART_BINARY_TREE_H

#include "i_smart_object.h"
#include <bits/stdc++.h>


namespace SmartHeapBinaryTree
{

    const int INF = 2e9 + 7;

    bool debug = 0;

    struct Node : virtual public ISmartObject
    {
        int val;
        int sum, max_psum;
        int cnt;
        int y;
        Node * left, * right;
        Node(int val_)
                :val(val_), sum(val_), max_psum(val_), cnt(1), left(0), right(0)
        {
            y = rand();
        }
        virtual std::vector<ISmartObject *> pointers() const
        {
            std::vector<ISmartObject *> children;
            if (left != nullptr)
                children.push_back(dynamic_cast<ISmartObject*>(left));
            if (right != nullptr)
                children.push_back(dynamic_cast<ISmartObject*>(right));
            return children;
        }
    };

    inline int cnt(Node * &T)
    {
        return T ? T->cnt : 0;
    }

    inline int sum(Node * &T)
    {
        return T ? T->sum : 0;
    }

    inline void push(Node * &)
    {}

    inline int max_psum(Node * &T)
    {
        return T ? T->max_psum : -INF;
    }

    void recalc(Node * &T)
    {
        if (T) {
            T->cnt = cnt(T->left) + cnt(T->right) + 1;
            T->sum = sum(T->left) + sum(T->right) + T->val;
            T->max_psum = max_psum(T->left);
            T->max_psum = std::max(T->max_psum, sum(T->left) + T->val);
            if (T->right)
                T->max_psum = std::max(T->max_psum, sum(T->left) + T->val + max_psum(T->right));
        }
    }

    std::pair <Node*, Node *> split(Node * &T, int key)
    {
        if (!T)
            return std::pair<Node *, Node *>(0, 0);
        if (key < cnt(T->left) + 1) {
            auto tmp(split(T->left, key));
            T->left = tmp.second;
            recalc(T);
            return std::make_pair(tmp.first, T);
        } else {
            auto tmp(split(T->right, key - cnt(T->left) - 1));
            T->right = tmp.first;
            recalc(T);
            return std::make_pair(T, tmp.second);
        }
    }

    Node * merge(Node * L, Node * R)
    {
        push(L);
        push(R);
        if (!L)
            return R;
        if (!R)
            return L;
        if (L->y > R->y) {
            L->right = merge(L->right, R);
            recalc(L);
            return L;
        } else {
            R->left = merge(L, R->left);
            recalc(R);
            return R;
        }
    }

    Node * insert(Node * T, int pos, int val) {
        auto tmp(split(T, pos));
        return merge(merge(tmp.first, new Node(val)), tmp.second);
    }

    Node * erase(Node * T, int pos) {
        if (pos == cnt(T->left)) {
            return merge(T->left, T->right);
        }
        if (pos < cnt(T->left)) {
            T->left = erase(T->left, pos);
            recalc(T);
            return T;
        } else {
            T->right = erase(T->right, pos - cnt(T->left) - 1);
            recalc(T);
            return T;
        }
    }

    void print(Node * T, int d=0)
    {
        if (!T)
            return;
        print(T->right, d + 1);
        for (int i = 0; i < d; ++i)
            std::cout << "\t";
        std::cout << T->val<< /*" max=" << T->max_psum << " sum=" << T->sum << */std::endl;
        print(T->left, d + 1);
    }
}

#endif //GARBAGECOLLECTION_SMART_BINARY_TREE_H
