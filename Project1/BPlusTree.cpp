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
using namespace std;

Node::Node(int max_keys)
{
    keys = new key_struct[max_keys];
    node_ptr = new Node*[max_keys + 1];
    isLeaf = true;
}

void BPlusTree::insertInternal(const key_struct& x, Node* cur, Node* child){

}

void BPlusTree::removeInternal(const key_struct& x, Node* cur, Node* child){

}

Node* BPlusTree::findParent(Node* cur, Node* child){

    return cur;
}

BPlusTree::BPlusTree(){
    root = NULL;
}

int BPlusTree::get_height(Node* cur){
    if (cur == NULL)
    {
        return 0;
    }
    if (cur->isLeaf == false)
    {
        return (get_height(cur->node_ptr[0]) + 1);
    }
    else if (cur->isLeaf == true) {
        return 1;
    }
    else
    {
        return 0;
    }
}

Node* BPlusTree::get_root(){
    return root;
}

int BPlusTree::search(float search_key){
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
        while(cur->isLeaf == false)
        {
            bool flag = true;
            for(int i = 0; i < cur->num_keys; i++)
            {
                // if search key is smaller than a key in node
                if(search_key < cur->keys[i].key)
                {
                    cur = cur->node_ptr[i];
                    flag = false;
                    break;
                }
            }
            // if search key is larger than all keys in node
            if (flag)
            {
                cur = cur->node_ptr[cur->num_keys+1];
            }
            displayNode(cur);
            cout << endl;
            nodecount++;
        }

        // Find search key in leaf node
        for(int i = 0; i < cur->num_keys; i++)
        {
            if(cur->keys[i].key == search_key)
            {
                cout<<"Key found!"<<endl;
                return nodecount;
            }
        }
        cout<<"Key not found!"<<endl;
        return nodecount;
    }
}

void BPlusTree::insert(const key_struct& x){

}

void BPlusTree::remove(const key_struct& x){

}

void BPlusTree::display(Node* cur){
    //Function to display the content of the root node and its 1st child nodes
    if (cur != NULL) {
        cout << "Root" << endl;
        displayNode(cur);

        cout << "Level 1 : " << endl;
        //Display the contents of the 1st child nodes
        for (int i = 0; i <= cur->num_keys; i++)
        {
            displayNode(cur->node_ptr[i]);
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
    int i = 0;
    cout << "|";

    //Printing all filled keys
    for (int i = 0; i < cur->num_keys; i++)
    {
        cout << cur->keys[i].key << "|";
    }
}
