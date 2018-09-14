#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include <cctype>
#include <stack>

using namespace std;

// counter for static single assignment variables
class Counter{
public:
    // returns the next number for static single assignment variables
    int getCountNum(){
        count++;
        return count;
    }
private:
    int count = 0;
};

// prints line and message of error to standart output
// and then exits the program
void printError(int lineCount, string message){
    cout << "Error: Line " << lineCount <<": " << message << endl;
    exit(0);
}

// returns the type of first token in the string str
// token types: endOfLine ( ) + - * / id num error
string  getTokenType(string str, int lineCount){
    if(str.empty()){
        return "endOfLine";
    }
    char firstCharacter = str.at(0);
    if(firstCharacter=='('){
        return "(";
    }else if(firstCharacter==')'){
        printError(lineCount,"missing open parenthesis");
        return ")";
    }else if(firstCharacter=='+'){
        return "+";
    }else if(firstCharacter=='-'){
        return "-";
    }else if(firstCharacter=='*'){
        return "*";
    }else if(firstCharacter=='/'){
        return "/";
    }else if(isalpha(firstCharacter)){
        return "id";
    }else if(isdigit(firstCharacter)){
        return "num";
    }else{
        printError(lineCount,"unknown token type");
        return "error";
    }
}

// returns the position of the matching right parenthesis in the string str
// if there is not any matching right parenthesis prints error
int posOfMatchingParenthesis(string str, int lineCount){
    int counter = 0;
    for(int i=0;i<str.length();i++){
        char c = str[i];
        if(c=='('){
            counter++;
        }else if(c==')'){
            counter--;
        }
        if(counter==0){
            return i; //closing parenthesis position
        }
    }
    printError(lineCount,"missing close parenthesis");
    return -1;
}

// returns the index of the end of the first token in the string
int getTokenPos(string str, int lineCount){
    string tokenType = getTokenType(str, lineCount);
    int pos = 0;
    if(tokenType=="("){
        pos = 1 + posOfMatchingParenthesis(str,lineCount);
    }else if(tokenType=="id"){
        char c;
        do{
            pos++;
            c = str[pos];
        }while(isalnum(c));
    }else if(tokenType=="num"){
        char c;
        do{
            pos++;
            c = str[pos];
        }while(isdigit(c));
    }else{// token is + - / *  its length is 1, so return 1
        pos = 1;
    }
    return pos;
}

// forward declaration of expr
// to be able to use expr in the function factor
void expr(string str, stack<string> &stack, Counter &variableCounter, ofstream &outputFile, int lineCount, set<string> variableSet);

// pushes nums and ids into stack. if there is a expression calls expr function
void factor(string str, stack<string> &stack, Counter &variableCounter, ofstream &outputFile, int lineCount,set<string> variableSet){
    string tokenType = getTokenType(str, lineCount);
    if(tokenType=="("){ //expression
        int pos = getTokenPos(str, lineCount);
        string tokenWithoutBraces = str.substr(1,pos-2);
        expr(tokenWithoutBraces, stack, variableCounter, outputFile, lineCount,variableSet);
    }else if(tokenType=="id"){ //variable
        if(variableSet.find(str)==variableSet.end()){ // if the variable used in expression is not defined (not in the variableSet)
            printError(lineCount,"undefined variable "+str);
        }
        int countNum = variableCounter.getCountNum(); // static single assignment number
        outputFile << "%" << countNum << " = load i32* %" << str <<"\n";
        stack.push("%"+to_string(countNum));
    }else if(tokenType=="num"){ //number
        stack.push(str);
    }else{
        printError(lineCount,"error");
    }
}

// parses str into factor and morefactors and print to file the multiplication and division command
void morefactors(string str, stack<string> &stack, Counter &variableCounter, ofstream &outputFile, int lineCount,set<string> variableSet){
    string signType = getTokenType(str, lineCount);
    if(signType=="*" || signType=="/"){
        string tokenWithoutSign = str.substr(1,str.length()-1); // removes the sign at the beginning of the token
        string tokenType = getTokenType(tokenWithoutSign, lineCount);
        if(tokenType=="endOfLine"){
            printError(lineCount,"operand expected");
        }else if(tokenType=="+"||tokenType=="-"||tokenType=="*"||tokenType=="/"){
            printError(lineCount,"extra operator");
        }
        int tokenPos = getTokenPos(tokenWithoutSign, lineCount);
        string factorString = tokenWithoutSign.substr(0,tokenPos);
        factor(factorString, stack, variableCounter, outputFile, lineCount,variableSet);
        string operand1 = stack.top();
        stack.pop();
        string operand2 = stack.top();
        stack.pop();
        int countNum = variableCounter.getCountNum(); // static single assignment number
        if(signType=="*"){
            outputFile << "%" << countNum << " = mul i32 " << operand2 <<"," << operand1 <<"\n";
        } else{// sign /  div
            outputFile << "%" << countNum << " = sdiv i32 " << operand2 <<"," << operand1 <<"\n";
        }
        stack.push("%"+to_string(countNum)); //pushes the result of arithmetic operation into stack
        string morefactorsString = tokenWithoutSign.substr(tokenPos);
        morefactors(morefactorsString, stack, variableCounter, outputFile, lineCount,variableSet);
    }else if(signType=="endOfLine"){
        return;
    }else{
        printError(lineCount,"error");
    }
}


// parses the str into factor and morefactors
void term(string str, stack<string> &stack, Counter &variableCounter, ofstream &outputFile, int lineCount,set<string> variableSet){
    int tokenPos = getTokenPos(str, lineCount);
    string factorString = str.substr(0,tokenPos);
    factor(factorString, stack, variableCounter, outputFile, lineCount,variableSet);
    string morefactorsString = str.substr(tokenPos);
    morefactors(morefactorsString, stack, variableCounter, outputFile, lineCount,variableSet);
}

