//
//  main.c
//  DazibaoServer
//
//  Created by Владислав Фиц on 09.12.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

#define SOCKET_NAME ".dazibao-notification-socket"
#define BUF_SIZE 1024

int main(void)
{
    struct sockaddr_un address;
    size_t bytes;
    char buffer[BUF_SIZE];
    int socket_fd, connection_fd;
    socklen_t address_length;
    
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    char socketPath[50];
    sprintf(socketPath, "%s/%s", homedir, SOCKET_NAME);
   
    
    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("socket() failed\n");
        return 1;
    }
    
    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;

    strncpy(address.sun_path, socketPath, sizeof(address.sun_path));
    
    if(access(address.sun_path, F_OK) == 0)
	    unlink(address.sun_path);

    if(bind(socket_fd,
            (struct sockaddr *) &address,
            sizeof(struct sockaddr_un)) != 0)
    {
        perror("bind() failed");
        return 1;
    }
    
    if(listen(socket_fd, 5) != 0)
    {
        perror("listen() failed");
        return 1;
    }


    for(;;) {
	if( (connection_fd = accept(socket_fd, (struct sockaddr *) &address, &address_length)) < 0)
		perror("accept_error");
	
	while ( (bytes = read(connection_fd, buffer, BUF_SIZE)) > 0 )
		if (write(STDOUT_FILENO, buffer, bytes) != bytes)
			perror("write error");
	
	if (bytes < 0)
		perror("read error");
	
	if(close(socket_fd) < 0) 
		perror("close error");
    }

    return 0;
}
