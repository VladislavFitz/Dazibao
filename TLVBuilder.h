//
//  TLVBuilder.h
//  Dazibao
//
//  Created by Владислав Фиц on 12.11.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#ifndef Dazibao_TLVBuilder_h
#define Dazibao_TLVBuilder_h

struct TLV * createTextTLV(char * text);
struct TLV * createJPEGTLV(unsigned long len, char * JPEGBytes);
struct TLV * createPNGTLV(unsigned long len, char * PNGBytes);
struct TLV *createGIFTLV(unsigned long len, char * GIFBytes);
struct TLV * createDatedTLV(time_t date, struct TLV * inTLV);
struct TLV * createCompoundTLV(struct TLV ** innerTLVs, unsigned int numOfTLV);
struct TLV * createPadNTLV(size_t length);
struct TLV * createPad1TLV();

#endif
