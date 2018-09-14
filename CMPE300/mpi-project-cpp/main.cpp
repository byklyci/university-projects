#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <mpi.h>

using namespace std;

static const int max_word_length = 50;

// structure to store the frequency of the words
struct word_count {
    char word[max_word_length]; // string to store the word
    int count; // the word's frequency
};

void Merge(struct word_count arr[], int low, int mid, int high) {
    int mergedSize = high - low + 1;
    struct word_count temp[mergedSize];
    int mergePos = 0;
    int leftPos = low;
    int rightPos = mid + 1;

    while (leftPos <= mid && rightPos <= high) {

        if (strcmp(arr[leftPos].word, arr[rightPos].word) < 0) {
            temp[mergePos++] = arr[leftPos++];
        }
        else {
            temp[mergePos++] = arr[rightPos++];
        }
    }

    while (leftPos <= mid) {
        temp[mergePos++] = arr[leftPos++];
    }

    while (rightPos <= high) {
        temp[mergePos++] = arr[rightPos++];
    }

    for (mergePos = 0; mergePos < mergedSize; ++mergePos) {
        arr[low + mergePos] = temp[mergePos];
    }
}

// Classical MergeSort
void MergeSort(struct word_count arr[], int low, int high) {
    if (low < high) {
        int mid = (low + high) / 2;
        MergeSort(arr, low, mid);
        MergeSort(arr, mid + 1, high);
        Merge(arr, low, mid, high);
    }
}

