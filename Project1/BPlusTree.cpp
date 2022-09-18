//
// Created by Andre on 10/9/2022.
//
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<climits>
#include<vector>
#include "BPlusTree.h"
#include "MemoryPool.h"
#include "math.h"
using namespace std;

int MAX_NODE_KEYS = 5;
int MAX_NODE_POINTERS = MAX_NODE_KEYS + 1;

Node::Node()
{
    keys = new key[MAX_NODE_KEYS];
    nodePtr = new Node*[MAX_NODE_POINTERS];
}

void BPlusTree::insertInternal(Node* cur, Node* child, const key& numVotes){

}

void BPlusTree::removeInternal(Node* cur, Node* child, const key& numVotes){

}

Node* BPlusTree::searchParent(Node* cur, Node* child){
    Node *search;
    if(cur -> leafNode){
        return nullptr;
    }
    for(int i = 0; i < cur->numOfKeys+1; i++){
        if(cur->nodePtr[i] == child){
            return cur;
        }
        else{
            search = searchParent(cur->nodePtr[i],child);
            if(search!=nullptr){
                return search;
            }
        }
    }
    return search;
}

BPlusTree::BPlusTree(){
    root = nullptr;
}

int BPlusTree::getHeightOfTree(Node* cur){
    if (cur == NULL)
    {
        return 0;
    }
    if (!cur->leafNode)
    {
        return (getHeightOfTree(cur->nodePtr[0]) + 1);
    }
    else if (cur->leafNode) {
        return 1;
    }
    else
    {
        return 0;
    }
}

Node* BPlusTree::getRootOfTree(){
    return root;
}

int BPlusTree::searchNode(int search_key){
    if(root == NULL)
    {
        cout << "B+ Tree is empty!" << endl;
        return NULL;
    }
    else
    {
        Node* cur = root;
        displayNode(cur);
        cout << endl;
        int nodecount = 1;
        // Travel to leaf node
        while(!cur->leafNode)
        {
            bool flag = true;
            for(int i = 0; i < cur->numOfKeys; i++)
            {
                // if search key is smaller than a key in node
                if(search_key < cur->keys[i].keyValue)
                {
                    cur = cur->nodePtr[i];
                    flag = false;
                    break;
                }
            }
            // if search key is larger than all keys in node
            if (flag)
            {
                cur = cur->nodePtr[cur->numOfKeys+1];
            }
            displayNode(cur);
            cout << endl;
            nodecount++;
        }

        // Find search key in leaf node
        for(int i = 0; i < cur->numOfKeys; i++)
        {
            if(cur->keys[i].keyValue == search_key)
            {
                cout<<"Key found!"<<endl;
                return nodecount;
            }
        }
        cout<<"Key not found!"<<endl;
        return nodecount;
    }
}

void BPlusTree::insertKey(const key& numVotes){
    //This function inserts a key (numVotes) into a node
    if (root == nullptr){
        //This function runs if the root is empty, it creates a new node, and inserts the key.
        root = new Node;
        root->keys[0] = numVotes;
        root->numOfKeys+=1;
        root->leafNode = true;
    }
    else {
        //Instantiate a new pointer that points toward the root for manipulation.
        Node *cursor = root;
        //To keep track of the parent node per loop
        Node *parent;

        //This while loop helps to traverse the tree downwards, until it reaches the leaf node
        while(!cursor -> leafNode){
            parent = cursor;
            //This for loop helps to traverse the node to the correct pointer.
            for(int i = 0; i< cursor->numOfKeys; i++){
                if(numVotes.keyValue < cursor->keys[i].keyValue){
                    cursor = cursor -> nodePtr[i];
                    break;
                }
                if (i == cursor->numOfKeys - 1){
                    cursor = cursor->nodePtr[i+1];
                    break;
                }
            }
        }
        //This condition runs only when this node is not full yet
        if (cursor->numOfKeys < MAX_NODE_KEYS){
            int pos = 0;
            for(;pos < cursor->numOfKeys; pos++){
                if(numVotes.keyValue < cursor->keys[pos].keyValue){
                    break;
                }
            }
            for(int j = cursor->numOfKeys; j> pos; j--){
                cursor->keys[j] = cursor->keys[j-1];
            }
            cursor->keys[pos] = numVotes;
            cursor->numOfKeys+=1;
            cursor->nodePtr[cursor->numOfKeys] = cursor->nodePtr[cursor->numOfKeys-1];
            cursor->nodePtr[cursor->numOfKeys-1] = nullptr;
        }
        //This condition runs when this node is full, hence a split needs to be done
        else{
            Node* newNode = new Node;
            key nodeSeparator[MAX_NODE_KEYS+1];
            for(int i = 0; i< MAX_NODE_KEYS; i++){
                nodeSeparator[i] = cursor->keys[i];
            }
            int pos = 0;
            for(;pos < MAX_NODE_KEYS; pos++){
                if(numVotes.keyValue < nodeSeparator[pos].keyValue){
                    break;
                }
            }
            for(int j = MAX_NODE_KEYS; j>pos; j--){
                nodeSeparator[j] = nodeSeparator[j-1];
            }
            nodeSeparator[pos] = numVotes;
            newNode->leafNode = true;
            int leftNodeKeyCount =  ceil((MAX_NODE_KEYS+1)/2);
            int rightNodeKeyCount = floor((MAX_NODE_KEYS+1)/2);
            cursor->numOfKeys = leftNodeKeyCount;
            newNode->numOfKeys = rightNodeKeyCount;
            cursor->nodePtr [cursor->numOfKeys] = newNode;
            newNode->nodePtr[newNode->numOfKeys] = cursor->nodePtr[MAX_NODE_KEYS];
            cursor->nodePtr[MAX_NODE_KEYS] = nullptr;
            for(int i = 0; i < cursor->numOfKeys; i++){
                cursor->keys[i] = nodeSeparator[i];
            }
            for(int i=0, j=cursor->numOfKeys; i< newNode->numOfKeys; i++, j++){
                newNode->keys[i] = nodeSeparator[j];
            }
            if(cursor == root){
                Node *newRoot = new Node;
                newRoot->keys[0] = newNode->keys[0];
                newRoot->nodePtr[0] = cursor;
                newRoot->nodePtr[1] = newNode;
                newRoot->leafNode = false;
                newRoot->numOfKeys = 1;
                this->root = newRoot;
            } else{
                insertInternal(parent, newNode, newNode->keys[0]);
            }
        }
    }
}

void BPlusTree::removeKey(const key& numVotes){

}

void BPlusTree::display(Node* cur){
    //Function to display the content of the root node and its 1st child nodes
    if (cur != NULL) {
        cout << "Root" << endl;
        displayNode(cur);

        cout << "Level 1 : " << endl;
        //Display the contents of the 1st child nodes
        for (int i = 0; i <= cur->numOfKeys; i++)
        {
            displayNode(cur->nodePtr[i]);
            cout << "   ";
        }
        cout << endl;
    }
    else
    {
        cout << "B+ Tree is empty!" << endl;
    }
}

void BPlusTree::displayNode(Node* cur) {
    // Displaying a node and its contents in the B+ Tree.
    //Print out all contents in the node such as |key1|key2|key3|...

    cout << "|";
    //Printing all filled keys
    for (int i = 0; i < cur->numOfKeys; i++)
    {
        cout << cur->keys[i].keyValue << "|";
    }
}
