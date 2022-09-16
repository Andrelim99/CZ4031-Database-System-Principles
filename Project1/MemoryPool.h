//
// Created by Andre on 10/9/2022.
//



#ifndef CZ4031_PROJ1_MEMORYPOOL_H
#define CZ4031_PROJ1_MEMORYPOOL_H

#include "vector"
#include "tuple"
using namespace std;

struct Record{
    char tconst[10];
//    int tconst;
    float averageRating;
    int numVotes;
};


struct RecAddr{
    int blockID;
    int offset;
};

struct Block{
//    Record * recPointer;
    int remainingCapacity;
    int numRecords;
};


class MemoryPool {
private:
    int numBlocks, blockSize, dbSize, availableNumBlocks, curBlockIndex;
    unsigned char * startBlockPtr, *startMemoryPtr;
    vector<Block> blocks;




    bool saveRecord(int numRecords, int blkIndex, Record * rec);

public:
    const int DISK_CAPACITY = 200000000; // 200MB, not using standard 2^20 value for simplicity
    const int RECORD_SIZE = 20;
    MemoryPool(int blockSize);

//    ~MemoryPool();

    bool insertRecord(Record record);

    void displayFirstNBlocks(int n);
    void displayBlock(int i);
    void displayDatablockBasedOnRecordAddress(void * ptr);

    void printMemoryPoolDetails();

    tuple<int, int> convertRecordAddressToBlockOffset(void * ptr);

    bool deleteRecord(unsigned char * addr);

    int getNumBlocks();

    tuple<void *, Block> getBlockI(int i);

    Record getRecord(int blockID, int offset);

    vector<void *> getAllRecordAddress();

    int getDbSize();


};


#endif //CZ4031_PROJ1_MEMORYPOOL_H