// parses the str into terms and more terms and print to file addition or subtraction commands
void moreterms(string str, stack<string> &stack, Counter &variableCounter, ofstream &outputFile, int lineCount,set<string> variableSet){
    string signType = getTokenType(str, lineCount);
    if(signType=="+" || signType=="-"){
        string tokenWithoutSign = str.substr(1,str.length()-1); //removes the sign at the beginning of the token
        string tokenType = getTokenType(tokenWithoutSign, lineCount);
        if(tokenType=="endOfLine"){
            printError(lineCount,"operand expected");
        }else if(tokenType=="+"||tokenType=="-"||tokenType=="*"||tokenType=="/"){
            printError(lineCount,"extra operator");
        }
        int tokenPos = getTokenPos(tokenWithoutSign, lineCount);
        string termString = tokenWithoutSign.substr(0,tokenPos); // will contain the tokens until + or - sign
        tokenWithoutSign.erase(0,tokenPos);
        tokenType = getTokenType(tokenWithoutSign, lineCount);
        while(!(tokenType=="+" || tokenType=="-" || tokenType=="endOfLine")){ // add tokens to termString until + or - sign
            tokenPos =getTokenPos(tokenWithoutSign, lineCount);
            termString.append(tokenWithoutSign.substr(0,tokenPos));
            tokenWithoutSign.erase(0,tokenPos);
            tokenType = getTokenType(tokenWithoutSign, lineCount);
        }
        term(termString,stack, variableCounter, outputFile, lineCount,variableSet);
        string operand1 = stack.top();
        stack.pop();
        string operand2 = stack.top();
        stack.pop();
        int countNum = variableCounter.getCountNum(); // static single assignment number
        if(signType=="+"){
            outputFile << "%" << countNum << " = add i32 " << operand2 <<"," << operand1 <<"\n";
        } else{ // - sign
            outputFile << "%" << countNum << " = sub i32 " << operand2 <<"," << operand1 <<"\n";
        }
        stack.push("%"+to_string(countNum)); //pushes the result of arithmetic operation into stack
        string moretermsString = tokenWithoutSign.substr(0);
        moreterms(moretermsString, stack, variableCounter, outputFile, lineCount,variableSet);
    }else if(signType=="endOfLine"){
        return;
    }else{
        printError(lineCount,"error");
    }
}

// parses the string str into term and moreterm
void expr(string str, stack<string> &stack, Counter &variableCounter, ofstream &outputFile, int lineCount,set<string> variableSet){
    string tokenType = getTokenType(str, lineCount);
    if(tokenType=="endOfLine"){
        printError(lineCount,"expression expected");
    }
    int tokenPos = getTokenPos(str, lineCount); // end index of first token
    string termString = str.substr(0,tokenPos); // will contain the tokens until + or - sign
    str.erase(0,tokenPos);
    tokenType = getTokenType(str, lineCount);
    while(!(tokenType=="+" || tokenType=="-" || tokenType=="endOfLine")){ // add tokens to termString until + or - sign
        tokenPos =getTokenPos(str, lineCount);
        termString.append(str.substr(0,tokenPos));
        str.erase(0,tokenPos);
        tokenType = getTokenType(str, lineCount);
    }
    term(termString, stack, variableCounter, outputFile, lineCount,variableSet);
    string moretermsString = str.substr(0); // rest of the str is the moreterm
    moreterms(moretermsString, stack, variableCounter, outputFile, lineCount,variableSet);
}

int main(int argc, char *argv[]) {
    //Setting input stream
    ifstream inputFile(argv[1]);

    //creating output file name
    string inputFileName = argv[1];
    string outputFileName = inputFileName.substr(0,inputFileName.find("."));
    outputFileName.append(".ll");

    string line;

    //printing the head part of the file
    ofstream outputFile(outputFileName);
    outputFile << "; ModuleID = 'stm2ir'\n"
               << "declare i32 @printf(i8*, ...)\n"
               << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n\n"
               << "define i32 @main() {\n";

    set<string> variableSet; // contains the variables declared in the input file until now
    stack<string> stack; // stack for storing operands

    Counter variableCounter; // counter for static single assignment variables
    int lineCount = 0; // current line number of the input

    //Reading input
    while(getline(inputFile,line)){//while there is line
        lineCount++;
        line.erase(::remove_if(line.begin(),line.end(), ::isspace),line.end()); // deletes spaces in the line
        int indexOfEqual = line.find("=");
        if(indexOfEqual==-1){// there isn't a '=' in line . So it is print expression command
            expr(line, stack, variableCounter, outputFile, lineCount,variableSet);
            string exprResult = stack.top();
            stack.pop();
            outputFile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 "
                       << exprResult <<" )\n";
            variableCounter.getCountNum();
        }else{// this line is an assignment
            // <var>=<expr>
            string varName = line.substr(0,indexOfEqual);
            string expression = line.substr(indexOfEqual+1);
            if(variableSet.find(varName)==variableSet.end()) {//set does not contain the variable
                outputFile << "%" << varName << "= alloca i32\n"; //output the allocate command
                variableSet.insert(varName);
            }
            expr(expression, stack, variableCounter, outputFile, lineCount,variableSet);
            string exprResult = stack.top();
            stack.pop();
            outputFile << "store i32 " << exprResult <<", i32* %" << varName <<"\n";
        }
    }

    outputFile << "ret i32 0\n" << "}\n";

    return 0;
}
