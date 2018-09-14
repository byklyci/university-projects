#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // strcpy, strcmp
#include <sys/stat.h> // mkdir

using namespace std;

const int typeNameLength = 12;
const int fieldNameLength = 12;
const int maxNumOfFields = 8;
const int maxNumOfFiles = 159; // same as max num of types. one file is reserved for system catalog
const int numOfPagesInFile = 64; // (159+1) * 64 * 1KB = 10240 KB = 10 MB
const int numOfRecordsInPage = 28;

struct Type {
    bool isFull;
    short fieldCount;
    bool primaryKeyBitmap[maxNumOfFields];
    char name[typeNameLength];
    char fieldNames[maxNumOfFields][fieldNameLength];
};

struct SystemCatalogue {
    short typeCount;
    short nextEmptyIndex;
    short nextEmptyFile;
    Type types[maxNumOfFiles];
    short fileToTypeMapping[maxNumOfFiles];
};

struct Record {
    bool isFull;
    int fields[maxNumOfFields];
};

struct Page {
    bool isNextPageUsed;
    short numOfRecords;
    short nextEmptyRecordIndex;
    Record records[numOfRecordsInPage];
    char padding[8]; // padding to increase the size of page to 1 kb
};

struct File {
    Page pages[numOfPagesInFile];
};

void readSyscat(SystemCatalogue &syscat){
    ifstream inputFile("data/syscat", ios::binary);
    inputFile.read((char*)&syscat, sizeof(syscat));
}

void writeSyscat(SystemCatalogue &syscat){
    ofstream outputFile("data/syscat", ios::binary| ios::in | ios::out);
    outputFile.write((char*)&syscat, sizeof(syscat));
}

void setNextEmptyFileIndex(SystemCatalogue &syscat){
    int emptyFileIndex = syscat.nextEmptyFile + 1;
    while(syscat.fileToTypeMapping[emptyFileIndex] != -1 && emptyFileIndex < maxNumOfFiles){
        emptyFileIndex++;
    }
    if(emptyFileIndex >= maxNumOfFiles){
        emptyFileIndex = -1;
    }
    syscat.nextEmptyFile = emptyFileIndex;
}

void createType(const string &typeName,vector<string> fieldNames, vector<int> primaryKeys){
    SystemCatalogue syscat;
    readSyscat(syscat);

    if(syscat.nextEmptyFile == -1){
        cout << "Disk is full. Could not create a new type." << endl;
        return;
    }
    int index = syscat.nextEmptyIndex;
    if(index == -1){
        cout << "System catalog is full. Could not create a new type." << endl;
        return;
    }
    syscat.typeCount++;
    syscat.types[index].isFull = true;
    strcpy(syscat.types[index].name, typeName.c_str());
    syscat.types[index].fieldCount = fieldNames.size();
    for(int i=0;i<fieldNames.size();i++){
        strcpy(syscat.types[index].fieldNames[i], fieldNames[i].c_str());
    }
    for(int i=0;i<primaryKeys.size();i++){
        syscat.types[index].primaryKeyBitmap[i] = primaryKeys[i] == 1;
    }
    syscat.fileToTypeMapping[syscat.nextEmptyFile] = index;

    // find next empty index
    index++;
    while(syscat.types[index].isFull && index < maxNumOfFiles){
        index++;
    }
    if(index >= maxNumOfFiles){
        index = -1;
    }
    syscat.nextEmptyIndex = index;

    // find next empty file
    setNextEmptyFileIndex(syscat);

    writeSyscat(syscat);

    cout << typeName << " is created" << endl;
}

int findIndexOfType(SystemCatalogue &syscat, const string &typeName){
    int index = 0;
    while(strcmp(syscat.types[index].name, typeName.c_str()) != 0){
        index++;
    }
    return index;
}

vector<int> findIndexOfFile(SystemCatalogue &syscat, int typeIndex){
    vector<int> indexOfFiles;
    for(int i=0;i<maxNumOfFiles;i++){
        if(syscat.fileToTypeMapping[i] == typeIndex){
            indexOfFiles.push_back(i);
        }
    }
    return indexOfFiles;
}

