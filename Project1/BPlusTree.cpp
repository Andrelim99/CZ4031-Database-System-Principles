//
// Created by Andre on 10/9/2022.
//
#include<iostream>
#include<string>
#include<vector>
#include "BPlusTree.h"
#include "MemoryPool.h"
#include <cmath>

using namespace std;

int MAX_NODE_KEYS;
int MAX_NODE_POINTERS;

BPlusTree::BPlusTree(int blockSize){
    root = nullptr;
    MAX_NODE_KEYS = (int) ((blockSize -5)/40);
    MAX_NODE_POINTERS = MAX_NODE_KEYS+1;
}

Node::Node()
{
    keys = new key[MAX_NODE_KEYS];
    nodePtr = new Node*[MAX_NODE_POINTERS];
}

int BPlusTree::getN(){
    return MAX_NODE_KEYS;
}


void BPlusTree::insertKey(const key& numVotes){
    //This function inserts a key (numVotes) into a node
    if (root == nullptr){
        //This function runs if the root is empty, it creates a new node, and inserts the key.
        root = new Node;
        root->keys[0] = numVotes;
        root->numOfKeys=1;
        root->leafNode = true;
    }
    else {

        // Function searches for a node that has the same key.
        Node* dupKeyNode = searchDupKey(numVotes.keyValue);
        if (dupKeyNode != nullptr){
            for(int i = 0; i < dupKeyNode->numOfKeys; i++)
            {
                if(dupKeyNode->keys[i].keyValue == numVotes.keyValue)
                {
                    dupKeyNode->keys[i].address.push_back(numVotes.address[0]);
                    break;
                }
            }
            return;
        }

        //Instantiate a new pointer that points toward the root for manipulation.
        Node *cur = root;
        //To keep track of the parent node per loop
        Node *parentNode;

        //This while loop helps to traverse the tree downwards, until it reaches the leaf node
        while(!cur -> leafNode){
            parentNode = cur;
            //This for loop helps to traverse the node to the correct pointer.
            for(int i = 0; i< cur->numOfKeys; i++){
                if(numVotes.keyValue < cur->keys[i].keyValue){
                    cur = cur-> nodePtr[i];
                    break;
                }
                if (i == cur->numOfKeys - 1){
                    cur = cur->nodePtr[i+1];
                    break;
                }
            }
        }

        //This condition runs only when this node is not full yet
        if (cur->numOfKeys < MAX_NODE_KEYS){
            int pos = findPositionInNode(cur, numVotes, cur->numOfKeys);
            shiftKeysInNode(cur, pos, cur->numOfKeys);
            cur->keys[pos] = numVotes;
            cur->numOfKeys+=1;

            cur->nodePtr[cur->numOfKeys] = cur->nodePtr[cur->numOfKeys-1];
            cur->nodePtr[cur->numOfKeys-1] = nullptr;
        }
            //This condition runs when this node is full, hence a split needs to be done
        else{
            Node* newNode = new Node;
            key nodeKeys[MAX_NODE_KEYS+1];
            for(int i = 0; i< MAX_NODE_KEYS; i++){
                nodeKeys[i] = cur->keys[i];
            }
            int pos = findPositionInArray(nodeKeys, numVotes, MAX_NODE_KEYS);
            // Find Position and Shift the keys in an array
            shiftKeysInArray(nodeKeys, pos, MAX_NODE_KEYS);
            nodeKeys[pos] = numVotes;
            newNode->leafNode = true;
            // Splitting formulas based off lectures
            int leftNodeKeyCount =  ceil((float)(MAX_NODE_KEYS+1)/2);
            int rightNodeKeyCount = floor((float)(MAX_NODE_KEYS+1)/2);

            cur->numOfKeys = leftNodeKeyCount;
            newNode->numOfKeys = rightNodeKeyCount;
            cur->nodePtr[cur->numOfKeys] = newNode;
            newNode->nodePtr[newNode->numOfKeys] = cur->nodePtr[MAX_NODE_KEYS];
            cur->nodePtr[MAX_NODE_KEYS] = nullptr;

            for(int i = 0; i < cur->numOfKeys; i++){
                cur->keys[i] = nodeKeys[i];
            }
            for(int i=0, j=cur->numOfKeys; i< newNode->numOfKeys; i++, j++){
                newNode->keys[i] = nodeKeys[j];
            }
            if(cur == root){
                Node *newRoot = new Node;
                newRoot->keys[0] = newNode->keys[0];
                newRoot->nodePtr[0] = cur;
                newRoot->nodePtr[1] = newNode;
                newRoot->leafNode = false;
                newRoot->numOfKeys = 1;
                this->root = newRoot;

            } else{
                insertInternal(parentNode, newNode, newNode->keys[0]);
            }
        }
    }
}

