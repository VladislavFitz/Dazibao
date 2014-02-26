//
//  main.c
//  Dazibao
//
//  Created by Владислав Фиц on 01.11.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/un.h>
#include "gtk/gtk.h"

#include "errno.h"

#include "Structures.h"
#include "DazibaoProcessing.h"
#include "GUI.h"


int dazibaoFD;
char *dazibaoPath;

char *mainTLVPointer;
size_t mainTLVSize;

char *currentTLVpointer;
size_t currentTLVsize;
int currentTLVoffset;
int DRDONLY;

int main(int argc, char * argv[])
{
    gtk_init( &argc, &argv );
    showWelcomeWindow();
    gtk_main ();
    
    exit(EXIT_SUCCESS);
}





