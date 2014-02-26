//
//  TLVProcessing.h
//  Dazibao
//
//  Created by Владислав Фиц on 10.11.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#ifndef Dazibao_TLVProcessing_h

#include "time.h"
#define Dazibao_TLVProcessing_h



time_t deserializeDate(const char * datedContent);
char * serializeDate(time_t time);

struct TLV *deserializeTLV(char * TLVBytes);
char * serializeTLV(struct TLV * inTLV);

void processTLV(struct TLV * inTLV);
size_t TLVSize(struct TLV * inTLV);
void printTLVType(struct TLV t);
char *getTLVTypeName(struct TLV t);

char getTLVType(char *tlvPointer);
size_t getTLVLength(char *tlvPointer);
char* getTLVContentPointer(char *tlvPointer);

TLVtype getFileType(char *path);

#endif