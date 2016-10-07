#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <regex>
#include <locale>
#include <dirent.h>

using namespace std;

enum Messages {
    SUCCESSFUL_LISTEN, 
    NO_ARGS, 
    NO_PORT_ENVAR, 
    INVALID_ARG, 
    INVALID_PORT,
    INVALID_ENVAR, 
    TOO_MANY_ARGS,
    ERROR_OPENING
};

enum Docs { 
    USAGE,
    ABOUT, 
    VERSION
};

enum Tests {
    MAIN, 
    LANG
};

string Tests[] {
    "TestCmds/mainTestCmds.txt",
    "TestCmds/langTestCmds.txt"
};

vector<string> lang;
vector<string> docs;

//Name: compseMsgArray
//Purpose: 
    //Will get each line of a file and put that into a vector
//Parameters: 
    //String - Will be the name of a file
//Returns:
    //Vector of each of the lines in the file
vector<string> composeMsgArray(string toSplit){
    ifstream reading(toSplit.c_str(), ifstream::in);
    vector<string> toRet;
    
    while(reading.good()){
        char tmp[256];
        reading.getline(tmp, 256);
        toRet.push_back(tmp);
    }
    return toRet;
}


//Name: skipEnvVar
//Purpose: 
    //Will skip the environment variableif it is a known
    //value that is not helpful
//Parameters:
    //char* - The environment variable that is being reviewed
//Returns: 
    //True if the value is known, and should be skipped
    //False if the value is not known
bool skipEnvVar(char* env_lang){
    int skipVarCount = 4;
    
    const char* toSkip[] = {
        "\0",
        "",
        "C",
        "C.UTF-8"
    };
    
    for(int c = 0; c < skipVarCount; c++){
        if(strcmp(env_lang, toSkip[c]) == 0) {
            return true;
        }
    }
    return false;
}

//Name: setLang
//Purpose:
    // Will set the language to any 2 digit language code that has a corresponding file in 'doc'
//Patameters:
    //N/A
//Returns:
    //N/A
string checkCurrLang(){
    
    DIR *d = opendir("../doc/");
    struct dirent *dir;
    vector<string> dirlist;
    if (d)
    {
        while ( (dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
                string tmp = dir->d_name;
                dirlist.push_back(tmp);
            }
        }
        closedir(d);
    }
    
    regex myRegex;
    locale mylocale ("");
    
    myRegex.imbue (mylocale);
    myRegex.assign ("[a-z][a-z].*");

    int envVarToCheck = 3;
    
    const char* toCheck[] = {
        "LANGUAGE", 
        "LC_ALL",
        "LANG"
    };
    
    for(int i = 0; i < envVarToCheck; i++){
        char* env_lang = getenv(toCheck[i]);
        if(skipEnvVar(env_lang)) continue;
        
        if(regex_search(env_lang, myRegex)){
            for(int j = 0; j < dirlist.size(); j++){
                if(strcmp(env_lang, dirlist[j].c_str()) == 0){
                    string temp = dirlist[j];
                    lang = composeMsgArray("../doc/" + temp + "/setport.messages.txt");
                    docs.push_back("../doc/" + temp + "/setport.usage.txt");
                    docs.push_back("../doc/" + temp + "/setport.about.txt");
                    docs.push_back("../doc/" + temp + "/setport.version.txt");
                    temp = env_lang;
                    return "Changing to " + temp + ", one second!";
                }
            }
        }
    }
    lang = composeMsgArray("../doc/en/setport.messages.txt");
    docs.push_back("../doc/en/setport.usage.txt");
    docs.push_back("../doc/en/setport.about.txt");
    docs.push_back("../doc/en/setport.version.txt");
    return "We will proceed using English.";
}

//Name: getInitialLang
    //Purpose: 
        // Will get the initial language variables, and return a 
        //string to allow it to be reset to that default after the tests have run
    //Parameters: 
        //  N/A
    //Returns: 
        // Char* - Contains command to reset the language at the end of the tests
