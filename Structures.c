//
//  Structures.c
//  Dazibao
//
//  Created by Владислав Фиц on 01.11.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include <stdio.h>

struct Header {
    char magic;
    char version;
    short mbz;
};

struct TLV {
    char type;
    unsigned int length;
    char * content;
};

struct Dazibao {
    unsigned int numberOfTLV;
    struct TLV ** tlvs;
};