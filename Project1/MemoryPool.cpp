//
// Created by Andre on 10/9/2022.
//

#include <iostream>
#include "MemoryPool.h"
#include "vector"

using namespace std;

MemoryPool ::MemoryPool(int blockSize) {
    this->blockSize = blockSize;
    this->numBlocks = DISK_CAPACITY/blockSize;
    this->dbSize = 0;

//    Declare blocks array

    this->startBlockPtr = &blocks[0];

//    Testing
    Record rec1, rec2;
    rec1.averageRating = 1;
    rec1.numVotes = 1;
    rec1.tconst[0] = '1';
    rec1.tconst[1] = '\0';

    rec2.averageRating = 2;
    rec2.numVotes = 2;
    rec2.tconst[0] = '2';
    rec2.tconst[1] = '\0';

//    Create empty blocks
    for(int i = 0; i < 2; i++){
        Block block;
        block.remainingCapacity = blockSize;
        block.numRecords = 0;
//        block.recPointer = nullptr;
        if(i == 0)  block.recPointer = &rec1;
        else block.recPointer = &rec2;
//        Access startPointerAddress
        blocks.push_back(block);
    }

    for(int i = 0; i < 2; i++){
        cout << blocks[i].recPointer->tconst << endl;
    }


}




bool MemoryPool::insertRecord(Record record) {
    int recSize = sizeof(record);

//    Sanity Check(s)
//    No more space
    if(DISK_CAPACITY - dbSize <= 0 || recSize > DISK_CAPACITY - dbSize){
        cout << "Not enough space" << endl;
        return false;
    }

    if(recSize > blockSize){
        cout << "recSize is larger than blockSize" << endl;
        return false;
    }

//    Find a recAddr = {block, offset} for record
//    Copy contents of record into &block + offset




    return true;
}

void MemoryPool::displayNBlocks(int n) {
    for(int i = 0; i < n; i++){
        displayBlock(i);
    }
}

int MemoryPool::getNumBlocks() {
    return numBlocks;
}

int MemoryPool::getDbSize() {
    return dbSize;
}

void MemoryPool::displayBlock(int i) {
    cout << "Block " << i << ": "<< endl;
//    Print records in block

}