char* getInitialLang(){
    
    DIR *d = opendir("../doc/");
    struct dirent *dir;
    vector<string> dirlist;
    if (d)
    {
        while ( (dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
                string tmp = dir->d_name;
                dirlist.push_back(tmp);
            }
        }
        closedir(d);
    }
    
    regex myRegex;
    locale mylocale ("");
    
    myRegex.imbue (mylocale);
    myRegex.assign ("[a-z][a-z](.|_)*");
    
    string envVar = "";
    string envVal = "";
    
    int envVarToCheck = 3;
    
    const char* toCheck[] = {
        "LANGUAGE", 
        "LC_ALL",
        "LANG"
    };
    
    char* composedCmd = (char*)malloc(100);
    for(int i = 0; i < envVarToCheck; i++){
        memset(composedCmd,0,strlen(composedCmd));;
        char* env_lang = getenv(toCheck[i]);
        
        if(!skipEnvVar(env_lang)) break;

        if(regex_search(env_lang, myRegex)){
            
            for(int j = 0; j < dirlist.size(); j++){
                if(strcmp(env_lang, dirlist[j].c_str()) == 0){
                    strcpy(composedCmd, "export ");
                    strcat(composedCmd, toCheck[i]);
                    strcat(composedCmd, "=");
                    strcat(composedCmd, env_lang);
                    return composedCmd;
                }
            }
        }
    }
    strcpy(composedCmd, "export ");
    strcpy(composedCmd, toCheck[2]);
    strcat(composedCmd, "=");
    strcat(composedCmd, getenv(toCheck[2]));
    return composedCmd;
}

//Name: msgSubString
    //Purpose: 
        // Used to split strings on a given delim
    //Parameters: 
        // String/toSplit - What will be split
        // string/delim = what the function will split on
    //Returns:
        // vector<string> = a vector of the string after it has been split on the delim.
vector<string> msgSubString(string toSplit, string delim){
    string tmpStr = toSplit;
    size_t pos = 0;
    string token;
    vector<string> cmdArray;
    pos = tmpStr.find(delim);
    while((pos = tmpStr.find(delim)) != string::npos) {
        token = tmpStr.substr(0, pos);
        cmdArray.push_back(token);
        tmpStr.erase(0, pos + delim.length());
    }
    cmdArray.push_back(tmpStr);
    return cmdArray;
}

//Name: getFile
    //Purpose: 
        // This will open a file, resize a string to hold ALL of the contents of said file,
        // then return that string
    //Parameters: 
        // strings/fileName - Name of the file that will be read from
    //Returns:
        // string - The large string with the file contents 
        // usually used when printing something like the 'usage' statement
string getFile(string fileName){
    ifstream usageFile(fileName, ifstream::in);
    string toPrint;
    
    if(!usageFile.is_open()) {
        return lang[ERROR_OPENING]  + fileName + "\n";
    }
    
    usageFile.seekg(0, ios::end);
    toPrint.reserve(usageFile.tellg());
    usageFile.seekg(0, ios::beg);
    
    toPrint.assign(istreambuf_iterator<char>(usageFile), istreambuf_iterator<char>());
    
    usageFile.close();
    return toPrint;
}

//Name: compareOutExpected
    //Purpose: 
        //compare the output (from test.txt) against the msg (if there is any), 
        //and the expected file
    //Parameters: 
        // vector<string>/msg - any messages that printed that were not part of the 
        // 'body' of the output, such as anything that would be found in the setport.messages.txt file
    //Returns:
        // int:
            // 0 - The test was successful
            // 1 - The test was unsuccessful
