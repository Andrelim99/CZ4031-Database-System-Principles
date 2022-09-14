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
    return 0;
}

Node* BPlusTree::get_root(){
    return root;
}

Node* BPlusTree::search(float search_key){
    if(root == NULL)
    {
        cout << "B+ Tree is empty!" << endl;
        return NULL;
    }
    else
    {
        Node* cur = root;
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
        }

        // Find search key in leaf node
        for(int i = 0; i < cur->num_keys; i++)
        {
            if(cur->keys[i].key == search_key)
            {
                cout<<"Key found!"<<endl;
                return cur;
            }
        }
        cout<<"Key not found!"<<endl;
        return NULL;
    }
}

void BPlusTree::insert(const key_struct& x){

}

void BPlusTree::remove(const key_struct& x){

}

int BPlusTree::display(Node* cursor, int nodecount, bool first ){
    return 1;
}