void deleteType(const string &typeName){
    SystemCatalogue syscat;
    readSyscat(syscat);

    int index = findIndexOfType(syscat,typeName);

    // delete type
    syscat.types[index].isFull = false;
    syscat.typeCount--;

    // delete file mapping
    for(int i=0;i<maxNumOfFiles;i++){
        if(syscat.fileToTypeMapping[i] == index){ //if file point to this type
            syscat.fileToTypeMapping[i] = -1;

            // update next empty file index
            if(i < syscat.nextEmptyFile){
                syscat.nextEmptyFile = i;
            }
        }
    }

    // update next empty index
    if(index < syscat.nextEmptyIndex){
        syscat.nextEmptyIndex = index;
    }

    writeSyscat(syscat);

    cout << typeName << " is deleted" << endl;
}

void listTypes(){
    SystemCatalogue syscat;
    readSyscat(syscat);

    int index = 0;
    for(int i=0;i<syscat.typeCount;i++){
        while(!syscat.types[index].isFull){
            index++;
        }
        cout << syscat.types[index].name << endl;
        index++;
    }
}

void addRecordToPage(Page &page, vector<int> fieldValues){
    int recordIndex = page.nextEmptyRecordIndex;
    page.records[recordIndex].isFull = true;
    for(int k=0;k<fieldValues.size();k++){
        page.records[recordIndex].fields[k] = fieldValues[k];
    }
    page.numOfRecords++;

    recordIndex++;
    while(page.records[recordIndex].isFull && recordIndex < numOfRecordsInPage){
        recordIndex++;
    }
    if(recordIndex >= numOfRecordsInPage){
        page.nextEmptyRecordIndex = -1;
        page.isNextPageUsed = true;
    }else{
        page.nextEmptyRecordIndex = recordIndex;
    }
}

void createRecord(const string &typeName, const vector<int> &fieldValues){
    SystemCatalogue syscat;
    readSyscat(syscat);

    int typeIndex = findIndexOfType(syscat,typeName);
    vector<int> indexOfFiles = findIndexOfFile(syscat,typeIndex);

    for(int i=0;i<indexOfFiles.size();i++){
        string filename = "data/file" + to_string(indexOfFiles[i]);

        for(int j=0;j<numOfPagesInFile;j++){
            Page page;

            ifstream inputFile(filename, ios::binary);
            inputFile.seekg(j* sizeof(Page),ios::beg);
            inputFile.read((char*)&page, sizeof(Page));
            inputFile.close();

            cout << "Reading page " << (j+1) << " in file " <<  indexOfFiles[i] << endl;

            if(page.nextEmptyRecordIndex != -1){
                addRecordToPage(page, fieldValues);

                ofstream outputFile(filename, ios::binary | ios::in | ios::out);
                outputFile.seekp(j* sizeof(Page), ios::beg);
                outputFile.write((char*)&page, sizeof(Page));
                outputFile.close();

                cout << "New record is created in page " << (j+1) << " in file " << indexOfFiles[i] << endl;
                return;
            }
        }
    }

    // create next file
    if(syscat.nextEmptyFile == -1){
        cout << "Disk is full. Could not create a new type." << endl;
        return;
    }

    syscat.fileToTypeMapping[syscat.nextEmptyFile] = typeIndex;

    int emptyFileIndex = syscat.nextEmptyFile;

    setNextEmptyFileIndex(syscat);
    writeSyscat(syscat);

    string filename = "data/file" + to_string(emptyFileIndex);

    Page page;

    ifstream inputFile(filename, ios::binary);
    inputFile.read((char*)&page, sizeof(Page));
    inputFile.close();

    addRecordToPage(page,fieldValues);

    ofstream outputFile(filename, ios::binary | ios::in | ios::out);
    outputFile.write((char*)&page, sizeof(Page));
    outputFile.close();

    cout << "New record is created in page " << 1 << " in file " << emptyFileIndex << endl;
}

bool recordMatches(Record &record, vector<int> keys, const bool primaryKeyBitmap[]){
    int keyPos = 0;
    for(int i=0;i<keys.size();i++){
        while(primaryKeyBitmap[keyPos] == 0){
            keyPos++;
        }
        if(record.fields[keyPos] != keys[i]){
            return false;
        }
        keyPos++;
    }
    return true;
}

