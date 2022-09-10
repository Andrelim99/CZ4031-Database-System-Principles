#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


using namespace std;



int main() {

    ifstream dataFile("../data.tsv");


    if(dataFile.is_open()){
        string line;
//        Remove header
        getline(dataFile, line);
//        Get first data entry
        getline(dataFile, line);
        cout << "Line 1: " << line << endl;

        stringstream s(line);
        string s1, s2, s3;
        s >> s1 >> s2 >> s3;

        cout << s1 << " " << s2 << " " << s3 << endl;

        s >> s2;
        cout << s2;





        dataFile.close();
    }
    else{
        cout <<"OOF";
    }


}