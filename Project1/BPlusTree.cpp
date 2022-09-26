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
#include <cmath>
using namespace std;

int MAX_NODE_KEYS = 5;
int MAX_NODE_POINTERS = MAX_NODE_KEYS + 1;

Node::Node()
{
    keys = new key[MAX_NODE_KEYS];
    nodePtr = new Node*[MAX_NODE_POINTERS];
}

BPlusTree::BPlusTree(){
    root = nullptr;
}

void BPlusTree::insertKey(const key& numVotes){
    //This function inserts a key (numVotes) into a node
    if (root == nullptr){
        //This function runs if the root is empty, it creates a new node, and inserts the key.
        root = new Node;
        root->keys[0] = numVotes;
        root->numOfKeys=1;
        root->leafNode = true;

        //Part of the function verified.
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
                //Might be subjected to changes
                if(numVotes.keyValue < cursor->keys[i].keyValue){
                    cursor = cursor -> nodePtr[i];
                    break;
                }
                if (i == cursor->numOfKeys - 1){
                    cursor = cursor->nodePtr[i+1];
                    break;
                }
            }
            //While loop is correct
        }

        //This condition runs only when this node is not full yet
        if (cursor->numOfKeys < MAX_NODE_KEYS){
            int pos = findPositionInNode(cursor, numVotes, cursor->numOfKeys);
            shiftKeysInNode(cursor, pos, cursor->numOfKeys);
            cursor->keys[pos] = numVotes;
            cursor->numOfKeys+=1;

            cursor->nodePtr[cursor->numOfKeys] = cursor->nodePtr[cursor->numOfKeys-1];
            cursor->nodePtr[cursor->numOfKeys-1] = nullptr;
        }
            //This condition runs when this node is full, hence a split needs to be done
        else{
            Node* newNode = new Node;
            key nodeKeys[MAX_NODE_KEYS+1];
            for(int i = 0; i< MAX_NODE_KEYS; i++){
                nodeKeys[i] = cursor->keys[i];
            }
            int pos = findPositionInArray(nodeKeys, numVotes, MAX_NODE_KEYS);
            shiftKeysInArray(nodeKeys, pos, MAX_NODE_KEYS);
            nodeKeys[pos] = numVotes;
            newNode->leafNode = true;
            int leftNodeKeyCount =  ceil((float)(MAX_NODE_KEYS+1)/2);
            int rightNodeKeyCount = floor((float)(MAX_NODE_KEYS+1)/2);
            cursor->numOfKeys = leftNodeKeyCount;
            newNode->numOfKeys = rightNodeKeyCount;
            cursor->nodePtr[cursor->numOfKeys] = newNode;
            newNode->nodePtr[newNode->numOfKeys] = cursor->nodePtr[MAX_NODE_KEYS];
            cursor->nodePtr[MAX_NODE_KEYS] = nullptr;

            for(int i = 0; i < cursor->numOfKeys; i++){
                cursor->keys[i] = nodeKeys[i];
            }
            for(int i=0, j=cursor->numOfKeys; i< newNode->numOfKeys; i++, j++){
                newNode->keys[i] = nodeKeys[j];
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
                //Go through the recursion
                insertInternal(parent, newNode, newNode->keys[0]);
            }
        }
    }
}