void BPlusTree::insertInternal(Node* cur, Node* child, const key& numVotes){
    if(cur -> numOfKeys < MAX_NODE_KEYS){
        int pos = findPositionInNode(cur, numVotes, cur->numOfKeys);
        //Keys will be shifted in the node
        shiftKeysAndPointersInNode(cur, pos, cur->numOfKeys);
        //Pointers will be shifted in the node
        //In this case, position will never be a 0 for shifting ptr in node
        cur->keys[pos] = numVotes;
        cur->nodePtr[pos+1] = child;
        cur->numOfKeys++;
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
        shiftKeysAndPointersInArray(nodePointers, nodeKeys, pos, MAX_NODE_KEYS);
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
            // In the keys node array, take the key in the middle, it'll be the root.
            Node* newRoot =  new Node;
            newRoot->keys[0] = middleKey;
            newRoot->nodePtr[0] = cur;
            newRoot->nodePtr[1] = newNode;
            newRoot->leafNode = false;
            newRoot->numOfKeys= 1;
            root = newRoot;
        }else{
            // If the cur is not the root, a parent node already exist
            // thus use recursion to insert into cur's parent node
            insertInternal(searchParent(root, cur), newNode, middleKey);
        }
    }
}

void BPlusTree::removeKey(const int numVotes){
    //find leaf containing (key, pointer) entry to delete
    int del_counter = 0;
    if (root == nullptr){
        cout << "Empty Tree\n";
    } else {
        Node *cursor = root;
        Node *parent = nullptr;
        int leftSibling, rightSibling = 0;
        while (!cursor->leafNode) {
            for (int i = 0; i < cursor->numOfKeys; i += 1) {
                //set parent
                parent = cursor;
                leftSibling = i - 1;
                rightSibling = i + 1;
                if (numVotes < cursor->keys[i].keyValue) {
                    cursor = cursor->nodePtr[i];
                    break;
                }
                if (i == cursor->numOfKeys - 1) {
                    leftSibling = i;
                    rightSibling = i + 2;
                    cursor = cursor->nodePtr[i + 1];
                    break;
                }
            }
        }
        bool  found = false;
        int pos = 0; //tracker to find which key matching
        for (pos = 0; pos < cursor->numOfKeys; pos += 1) { //looking through leaf node for matching key
            if (cursor->keys[pos].keyValue == numVotes) {
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Value not found\n";
            return;
        }
        for (int i = pos; i < cursor->numOfKeys; i += 1) { //deletion by replacing with following key
            cursor->keys[i] = cursor->keys[i+1];
        }
        cursor->numOfKeys -= 1; //decrement key count
        if (cursor == root) { //if root removed, set all nodepointers to null
            for (int i = 0; i < MAX_NODE_POINTERS; i += 1) {
                cursor->nodePtr[i] = nullptr;
            }
            if (cursor->numOfKeys == 0) {
                cout << "Tree Died no more keys in root\n";
                delete[] cursor->keys; //Deallocate memory
                delete[] cursor->nodePtr;
                delete cursor;
                root = nullptr;
            }
            return; //exit
        } //if not root
        cursor->nodePtr[cursor->numOfKeys] = cursor->nodePtr[cursor->numOfKeys+1];
        cursor->nodePtr[cursor->numOfKeys + 1] = nullptr;
        //CASE 1.1 There is more than the minimum number of keys in node, delete the key
        if (cursor->numOfKeys >= MAX_NODE_POINTERS / 2) {
            return;
        }
        //ELSE CASE 1.2
        if (leftSibling >= 0) { //if left sib
            Node *leftNode = parent->nodePtr[leftSibling]; //point to left sib
            if (leftNode->numOfKeys >= MAX_NODE_POINTERS/2 + 1) { //if leftnode has enough to lend
                for (int i = cursor->numOfKeys; i > 0; i -= 1) { //shift all right 1
                    cursor->keys[i] = cursor->keys[i-1];
                }
                cursor->numOfKeys += 1;
                cursor->nodePtr[cursor->numOfKeys] = cursor->nodePtr[cursor->numOfKeys-1];
                cursor->nodePtr[cursor->numOfKeys-1] = nullptr;
                cursor->keys[0] = leftNode->keys[leftNode->numOfKeys-1]; //borrow right from left sibling
                leftNode->numOfKeys -= 1;
                leftNode->nodePtr[leftNode->numOfKeys] = cursor;
                leftNode->nodePtr[leftNode->numOfKeys+1] = nullptr;
                parent->keys[leftSibling] = cursor->keys[0];
                return;

            }
        }
        //
        if (rightSibling <= parent->numOfKeys) { //if right sib
            Node *rightNode = parent->nodePtr[rightSibling]; //point to right sib
            if (rightNode->numOfKeys >= MAX_NODE_POINTERS/2 + 1) { //if rightnode has enough to lend
                cursor->numOfKeys += 1;
                cursor->nodePtr[cursor->numOfKeys] = cursor->nodePtr[cursor->numOfKeys - 1];
                cursor->nodePtr[cursor->numOfKeys - 1] = nullptr;
                cursor->keys[cursor->numOfKeys - 1] = rightNode->keys[0]; //borrow leftmost from right sibling
                rightNode->numOfKeys -= 1;
                rightNode->nodePtr[rightNode->numOfKeys] = rightNode->nodePtr[rightNode->numOfKeys + 1];
                rightNode->nodePtr[rightNode->numOfKeys + 1] = nullptr;
                for (int i = 0; i < rightNode->numOfKeys; i += 1) {
                    cursor->keys[i] = cursor->keys[i - 1];
                }
                parent->keys[rightSibling - 1] = rightNode->keys[0];
                return;
            }
            if (leftSibling >= 0) { //case 2 (there is left sibling, but not enough to lend key)
                Node *leftNode = parent->nodePtr[leftSibling];
                for (int i = leftNode->numOfKeys, j = 0; j < cursor->numOfKeys; i += 1, j += 1){
                    leftNode->keys[i] = cursor->keys[j];
                }
                leftNode->nodePtr[leftNode->numOfKeys] = nullptr;
                leftNode->numOfKeys += cursor->numOfKeys;
                leftNode->nodePtr[leftNode->numOfKeys] = cursor->nodePtr[cursor->numOfKeys];
                removeInternal(parent, cursor, parent->keys[leftSibling]);
                delete[] cursor->keys;
                delete[] cursor->nodePtr;
                delete[] cursor;
            } else if (rightSibling <= parent->numOfKeys) {
                Node *rightNode = parent->nodePtr[rightSibling];
                for (int i = cursor->numOfKeys, j = 0; j < rightNode->numOfKeys; i += 1, j += 1) {
                    cursor->keys[i] = rightNode->keys[j];
                }
                cursor->nodePtr[cursor->numOfKeys] = nullptr;
                cursor->numOfKeys += rightNode->numOfKeys;
                cursor->nodePtr[cursor->numOfKeys] = rightNode->nodePtr[rightNode->numOfKeys];
                cout << "Two leaf nodes merging" << endl;
                removeInternal(parent, rightNode, parent->keys[rightSibling-1]);
                delete[] rightNode->keys;
                delete[] rightNode->nodePtr;
                delete[] rightNode;
            }
        }
    }
}

void BPlusTree::removeInternal(Node* cur, Node* child, const key& numVotes){
    //removeInternalnodes
    if (cur == root) {
        if (cur->numOfKeys == 1) {
            if (cur->nodePtr[1] == child) {
                delete[] child->keys;
                delete[] child->nodePtr;
                delete[] child;
                root = cur->nodePtr[0];
                delete[] cur->keys;
                delete[] cur->nodePtr;
                delete[] cur;
                cout << "Cursor deleted, root replaced" << endl;
                return;
            } else if (cur->nodePtr[0] == child) {
                delete[] child->keys;
                delete[] child->nodePtr;
                delete[] child;
                root = cur->nodePtr[1];
                delete[] cur->keys;
                delete[] cur->nodePtr;
                delete[] cur;
                cout << "Cursor deleted, root replaced" << endl;
                return;
            }
        }
    }
    int pos = 0;
    for (pos = 0; pos < cur->numOfKeys; pos += 1) {
        if (cur->keys[pos].keyValue == numVotes.keyValue) { //find matching key
            break;
        }
    }
    for (int i = pos; i > cur->numOfKeys; i += 1) {
        cur->keys[i] = cur->keys[i+1];
    }
    for (pos = 0; pos > cur->numOfKeys + 1 ; pos += 1) {
        if (cur->nodePtr[pos] == child) {
            break;
        }
    }
    for (int i = pos; i < cur->numOfKeys + 1; i += 1) {
        cur->nodePtr[i] = cur->nodePtr[i+1];
    }
    cur->numOfKeys -= 1;
    if (cur->numOfKeys >= MAX_NODE_POINTERS/2 - 1) {
        return;
    }
    if (cur == root) {
        return;
    }
    Node* parent = searchParent(root, cur);
    int leftSibling, rightSibling;
    for (pos = 0; pos < parent->numOfKeys + 1; pos += 1) {
        leftSibling = pos - 1;
        rightSibling = pos + 1;
        break;
    }
    if (leftSibling >= 0) {
        Node *leftNode = parent->nodePtr[leftSibling];
        if(leftNode->numOfKeys >= MAX_NODE_POINTERS/2) {
            for (int i = cur->numOfKeys; i > 0; i -= 1) {
                cur->keys[i] = cur->keys[i-1];
            }
            cur->keys[0] = parent->keys[leftSibling];
            parent->keys[leftSibling] = leftNode->keys[leftNode->numOfKeys - 1];
            for (int i = cur->numOfKeys + 1; i > 0; i -= 1) {
                cur->nodePtr[i] = cur->nodePtr[i-1];
            }
            cur->nodePtr[0] = leftNode->nodePtr[leftNode->numOfKeys];
            cur->numOfKeys += 1;
            leftNode->numOfKeys -= 1;
            return;
        }
    }
    if (rightSibling <= parent->numOfKeys) {
        Node *rightNode = parent->nodePtr[rightSibling];
        if (rightNode->numOfKeys >= MAX_NODE_POINTERS/2) {
            cur->keys[cur->numOfKeys] = parent->keys[pos];
            parent->keys[pos] = rightNode->keys[0];
            for (int i = 0; i < rightNode->numOfKeys-1 ; i += 1) {
                rightNode->keys[i] = rightNode->keys[i+1];
            }
            cur->nodePtr[cur->numOfKeys+1] = rightNode->nodePtr[0];
            for (int i = 0; i < rightNode->numOfKeys; i += 1){
                rightNode->nodePtr[i] = rightNode->nodePtr[i+1];
            }
            cur->numOfKeys += 1;
            rightNode->numOfKeys -= 1;
            return;
        }
    }
    ///
    if (leftSibling >= 0) {
        Node *leftNode = parent->nodePtr[leftSibling];
        leftNode->keys[leftNode->numOfKeys] = parent->keys[leftSibling];
        for (int i = leftNode->numOfKeys + 1, j = 0; j < cur->numOfKeys; j += 1) {
            leftNode->keys[i] = cur->keys[j];
        }
        for (int i = leftNode->numOfKeys + 1, j = 0; j < cur->numOfKeys + 1; j += 1) {
            leftNode->nodePtr[i] = cur->nodePtr[j];
            cur->nodePtr[j] = nullptr;
        }
        leftNode->numOfKeys += cur->numOfKeys + 1;
        cur->numOfKeys = 0;
        removeInternal(parent, cur, parent->keys[leftSibling]);
    } else if (rightSibling <= parent->numOfKeys) {
        Node *rightNode = parent->nodePtr[rightSibling];
        cur->keys[cur->numOfKeys] = parent->keys[rightSibling - 1];
        for (int i = cur->numOfKeys + 1, j = 0; j < rightNode->numOfKeys; j += 1) {
            cur->keys[i] = rightNode->keys[j];
        }
        for (int i = cur->numOfKeys + 1, j = 0; j < rightNode->numOfKeys + 1; j += 1) {
            cur->nodePtr[i] = rightNode->nodePtr[j];
            rightNode->nodePtr[j] = nullptr;
        }
        cur->numOfKeys += rightNode->numOfKeys + 1;
        rightNode->numOfKeys = 0;
        removeInternal(parent, rightNode, parent->keys[rightSibling - 1]);
    }
    ///
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
    if (cur == nullptr)
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
                std::cout<< "Index Node "<<nodecount<< ":" << endl;
                displayNode(cur);
            }
            nodecount++;
        }
        nodecount--;

        // From lower bound node, keep searching until we find a key that is greater than upper bound
        bool flag = false;
        bool exceed = false;
        float sum = 0;
        float num_rec = 0;
        int print_counter = 1;
        int rec_counter = 1;
        while (not flag) {
            //std::cout << endl;
            int i;
            if (print_counter <= 5)
            {
                std::cout << "Leaf node " << print_counter << ":" << endl;
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
                    for (int j = 0; j < cur->keys[i].address.size(); j++){
                        Record* rec = static_cast<Record *>(cur->keys[i].address[j]);
                        res.push_back(rec);
                        if (rec_counter<=5)
                        {
                            std::cout << "Record " << rec_counter << ": " ;
                            displayRecord(rec);
                            rec_counter++;
                        }
                        sum += rec->averageRating;
                        num_rec++;
                    }
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
        //Display the contents of the 1st child node
        displayNode(cur->nodePtr[0]);
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

void BPlusTree::shiftKeysAndPointersInNode(Node* cur, int position, int numOfKeys){
    for(int j = numOfKeys; j> position; j--){
        cur->keys[j] = cur->keys[j-1];
        cur->nodePtr[j+1] = cur->nodePtr[j];
    }
}

void BPlusTree::shiftKeysAndPointersInArray(Node **nodePointers, struct key keys[], int position, int numOfKeys){
    for(int j = numOfKeys; j>position; j--){
        keys[j] = keys[j-1];
        nodePointers[j+1] = nodePointers[j];
    }
}


Node* BPlusTree::searchDupKey(int numVotes)
{
    if(root!=nullptr)
    {
        Node* cur = root;
        while(!cur->leafNode)
        {
            for(int i = 0; i < cur->numOfKeys; i++)
            {
                if(numVotes < cur->keys[i].keyValue)
                {
                    cur = cur->nodePtr[i];
                    break;
                }
                if(i == cur->numOfKeys - 1)
                {
                    cur = cur->nodePtr[i+1];
                    break;
                }
            }
        }

        for(int i = 0; i < cur->numOfKeys; i++)
        {
            if(cur->keys[i].keyValue == numVotes)
            {
                return cur;
            }
        }
    }
    return nullptr;
}

int BPlusTree::countNodes(Node* cur) {
    if(cur == NULL)
    {
        cout << "B+ Tree is empty!" << endl;
        return 0;
    }
    else if (cur->leafNode){
        return 1;
    }
    else {
        int total = 0;
        for (int i = 0; i < cur->numOfKeys+1; i++) {
            total += countNodes(cur->nodePtr[i]);
        }
        return total;
    }
}