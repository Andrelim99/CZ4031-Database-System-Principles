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

struct key_struct{
    float key;
    vector <void*> address;
};
class Node
{
private:
    bool isLeaf;
    key_struct *keys;
    int num_keys;
    Node** node_ptr;
    friend class BPlusTree;
public:
    Node(int max_keys)
    {
        // Ptr|Key|Ptr...|Key|Ptr
        keys = new key_struct[max_keys];
        node_ptr = new Node*[max_keys + 1];
        isLeaf = true;
    }
};
class BPlusTree
{
private:
    Node *root;
    void insertInternal(key_struct x, Node* cur, Node* child)
    {
        // insert into internal (non-leaf) node
    }
    void removeInternal(key_struct x, Node* cur, Node* child)
    {
        // remove from internal (non-leaf) node
    }
    Node* findParent(Node* cur, Node* child)
    {

    }
public:
    BPlusTree()
    {
        root = NULL;
    }
    int get_height(Node* cur){
        if (cur->isLeaf == false){
            return get_height(cur->node_ptr[0])+1;
        }
        else if(cur->isLeaf==true){
            return 1;
        }
        return NULL;
    }
    Node* get_root()
    {
        return root;
    }
    Node* search(float search_key)
    {
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
    void insert(key_struct x)
    {

    }
    void remove(key_struct x)
    {

    }
    int display(Node* cursor, int nodecount, bool first) {

    }
};