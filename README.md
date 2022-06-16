# Dynamic Web-censorship Proxy :zap: :zap:
The following is a dynamic web censorship proxy that is able to properly process, censor and redirect webpages based on the words provided by the users. The user can dynamically add or remove words from the block list and the proxy will appropriately censor pages based on the block list.

## How to compile:
Enter the following commands on the terminal:

    g++ Server.cpp
    ./a.out <Port Number>


Compile the program using g++ and then when running the code make sure to include the port number as shown above. This port number should match the port number in your browser proxy network settings. Additionally, it should also match the port number you plan to use when connecting through the terminal using telnet.

## How to use the Proxy:

Firstly make sure that you have configured your browser’s HTTP proxy setting to manual and make sure that it routes to your local IP (can be found using ifconfig). Next, compile the server.cpp file with a port number (make sure that the port number in the proxy setting, code and, telnet is the same). If done correctly two “waiting for connections...” messages will appear in the terminal. These are two processes running at the same time. Now either load an HTTP website on the web browser or connect to the proxy server using telnet in a separate terminal. If you load an HTTP page the request and hostname will appear on the proxy’s terminal indicating that the request has been processed. On the contrary, if you connect using telnet you can type BLOCK followed by a word to block followed by the enter key. This word will now be blocked from the URLs and if seen on the URL the page will be redirected to the error page. Reload the browser to make sure the terminal changes have been processed. If you want to unblock the blocked words, type UNBLOCK followed by the enter key and ALL the blocked words will be removed.

**Note:** The way I have my dynamically configurable Web censorship proxy set up is once you enter a block command, for example, BLOCK curling you MUST reload any HTTP page on the browser at least once before adding another BLOCK or UNBLOCK command. You can have many words blocking at once but between each BLOCK just make sure to reload an HTTP page. I did this so it’s easier to trace which BLOCK on UNBLOCK caused an error if things go wrong.

## Features Implemented:
<ol>
  <li><b><u>Transparent proxy: </u></b> Can process a request from a client, create an appropriate request to send to the appropriate server, process the request from the server, and forward it to the client unaltered.</li>
  <li><b><u>Can process images: </u></b> The ability to process images in a transparent and censored proxy.</li>
  <li><b><u>Parse and modify HTTP requests: </u></b> Can extract the URL information from an HTTP request, decide whether the request should be blocked or not, and redirect blocked requests to the
error page.</li>
  <li><b><u>Dynamically configurable Proxy: </u></b> By using telnet to connect to the server an individual can
dynamically update the list of blocked words for the censorship proxy while the proxy is running and see live results by refreshing HTTP pages.</li>
</ol>
