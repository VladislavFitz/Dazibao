//
//  TLVProcessing.c
//  Dazibao
//
//  Created by Владислав Фиц on 10.11.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include "Conversions.h"
#include "Structures.h"
#include "TLVProcessing.h"

time_t deserializeDate(const char * datedContent){
    /* Reads 4 first bytes of bytes array datedContent */
    /* Returns date like time_t */
    unsigned int date; 
    memcpy(&date, datedContent, DATE_LENGTH);
    time_t tm = toBigEndianInt(date);
    return tm;
}

char * serializeDate(time_t time){
    unsigned int bigEndianDate = toBigEndianInt((unsigned int)time);
    char * dateInBytes = malloc(DATE_LENGTH * sizeof(char));
    memcpy(dateInBytes, &bigEndianDate, DATE_LENGTH);
    return dateInBytes;
}

char * serializeTLV(struct TLV * inTLV){
    char * TLVbytes = malloc(TLVSize(inTLV));
    memcpy(TLVbytes, &inTLV->type, TYPE_LENGTH);
    memcpy(TLVbytes+TYPE_LENGTH, intToChar24Bit(inTLV->length), LENGTH_LENGTH);
    memcpy(TLVbytes+TYPE_LENGTH+LENGTH_LENGTH, inTLV->content, inTLV->length);

    return TLVbytes;
}

struct TLV *deserializeTLV(char * TLVBytes){
    /* Extracts next TLV from bytes array *TLVBytes */
    /* Returns pointer to TLV struct if TLV was successfully read else returns NULL */

    char tlvType = getTLVType(TLVBytes);
    if (tlvType == PAD1)
    {
    	struct TLV *newTLV = malloc(sizeof(newTLV));
	newTLV->type = PAD1;
        newTLV->length = 0;
        newTLV->content = NULL;
	return newTLV;
    }

    size_t tlvLength = getTLVLength(TLVBytes);
    
    struct TLV *newTLV = malloc(tlvLength + TYPE_LENGTH + LENGTH_LENGTH);

    newTLV->type = tlvType;
    newTLV->length = tlvLength;
    newTLV->content = malloc(tlvLength);

    memcpy(newTLV->content, TLVBytes + TYPE_LENGTH + LENGTH_LENGTH, tlvLength);

    return newTLV;
}

void printTLVSeparator(){
    printf("\n--------------------------------------------------------------------------\n");
}

char *getTLVTypeName(struct TLV t){
    switch (t.type) {
        case PAD1:
	    return "PAD1";
            break;
            
        case PADN:
            return "PADN";
            break;
            
        case TEXT:
            return "TEXT";
            break;
            
        case PNG:
            return "PNG";
            break;
            
        case JPEG:
            return "JPEG";
            break;
	
	case GIF:
	    return "GIF";
	    break;
            
        case COMPOUND:
            return "COMPOUND";
            break;
            
        case DATED:
            return "DATED";
            break;

  	case MAIN:
	    return "MAIN";
	    break;
            
        default:
            return "UNKNOWN";
            break;
    }
}

void printTLVType(struct TLV t){
    /* Prints TLV type */
    char *type = getTLVTypeName(t);
    printf("TLV type: %s\n", type);
}

void processTLV(struct TLV * inTLV){
    /* Process TLV depending on TLV type*/
   
    printTLVSeparator();
    printTLVType(*inTLV);
    printf("Length: %u \n", inTLV->length);

    switch (inTLV->type) {
        case TEXT:
        {
            char text[inTLV->length+1];
            memcpy(text, inTLV->content, inTLV->length);
            text[inTLV->length+1] = '\0';
            printf("Text: %s\n", inTLV->content);
        }
            break;

	case PAD1:
	{
	    printf("PAD1 was processed\n");
	    exit(EXIT_SUCCESS);
	}
	    break;
            
        case PADN:
        {
            printf("PADN: %i bytes ignored\n", inTLV->length);
        }
            break;
            
        case PNG:
        case JPEG:
	case GIF:
        {
            printf("Jpeg/Png/Gif was successfully read\n");
        }
            break;
            
        case DATED:
        {
            time_t date = deserializeDate(inTLV->content);
            char * temps = ctime(&date);
            printf("Date: %s\n", temps);
            char * innerTLV = inTLV->content + DATE_LENGTH;
            struct TLV *datedTLV = deserializeTLV(innerTLV);
            processTLV(datedTLV);
            free(datedTLV);
        }
            break;

        case MAIN:
        case COMPOUND:
        {
            unsigned int lk = inTLV->length;
            while (lk != 0) {
                struct TLV *childTLV = deserializeTLV(inTLV->content + (inTLV->length - lk));
                unsigned int childLength = childTLV->length+TYPE_LENGTH + LENGTH_LENGTH;
                lk -= childLength;
                processTLV(childTLV);
                free(childTLV);
            }
        }
            break;
            
        default:
        {
            printf("Unrecognized TLV was successfully read\n");
        }
            break;
    }
}

TLVtype getFileType(char *path){
	char *fileExt = strrchr(path, '.');

	if(strcmp(fileExt, ".jpeg") == 0 || strcmp(fileExt, ".jpg") == 0){
		return JPEG;
	} else if (strcmp(fileExt, ".png") == 0){
		return PNG;
	} else if (strcmp(fileExt, ".gif") == 0){
		return GIF;
	} else
		return UNKNOWN;
}

size_t TLVSize(struct TLV * inTLV){
    return TYPE_LENGTH + LENGTH_LENGTH + inTLV->length;
}

char getTLVType(char *tlvPointer){
    return tlvPointer[0];
}

size_t getTLVLength(char *tlvPointer){
    return char24BitToInt(tlvPointer+TYPE_LENGTH);
}

char* getTLVContentPointer(char *tlvPointer){
    return tlvPointer + HEADER_LENGTH;
}