vector<int> findRecord(SystemCatalogue &syscat, const string &typeName, const vector<int> &keys, int typeIndex){
    vector<int> indexOfFiles = findIndexOfFile(syscat,typeIndex);

    for(int i=0;i<indexOfFiles.size();i++){
        string filename = "data/file" + to_string(indexOfFiles[i]);

        for(int j=0;j<numOfPagesInFile;j++){
            Page page;

            ifstream inputFile(filename, ios::binary);
            inputFile.seekg(j* sizeof(Page),ios::beg);
            inputFile.read((char*)&page, sizeof(Page));
            inputFile.close();

            int recordCount = page.numOfRecords;
            if(recordCount != 0){
                cout << "Reading page " << (j+1) << " in file " <<  indexOfFiles[i] << endl;
            }
            int recordIndex = 0;
            for(int k=0;k<recordCount;k++){
                while(!page.records[recordIndex].isFull){
                    recordIndex++;
                }

                if(recordMatches(page.records[recordIndex],keys, syscat.types[typeIndex].primaryKeyBitmap)){
                    vector<int> result;
                    result.push_back(indexOfFiles[i]); // file index
                    result.push_back(j); // page index
                    result.push_back(recordIndex); // record index
                    return result;
                }
                recordIndex++;
            }
        }
    }
    vector<int> result;
    result.push_back(-1);
    result.push_back(-1);
    result.push_back(-1);
    return result;
}

void searchRecord(const string &typeName, const vector<int> &keys){
    SystemCatalogue syscat;
    readSyscat(syscat);

    int typeIndex = findIndexOfType(syscat,typeName);

    cout << syscat.types[typeIndex].name;
    for(int i=0;i<syscat.types[typeIndex].fieldCount;i++){
        cout << " " << syscat.types[typeIndex].fieldNames[i];
    }
    cout << endl;

    vector<int> address = findRecord(syscat,typeName,keys,typeIndex);
    cout << "Record is found in page " << (address[1]+1) << " in file " << address[0] << endl;
    string filename = "data/file" + to_string(address[0]);
    Page page;

    ifstream inputFile(filename, ios::binary);
    inputFile.seekg(address[1]* sizeof(Page),ios::beg);
    inputFile.read((char*)&page, sizeof(Page));
    inputFile.close();

    cout << "Record:";
    for(int i=0;i<syscat.types[typeIndex].fieldCount;i++){
        cout << " " << page.records[address[2]].fields[i];
    }
    cout << endl;
}

void deleteRecord(const string &typeName, const vector<int> &keys){
    SystemCatalogue syscat;
    readSyscat(syscat);

    int typeIndex = findIndexOfType(syscat,typeName);
    vector<int> address = findRecord(syscat,typeName,keys,typeIndex);

    string filename = "data/file" + to_string(address[0]);
    Page page;

    ifstream inputFile(filename, ios::binary);
    inputFile.seekg(address[1]* sizeof(Page),ios::beg);
    inputFile.read((char*)&page, sizeof(Page));
    inputFile.close();

    // delete record
    page.records[address[2]].isFull = false;
    page.numOfRecords--;

    cout << "Record is deleted from page " << (address[1]+1) << " file " << address[0] << endl;

    // update the next empty record index
    if(page.nextEmptyRecordIndex == -1){
        page.nextEmptyRecordIndex = address[2];
    }else if(address[2] <  page.nextEmptyRecordIndex){
        page.nextEmptyRecordIndex = address[2];
    }

    ofstream outputFile(filename, ios::binary| ios::in | ios::out);
    outputFile.seekp(address[1]* sizeof(Page),ios::beg);
    outputFile.write((char*)&page, sizeof(Page));
    outputFile.close();

    if(page.numOfRecords == 0){
        vector<int> indexOfFiles = findIndexOfFile(syscat,typeIndex);
        if(indexOfFiles.size() < 2){
            return;
        }
        // continue if there are more than one file for a type

        File file;
        ifstream inFile(filename, ios::binary);
        inFile.read((char*)&file, sizeof(File));
        inFile.close();
        for(int i=0;i<numOfPagesInFile;i++){
            if(file.pages[i].numOfRecords != 0){
                return;
            }
        }
        // if we can reach here, this means that this file is empty
        // and there are other files for this type
        // so we can delete this file since it is empty

        // delete file mapping
        syscat.fileToTypeMapping[address[0]] = -1;

        // update next empty file index
        if(address[0] < syscat.nextEmptyFile){
            syscat.nextEmptyFile = address[0];
        }

        // reset file
        Page page;
        page.numOfRecords = 0;
        page.nextEmptyRecordIndex = 0;
        page.isNextPageUsed = false;

        for(int i=0;i<numOfPagesInFile;i++){
            file.pages[i] = page;
        }

        ofstream outFile("data/file"+to_string(address[0]), ios::binary| ios::in | ios::out);
        outFile.write((char*)&file, sizeof(file));
        outFile.close();

        // update syscat
        writeSyscat(syscat);
    }
}

