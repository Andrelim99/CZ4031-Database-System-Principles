#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <string.h>
#include "MemoryPool.h"


using namespace std;



int main() {

    MemoryPool mempool = MemoryPool(200);


    ifstream dataFile("../Project1/data.tsv");

//Loading of data into memory
    if(dataFile.is_open()){
        string line;
//        Remove header
        getline(dataFile, line);
        int counter = 0;
        while(getline(dataFile, line) /*&& counter < 11*/){
//        cout << "Line 1: " << line << endl;

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
        mempool.displayNBlocks(3);
    }
    else{
        cout <<"OOF";
    }

    return 0;


}