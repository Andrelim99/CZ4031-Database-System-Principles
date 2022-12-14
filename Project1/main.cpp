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
    int  blockSizes [2]= {200, 500};
    for(int size : blockSizes){
        cout << "Using a block size of " << size << endl;
        MemoryPool mempool = MemoryPool(size);

        ifstream dataFile("../Project1/data.tsv");

        //Loading of data into memory
        cout << "Loading dataset into memory..." << endl;
        if(dataFile.is_open()){
            string line;
            //Remove header
            getline(dataFile, line);
            int counter = 0;
            while(getline(dataFile, line)
                    ){
                stringstream s(line);
                string s1, s2, s3;
                s >> s1 >> s2 >> s3;
                Record newRecord;
                strcpy(newRecord.tconst, line.substr(0, line.find('\t')).c_str());
                newRecord.averageRating = stof(s2);
                newRecord.numVotes = stoi(s3);
                if(!mempool.insertRecord(newRecord)){
                    cout << "Record not inserted!" << endl;
                }
                counter++;
            }
            dataFile.close();

            cout << "Loading done!" << endl << endl;

            mempool.displayFirstNBlocks(5);
        }

        cout << "---------------- Experiment 1-----------------" << endl;
        mempool.printMemoryPoolDetails();

        //Get all record address via block (Shows block access)
        int blockIndex = 0;
        tuple<void*, Block> tmpTuple = mempool.getBlockI(blockIndex);
        Block tmpBlk = get<1>(tmpTuple);

/*
 * Iterate through all non-empty blocks
 * Save address and numVotes
 * Insert into B+ Tree
 */

        //Initialize B+ tree to insert keys
        BPlusTree newTree(size);
        int count = 0;
        while(tmpBlk.numRecords > 0){
        //Save records into B+ tree
            for(int i = 0;  i < tmpBlk.numRecords; i++){
                key keyStruct;
                //Pass ADDRESS of RECORD and numVotes
                keyStruct.address.push_back((unsigned char *)get<0>(tmpTuple) + i * 20);
                Record tmpRec = mempool.getRecord(blockIndex, i);
                keyStruct.keyValue = tmpRec.numVotes;
                if(count == 81){
                    cout<<endl;
                }
                newTree.insertKey(keyStruct);
                count++;
            }

            blockIndex++;
            //Get next tuple
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
        cout << "Parameter n of the B+ Tree      : " << newTree.getN() << endl;
        cout << "Number of nodes of the B+ Tree  : " << newTree.countNodes(newTree.getRootOfTree())<< endl;
        cout << "Height of the B+ Tree           : " << newTree.getHeightOfTree(newTree.getRootOfTree()) << endl;
        cout << "Content of Root Node and its 1st Child : " << endl;
        newTree.display(newTree.getRootOfTree());

        /*
        Experiment 3:
        Retrieve those movies with the ???numVotes??? equal to 500 and report the following statistics:
        - The number and the content of index nodes the process accesses;
        - The number and the content of data blocks the process accesses;
        - The average of ???averageRating???s??? of the records that are returned;
        */

        cout << "-----------------Experiment 3-----------------" << endl;
        // retrieving number of index nodes accessed as well as printing content of index nodes
        vector<void*> rec_address_3 = newTree.searchNode(500,500);
        // Print datablocks that were accessed based on records found.
        mempool.computeDatablockAccessed(rec_address_3);

        std::cout<<endl;

        /*
        Experiment 4:
        Retrieve those movies with the attribute ???numVotes??? from 30,000 to 40,000, both inclusively and report the following statistics:
        - The number and the content of index nodes the process accesses;
        - The number and the content of data blocks the process accesses;
        - The average of ???averageRating???s??? of the records that are returned;
        */

        cout << "-----------------Experiment 4-----------------" << endl;
        vector<void*> rec_address_4 = newTree.searchNode(30000,40000);

        // Print datablocks that were accessed based on records found.
        mempool.computeDatablockAccessed(rec_address_4);

        std::cout<<endl;

        /*
        Experiment 5:
        Delete those movies with the attribute ???numVotes??? equal to 1,000, update the B+ tree accordingly, and report the following statistics:
        - The number of times that a node is deleted (or two nodes are merged) during the process of the updating the B+ tree;
        - The number nodes of the updated B+ tree;
        - The height of the updated B+ tree;-
        - The content of the root node and its 1st child node of the updated B+ tree;
        */

       cout << "-----------------Experiment 5-----------------" << endl;
       //int node_del_count = newTree.remove(1000);

        // vector<void*> rec_address_5 = newTree.searchNode(1000,1000);
        // mempool.computeDatablockAccessed(rec_address_5);
        int pre = newTree.countNodes(newTree.getRootOfTree());
        cout << "Number of nodes before Deletion        :" << pre << endl;
       newTree.removeKey(1000);
        int post = newTree.countNodes(newTree.getRootOfTree());;

        // vector<void*> rec_address_6 = newTree.searchNode(1000,1000);
        // mempool.computeDatablockAccessed(rec_address_6);
        cout << "Height of updated B+ Tree              : " << newTree.getHeightOfTree(newTree.getRootOfTree()) << endl;
        cout << "Number of nodes after Deletion         :" << post<< endl;
        cout << "Number of nodes deleted/merged         : " << post-pre << endl;
        cout << "Search for records with numVotes = 1000: " << endl;
        vector<void*> rec_address_5 = newTree.searchNode(1000,1000);
        mempool.computeDatablockAccessed(rec_address_5);
        std::cout<<endl;
        cout << "-----------------End of blocksize " <<  size << "-----------------" << endl << endl;
    }
    return 0;
}