void listRecords(const string &typeName){
    SystemCatalogue syscat;
    readSyscat(syscat);

    int typeIndex = findIndexOfType(syscat,typeName);
    vector<int> indexOfFiles = findIndexOfFile(syscat,typeIndex);

    cout << syscat.types[typeIndex].name;
    for(int i=0;i<syscat.types[typeIndex].fieldCount;i++){
        cout << " " << syscat.types[typeIndex].fieldNames[i];
    }
    cout << endl;

    for(int i=0;i<indexOfFiles.size();i++){
        string filename = "data/file" + to_string(indexOfFiles[i]);

        for(int j=0;j<numOfPagesInFile;j++){
            Page page;

            ifstream inputFile(filename, ios::binary);
            inputFile.seekg(j* sizeof(Page),ios::beg);
            inputFile.read((char*)&page, sizeof(Page));
            inputFile.close();

            int recordCount = page.numOfRecords;
            if(recordCount != 0){
                cout << "Reading page " << (j+1) << " in file " <<  indexOfFiles[i] << endl;
            }
            int recordIndex = 0;
            for(int k=0;k<recordCount;k++){
                while(!page.records[recordIndex].isFull){
                    recordIndex++;
                }
                cout << "Record:";
                for(int l=0;l<syscat.types[typeIndex].fieldCount;l++){
                    cout << " " << page.records[recordIndex].fields[l];
                }
                cout << endl;
                recordIndex++;
            }
        }
    }
}