int compareOutExpected(vector<string> msg, string fileName = "N/A") {
    
    //This will get the output of the outputfile from the cmd
    string outputString = getFile("../test.txt");
    string txtLine = getFile(fileName);
    
    system("rm ../test.txt");
    
    vector<string> outputSplitString = msgSubString(outputString, "\n");
    vector<string> splitTxtLine = msgSubString(txtLine, "\n");
    
    while(!msg.empty()){
        if(outputSplitString.front().compare(msg.front()) != 0){
            cout << "Initial Message Difference" << endl;
            return 1;  
        }
        outputSplitString.erase(outputSplitString.begin());
        msg.erase(msg.begin());
    }

    if( fileName == "N/A") return 0; 
    
    while(outputSplitString.size() > 0 && splitTxtLine.size() > 0){
        if(splitTxtLine.front().compare(outputSplitString.front()) != 0){
            cout << "Output Difference" << endl;
            return 1;
        } 
        splitTxtLine.erase(splitTxtLine.begin());
        outputSplitString.erase(outputSplitString.begin());
    }
    return 0;
}

//Name: test
    //Purpose: 
        // This will run the program, and determine what its output is from the 
        //arguments that it used to call the software 
    //Parameters: 
        // const string&/commandLine - This is the command that will run the software
        // int/expectedReturn - This is what the exit code of the software should be
        // vector<string> msg - if a message has occured up to this point it can be 
            //passed through to be evaluated after any other messages have been passed to it
    //Returns:
        // int:
            // 0 - Successful test
            // non-0 - unsuccessful test
int test(const string& commandLine, int expectedReturn, vector<string> msg) {
    
    
    string program = "cd .. && " + commandLine + " > ./test.txt";
    
    string commandLn;
    commandLn.assign(commandLine);
    
    int statusCode = system(program.c_str()) / 256;
    //cout << "Status: " << statusCode << " Expected: " << expectedReturn << endl;
    
    if(statusCode != expectedReturn){ //Test failed the status test, return fail state
        return 1;
    }
    
    //get each argument of commandLine in its own array
    vector<string> cmdArray = msgSubString(commandLn, " ");
    
    if( expectedReturn == 0 ){
        if(cmdArray.size() == 2) {
            if (strcmp(cmdArray[1].c_str(), "-h") == 0  || strcmp(cmdArray[1].c_str(), "--help") == 0 || strcmp(cmdArray[1].c_str(), "-?") == 0) {
                return compareOutExpected(msg, docs[USAGE]);
            }
            else if(strcmp(cmdArray[1].c_str(), "-a") == 0 || strcmp(cmdArray[1].c_str(), "--about") == 0 || strcmp(cmdArray[1].c_str(), "-!") ==0 ){
                return compareOutExpected(msg, docs[ABOUT]);
            }
            else if(strcmp(cmdArray[1].c_str(), "-v") == 0 || strcmp(cmdArray[1].c_str(), "--version") == 0){
                return compareOutExpected(msg, docs[VERSION]);
            }
            else{
                msg.push_back(lang[INVALID_ARG] + cmdArray[1] + "\n");
                return compareOutExpected(msg, docs[VERSION]);
            }
        }
        else if(cmdArray.size() == 3){ 
            if(cmdArray[2] == "-e"){
                char* env_p = getenv("PORT");
                if(env_p != NULL){
                    msg.push_back(lang[SUCCESSFUL_LISTEN] + (string)env_p);
                    return compareOutExpected(msg);
                }
                return 1;
            }
            if(long int prt = atol(cmdArray[2].c_str()) != 0){
                msg.push_back(lang[SUCCESSFUL_LISTEN] + cmdArray[2]);
                return compareOutExpected(msg);
            }
            msg.push_back(lang[SUCCESSFUL_LISTEN] + cmdArray[2]);
            return compareOutExpected(msg);
        }
        else if(cmdArray.size() == 4){
            char* env_p = getenv(cmdArray[3].c_str());
            if(env_p != NULL){
                msg.push_back(lang[SUCCESSFUL_LISTEN] + (string)env_p);
                return compareOutExpected(msg);
            }
            return 1; 
        }
        else return 1;
    }
    else if( expectedReturn == 1 ) {
        msg.push_back(lang[INVALID_PORT] + cmdArray[2]);
    }
    else if( expectedReturn == 2 ) {
        msg.push_back(lang[INVALID_ARG] + cmdArray[1]);
    }
    else if( expectedReturn == 3 ) {
        if(strcmp(cmdArray[1].c_str(), "-p") == 0 || strcmp(cmdArray[1].c_str(), "--port") == 0) {
            if(strcmp(cmdArray[2].c_str(), "-e") == 0){
                if(char* env_p = getenv(cmdArray[3].c_str())){
                    return 1;
                }
                else {
                    msg.push_back(lang[INVALID_ARG] + (string)cmdArray[3]);
                    return compareOutExpected(msg);
                }
            }
            else{
                msg.push_back(lang[INVALID_ARG] + (string)cmdArray[2]);
                return compareOutExpected(msg);
            }
        }
    }
    else if( expectedReturn == 4 ) {
        msg.push_back(lang[NO_ARGS]);
    }
    else cout << "Error" << endl;
    return compareOutExpected(msg, docs[USAGE]);
}

