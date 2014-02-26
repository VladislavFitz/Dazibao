//
//  Conversions.c
//  Dazibao
//
//  Created by Владислав Фиц on 10.11.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"


unsigned int revertInt(unsigned int in){
    int b0 = in & 0xff;
    b0 = b0<<24;
    int b1 = in & 0xff00;
    b1 = b1<<8;
    int b2 = in & 0xff0000;
    b2 = b2>>8;
    int b3 = in & 0xff000000;
    b3 = b3>>24;
    
    return b0|b1|b2|b3;
}

unsigned int toBigEndianInt(unsigned int in){
    return revertInt(in);
}

unsigned int toLittleEndianInt(unsigned int in){
    return revertInt(in);
}



char * intToChar24Bit(int in){
    char * out = malloc(LENGTH_LENGTH * sizeof(char));
    
    out[2] = (char)(in & 0xff);
    out[1] = (char)((in & 0xff00)>>8);
    out[0] = (char)((in & 0xff0000)>>16);
    
    return out;
}

unsigned int char24BitToInt(char * in){
    unsigned int c = in[2];
    c = c & 0xff;
    
    unsigned int b = in[1];
    b = b & 0xff;
    b = b<<8;
    
    unsigned int a = in[0];
    a = a & 0xff;
    a= a<<16;
    
    return a|b|c;;
}