void BPlusTree::insertInternal(Node* cur, Node* child, const key& numVotes){
    if(cur -> numOfKeys < MAX_NODE_KEYS){
        int pos = findPositionInNode(cur, numVotes, cur->numOfKeys);
        //Keys will be shifted in the node
        shiftKeysInNode(cur, pos, cur->numOfKeys);
        //Pointers will be shifted in the node
        shiftPtrInNode(cur,pos+1, cur->numOfKeys+1);
        cur->keys[pos] = numVotes;
        cur->numOfKeys++;
        cur->nodePtr[pos+1] = child;
    } else{
        Node* newNode = new Node;
        key nodeKeys[MAX_NODE_KEYS+1];
        Node* nodePointers[MAX_NODE_POINTERS+1];
        for(int i=0; i<MAX_NODE_KEYS; i++){
            nodeKeys[i] = cur->keys[i];
        }
        for(int i=0; i< MAX_NODE_POINTERS; i++){
            nodePointers[i] = cur->nodePtr[i];
        }
        int pos = findPositionInArray(nodeKeys,numVotes,MAX_NODE_KEYS);
        shiftKeysInArray(nodeKeys,pos,MAX_NODE_KEYS);
        shiftPtrInArray(nodePointers, pos+1, MAX_NODE_POINTERS);
        nodeKeys[pos] = numVotes;
        nodePointers[pos+1] = child;
        newNode->leafNode = false;
        //Added a cast to the float as to get the actual ceiling and floor when the split is done.
        cur->numOfKeys = ceil((float)MAX_NODE_KEYS/2);
        newNode->numOfKeys = floor((float)MAX_NODE_KEYS/2);

        //From this point, we probably need to also reinitialize the cur node with the new key values
        //As well as change the pointer values if necessary. This is for the accuracy check

        //We also might need to set some additional pointers in the cur node to nullptr, as the other parent nodes will be taking child nodes from the
        //Left node.

        //We will always skip the middle key, because when we split the nodes up, cur will take ceil n/2 = 3, new node will take floor n/2 = 2;
        //Cur will keep index 0 1 2 , skip 3, newNode will keep 4, 5

        //Cur will keep pointers 0 1 2 3, newNode will keep 4 5 6.


        // For Reinitializing, left Node = cur, and cur will always have 3 keys, thus we keep i to maximum of 3, so it only takes 0 1 2 as index.
        for(int i = 0; i<cur->numOfKeys; i++){
            cur->keys[i] = nodeKeys[i];
        }

        //For Reinitializing, left Node = cur, and cur will have 3 keys, which means 3+1 = 4 pointers. Thus, we will keep i to a maximum of 4, so its
        //Takes 0 1 2 3 as index from the pointers. After doing this, initialize the remaining pointers as nullptrs.
        int keyLimit = cur->numOfKeys + 1;
        for(int i = 0; i< MAX_NODE_POINTERS; i++ ){
            cur->nodePtr[i] = (i<keyLimit) ? nodePointers[i] : nullptr;
        }

        // For the New Node, only take 4 and 5 keys. Hence, i will be kept to a maximum of 2 keys, so that is only has 0 1 as index.
        for(int i = 0, j = cur->numOfKeys+1; i<newNode->numOfKeys; i++, j++){
            newNode->keys[i] = nodeKeys[j];
        }

        // For the New Node, only take pointers 4,5,6. Hence, i will be kept to a maximum of keys+1 = 2+1 = 3 pointers.
        for(int i = 0, j= cur->numOfKeys+1; i<newNode->numOfKeys+1; i++, j++){
            newNode->nodePtr[i] = nodePointers[j];
        }

        int middle = (MAX_NODE_KEYS+1)/2;
        key middleKey = nodeKeys[middle];
        if(cur == root){
            //In the keys node array, take the key in the middle, it'll be the root.
            Node* newRoot =  new Node;
            newRoot->keys[0] = middleKey;
            newRoot->nodePtr[0] = cur;
            newRoot->nodePtr[1] = newNode;
            newRoot->leafNode = false;
            newRoot->numOfKeys= 1;
            root = newRoot;
        }else{
            insertInternal(searchParent(root, cur), newNode, middleKey);
        }
    }
}

void BPlusTree::removeKey(const key& numVotes){

}

void BPlusTree::removeInternal(Node* cur, Node* child, const key& numVotes){

}

