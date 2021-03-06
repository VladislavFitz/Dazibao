//
//  DazibaoProcessing.h
//  Dazibao
//
//  Created by Владислав Фиц on 15.12.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#ifndef Dazibao_DazibaoProcessing_h
#define Dazibao_DazibaoProcessing_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#include "Structures.h"
#include "TLVProcessing.h"
#include "TLVBuilder.h"
#include "Conversions.h"
#include "NotificationHandler.h"


#pragma mark - Dazibao Processing
int checkHeader(struct Header h);
struct Header *deserializeHeader(char *dazibao);
char *processHeader(char * dazibao);
void dazibaoLoad(int flags);

#pragma mark - Dazibao extension
unsigned long extendedDazibaoLength(unsigned long dazibaoLength, struct TLV *tlv);
char *addTLVtoDazibao(struct TLV *tlv, char *tlvPointer, size_t tlvSize, int currentTLVoffset);

#pragma mark - Dazibao TLV removing
size_t removeTLVatIndex(unsigned int tlvIndex, unsigned int tlvCount, char *tlvPointer, size_t tlvSize);

#pragma mark - Dazibao Compression
size_t compressDazibao(char *tlvPointer, size_t tlvSize);

#pragma mark - Dazibao Writing to file
size_t writeDazibaoToFile(char *dazibao, size_t dazibaoLength);

#pragma mark - Attributes of TLVs in Dazibao getters
size_t getSizeOfTLVatIndex(unsigned int tlvIndex, char *tlvPointer, size_t tlvSize);
unsigned int getLengthOfTLVatIndex(unsigned int tlvIndex, char *tlvPointer, size_t tlvSize);
char * getPointerToTLVatIndex(unsigned int tlvIndex, char * tlvPointer, size_t tlvSize);
int getOffsetToTLVatIndex(unsigned int tlvIndex, char *tlvPointer, size_t tlvSize);


int getDazibaoTLVCount();

#endif
