//
//  main.c
//  DazibaoNotifications
//
//  Created by Владислав Фиц on 28.12.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "sys/stat.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <pwd.h>

#include "NotificationHandler.h"

#define SOCKET_NAME ".dazibao-notification-socket"
#define BUFFER_SIZE 1024
#define CHECK_TIMEOUT 1

int establishConnection(){
    struct sockaddr_un address;
    int socket_fd;
    
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    int pathLength = strlen(homedir) + strlen(SOCKET_NAME) + 2;
    char socketPath[pathLength];
    snprintf(socketPath, pathLength, "%s/%s", homedir, SOCKET_NAME);
    socketPath[pathLength] = '\0';
    printf("Socket path %s, length %i\n", socketPath, pathLength);
    
    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        perror("socket() failed");
        return -1;
    }
    
    memset(&address, 0, sizeof(struct sockaddr_un));
    
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socketPath, sizeof(address.sun_path)-1);
    
    int rc = connect(socket_fd, (struct sockaddr*)&address, sizeof(address));
    if(rc < 0) {
        perror("connect() failed");
        return -1;
    }

    printf("Notification socket connection established\n");
    return socket_fd;
}

int sendNotification(const char *path, int socket_fd){
    
    char buffer[BUFFER_SIZE];
    size_t nbytes;
    
    nbytes = snprintf(buffer, BUFFER_SIZE, "%c%s",'C',path);
    
    size_t ws = write(socket_fd, buffer, nbytes);
    if (ws == -1) {
        perror("Error while sending notification to socket");
        return -1;
    }
    return 0;
}



int main(int argc, const char * argv[])
{
    int dazibaoCount = argc-1;
    int socket_fd;
    char *dazibaoPaths[dazibaoCount];
    long dazibaoMtimes[dazibaoCount];
    int i;

    initNotifications();
    if (argc < 2) {
        printf("No dazibao to handle\n");
        exit(EXIT_SUCCESS);
    } else {
        for (i = 1; i <= dazibaoCount; ++i) {
	    dazibaoPaths[i-1] = realpath(argv[i], NULL);
            printf("[%i] %s\n",i-1, dazibaoPaths[i-1]);
        }
    }
 
    for (i = 0; i < dazibaoCount; ++i) {
        int dazibaoFD;
        if( (dazibaoFD = open(dazibaoPaths[i], O_RDONLY)) == -1){
            perror("Dazibao file open error");
            exit(EXIT_FAILURE);
        }
        
        struct stat dazibaoFS;
        
        memset(&dazibaoFS, 0, sizeof(dazibaoFS));
        
        if(fstat(dazibaoFD, &dazibaoFS) == -1){
            perror("Dazibao fstat error");
            exit(EXIT_FAILURE);
        }
        
        dazibaoMtimes[i] = dazibaoFS.st_mtime;
        //printf("Set init mtime %lu for dazibaoPath %s\n", dazibaoMtimes[i], dazibaoPaths[i]);
    }
    socket_fd = establishConnection();
    if(socket_fd < 0)
        exit(EXIT_FAILURE);
    
    do{
        sleep(CHECK_TIMEOUT);

        for (i = 0; i < dazibaoCount; ++i) {
            
            int dazibaoFD;
            if( (dazibaoFD = open(dazibaoPaths[i], O_RDONLY)) == -1){
                perror("Dazibao file open error");
    		if(close(socket_fd) < 0)
        		perror("Socket close error");
                exit(EXIT_FAILURE);
            }
            
            struct stat dazibaoFS;
            memset(&dazibaoFS, 0, sizeof(dazibaoFS));
            
            if(fstat(dazibaoFD, &dazibaoFS) == -1){
                perror("Dazibao fstat error");
    		if(close(socket_fd) < 0)
        		perror("Socket close error");
                exit(EXIT_FAILURE);
            }
	    //printf("Pending %lu   ->    %lu\n", dazibaoMtimes[i], dazibaoFS.st_mtime);
            if (dazibaoMtimes[i] != dazibaoFS.st_mtime) {
                printf("%s was changed\n", dazibaoPaths[i]);
		char message[BUFFER_SIZE];
		snprintf(message, 21+strlen(dazibaoPaths[i]), "Dazibao %s was changed", dazibaoPaths[i]);
		postNotification("Dazibao", message, 500);
                if(sendNotification(dazibaoPaths[i], socket_fd) < 0){
    		    if(close(socket_fd) < 0)
        		perror("Socket close error");
                    exit(EXIT_FAILURE);
		}
                
                dazibaoMtimes[i] = dazibaoFS.st_mtime;
            }
            
        }
        
    } while (1);
    

    
    exit(EXIT_SUCCESS);
}

