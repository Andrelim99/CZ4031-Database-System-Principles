//
// Created by Andre on 10/9/2022.
//

//Include all the packages

#include<iostream>
#include<string>
using namespace std;
//Declare all the classes as well as its functions in header files

#ifndef CZ4031_PROJ1_BPLUSTREE_H
#define CZ4031_PROJ1_BPLUSTREE_H


//Declaration to be within the guards

struct key_struct{
    float key;
    vector <void *> address;
};

class Node{
    //Set the variables of the tree to private
private:
    bool isLeaf;
    key_struct *keys;
    int num_keys;
    Node** node_ptr;
    friend class BPlusTree;
public:
    //Set the constructor to public
    Node(int max_keys);
};

class BPlusTree{
private:
    //Set the variables of the tree
    Node *root;
public:
    //Set the constructor and methods of BPlusTree
    BPlusTree();
    void insertInternal(const key_struct& x, Node* cur, Node* child);
    void removeInternal(const key_struct& x, Node* cur, Node* child);
    Node* findParent(Node* cur, Node* child);
    int get_height(Node* cur);
    Node* get_root();
    Node* search(float search_key);
    void insert(const key_struct& x);
    void remove(const key_struct& x);
    int display(Node* cursor, int nodecount, bool first);
};

#endif //CZ4031_PROJ1_BPLUSTREE_H