Node* BPlusTree::searchParent(Node* cur, Node* child){
    Node *search;
    if(cur -> leafNode  || (cur->nodePtr[0])->leafNode){
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

vector<void*> BPlusTree::searchNode(int lower_bound, int upper_bound){
    vector<void*> res;
    if(root == NULL)
    {
        cout << "B+ Tree is empty!" << endl;
        return res;
    }
    else
    {
        Node* cur = root;
        std::cout<<"Root Node:"<<endl;
        displayNode(cur);
        int nodecount = 1;
        // Travel to leaf node
        while(!cur->leafNode)
        {
            bool flag = true;
            for(int i = 0; i < cur->numOfKeys; i++)
            {
                // if search key is smaller than a key in node
                if(lower_bound < cur->keys[i].keyValue)
                {
                    cur = cur->nodePtr[i];
                    flag = false;
                    break;
                }
            }
            // if search key is larger than all keys in node
            if (flag)
            {
                cur = cur->nodePtr[cur->numOfKeys];
            }
            // Only display content for the first 5 index nodes
            if (nodecount <= 5) {
                std::cout<< "Index Node: "<<nodecount<<endl;
                displayNode(cur);
            }
            nodecount++;
        }

        // From lower bound node, keep searching until we find a key that is greater than upper bound
        bool flag = false;
        bool exceed = false;
        float sum = 0;
        float num_rec = 0;
        int print_counter = 1;
        while (not flag) {
            //std::cout << endl;
            int i;
            if (print_counter <= 5)
            {
                std::cout << "Leaf node: " << print_counter << endl;
                displayNode(cur);
            }
            for (i = 0; i < cur->numOfKeys; i++) {
                // Found a key within range, now we need to iterate through the entire range until the upperBoundKey.
                if (cur->keys[i].keyValue > upper_bound) {
                    flag = true;
                    exceed = true;
                    break;
                }
                if (cur->keys[i].keyValue >= lower_bound && cur->keys[i].keyValue <= upper_bound)
                {
                    Record* rec = static_cast<Record *>(cur->keys[i].address[0]);
                    res.push_back(rec);
                    if (print_counter<=5)
                    {
                        displayRecord(rec);
                    }
                    sum += rec->averageRating;
                    num_rec++;
                }
            }

            // On the last pointer, check if last key is max, if it is, stop. Also stop if it is already equal to the max
            if ((cur->nodePtr[cur->numOfKeys] != nullptr) && (!exceed)) {
                // Set cursor to be next leaf node (load from disk).
                cur = cur->nodePtr[cur->numOfKeys];
            } else {
                flag = true;
            }
            print_counter++;
        }
        std::cout << "Number of index nodes accessed: " << nodecount << endl;
        if (num_rec == 0){
            std::cout << "No records found!" << endl;
        }
        else {
            std::cout << "Number of records: " << num_rec << endl;
            std::cout << "Average rating of 'averageRatings': " << sum / num_rec << endl;
        }
        return res;
    }
}

void BPlusTree::display(Node* cur){
    //Function to display the content of the root node and its 1st child nodes
    if (cur != NULL) {
        cout << "Root: " << endl;
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

void BPlusTree::displayRecord(Record* rec){
    // Displaying an individual record
    cout << rec->numVotes << " " << rec->tconst << " " << rec->averageRating << endl;
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
    cout<<endl;
}

int BPlusTree::findPositionInNode(Node* cur, const key& numVotes, int numOfKeys){
    int pos = 0;
    for(;pos < numOfKeys; pos++){
        if(numVotes.keyValue < cur->keys[pos].keyValue){
            break;
        }
    }
    return pos;
}

int BPlusTree::findPositionInArray(struct key keys[], const key& numVotes, int numOfKeys){
    int pos = 0;
    for(;pos < numOfKeys; pos++){
        if(numVotes.keyValue < keys[pos].keyValue){
            break;
        }
    }
    return pos;
}

void BPlusTree::shiftKeysInNode(Node* cur, int position, int numOfKeys){
    for(int j = numOfKeys; j> position; j--){
        cur->keys[j] = cur->keys[j-1];
    }
}

void BPlusTree::shiftKeysInArray(struct key keys[], int position, int numOfKeys){
    for(int j = numOfKeys; j>position; j--){
        keys[j] = keys[j-1];
    }
}

void BPlusTree::shiftPtrInNode(Node* cur, int position, int numOfKeys){
    for(int j =  numOfKeys; j>position; j--){
        cur->nodePtr[j] = cur->nodePtr[j-1];
    }
}

void BPlusTree::shiftPtrInArray(Node** nodePointers, int position, int numOfKeys){
    for(int j = numOfKeys; j>position; j--){
        nodePointers[j] = nodePointers[j-1];
    }
}