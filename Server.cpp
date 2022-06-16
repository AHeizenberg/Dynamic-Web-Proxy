
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <unistd.h>
#include<ctype.h>
#include <stdint.h>
#include <sys/mman.h>
#include <cstdint>

using namespace std;


// Global vector of strings that stores all the blocked words
vector<string> block_words;


// check if any words from blocked words vector is in the url
// return true if a blocked word is in url false otherwise
bool check_url(string url){

    for(string i : block_words){
        if(url.find(i) != string::npos){
            return true;
        }
    }
    return false;
}

