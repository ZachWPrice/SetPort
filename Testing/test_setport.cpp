#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <regex>
#include <locale>

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

string msg_en[] = {
    "Listening on Port: ", 
    "No command line arguments passed!",
    "No valid 'PORT' environment variable!\n",
    "Invalid argument: ",
    "Invalid Port: ",
    "Invalid environment variable: " ,
    "Too many arguments passed\n",
    "Error Opening: "

};

string msg_es[] = {
    "La escucha del puerto:",
    "No hay argumentos de línea de comando que se pasa!",
    "No 'PORT' variable de entorno válido!\n",
    "Argumento no válido: ",
    "El puerto no válido:",
    "Variable de entorno no válido:",
    "Demasiados argumentos pasado\n",
    "Apertura de error: "
};

string doc_en[]{
    "../doc/en/setport.usage_en.txt",
    "../doc/en/setport.about_en.txt",
    "../doc/en/setport.version_en.txt"
};

string doc_es[]{
    "../doc/es/setport.usage_es.txt",
    "../doc/es/setport.about_es.txt",
    "../doc/es/setport.version_es.txt"
};

string* lang = msg_en;
string* docs = doc_en;

int skipEnvVar(char* env_lang){
    
    int skipVarCount = 4;
    
    const char* toSkip[skipVarCount] = {
        "\0",
        "",
        "C",
        "C.UTF-8"
    };
    for(int c = 0; c < skipVarCount; c++){
        if(strcmp(env_lang, toSkip[c]) == 0) {
            return 1;
        }
    }
    return 0;
}

char* getInitialLang(){
    
    regex myRegex;
    locale mylocale ("");
    
    myRegex.imbue (mylocale);
    myRegex.assign ("[a-z][a-z](.|_)*");
    
    string envVar = "";
    string envVal = "";
    
    int envVarToCheck = 3;
    
    const char* toCheck[envVarToCheck] = {
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
            //There was a valid 2digit lang code
            //see if it is english
            if(strcmp(env_lang, "en") == 0){
                strcpy(composedCmd, "export ");
                strcat(composedCmd, toCheck[i]);
                strcat(composedCmd, "=");
                strcat(composedCmd, env_lang);
                return composedCmd;
            }
            else if(strcmp(env_lang, "es") == 0){
                strcpy(composedCmd, "export ");
                strcat(composedCmd, toCheck[i]);
                strcat(composedCmd, "=");
                strcat(composedCmd, env_lang);
                return composedCmd;
            }
            else{
                //Language Found, not valid for program
                strcpy(composedCmd, "export ");
                strcat(composedCmd, toCheck[i]);
                strcat(composedCmd, "=");
                strcat(composedCmd, env_lang);
                return composedCmd;
            }
        }
    }
    strcpy(composedCmd, "export ");
    strcpy(composedCmd, toCheck[2]);
    strcat(composedCmd, "=");
    strcat(composedCmd, getenv(toCheck[2]));
    return composedCmd;
}

string checkCurrLang(){
        
    regex myRegex;
    locale mylocale ("");
    
    myRegex.imbue (mylocale);
    myRegex.assign ("[a-z][a-z](.|_)*");
    
    int envVarToCheck = 3;
    
    const char* toCheck[3] = {
        "LANGUAGE", 
        "LC_ALL",
        "LANG"
    };
    
    for(int i = 0; i < envVarToCheck; i++){
        char* env_lang = getenv(toCheck[i]);
        
        if(!skipEnvVar(env_lang)) break;

        if(regex_search(env_lang, myRegex)){
            //There was a valid 2digit lang code
            //see if it is english
            if(strcmp(env_lang, "en") == 0){
                lang = msg_en; 
                docs = doc_en; 
                return "";
            }
            else if(strcmp(env_lang, "es") == 0){
                lang = msg_es; 
                docs = doc_es;
                return "Changing to Spanish, one second!";
            }
            else{

                lang = msg_en; 
                docs = doc_en;
                string envVal;
                envVal.assign(env_lang);
                return "No Valid translation for: " + envVal + "\nProceeding using English.";
            }
        }
    }
    return "";
}

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

