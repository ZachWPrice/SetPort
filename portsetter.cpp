#include <iostream>
#include <fstream>
#include <cstring>
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

string msg_en[] = {
    "Listening on Port: ", 
    "No command line arguments passed!\n",
    "No valid 'PORT' environment variable!\n",
    "Invalid argument: ",
    "Invalid Port: ",
    "Invalid environment variable: " ,
    "Too many arguments passed\n",
    "Error Opening: "

};

string msg_es[] = {
    "La escucha del puerto:",
    "No hay argumentos de línea de comando que se pasa!\n",
    "No 'PORT' variable de entorno válido!\n",
    "Argumento no válido: ",
    "El puerto no válido:",
    "Variable de entorno no válido:",
    "Demasiados argumentos pasado\n",
    "Apertura de error: "
};

string doc_en[]{
    "doc/en/setport.usage_en.txt",
    "doc/en/setport.about_en.txt",
    "doc/en/setport.version_en.txt"
};

string doc_es[]{
    "doc/es/setport.usage_es.txt",
    "doc/es/setport.about_es.txt",
    "doc/es/setport.version_es.txt"
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
        if(strcmp(env_lang, toSkip[c])) {
            return 1;
        }
    }
    return 0;
}

void setLang(){
    
    regex myRegex;
    locale mylocale ("");
    
    myRegex.imbue (mylocale);
    myRegex.assign ("[a-z][a-z](.|_)*");

    int envVarToCheck = 3;
    
    const char* toCheck[envVarToCheck] = {
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
                return;
            }
            else if(strcmp(env_lang, "es") == 0){
                cout << "Changing to Spanish, one second!" << endl;
                lang = msg_es; 
                docs = doc_es;
                return;
            }
            else{
                cout << "No Valid translation for: " << env_lang << "\n" << "Proceeding using English." << endl;
                lang = msg_en; 
                docs = doc_en;
                return;
            }
        }
    }
    return;
}

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

//Run when -p with a valid port has been passed (or --port)
void printSuccess(long port) {
    cout << lang[SUCCESSFUL_LISTEN] << port << "\n";
    return;
}

//This will be called when -h is passed by iteself (or --help)
void printUsage() {
    cout << getFile(docs[USAGE]);
    return;
}

void printAbout() {
    
    cout << getFile(docs[ABOUT]);
    return;
}

void printVersion(){
    cout << getFile(docs[VERSION]);
    return;
}

//After printing the error message, it will display the usage statement
void printError(string msg) {
    cout << msg;
    printUsage();
}

//Main, will check the # of arguments
//Depending on the number of args, it will check what was passed for a valid option
int main(int argc, char* args[]) {
    string errMsg = "";
    int endStatus = 0;
    
    setLang();
    
    switch(argc){
        //When no arguments have been passed
        case 1:
            errMsg = lang[NO_ARGS];
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
                        errMsg = lang[NO_PORT_ENVAR];
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
                errMsg = lang[TOO_MANY_ARGS];
                endStatus = 3;
                break;
            }
            break;
        //When more than two arguments have been passed
        default: 
            errMsg = lang[TOO_MANY_ARGS];
            endStatus = 3;
            break;
    }
    
    printError(errMsg);
    return endStatus;
}