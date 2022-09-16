#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <string.h>
#include "MemoryPool.h"
#include "BPlusTree.h"


using namespace std;



int main() {

    MemoryPool mempool = MemoryPool(200);


    ifstream dataFile("../Project1/data.tsv");

//Loading of data into memory
    cout << "Loading dataset into memory..." << endl;
    if(dataFile.is_open()){
        string line;
//        Remove header
        getline(dataFile, line);
        int counter = 0;
        while(getline(dataFile, line)
        && counter < 21
        ){
            stringstream s(line);
            string s1, s2, s3;
            s >> s1 >> s2 >> s3;
            Record newRecord;
            strcpy(newRecord.tconst, line.substr(0, line.find('\t')).c_str());
            newRecord.averageRating = stof(s2);
            newRecord.numVotes = stoi(s3);
//            cout << "Inserting Record: " << newRecord.tconst << endl;
            if(!mempool.insertRecord(newRecord)){
                cout << "Record not inserted!" << endl;
            }
            counter++;
        }
        dataFile.close();

        cout << "Loading done!" << endl << endl;

        mempool.displayFirstNBlocks(5);
    }

    mempool.printMemoryPoolDetails();

//    Get all record address via block (Shows block access)
    int blockIndex = 0;
    tuple<void*, Block> tmpTuple = mempool.getBlockI(blockIndex);
    Block tmpBlk = get<1>(tmpTuple);

/*
 * Iterate through all non-empty blocks
 * Save address and numVotes
 * Insert into B+ Tree
 */
    while(tmpBlk.numRecords > 0){
//        Save records into B+ tree
        for(int i = 0;  i < tmpBlk.numRecords; i++){
            key_struct keyStruct;
//            Pass ADDRESS of RECORD and numVotes
            keyStruct.address.push_back((unsigned char *)get<0>(tmpTuple) + i * 20);
            Record tmpRec = mempool.getRecord(blockIndex, i);
            keyStruct.key = tmpRec.numVotes;

//            Testing
//            cout << "Address: " << keyStruct.address[0] << "\t" << "Key value: " << keyStruct.key << endl;


//            Call insert key to B+ tree function here!



        }



        blockIndex++;
//        Get next tuple
        tmpTuple = mempool.getBlockI(blockIndex);
        tmpBlk = get<1>(tmpTuple);
    }



    return 0;


}