//This method will compare the output (from test.txt) against the msg (if there is any), and the expected file
int compareOutExpected(string msg, string fileName = "N/A") {
    
    //This will get the output of the outputfile from the cmd
    string outputString = getFile("../test.txt");
    string txtLine = getFile(fileName);
    
    system("rm ../test.txt");
    
    vector<string> outputSplitString = msgSubString(outputString, "\n");
    vector<string> splitTxtLine = msgSubString(txtLine, "\n");
    
    
    if(msg != "N/A"){
        if(outputSplitString.front().compare(msg) != 0){
            cout << "Initial Message Difference" << endl;
            return 1;  
        }
        outputSplitString.erase(outputSplitString.begin());
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

//After printing the error message, it will display the usage statement
int test(const string& commandLine, int expectedReturn, string msg) {
    
    
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
                msg =  lang[INVALID_ARG] + cmdArray[1] + "\n";
                return compareOutExpected(msg, docs[VERSION]);
            }
        }
        else if(cmdArray.size() == 3){ 
            if(cmdArray[2] == "-e"){
                char* env_p = getenv("PORT");
                if(env_p != NULL){
                    msg = lang[SUCCESSFUL_LISTEN] + (string)env_p;
                    return compareOutExpected(msg);
                }
                return 1;
            }
            if(long int prt = atol(cmdArray[2].c_str()) != 0){
                msg = lang[SUCCESSFUL_LISTEN] + cmdArray[2];
                return compareOutExpected(msg);
            }
            msg = lang[SUCCESSFUL_LISTEN] + cmdArray[2];
            return compareOutExpected(msg);
        }
        else if(cmdArray.size() == 4){
            char* env_p = getenv(cmdArray[3].c_str());
            if(env_p != NULL){
                msg = lang[SUCCESSFUL_LISTEN] + (string)env_p;
                return compareOutExpected(msg);
            }
            return 1; 
        }
        else return 1;
    }
    else if( expectedReturn == 1 ) {
        msg = lang[INVALID_PORT] + cmdArray[2];
    }
    else if( expectedReturn == 2 ) {
        msg =  lang[INVALID_ARG] + cmdArray[1];
    }
    else if( expectedReturn == 3 ) {
        if(strcmp(cmdArray[1].c_str(), "-p") == 0 || strcmp(cmdArray[1].c_str(), "--port") == 0) {
            if(strcmp(cmdArray[2].c_str(), "-e") == 0){
                if(char* env_p = getenv(cmdArray[3].c_str())){
                    return 1;
                }
                else {
                    msg = lang[INVALID_ARG] + (string)cmdArray[3];
                    return compareOutExpected(msg);
                }
            }
            else{
                msg = lang[INVALID_ARG] + (string)cmdArray[2];
                return compareOutExpected(msg);
            }
        }
    }
    else if( expectedReturn == 4 ) {
        msg = lang[NO_ARGS];
    }
    else cout << "Error" << endl;
    return compareOutExpected(msg, docs[USAGE]);
}

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
    
    
    string msg = "N/A"; 
    string tmp = "";
    if((tmp = checkCurrLang()) != "") msg = tmp;
    
    
    int endStatus = 0;
    //loop through each of the test cases, passing them to the 'test' function
    for(int i = 0; i < tests.size(); i++){
        int ret = test(tests[i][1], stoi(tests[i][0]), msg);
        if(ret != 0) return endStatus = 1;

    }
    return endStatus;
}

//Main will now check the language, 'save' that value. 
//It will set the language to english, and run the main tests. 
//Then it will set the language to spanish, and run the tests. 
//Then it will set the language back to the initial value
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