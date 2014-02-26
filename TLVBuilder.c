//
//  TLVBuilder.c
//  Dazibao
//
//  Created by Владислав Фиц on 12.11.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"
#include "time.h"
#include "string.h"
#include "TLVProcessing.h"

#define NULL_TERM_LENGTH 1

struct TLV * createTLV(TLVtype type, unsigned long length, char * content){
    struct TLV * newTLV = malloc(sizeof(struct TLV));
    newTLV->type = type;
    newTLV->length = (unsigned int)length;
    newTLV->content = malloc(length);
    memcpy(newTLV->content, content, length);
    
    return newTLV;
}

struct TLV * createTextTLV(char * text){
    struct TLV *textTLV = createTLV(TEXT, (unsigned int)strlen(text), text);
    return textTLV;
}

struct TLV * createJPEGTLV(unsigned long len, char * JPEGBytes){
    struct TLV *jpegTLV = createTLV(JPEG, len, JPEGBytes);
    return jpegTLV;
}


struct TLV * createPNGTLV(unsigned long len, char * PNGBytes){
    struct TLV *pngTLV = createTLV(PNG, len, PNGBytes);
    return pngTLV;
}

struct TLV *createGIFTLV(unsigned long len, char * GIFBytes){
    struct TLV *gifTLV = createTLV(GIF, len, GIFBytes);
    return gifTLV;
}

struct TLV * createDatedTLV(time_t date, struct TLV * inTLV){
    char *content = malloc(DATE_LENGTH + TLVSize(inTLV));
    char *serializedDate = serializeDate(date);
    char *serializedTLV = serializeTLV(inTLV);
    
    memcpy(content, serializedDate, DATE_LENGTH);
    memcpy(content + DATE_LENGTH, serializedTLV, TLVSize(inTLV));
    
    struct TLV *datedTLV = createTLV(DATED, DATE_LENGTH + TLVSize(inTLV), content);
    free(serializedTLV);
    free(content);
    return datedTLV;
}

struct TLV * createCompoundTLV(struct TLV ** innerTLVs, unsigned int numOfTLV){
    unsigned long totalLength = 0;
    int i = 0;

    for (i = 0; i<numOfTLV; ++i) {
        struct TLV * inner = innerTLVs[i];
        totalLength += TLVSize(inner);
    }
    
    char * totalContent = malloc(totalLength);
    unsigned long offset = 0;

    for (i = 0; i<numOfTLV; ++i) {
        struct TLV * inner = innerTLVs[i];
        char * innerSerialized = serializeTLV(inner);
        unsigned long innerLength = TLVSize(inner);
        memcpy(totalContent + offset, innerSerialized, innerLength);
        offset += innerLength;
        free(innerSerialized);
    }
    
    struct TLV *compoundTLV = createTLV(COMPOUND, totalLength, totalContent);
    free(totalContent);
    
    return compoundTLV;
}

struct TLV * createPadNTLV(size_t length){
    char *content = malloc(sizeof(content)*length);
    memset(content, 0, length);
    struct TLV * padNTLV = createTLV(PADN, length, content);
    free(content);
    return padNTLV;
}
