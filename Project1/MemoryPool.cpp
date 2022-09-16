//
// Created by Andre on 10/9/2022.
//

/*
 * Accessing of records will be done by blockStartPointer + block * blockSize + offset
 */

#include <iostream>
#include "MemoryPool.h"
#include "vector"

using namespace std;

MemoryPool::MemoryPool(int blockSize) {
    this->blockSize = blockSize;
    this->numBlocks = DISK_CAPACITY/blockSize;
    this->availableNumBlocks = numBlocks;
    this->dbSize = 0;
    this->startMemoryPtr = new unsigned char[DISK_CAPACITY];
    this->curBlockIndex = 0;

    //    Declare blocks array
    for(int i = 0; i < numBlocks; i++){
        Block newBlock;
//        newBlock.recPointer = nullptr;
        newBlock.numRecords = 0;
        newBlock.remainingCapacity = blockSize;
        this->blocks.push_back(newBlock);
    }
}




bool MemoryPool::insertRecord(Record record) {
    int recSize = sizeof(record);
    if(recSize != RECORD_SIZE) {
        cout << "Size mismatch!" << recSize << endl;
        return false;
    }

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


    Block *currentBlock = &blocks[curBlockIndex];
//    if can place record in current block
    if(currentBlock->remainingCapacity < RECORD_SIZE){
        curBlockIndex++;
        availableNumBlocks--;
        currentBlock = &blocks[curBlockIndex];
    }

//        copy contents of record into block
   if(saveRecord(currentBlock->numRecords, curBlockIndex,  &record)){
       //        update all values
       currentBlock->remainingCapacity -= RECORD_SIZE;
       currentBlock->numRecords++;
       dbSize += recSize;
       return true;
   }
   return false;
}

void MemoryPool::displayFirstNBlocks(int n) {
    if(n == -1) n = curBlockIndex;
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

void MemoryPool::displayBlock(int blockIndex) {
    cout << "#####################################"<< endl;
    cout << "Block " << blockIndex << " contains " << blocks[blockIndex].numRecords << " records" << endl;
    for(int k = 0; k < blocks[blockIndex].numRecords; k++){
//            Access pointer and print records
        void *ptr = startMemoryPtr + (blockIndex * blockSize) + k * RECORD_SIZE;
        Record * rPtr = (Record *) ptr;
        cout << rPtr->tconst << "\t" << rPtr->numVotes << "\t" << rPtr->averageRating << endl;
//

    cout << "-----------------------------------"<< endl;
}
//    cout << "Block " << i << ": "<< endl;
//    Print records in block

}

bool MemoryPool::saveRecord(int numRecords, int blkIndex, Record *rec) {
    try {
//    Copy
        void* recAddr = (startMemoryPtr) + (blkIndex * blockSize) + (RECORD_SIZE * numRecords);
        memcpy(recAddr, rec, RECORD_SIZE);
//        Record *test = (Record *) recAddr;
//        cout << test->tconst << " " << test->numVotes << " " << test->averageRating << endl;
        return true;

    } catch(...){
        cout << "error saving record!" << endl;
        return false;
    }

}
