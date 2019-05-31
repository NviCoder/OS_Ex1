// This is C program to Q2 in our HW.
// We use two pipes 
// First pipe to send input string from parent 
// Second pipe to send concatenated string from child 

// Elad and Yoav
// based on : https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include "md5.h"

int childPID;
char md5String[33];
//This function will kill the son from the signal handler
void killC(int sig){
    exit(0);
}
//signal handler for checking the MD5 string
void handleMD5(int sig) 
 { 
   if(strlen(md5String)==32){
    printf("encrypted by process %d: ",childPID);
    printf("%s\n",md5String);
    kill(childPID,SIGUSR2);
    }
 }  
int main() 
{ 
    //Signal for checking the string 
    signal(SIGUSR1, handleMD5);
    signal(SIGUSR2, killC);
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
    char input_str[100]; 
    int p; 
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    scanf("%s", input_str); 
    p = fork();
    childPID = p;
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
    // Parent process 
    else if (p > 0) 
    {  
        close(fd1[0]);  // Close reading end of first pipe 
        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str, strlen(input_str)+1); 
        close(fd1[1]); 
        close(fd2[1]); // Close writing end of second pipe 
        // Read string from child, print it and close 
        // reading end. 
        read(fd2[0],md5String, 100); 
        // Wait for child to send a string
        wait(NULL); 
        close(fd2[0]); 
        kill(p, SIGUSR1);
    }
    // child process 
    else
    {           
        close(fd1[1]);  // Close writing end of first pipe 
        int ppid=getppid();
        // Read a string using first pipe 
        char concat_str[100]; 
        read(fd1[0], concat_str, 100); 
        // Close both reading ends 
        close(fd1[0]); 
        close(fd2[0]); 
        // Write concatenated string and close writing end 
        write(fd2[1], md5(concat_str).c_str(), 32); 
        close(fd2[1]); 
        kill(ppid,SIGUSR1);
    } 
      exit(0); 
} 
