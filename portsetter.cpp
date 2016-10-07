#include <iostream>
#include <fstream>
#include <cstring>
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

vector<string> lang;
vector<string> docs;

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
    if(env_lang == NULL) return true;
    
    int skipVarCount = 3;
    
    const char* toSkip[] = {
        "",
        "C",
        "C.UTF-8",
    };
    
    for(int c = 0; c < skipVarCount; c++){
        if(strcmp(env_lang, toSkip[c]) == 0) {
            return true;
        }
    }
    return false;
}

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

//Name: setLang
//Purpose:
    // Will set the language to any 2 digit language code that has a corresponding file in 'doc'
//Patameters:
    //N/A
//Returns:
    //N/A
void setLang(){
    
    DIR *d = opendir("doc/");;
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

    int envVarToCheck = 4;
    
    const char* toCheck[] = {
        "LANGUAGE", 
        "LC_ALL",
        "LANG",
        "LC_MESSAGE"
    };
    
    for(int i = 0; i < envVarToCheck; i++){
        char* env_lang = getenv(toCheck[i]);
        if(skipEnvVar(env_lang)) continue;
        
        if(regex_search(env_lang, myRegex)){
            for(int j = 0; j < dirlist.size(); j++){
                if(strcmp(env_lang, dirlist[j].c_str()) == 0){
                    cout << "Changing to " << env_lang << ", one second!" << endl;
                    string temp = dirlist[j];
                    lang = composeMsgArray("doc/" + temp + "/setport.messages.txt");
                    docs.push_back("doc/" + temp + "/setport.usage.txt");
                    docs.push_back("doc/" + temp + "/setport.about.txt");
                    docs.push_back("doc/" + temp + "/setport.version.txt");
                    return;
                }
            }
        }
    }
    cout << "We will proceed using English." << endl;
            lang = composeMsgArray("doc/en/setport.messages.txt");
            docs.push_back("doc/en/setport.usage.txt");
            docs.push_back("doc/en/setport.about.txt");
            docs.push_back("doc/en/setport.version.txt");
    return;
}

//Name: getFile
//Purpose:
    //Opens a file, and returns a string with all of the contents of said file.
//Patameters:
    // The file name that will be opened.
//Returns:
    // String with all contents of the file.
string getFile(string fileName){
    ifstream usageFile(fileName, ifstream::in);
    string toPrint;
    
    if(!usageFile.is_open()) {
        return lang[ERROR_OPENING] + fileName + "\n";
    }
    
    usageFile.seekg(0, ios::end);
    toPrint.reserve(usageFile.tellg());
    usageFile.seekg(0, ios::beg);
    
    toPrint.assign(istreambuf_iterator<char>(usageFile), istreambuf_iterator<char>());
    
    usageFile.close();
    return toPrint;
}

//Name: print Success
//Purpose:
    // Will print out "listening on port: x" when invoked
//Patameters:
    // Port number being listened on
//Returns:
    // N/A
void printSuccess(long port) {
    cout << lang[SUCCESSFUL_LISTEN] << port << "\n";
    return;
}

//Name: printUsage
//Purpose:
    // Will print the usage message when invoked
//Patameters:
    // N/A
//Returns:
    // N/A
void printUsage() {
    cout << getFile(docs[USAGE]);
    return;
}

//Name: printAbout
//Purpose:
    // Will print the 'About' message
//Patameters:
    // N/A
//Returns:
    // N/A
void printAbout() {
    
    cout << getFile(docs[ABOUT]);
    return;
}

//Name: printVersion
//Purpose:
    // Will print the version information
//Patameters:
    // N/A
//Returns:
    // N/A
void printVersion(){
    cout << getFile(docs[VERSION]);
    return;
}

//Name: printError
//Purpose:
    // Prints an error message, then will call printUsage()
//Patameters:
    // String message stating what the problem was
//Returns:
    // N/A
void printError(string msg) {
    cout << msg;
    printUsage();
}

//Name: main
//Purpose:
    // Heavy lifter Directs the program
//Patameters:
    // input from the command line
//Returns:
    // int: status code that can be used to tell of the software 
        //exited correctly or with an error
int main(int argc, char* args[]) {
    string errMsg = "";
    int endStatus = 0;
    
    setLang();
    
    switch(argc){
        //When no arguments have been passed
        case 1:
            errMsg = lang[NO_ARGS] + "\n";
            endStatus = 4;
            break;
        //When one argument has been passed
        case 2: 
            if (strcmp(args[1], "-h") == 0  || strcmp(args[1], "--help") == 0 || strcmp(args[1], "-?") == 0) {
                printUsage();
                return endStatus;
            }
            else if(strcmp(args[1], "-a") == 0 || strcmp(args[1], "--about") == 0 || strcmp(args[1], "-!") ==0 ){
                printAbout();
                return endStatus;
            }
            else if(strcmp(args[1], "-v") == 0 || strcmp(args[1], "--version") == 0){
                printVersion();
                return endStatus;
            }
            else{
                errMsg = lang[INVALID_ARG] + (string)args[1] + "\n";
                endStatus = 2;
            }
            break;
        //When two arguments have been passed
        case 3: 
            if(strcmp(args[1], "-p") == 0 || strcmp(args[1], "--port") == 0) {
                if((atol(args[2]) > 0 && atol(args[2]) <= pow(2, 16))) {
                        printSuccess(atol(args[2]));
                        return endStatus;
                }
                else if(strcmp(args[2], "-e") == 0){
                    if(char* env_p = getenv("PORT")){
                        printSuccess(atol(env_p));
                        return endStatus;
                    }
                    else {
                        errMsg = lang[NO_PORT_ENVAR] + "\n";
                        endStatus = 1;
                    }
                }
                else {
                    errMsg = lang[INVALID_PORT] + (string)args[2] + "\n";
                    endStatus = 1;
                }
            }
            else{
                errMsg = lang[INVALID_ARG] + (string)args[1] + "\n";
                endStatus = 2;
            }
            break;
        case 4:
            if(strcmp(args[1], "-p") == 0 || strcmp(args[1], "--port") == 0) {
                if(strcmp(args[2], "-e") == 0){
                    if(char* env_p = getenv(args[3])){
                        printSuccess(atol(env_p));
                        return endStatus;
                    }
                    else {
                        errMsg = lang[INVALID_ENVAR] + (string)args[3] + "\n";;
                        endStatus = 3;
                    }
                }
                else{
                    errMsg = lang[INVALID_ARG] + (string)args[2] + "\n";;
                        endStatus = 3;
                }
            }
            else{
                errMsg = lang[TOO_MANY_ARGS] + "\n";
                endStatus = 3;
                break;
            }
            break;
        //When more than two arguments have been passed
        default: 
            errMsg = lang[TOO_MANY_ARGS] + "\n";
            endStatus = 3;
            break;
    }
    
    printError(errMsg);
    return endStatus;
}