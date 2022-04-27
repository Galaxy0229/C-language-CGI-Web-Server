#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <getopt.h>

// The maximum length of an HTTP message line
#define MAX_LINE 256
// The maximum length of an HTTP response message
#define MAX_LENGTH 16*1024
// The size of a chunk of HTTP response to read from the pipe
#define CHUNK_SIZE 1024


void printError(char *);
void printServerError();
void printResponse(char *str);

int debug = 0;


int main(int argc, char **argv) {
    char msg[MAX_LENGTH];
    // int result;

    FILE *fp = stdin; // default is to read from stdin

    // Parse command line options.
    int opt;
    while((opt = getopt(argc, argv, "v")) != -1) {
        switch(opt) {
            case 'v':
                debug = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-v] [filename]\n", argv[0]);
                exit(1);
        }
    }
    if(optind < argc) {
        if((fp = fopen(argv[optind], "r")) == NULL) {
            perror("fopen");
            exit(1);
        }
    }


    // TODO Complete program

    char http[MAX_LINE] = "\0";
    char path2[MAX_LINE];

    // Read HTTP request from stdin or file
    while (fgets(http, MAX_LINE, fp)!= 0){

        // check whether the line begins with "GET"
        if (http[0]== 'G' && http[1]== 'E' && http[2]== 'T'){

            char * path = strchr(http, '/');
            char * query;
            if (strchr(http, '?') != NULL){
                query = strchr(http, '?');

                for (int i = 0; i < strlen(path); i++){
                    if (path[i] == '?'){
                        path[i] = '\0';
                    }
                }
            
            *query = '\0';
            query += 1;

            for (int j = 0; j < strlen(query); j++){
                if (query[j] == ' '){
                    query[j] = '\0';
                }
            }
            path2[0] = '.';
            path2[1] = '\0';
            
            strcat(path2, path); 
            

            // // split the http line by space into three components
            // char * pch = strtok (http, " ");

            // // assign the method
            // strcat(method, pch);
            // pch = strtok (NULL, " ");

            // // assign the resource path
            // strcat(path_query, pch);
            // pch = strtok (NULL, " ");
            // // printf("path_query is %s\n", path_query);
   
            // // check if there is a query string
            // char *mark = strchr(path_query, '?');
            // // printf("mark is %s\n", mark);
            // // char *query;
            // if (mark != NULL){
            //     // split the request by "?"
            //     char * pch2 = strtok(path_query, "?");
            //     strcat(path, pch2);
            //     path[strlen(path)] = '\0';

            //     pch2 = strtok(NULL, "?");

                // assign the query string into QUERY_STRING
                int env = setenv("QUERY_STRING", query, 1);

                if (env < 0){
                perror("setenv");
                exit(1);
                }
                
            }
            // does not contain '?'
            else if (strchr(http, '?') == NULL){
                for (int i = 4; i < strlen(http); i++){
                    if (http[i] != ' ' && http[i] != '\0'){
                        if (path[i] == ' '){
                        path[i] = '\0';
                        }
                        path[i-4] = http[i];
                    }
                    

                    // fprintf(stderr, "http is %c\n", http[i]);

                query = "\0";
                }
                path[strlen(path)] = '\0';
                // fprintf(stderr, "path is %s\n", path);
                
                path2[0] = '.';
                path2[1] = '\0';
            
                strcat(path2, path); 
            }
            

            // char path_name[MAX_LINE] = ".";
            //         strcat(path_name, path);
            //         path_name[strlen(path_name)] = '\0';

            // check if path is valid 
            if (access(path2, F_OK) == 0){
                // run request
                // initialize file descriptors
                int fd[2];
                int status;

                // create a pipe between parent and child 
                pipe(fd);

                // new process
                int c = fork();
                
                if(c < 0){
                    perror("fork");
                    exit(1);
                }

                // child
                else if (c == 0){

                    close(fd[0]);
                    int d = dup2(fd[1], fileno(stdout));

                    if (d == -1){
                    perror("dup2");
                    exit(1);
                    }

                    // close the right 
                    close(fd[1]);

                    // modify the file string
                    char execute[MAX_LINE] = "./";
                    strcat(execute, path);
                    execute[strlen(execute)] = '\0';
                    // printf("execute is %s\n", execute);

                    // char execute[strlen(path) + 2];
                    // execute[0] = '.';
                    // for (int i = 0; i < strlen(path); i++){
                    //     execute[i+1] = path[i];
                    // }
                    // execute[strlen(path) + 1] = '\0';
                    // printf("execute is %s", execute);

                    // char execute_name[strlen(path) + 1];
                    // for (int i = 0; i < strlen(path); i++){
                    //     execute_name[i] = path[i+1];
                    // }
                    // execute_name[strlen(path)] = '\0';
                    // printf("execute_name is %s", execute_name);

                    // char *execute_name = strrchr(path, '/');
                    // printf("This is the name of execute: %s \n", execute_name);
                    // fprintf(stderr,"path is %s\n", path);
                    // fprintf(stderr,"execute_name is %s\n", execute_name);
                    execlp(execute, execute, NULL);
                    perror("execlp");
                    exit(1);
                }

                //  parent
                else {

                    close(fd[1]);
                    wait(&status);
                    

                    int total_bytes = 0;
                    char buffer[CHUNK_SIZE]; 
                    int num_bytes = 1;
                    msg[0] = '\0';

                    while (num_bytes > 0) {
                        num_bytes = read(fd[0], buffer, CHUNK_SIZE);
                        buffer[num_bytes / sizeof(char)] = '\0';
                        strcat(msg, buffer);
                        total_bytes += num_bytes;
                    }
                    msg[total_bytes / sizeof(char)] = '\0';

                    if (num_bytes == -1){
                        perror("read");
                        exit(1);
                    }

                    
                    if (status ==  0){
                        printResponse(msg);
                    }
                    else if (WIFSIGNALED(status) == 1){
                        printServerError();
                    }
                }
            }
            else if (access(path2, F_OK) == -1){
                // fprintf(stderr, "path2 is %s \n", path2);
                printError(path2);
            }    
        }
    }
    
    if(fp != stdin) {
        if(fclose(fp) == EOF) {
            perror("fclose");
            exit(1);
        }
    }
}


/* Print an http error page  
 * Arguments:
 *    - str is the path to the resource. It does not include the question mark
 * or the query string.
 */
void printError(char *str) {
    printf("HTTP/1.1 404 Not Found\r\n\r\n");

    printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n");
    printf("<html><head>\n");
    printf("<title>404 Not Found</title>\n");
    printf("</head><body>\n");
    printf("<h1>Not Found</h1>\n");
    printf("The requested resource %s was not found on this server.\n", str);
    printf("<hr>\n</body></html>\n");
}


/* Prints an HTTP 500 error page 
 */
void printServerError() {
    printf("HTTP/1.1 500 Internal Server Error\r\n\r\n");

    printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n");
    printf("<html><head>\n");
    printf("<title>500 Internal Server Error</title>\n");
    printf("</head><body>\n");
    printf("<h1>Internal Server Error</h1>\n");
    printf("The server encountered an internal error or\n");
    printf("misconfiguration and was unable to complete your request.<p>\n");
    printf("</body></html>\n");
}


/* Prints a successful response message
 * Arguments:
 *    - str is the output of the CGI program
 */
void printResponse(char *str) {
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("%s", str);
}