int main(int argc, char *argv[]) {
    /********** Setting up MPI **********/
    MPI_Init(&argc, &argv); // Initialize the MPI environment

    MPI_Status status; // stores the source of the message and the tag of the message for receive operation

    int data_tag = 1; // tag used as a argument while sending message. its value is unimportant for this project

    int my_id; // my process id
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id); // sets the value of my_id
    int num_of_procs; // how many processes were started
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_procs); // sets the value of num_of_procs

    int master_process = 0; // id of the master process

    /*
     * int MPI_Type_create_struct(
     *   int count,
     *   int array_of_blocklengths[],
     *   MPI_Aint array_of_displacements[],
     *   MPI_Datatype array_of_types[],
     *   MPI_Datatype *newtype
     * );
     *
     * count: [in] number of blocks (integer)
     * array_of_blocklength: [in] number of elements in each block (array of integer)
     * array_of_displacements: [in] byte displacement of each block (array of integer)
     * array_of_types: [in] type of elements in each block (array of handles to datatype objects)
     * newtype: [out] new datatype (handle)
     */

    /********** Creating data type for struct **********/

    MPI_Datatype word_count_datatype; // new MPI data type to send "word_count" structs
    int wc_block_num = 2; // number of variables in "word_count" struct
    int wc_block_len[2] = {50,1}; // number of elements of the variables in "word_count" struct. 50 for char , 1 for int
    MPI_Aint wc_displacement[2]; // byte displacements of each variable in struct
    wc_displacement[0] = offsetof(struct word_count,word);
    wc_displacement[1] = offsetof(struct word_count,count);
    MPI_Datatype wc_types[2] = { MPI_CHAR, MPI_INT}; // data types of the variables in "word count" struct

    // creates the new data type
    MPI_Type_create_struct(wc_block_num,wc_block_len,wc_displacement,wc_types,&word_count_datatype);
    MPI_Type_commit(&word_count_datatype); // commits new data type to the system


    /********** Master process **********/
    if(my_id == master_process){ // this process is the master process

        /********** Reading the input **********/
        ifstream input_file("speech_tokenized.txt"); // input file stream to read the input file

        // counts the number of newlines in the input file (iterates over input stream buffer and counts the number of '\n')
        int line_num = count(istreambuf_iterator<char>(input_file),istreambuf_iterator<char>(),'\n');

        // since we iterated over input, we have to reset the pointer that show the next input
        input_file.clear(); // clear fail and eof bits of file stream
        input_file.seekg(0, ios::beg); // sets position of the next character of the stream to the beginning of the file

        string line; // string to store input lines
        char word_array[line_num][max_word_length]; // array of strings to store words
        for(int i=0;i<line_num;i++){ // for each line in input get the line and store the word in the array
            input_file >> line;
            strcpy(word_array[i],line.c_str()); // copies word to the array
        }
        input_file.close();

        /********** Distributing words to slaves **********/
        int number_of_slaves = num_of_procs - 1; // number of slave processes
        int average_data_per_process = line_num / number_of_slaves;

        for(int i=0;i<number_of_slaves;i++){
            int data_start = i*average_data_per_process; // start index of the data to be send
            int data_end = (i+1)*average_data_per_process - 1; // end index of the data to be send

            // when the data is not divided evenly by number of slaves, we assign the remaining data to the last process
            if(i == number_of_slaves-1){
                data_end = line_num-1;
            }
            int num_of_data_to_send = data_end - data_start + 1;
            int target_id = i+1; // id of the process that will send the message to

            // sends the size of the data to be send
            MPI_Send(&num_of_data_to_send,1,MPI_INT,target_id,data_tag,MPI_COMM_WORLD);
            // sends the words to the slave
            MPI_Send(&word_array[data_start],num_of_data_to_send*max_word_length,MPI_CHAR,target_id,data_tag,MPI_COMM_WORLD);
        }

        /********** Collecting mapped words from slaves **********/
        struct word_count word_count_array[line_num]; // struct array to store the mapped words
        for(int i=0;i<number_of_slaves;i++){
            int data_start = i*average_data_per_process; // start index of the data to be received
            int data_end = (i+1)*average_data_per_process - 1; // end index of the data to be send
            // when the data is not divided evenly by number of slaves, we assign the remaining data to the last process
            if(i == number_of_slaves-1){
                data_end = line_num-1;
            }
            int num_of_data_to_receive = data_end - data_start + 1;
            int slave_id = i+1; // id of the process from which we will receive the message

            // we get mapped words and store them in word_count_array
            MPI_Recv(&word_count_array[data_start],num_of_data_to_receive,word_count_datatype,slave_id,data_tag,MPI_COMM_WORLD,&status);
        }

        /********** Distributing mapped words to slaves to be sorted **********/
        for(int i=0;i<number_of_slaves;i++){
            int data_start = i*average_data_per_process; // start index of the data to be send
            int data_end = (i+1)*average_data_per_process - 1; // end index of the data to be send
            // when the data is not divided evenly by number of slaves, we assign the remaining data to the last process
            if(i == number_of_slaves-1){
                data_end = line_num-1;
            }
            int num_of_data_to_send = data_end - data_start + 1;
            int target_id = i+1; // id of the process that will send the message to

            // sends the size of the data to be send
            MPI_Send(&num_of_data_to_send,1,MPI_INT,target_id,data_tag,MPI_COMM_WORLD);
            // sends the mapped words to the slave
            MPI_Send(&word_count_array[data_start],num_of_data_to_send,word_count_datatype,target_id,data_tag,MPI_COMM_WORLD);
        }

        /********** Collecting sorted mapped words from slaves **********/
        struct word_count word_count_array2[line_num]; // struct array to store the sorted mapped words
        for(int i=0;i<number_of_slaves;i++){
            int data_start = i*average_data_per_process; // start index of the data to be received
            int data_end = (i+1)*average_data_per_process - 1; // end index of the data to be send
            // when the data is not divided evenly by number of slaves, we assign the remaining data to the last process
            if(i == number_of_slaves-1){
                data_end = line_num-1;
            }
            int num_of_data_to_receive = data_end - data_start + 1;
            int slave_id = i+1; // id of the process from which we will receive the message

            // we get sorted mapped words and store them in word_count_array2
            MPI_Recv(&word_count_array2[data_start],num_of_data_to_receive,word_count_datatype,slave_id,data_tag,MPI_COMM_WORLD,&status);
        }

        MergeSort(word_count_array2,0,line_num-1); // the last merge for partially sorted "word_count" structs

        /********** Reducing the list **********/
        vector<word_count> reduced_list; // "word_count" vector to store the reduced list
        if(line_num > 0){ // if there is data
            int count = 1; // count of the word. initially equal to the count of the first word
            char word[50]; // char array to store a word. initially equal to the first word
            strcpy(word, word_count_array2[0].word);

            for(int i=1;i<line_num;i++){
                if(strcmp(word,word_count_array2[i].word) == 0){ // if the next word is the same increment the count
                    count++;
                }else{ // if the next word is different add the word to the reduced list
                    word_count wc;
                    strcpy(wc.word,word);
                    wc.count = count;
                    reduced_list.push_back(wc);
                    count = 1; // update the count for this new word
                    strcpy(word,word_count_array2[i].word); // update the string for this new word
                }
            }
            word_count wc;
            strcpy(wc.word,word);
            wc.count = count;
            // add the last word we counted to the reduced list since we do not check it in the for loop above
            reduced_list.push_back(wc);
        }

        /********** Writing the word counts to a file **********/
        ofstream outFile("output.txt"); // output stream to write the data
        for (auto &i : reduced_list) { // iterates over the reduced list and outputs to the file
            outFile << i.word << " " << i.count << endl;
        }

    }else{
        /********** Slave process **********/

        /********** Receiving the splitted list of words from master process **********/
        int num_of_data_to_receive;
        // we get the number of data to be received from the master process
        MPI_Recv(&num_of_data_to_receive,1,MPI_INT,master_process,data_tag,MPI_COMM_WORLD,&status);

        // string array to store splitted word array
        char slave_word_array[num_of_data_to_receive][max_word_length];
        MPI_Recv(&slave_word_array[0],num_of_data_to_receive*max_word_length,MPI_CHAR,master_process,data_tag,MPI_COMM_WORLD,&status);

        /********** Mapping the words **********/
        struct word_count word_count_array[num_of_data_to_receive]; // struct array to store the mapped words
        for(int i=0;i<num_of_data_to_receive;i++){
            strcpy(word_count_array[i].word, slave_word_array[i]);
            word_count_array[i].count = 1;
        }

        /********** Sending the mapped words to master process **********/
        MPI_Send(&word_count_array[0],num_of_data_to_receive,word_count_datatype,master_process,data_tag,MPI_COMM_WORLD);

        /********** Receiving the mapped words that are to be sorted from master process **********/
        MPI_Recv(&num_of_data_to_receive,1,MPI_INT,master_process,data_tag,MPI_COMM_WORLD,&status);
        struct word_count word_count_array2[num_of_data_to_receive]; // struct array to store the mapped words that are to be sorted
        MPI_Recv(&word_count_array2[0],num_of_data_to_receive,word_count_datatype,master_process,data_tag,MPI_COMM_WORLD,&status);

        // sorts the array
        MergeSort(word_count_array2,0,num_of_data_to_receive-1);

        /********** Sending the sorted mapped words to master process **********/
        MPI_Send(&word_count_array2[0],num_of_data_to_receive,word_count_datatype,master_process,data_tag,MPI_COMM_WORLD);

    }

    MPI_Type_free(&word_count_datatype);  // free datatype when done using it

    MPI_Finalize();  // terminates the MPI execution environment

    return 0;
}
