//
// Created by Andre on 10/9/2022.
//

#ifndef CZ4031_PROJ1_MEMORYPOOL_H
#define CZ4031_PROJ1_MEMORYPOOL_H

#include "vector"
using namespace std;

struct Record{
    char tconst[10];
    float averageRating;
    int numVotes;
};


struct RecAddr{
    int blockID;
    int offset;
};

struct Block{
    Record * recPointer;
    int remainingCapacity;
    int numRecords;
};


class MemoryPool {
private:
    int numBlocks, blockSize, dbSize;
    Block * startBlockPtr, **curBlockPtr;
    vector<Block> blocks;


    void displayBlock(int i);



public:
    const int DISK_CAPACITY = 200000000; // 200MB, not using standard 2^20 value for simplicity
    MemoryPool(int blockSize);

//    ~MemoryPool();

    bool insertRecord(Record record);

    void displayNBlocks(int n);

    int getNumBlocks();

    int getDbSize();

};


#endif //CZ4031_PROJ1_MEMORYPOOL_H
