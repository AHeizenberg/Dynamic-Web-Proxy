
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

int main(int argc, char const *argv[])
{
    // Allocate memory that can be used by multiple proccesses
    string *shared_memory =(string *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    // Set server info
    addrinfo hints, *server_info;
    memset(&hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    // TCP
    hints.ai_socktype = SOCK_STREAM;
    // use my IP
    hints.ai_flags = AI_PASSIVE;
    // use the first input by user as the the port number
    // NULL because it is the server connecting
    getaddrinfo(NULL, argv[1], &hints, &server_info);

    // create a server socket
    int server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

    // bind the socket to the proper addrress
    bind(server_socket, server_info->ai_addr, server_info->ai_addrlen);

    
    // Just ensures that bad words vector is cleared
    *shared_memory = "clear";

    // seperate the proccess
    fork();

    block_words.clear();
    // listen()

    listen(server_socket, 5);
   
   
    // infinite loop
    while(1) {

    cout << "Server: Waiting for connections ... \n";

    // accept(); either terminal client or web browser client
    int accp_socket = accept(server_socket,server_info->ai_addr,&server_info->ai_addrlen);
    // define the request buffer
    char request[1024];
    // clear the request buffer
    memset(request, 0, sizeof(request));
    // recv the data from the client and store into request
    recv(accp_socket, &request, sizeof(request), 0);

    if((strstr(request,"BLOCK")) || ((strstr(request,"UNBLOCK"))) ) {
        // add the recved buffer on line 88 to the vector if it is blocked command
        if(strstr(request,"BLOCK")) {
            // Get the second word of the recvd input
            char *second = strtok(request," ");
            second = strtok(NULL,"\r\n");
            *shared_memory = second;
            cout << "Blocked: " << *shared_memory << endl;
        }
        // infinite loop for the terminal client
        while(1) {
            
            // clear request buffer
            memset(request, 0, sizeof(request));
            // get the bytes
            size_t b_recv = recv(accp_socket, &request, sizeof(request), 0);
            // if unblock command set the shared memory to clear so the other process can clear the vector
            if(strstr(request,"UNBLOCK")) {
                *shared_memory = "clear";
                continue;
            }
            // if block command then take the second word and store into shared memory
            // so that other process can add it to the bad words vector
            else if (strstr(request,"BLOCK")) {
            char *second = strtok(request," ");
            second = strtok(NULL,"\r\n");
            *shared_memory = second;
            cout << "Blocked: " << *shared_memory << endl;
            }
        }
    }

   // Create the HTTP request and store it in string ans
    char *piece = strtok(request,"\r\n\r\n");
    string ans = piece;
    ans.append("\r\n");
    piece = strtok(NULL,"\r\n\r\n");
    ans.append(piece);
    ans.append("\r\n\r\n");
    
    // Extract host name and store it in host_name
    char *temp = strtok(piece," ");
    temp = strtok(NULL, " ");
    const char *host_name = temp;

    // Get request
    // Example format
    //char *hello = "GET /~carey/CPSC441/ass1/test1.html HTTP/1.1\r\nHost: pages.cpsc.ucalgary.ca\r\n\r\n";
    char * http_req = new char[ans.length()+1];
    strcpy(http_req, ans.c_str());

    // Requested webserver
    cout << "Request: " << ans << endl;
    cout << "Host name: " << host_name << endl;


    // Server info for webserver
    addrinfo webserver_hints, *webserver_info;
    memset(&webserver_hints, 0, sizeof(addrinfo));

    webserver_hints.ai_family = AF_INET;
    webserver_hints.ai_socktype = SOCK_STREAM;

    // Get webservers addr info
    getaddrinfo(host_name, "80", &webserver_hints, &webserver_info);

    // create webserver client socket
    int client_socket = socket(webserver_info->ai_family, webserver_info->ai_socktype, webserver_info->ai_protocol);
    if(client_socket < 0){
        cout << "client socket connection error \n";
    } else {
        cout << "client socket created" << endl;
    }


    // connect socket to  webserver server
    connect(client_socket, webserver_info->ai_addr, webserver_info->ai_addrlen);


    // declare the message buffer to 500 bytes
    char message[500];
    // clear the message buffer
    memset(message, 0, sizeof(message));

    // convert url into lowercase; just for my own capatalization simplicity program works fine without this
    for (int s = 0; s < ans.length(); s++){
        if(isalpha(ans[s])) {
            ans[s] = tolower(ans[s]);
        }
    }
    cout << "lower case url: " << ans << endl;


    // Check if clear message has been set by other proccess; if so clear the vector
    if((*shared_memory == "clear") && (block_words.size() > 0)) {
        block_words.clear();
    // if not and there is something in the shared_memory string that add it to the blocked words vector
    } else if (sizeof(shared_memory) > 0){
    block_words.push_back(*shared_memory);
    }



    // only check blocked words with url if there are blocked words
    if(block_words.size() > 0) {

    // Check if the blocked words are in the current url
    if(check_url(ans) == true){
        cout << "blocked word found in url!" << endl;
        // Redirect the current page to the error page
        char *error_page = "GET /~carey/CPSC441/ass1/error.html HTTP/1.1\r\nhost: pages.cpsc.ucalgary.ca\r\n\r\n";
        // Send the error_page http request to the webserver
        send(client_socket, error_page, strlen(error_page), 0);
        // clear the message buffer before recv data into it
        memset(message,0,sizeof(message));
        // Keep count of the bytes recvd in each recv call to make sure all bytes are recvd
        ssize_t count = 0;
        // loop till all bytes are recvd
        while ((count = recv(client_socket, &message, sizeof(message), 0)) > 0)
        {
            // make sure that all recvd bytes are send to the client; web browser
            send(accp_socket,message,count,0);
            // Clear the message buffer each loop iteration
            memset(message, '\0', sizeof(message));
        }
        // clear the message buffer for safety
        memset(message, '\0', sizeof(message));
        // close socket as we are done
        close(client_socket);
        continue;
    }else if (check_url(ans) == false){
        cout << "block word not in url" << endl;
    }
    }
    // If not block words then send webserver original request
    send(client_socket, http_req, strlen(http_req), 0);

     //keep count of the bytes received
    ssize_t count = 0;
    // Make sure all bytes are received
    while ((count = recv(client_socket, &message, sizeof(message), 0)) > 0)
    {
        // Makes sure that all the bytes that are recvd are also sent
        send(accp_socket,message,count,0);

        // Clear the message buffer for the next message
        memset(message, '\0', sizeof(message));
    }

    // Clear the message buffer for safety
    memset(message, '\0', sizeof(message));
    // Close the socket as we are done with this request
    close(client_socket);

    }
    // Never really gets here due to infinite while loop
    close(server_socket);

    return 0;
}




