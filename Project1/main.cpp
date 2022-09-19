#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <string.h>
#include "MemoryPool.h"
#include "BPlusTree.h"
#include <typeinfo>


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

    //Initialize B+ tree to insert keys
    BPlusTree newTree;
    int count = 0;
    while(tmpBlk.numRecords > 0){
//        Save records into B+ tree
        for(int i = 0;  i < tmpBlk.numRecords; i++){
            key keyStruct;
//            Pass ADDRESS of RECORD and numVotes
            keyStruct.address.push_back((unsigned char *)get<0>(tmpTuple) + i * 20);
            Record tmpRec = mempool.getRecord(blockIndex, i);
            keyStruct.keyValue = tmpRec.numVotes;
            newTree.insertKey(keyStruct);
            count++;

//            Testing
//            cout << "Address: " << keyStruct.address[0] << "\t" << "Key value: " << keyStruct.key << endl;


//            Call insert key to B+ tree function here!



        }



        blockIndex++;
//        Get next tuple
        tmpTuple = mempool.getBlockI(blockIndex);
        tmpBlk = get<1>(tmpTuple);
    }
    /*
     Experiment 2: 
     Build a B+ tree on the attribute "numVotes" by inserting the records sequentially and report the following statistics:
     - The parameter n of the B+ tree;
     - The number of nodes of the B+ tree;
     - The height of the B+ tree, ie., the number of levels of the B+ tree;
     - The content of the root node and its 1st child node
    */

    cout << "Records inserted in the B+ tree: " << count << endl;

    cout << "-----------------Experiment 2-----------------" << endl;
    cout << "Parameter n of the B+ Tree      : " << endl;
    cout << "Number of nodes of the B+ Tree  : " << endl;
    cout << "Height of the B+ Tree           : " << newTree.getHeightOfTree(newTree.getRootOfTree()) << endl;
    cout << "Content of Root Node and its 1st Child : " << endl;
    newTree.display(newTree.getRootOfTree());

    /*
    Experiment 3:
    Retrieve those movies with the “numVotes” equal to 500 and report the following statistics:
    - The number and the content of index nodes the process accesses;
    - The number and the content of data blocks the process accesses;
    - The average of “averageRating’s” of the records that are returned;
    */

    cout << "-----------------Experiment 3-----------------" << endl;
    // retreiving number of index nodes accesedd as well as printing content of index nodes
    int num = newTree.searchNode(500);
    cout << "Number of index nodes the precess accessed : " << num << endl;
    cout << "Number of data blocks the precess accessed : " <<  "To Be Filled"  << endl;

    /*
    Experiment 4:
    Retrieve those movies with the attribute “numVotes” from 30,000 to 40,000, both inclusively and report the following statistics:
    - The number and the content of index nodes the process accesses;
    - The number and the content of data blocks the process accesses;
    - The average of “averageRating’s” of the records that are returned;
    */
     
    cout << "-----------------Experiment 4-----------------" << endl;

    /*
    Experiment 5:
    Delete those movies with the attribute “numVotes” equal to 1,000, update the B+ tree accordingly, and report the following statistics:   
    - The number of times that a node is deleted (or two nodes are merged) during the process of the updating the B+ tree;
    - The number nodes of the updated B+ tree;
    - The height of the updated B+ tree;
    - The content of the root node and its 1st child node of the updated B+ tree;
    */
    
    cout << "-----------------Experiment 5-----------------" << endl;

    return 0;


}