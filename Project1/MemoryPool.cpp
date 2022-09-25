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

const int MB = 1000000;

MemoryPool::MemoryPool(int blockSize) {
    this->blockSize = blockSize;
    this->numBlocks = DISK_CAPACITY/blockSize;
    this->availableNumBlocks = numBlocks;
    this->dbSize = 0;
    this->startMemoryPtr = new unsigned char[DISK_CAPACITY];
    this->curBlockIndex = 0;
    this->blockAccessCounter = 0;

    //    Declare blocks array
    for(int i = 0; i < numBlocks; i++){
        Block newBlock;
//        newBlock.recPointer = nullptr;
        newBlock.numRecords = 0;
        newBlock.remainingCapacity = blockSize;
        newBlock.hasBeenAccessed = false;
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

// Data retrieval
tuple<void *, Block> MemoryPool::getBlockI(int i) {
    void *blockAddr = startMemoryPtr + (i * blockSize);
    Block blk = blocks[i];

    return tuple<void *, Block>(blockAddr, blk);
}

Record MemoryPool::getRecord(int blockID, int offset) {
    void *ptr1 = startMemoryPtr + blockID * blockSize + offset * RECORD_SIZE;
    Record *ptr = (Record *) ptr1;
//    cout << ptr->tconst << "\t" << ptr->numVotes << "\t" << ptr->averageRating << endl;
    return *ptr;
}

//Display
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
}

void MemoryPool::displayFirstNBlocks(int n) {
    if(n == -1) n = curBlockIndex;
    cout << "Printing first " << n << " blocks: " << endl;
    for(int i = 0; i < n; i++){
        displayBlock(i);
    }
}

void MemoryPool::displayBlocksFromXToY(int x, int y) {
    if(y > curBlockIndex){
        cout << "Block " << y << "exceeds current max block of " << curBlockIndex << endl;
        y = curBlockIndex;
    }
    cout << "Printing Blocks from block " << x << " to " << y << endl;
    for(int i = x; i < y; i++){
        displayBlock(i);
    }
}

void MemoryPool::displayDatablockBasedOnRecordAddress(void *ptr) {
    tuple<int, int> converted = convertRecordAddressToBlockOffset(ptr);
    int blockId = get<0>(converted);
    displayBlock(blockId);
}

void MemoryPool::displayBlocksAccessed() {
    unsigned int vecSize = blockAccessedList.size();
    cout << "Number of unique blocks accessed: " << vecSize << endl;
    if(vecSize > 5) vecSize = 5;
    cout << "Printing " << vecSize << " datablocks:" << endl;
    for(unsigned int i = 0; i < vecSize; i++){
        displayBlock(i);
    }
//    Reset
    resetBlocksAccessed();
}

void MemoryPool::printMemoryPoolDetails() {
    cout << "Printing Memory Pool Details..." << endl;
    cout << "Total Memory Capacity: " << DISK_CAPACITY/MB << "MB" << endl;
    cout << "Available Memory Capacity: " << (float)(DISK_CAPACITY - dbSize)/MB << "MB" << endl;
    cout << "Memory Used (DB Size): " << (float)dbSize/MB << "MB" << endl << endl;

    cout << "Block size: " << blockSize << "B" << endl;
    cout << "Total number of blocks: " << numBlocks << endl;
    cout << "Number of blocks used: " << curBlockIndex+1 << endl << endl;

    cout << "Record size: " << RECORD_SIZE << "B" << endl;
}


void MemoryPool::computeDatablockAccessed(vector<void *> recAddresses) {
/*
 * For each rec address
 * Find out which block
 * Change block access tag to true
 * increment counter
 */
    unsigned int vecSize = recAddresses.size();
    for(unsigned int i = 0; i < vecSize; i++){
        tuple<int, int> curTuple = convertRecordAddressToBlockOffset(recAddresses[i]);
        int tmpBlockId = get<0>(curTuple);

        if(blocks[tmpBlockId].hasBeenAccessed == false){
            blockAccessCounter++;
            blocks[tmpBlockId].hasBeenAccessed = true;
            blockAccessedList.push_back(tmpBlockId);
        }
    }
}

void MemoryPool::resetBlocksAccessed() {
    /*
     * Reset all blocks accessed value, Access Counter, blockAccessedList
     */
    unsigned int vecSize = blockAccessedList.size();
    for(unsigned int i = 0; i < vecSize; i++){
        blocks[blockAccessedList[i]].hasBeenAccessed = false;
    }
    blockAccessedList.clear();
    blockAccessCounter = 0;
}




tuple<int, int> MemoryPool::convertRecordAddressToBlockOffset(void *ptr) {
//    Find block id
    unsigned char * ptrConvert = (unsigned char *) ptr;
    int blockID = (ptrConvert - startMemoryPtr) / blockSize;
    int offset =  ((ptrConvert - startMemoryPtr) % blockSize) / RECORD_SIZE;

//    cout << "Block ID: " << blockID << " Offset: " << offset<< endl;

    return tuple<int, int>(blockID, offset);
}


int MemoryPool::getNumBlocks() {
    return numBlocks;
}


int MemoryPool::getDbSize() {
    return dbSize;
}