void initialize(){
    // Create "data" directory
    if (mkdir("data", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1 ){
        if(errno != EEXIST){ // if the error is not "file exists"
            cout << "Error creating directory! " << strerror(errno) << endl;
            exit(1);
        }
    }

    SystemCatalogue syscat;
    syscat.typeCount = 0;
    syscat.nextEmptyIndex = 0;
    syscat.nextEmptyFile = 0;
    for(int i=0;i<maxNumOfFiles;i++){
        syscat.fileToTypeMapping[i] = -1;
    }

    Type type;
    type.isFull = false;
    type.fieldCount = 0;
    strcpy(type.name, "");
    for(int i=0;i<maxNumOfFields;i++){
        type.primaryKeyBitmap[i] = false;
    }
    for(int i=0;i<maxNumOfFields;i++){
        strcpy(type.fieldNames[i], "");
    }

    for(int i=0;i<maxNumOfFiles;i++){
        syscat.types[i] = type;
    }

    ofstream output_file("data/syscat", ios::binary);
    output_file.write((char*)&syscat, sizeof(syscat));
    output_file.close();

    Page page;
    page.numOfRecords = 0;
    page.nextEmptyRecordIndex = 0;
    page.isNextPageUsed = false;

    File file;
    for(int i=0;i<numOfPagesInFile;i++){
        file.pages[i] = page;
    }

    for(int i=0;i<maxNumOfFiles;i++){
        ofstream outputFile("data/file"+to_string(i), ios::binary);
        outputFile.write((char*)&file, sizeof(file));
    }
}

void printHelp(){
    cout << "Usage: " <<  endl;
    cout << "storagemanager init" << endl;
    cout << "\tThis command must be run before using storagemanager. It formats hard disk." << endl;
    cout << endl;
    cout << "storagemanager create type [type name] [# of fields] [field 1 name] .. [field n name] [primary key bitmap] " << endl;
    cout << "storagemanager create type STUDENT 4 id mid1 mid2 final 1 0 0 0" << endl;
    cout << "\tThis creates a STUDENT type with 4 fields. Name of field 1 is id. Name of field 2 is mid1.";
    cout << "Name of field 3 is mid2. Name of field 4 is final. Only the id field is primary key ";
    cout << "because only the first number is 1 and the others are zeros.";
    cout << endl;
    cout << endl;
    cout << "storagemanager delete type [type name]" << endl;
    cout << "storagemanager delete type STUDENT" <<  endl;
    cout << "\tThis deletes STUDENT type." << endl;
    cout << endl;
    cout << "storagemanager list type" << endl;
    cout << "\tThis lists all types." <<  endl;
    cout << endl;
    cout << "storagemanager create record [type name] [field value 1] .. [field value n]" << endl;
    cout << "storagemanager create record STUDENT 1 70 75 80" << endl;
    cout << "\tThis creates a STUDENT record with id 1, mid1 70, mid2 75 and final 80." << endl;
    cout << endl;
    cout << "storagemanager delete record [type name] [primary key 1] .. [primary key n]" << endl;
    cout << "storagemanager delete record STUDENT 1" << endl;
    cout << "\tThis deletes the STUDENT record with id 1." << endl;
    cout << endl;
    cout << "storagemanager search record [type name] [primary key 1] .. [primary key n]" << endl;
    cout << "storagemanager search record STUDENT 1" << endl;
    cout << "\tThis searches the STUDENT record with id 1." << endl;
    cout << endl;
    cout << "storagemanager list record [type name]" << endl;
    cout << "storagemanager list record STUDENT" << endl;
    cout << "\tThis lists all STUDENT records." << endl;
}

int main(int argc, char *argv[]) {
/*
    cout << sizeof(Type) << endl;
    cout << sizeof(SystemCatalogue) << endl;
    cout << sizeof(Record) << endl;
    cout << sizeof(Page) << endl;
    cout << sizeof(File) << endl;
*/

    // Parsing Arguments
    vector<string> args;
    for(int i=0;i<argc;i++){
        args.push_back(argv[i]);
    }

    if(argc < 2){ // if no argument is given
        printHelp();
    }else if(args[1] == "init"){
        initialize();
    }else if(args[1] == "create"){
        if(args[2] == "type"){
            string typeName = args[3];
            if(typeName.size() > typeNameLength){
                cout << "Type name must be " << typeNameLength << " characters long." << endl;
                return 0;
            }
            int fieldCount = stoi(args[4]);
            vector<string> fieldNames;
            for(int i=0;i<fieldCount;i++){
                if(args[5+i].size() > fieldNameLength){
                    cout << "Field name must be " << fieldNameLength << " characters long." << endl;
                    return 0;
                }
                fieldNames.push_back(args[5+i]);
            }
            vector<int> primaryKeys;
            for(int i=0;i<fieldCount;i++){
                primaryKeys.push_back(stoi(args[5+fieldCount+i]));
            }
            createType(typeName,fieldNames,primaryKeys);
        }else if(args[2] == "record"){
            string typeName = args[3];
            vector<int> fieldValues;
            for(int i=4;i<args.size();i++){
                fieldValues.push_back(stoi(args[i]));
            }
            createRecord(typeName,fieldValues);
        }else{
            printHelp();
        }
    }else if(args[1] == "delete"){
        if(args[2] == "type"){
            string typeName = args[3];
            deleteType(typeName);
        }else if(args[2] == "record"){
            string typeName = args[3];
            vector<int> keys;
            for(int i=4;i<args.size();i++){
                keys.push_back(stoi(args[i]));
            }
            deleteRecord(typeName,keys);
        }else{
            printHelp();
        }
    }else if(args[1] == "list"){
        if(args[2] == "type"){
            listTypes();
        }else if(args[2] == "record"){
            string typeName = args[3];
            listRecords(typeName);
        }else{
            printHelp();
        }
    }else if(args[1] == "search"){
        string typeName = args[3];
        vector<int> keys;
        for(int i=4;i<args.size();i++){
            keys.push_back(stoi(args[i]));
        }
        searchRecord(typeName,keys);
    }else{
        printHelp();
    }

    return 0;
}
