# C-language-CGI-Web-Server

 A web server is a long running program. It waits for connections and when an HTTP request arrives, the server does some work to satisfy the request. If the request is for a static web page, then the server can simply read the file and send it back to the client. The Common Gateway Interface (CGI (Links to an external site.)) is a simple protocol that allows a separate program to be run to dynamically generate the response to an HTTP request.

In the CGI model, the server parses an HTTP request, and then forks and execs the program specified in the HTTP request. The output of the program is sent back to the server. This allows the server to detect and handle faulty CGI programs.

Your server will only handle CGI programs. It will not read statically defined HTML files. You can however write a CGI program that simply writes an HTML file to standard output.

HTTP Requests
HTTP is a simple, text-based protocol. When you see a URL like https://www.teach.cs.toronto.edu/local/path/to/resource?name1=value1&name2=value2, the browser will turn into an HTTP request.

An HTTP request has a "start line" followed by optional headers and optional body content. The start line has 3 components separated by a space: method, resource path, and protocol version. For these assignments, we are only interested in HTTP GET requests. The first line of a GET request has the following format:

GET /local/path/to/resource?name1=value1&name2=value2 HTTP/1.1
GET is the method
/local/path/to/resource is the part of the URL that comes after the host name. It specifies the path to the CGI program to run. For these assignments, the resource path will be relative to the current working directory.
The question mark identifies the beginning of the query string. The query string is a list of name=value pairs that form the arguments to a CGI program.
The HTTP/1.1 tells you which version of HTTP is being used. We are going to ignore the HTTP version for these assignments.
Part 1: Makefile and tests
You are asked to write a Makefile for this assignment. You should write it first so that it compiles the programs simple, die, large, and server. Then you can continue to modify it as you develop your server program.

The first target of your Makefile should cause all of the relevant programs to be compiled.

The Makefile will also include a target called "tests" that depends on the appropriate programs being compiled and will run several tests. You may use the sample CGI programs we have provided, but may also write one or two of your own.

You can set up the tests by putting the test input to the server in a file and redirecting standard input. The output from the server can be compared to expected output using diff. You should make sure that you kill the server (man killall) when your tests are finished and remove any temporary files. Your Makefile should run at least 5 different tests. (What we mean by "different" is that each test will test a different aspect of the program.)

Part 2: HTTP server
You will write a C program called server that reads HTTP GET requests from standard input or a file and creates a process to execute a CGI program. It will use a pipe to read the output of the child process. The server will run in an infinite loop reading one GET request at a time and executing the process to satisfy the request. It only needs to handle one request at a time and will not read the next request until it has finished processing the current request. The server process will terminate when standard input is closed or the end of the file is reached. (Similar to A2, you should use fgets to read each line of input and when fgets returns 0, the program exits.

The path to the CGI program will be relative to the current working directory when the server is run. For example, if the resource path is /simple, the server will execute the program ./simple. (Note that resource paths are not absolute paths. simple is not in the root directory of the file system.)

Your server will need to extract the resource string from the GET request, and extract everything after the "?" from the resource string if "?" is present. If the "name=value" pair list is present, then your server will set the environment variable QUERY_STRING to the name=value pair list (man 3 setenv). If there is no query string in the resource, then the environment variable should be set to the empty string.

For example, given the HTTP request GET /simple?colour=blue&size=10 HTTP/1.1, QUERY_STRING would be set to "colour=blue&size=10".

You don't need to worry about URL Encoding.  (Links to an external site.)You can assume that all characters in the resource string are valid URL characters, and do not need to be decoded. For example, there will be no spaces in the resource string, and the characters '?', '&', '=' will not appear as part of a path, a name, or a value. In other words, '?' is only used to separate the path from the query arguments, '=' is only used to separate a name from its value, and '&' is only used to separate "name=value" pairs.

The start line may be followed by a series of lines that have the format

Host: www.teach.cs.toronto.edu
Header1: value
Header2: value
Your server will not need to parse these lines, so it can skip them. In fact, for this assignment, the only part of the HTTP message that the server will use is the line that starts with GET. (For assignment 4, your program will be getting its input from a web browser, so it will need to be able to read the remaining header lines. The HTTP request can also include additional text called the “body” of the message, but we will not handle GET requests with a body in A3 or A4.)

CGI programs print to standard output. The output from the CGI program consists of the HTTP headers followed by an HTML document. You can run the example CGI programs on the command line to see what output they produce.

The server program will read the output of the CGI program from a pipe. For each successful request, the server will print the first line HTTP response and then will print the data from the CGI program that it read from the pipe.  For each unsuccessful request, the server will print the appropriate message provided in the starter code.

The first line of the response is also called the status line and has the following format. (The HTTP protocol stipulates that line endings are carriage return line feeds and \r\n is how they are represented in C strings.)

 HTTP/1.1 200 OK\r\n 
Once again, HTTP/1.1 refers to the version of HTTP that is being used.  The next field is a numeric status code, and the third field is an English phrase describing the status code. The fields are separated by a space.

The only status codes your program needs to worry about are the ones that follow:

200 OK
The request succeeded, and the resulting resource (the file or script output) is returned in the message body.
404 Not Found
The requested resource doesn't exist. (In other words, the CGI program could not be executed.)
500 Server Error
An unexpected server error. The most common cause is a server-side script that has bad syntax, fails, or otherwise can't run correctly. Your server will return this status when the CGI program it tries to execute fails for some reason. Use wait to determine if the process terminated abnormally.
All system calls must be checked for errors (except malloc). All pipes and files must be closed before the program terminates, and all dynamically allocated memory must be freed.

For more information on the HTTP protocol please see HTTP Made Really Easy (Links to an external site.). The appendix has pointers to the HTTP specifications and RFCs.

The initial response line is potentially followed by a series of Headers lines as above, and the body of the response. If the CGI program runs correctly, your server will print to standard output the initial response line and the message data it receives from the pipe connected to the CGI process. The CGI process is responsible for producing the header lines and the body of the response. If the CGI program does not exist or fails, then the server must produce a valid HTML page with an error message. Functions to produce this output are provided in the starter code.

Only the HTTP response messages are written to standard output. All error messages and debugging statements are written to standard error. Debugging statements should be wrapped in a conditional statement that is controlled by the "-v" argument to the program. If you redirect the output of your server program to a file, then the file will contain only the HTTP response message(s) produced by the server.

Note: If your program encounters any errors not specified in this document or the starter code, you should print an appropriate error message to standard error and exit.  You can assume that HTTP request lines are shorter than MAX_LINE, and that you will not get a CGI program that produces a message longer than MAX_LENGTH.

 

Reading data from the pipe
We don't know how much data the CGI program will produce, so when reading from the pipe, the server process needs to read data in fixed size chunks (CHUNK_SIZE) and append them together to produce the full message.  We have placed an arbitrary limit on the size of output from a CGI program for testing purposes (MAX_LENGTH).  The full message should be null-terminated to make it easier to print to standard output, but each chunk will not be null-terminated.

The program large.c is provided to test CGI programs with large outputs.  You are welcome to edit it and use it as a template for different kinds of tests.

 

Starter code
You are given a number of files as starter code:

simple.c - a CGI program that prints a list of the name value pairs from the query string.
die.c - a CGI program that kills itself. Useful for testing your server.
large.c - a CGI program that can be tuned to send different amounts of data.
cgi.c and cgi.h - These files contain some functions that are used by the CGI programs. If you create additional CGI programs as tests, you may find useful.