//Name: runTests
    //Purpose: 
        // This gathers the tests to run from mainTestCmds.txt, and runs each
        //of the tests by calling "test"
    //Parameters: 
        // string/testCmds - file to read all the tests from
    //Returns:
        // int
            // 0 - set of tests were successful
            // non-0 - set of tests was unsuccessful
//Main, will check the # of arguments
//Depending on the number of args, it will check what was passed for a valid option
int runTests(string testCmds){
    vector<vector<string> > tests;
    //obtain an array of all the test cases from testCmds.txt
    ifstream testFile(testCmds, ifstream::in);
    if(!testFile.is_open()){
        cout << "ERROR OPENING FILE!!!" << endl;
        return 3;
    }
    while(!testFile.eof()){
        string tempTest = "";
        string status = "";
        char tempExpected = '\0';
        
        while(testFile.get(tempExpected)){
            if(tempExpected == '[') ;
            
            else if(tempExpected == ']') break;
            
            else status += tempExpected;
        }
        
        getline(testFile,tempTest);

        vector<string> tempRow;
        tempRow.push_back(status);
        tempRow.push_back(tempTest);
        
        tests.push_back(tempRow);
    }
    testFile.close();
    
    
    vector<string> msg;
    string tmp = "";
    if((tmp = checkCurrLang()) != "") msg.push_back(tmp);
    
    //loop through each of the test cases, passing them to the 'test' function
    for(int i = 0; i < tests.size(); i++){
        int ret = test(tests[i][1], stoi(tests[i][0]), msg);
        if(ret != 0) return 1;

    }
    return 0;
}

//Name: main
    //Purpose: 
        //Main will now check the language, 'save' that value. 
        //It will set the language to english, and run the main tests. 
        //Then it will set the language to spanish, and run the tests. 
        //It will then set the language to polish (any language we don't have)
            //And test that output
        //Then it will set the language back to the initial value
    //Parameters: 
        // Command line arguments
    //Returns:
        // int: 
            // 0 - Tests were successful
            // 1 - Tests failed
int main(int argc, char* args[]) {
    
    //Check all of the envVars get a valid one, if none valid, continue3
    char* finalCmd =  getInitialLang();
    
    //Set english as the envVar Lang, 
    system("export LANG=en");
    //Run Tests
    cout << "Running English Tests" << endl;
    if(runTests(Tests[MAIN])) {
        system(finalCmd); 
        return 1;
    }
    //Set Spanish as envVar lang, 
    system("export LANG=es");
    //Run Tests
    cout << "Running Spanish Tests" << endl;
    if(runTests(Tests[MAIN])) {
        system(finalCmd); 
        return 1;
    }
    //Set Lang to Pl? 
    system("export LANG=pl");
    cout << "Running Invalid Language Test" << endl;
    //Run Test make sure "No Valid Translation" comes output
    if(runTests(Tests[LANG])) {
        system(finalCmd); 
        return 1;
    }
    cout << "All Tests have completed successfully!" << endl;
    //Set Language as the value we got initially
    cout << "I'll set the language back to the value you had previously" << endl;
    system(finalCmd);
    free(finalCmd);

    
    //return success or fail
    return 0; 
}