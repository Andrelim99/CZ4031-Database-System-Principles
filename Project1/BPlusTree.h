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

struct key{
    int keyValue;
    vector <void *> address;
};

class Node{
    //Set the variables of the tree to private
private:
    bool leafNode;
    key *keys;
    int numOfKeys;
    Node** nodePtr;
    friend class BPlusTree;
public:
    //Set the constructor to public
    Node();
};

class BPlusTree{
private:
    //Set the variables of the tree
    Node *root;
public:
    //Set the constructor and methods of BPlusTree
    BPlusTree();
    void insertInternal(Node* cur, Node* child, const key& numVotes);
    void removeInternal(Node* cur, Node* child, const key& numVotes);
    Node* searchParent(Node* cur, Node* child);
    int getHeightOfTree(Node* cur);
    Node* getRootOfTree();
    int searchNode(int key);
    void insertKey(const key& numVotes);
    void removeKey(const key& numVotes);
    void display(Node* cur);
    void displayNode(Node* cur);
};

#endif //CZ4031_PROJ1_BPLUSTREE_H
