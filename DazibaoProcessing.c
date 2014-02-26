//
//  DazibaoProcessing.c
//  Dazibao
//
//  Created by Владислав Фиц on 15.12.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include "DazibaoProcessing.h"

#pragma mark - Header processing
char *processHeader(char * dazibao){
    /*
      Check if dazibao has a correct header and return pointer
      to first TLV of Dazibao
      Input: dazibao - pointer to dazibao image in memory 
      Ouput: pointer to first TLV of Dazibao, or NULL in case of error
    */

    struct Header * daziHeader = deserializeHeader(dazibao);
    if (!daziHeader || !checkHeader(*daziHeader)) {
        printf("Incorrect file header\n");
        return NULL;
    } else {
    	free(daziHeader);
        return dazibao + HEADER_LENGTH;
    } 
}

int checkHeader(struct Header h){
    /* Check if Dazibao file header is correct */
    /* Input: h - Dazibao header structure
    /* Return 1 if it is correct else returns 0 */
    if (h.magic == 53 && h.version == 0) {
        return 1;
    } else {
        return 0;
    }
}


struct Header *deserializeHeader(char *dazibao){
    /* Extract Dazibao header */
    /* Input: dazibao - pointer to dazibao image in memory
    /* Return Dazibao header structure */
    char myByte[HEADER_LENGTH];
    memcpy(myByte, dazibao, HEADER_LENGTH);
    if (myByte) {
        struct Header *head = malloc(sizeof(struct Header));
        head->magic = myByte[0];
        head->version = myByte[1];
        short k = myByte[2] << 8 | myByte[3];
        head->mbz = k;
        return head;
    }
    else
        return NULL;
}

void dazibaoLoad(int flags){
    /*
	Load dazibao from file DAZIBAO_PATH and 
	fill global variables mainTLVPointer, mainTLVSize
    */

    if(dazibaoFD != -1)
	close(dazibaoFD);

    if(mainTLVPointer)
	free(mainTLVPointer);

    if( (dazibaoFD = open(dazibaoPath, flags)) == -1){
	printf("This file: %s\n", DAZIBAO_PATH);
        perror("Dazibao file open error");
        exit(EXIT_FAILURE);
    }
    
    struct stat dazibaoFS;
    memset(&dazibaoFS, 0, sizeof(dazibaoFS));
    
    if(fstat(dazibaoFD, &dazibaoFS) == -1){
        perror("Dazibao fstat error");
        exit(EXIT_FAILURE);
    }
    mainTLVSize = dazibaoFS.st_size;
    mainTLVPointer = malloc(mainTLVSize);

    if (read(dazibaoFD, mainTLVPointer, mainTLVSize) == -1){
        perror("Dazibao read error");
        exit(EXIT_FAILURE);
    }

    if(!processHeader(mainTLVPointer)){
	printf("Invalid Dazibao header\n");
	showDialog("Error", "Incorrect file header");
	exit(EXIT_FAILURE);
    }
}

int getDazibaoTLVCount()
{
    /*
	Return number of TLV in opened dazibao 
    */

    unsigned long notProcessedBytes = mainTLVSize - HEADER_LENGTH;
    int TLVcount = 0;
    
    struct TLV * nTLV = malloc(sizeof(struct TLV));
    while (notProcessedBytes) {
        nTLV = deserializeTLV(mainTLVPointer + (mainTLVSize-notProcessedBytes));
	++TLVcount;
        notProcessedBytes -= TLVSize(nTLV);
        free(nTLV);
    }
  
    return TLVcount;
}

unsigned long extendedDazibaoLength(unsigned long dazibaoLength, struct TLV *tlv) {
    /*
     dazibaoLength – size of dazibao to extend
     Input: tlv – tlv to add
     Output: size of extended dazibao
     */

    return dazibaoLength + TLVSize(tlv);
}


char *addTLVtoDazibao(struct TLV *tlv, char *tlvPointer, size_t tlvSize, int currentTLVoffset){
    /*

     Input: tlv – tlv to add
     dazibao – pointer to dazibao to extend
     dazibaoLength – size of dazibao to extend
     Output: pointer to extended dazibao
    
    */

    if (flock(dazibaoFD, LOCK_SH) < 0) {
	if ( errno == EWOULDBLOCK)
		perror("flock: already locked");
	else
		perror("flock: lock error");
    }

    unsigned long newDazibaoSize = extendedDazibaoLength(tlvSize, tlv);
    char *newDazibao = malloc(newDazibaoSize);
    char *tlvBytes = serializeTLV(tlv);   

    if(ftruncate(dazibaoFD, newDazibaoSize) == -1){
        perror("In add ftruncate error");
        return NULL;
    }
    
    g_print("New dazibao length %lu\n", newDazibaoSize);
    size_t gapSize = TLVSize(tlv);
    g_print("Gap size %lu\n", gapSize);
    int gapStartOffset = currentTLVoffset + tlvSize;
    g_print("Gap start offset %i\n", gapStartOffset);
    int gapEndOffset =  currentTLVoffset + tlvSize + gapSize;
    g_print("Gap end offset %i\n", gapEndOffset);
    size_t movedPartSize = tlvSize - gapStartOffset;
    g_print("Moved part size %lu\n", movedPartSize);

    char *movedPart = malloc(movedPartSize);

    if(lseek(dazibaoFD, gapStartOffset, SEEK_SET) == -1){
        perror("In add lseek error");
    }

    if (read(dazibaoFD, movedPart, movedPartSize) == -1){
        perror("In add read error");
    }

    if(lseek(dazibaoFD, gapStartOffset, SEEK_SET) == -1){
        perror("In add lseek error");
    }

    size_t written = write(dazibaoFD, tlvBytes, TLVSize(tlv)) ;
    if( written == -1){
        perror("In add TLV write error");
    }
    
    if(lseek(dazibaoFD, gapEndOffset, SEEK_SET) == -1){
        perror("In add movedPart lseek error");
    }
    written = write(dazibaoFD, movedPart, movedPartSize);
    if( written == -1){
        perror("In add movedPart write error");
    } 
    
    memcpy(newDazibao, tlvPointer, gapStartOffset);
    memcpy(newDazibao + gapStartOffset, tlvBytes, gapSize);
    memcpy(newDazibao + gapEndOffset, movedPart, movedPartSize);
    
    if(currentTLVoffset != 0){
	/* It's compound TLV*/
	printf("It's compound TLV\n");
	if(lseek(dazibaoFD, currentTLVoffset + TYPE_LENGTH, SEEK_SET) == -1){
        	perror("In add movedPart lseek error");
    	}
    	written = write(dazibaoFD, intToChar24Bit(currentTLVsize+gapSize), LENGTH_LENGTH) ;
	if( written == -1){
	        perror("In add TLV write error");
    	}
    }	

    if (flock(dazibaoFD, LOCK_UN) < 0)
	perror("flock: release error");

    free(tlvBytes);
    free(movedPart);
    return newDazibao;
}

size_t writeDazibaoToFile(char *dazibao, size_t dazibaoLength){
    if (flock(dazibaoFD, LOCK_SH) < 0) {
	if ( errno == EWOULDBLOCK)
		perror("flock: already locked");
	else
		perror("flock: lock error");
    }

    if(lseek(dazibaoFD, 0, SEEK_SET) == -1){
        perror("Dazibao lseek error");
        return -1;
    }
    
    if(ftruncate(dazibaoFD, dazibaoLength) == -1){
        perror("Dazibao ftruncate error");
        return -1;
    }
    size_t written = write(dazibaoFD, dazibao, dazibaoLength) ;
    if(written == -1){
        perror("Dazibao write error");
        return -1;
    }

    if (flock(dazibaoFD, LOCK_UN) < 0)
	perror("flock: release error");

    return written;
}


int getOffsetToTLVatIndex(unsigned int tlvIndex, char *tlvPointer, size_t tlvSize){
    unsigned long notProcessedBytes = tlvSize;
    notProcessedBytes -= HEADER_LENGTH;

    while (notProcessedBytes && tlvIndex > 0) {
        int tlvLength = char24BitToInt(tlvPointer + (tlvSize-notProcessedBytes) + TYPE_LENGTH);
        int tlvSize = tlvLength + TYPE_LENGTH + LENGTH_LENGTH;
        notProcessedBytes -= tlvSize;
        --tlvIndex;
    }
    
    if (notProcessedBytes == 0) {
        printf("TLV index out of range\n");
        return -1;
    }
    
    return (int)(tlvSize-notProcessedBytes);
}

char * getPointerToTLVatIndex(unsigned int tlvIndex, char * tlvPointer, size_t tlvSize){
    return tlvPointer + getOffsetToTLVatIndex(tlvIndex, tlvPointer, tlvSize);
}

unsigned int getLengthOfTLVatIndex(unsigned int tlvIndex, char *tlvPointer, size_t tlvSize){
    char *tlvLengthPointer = getPointerToTLVatIndex(tlvIndex, tlvPointer, tlvSize)+TYPE_LENGTH;
    if (tlvLengthPointer) {
        return char24BitToInt(tlvLengthPointer);
    } else {
        return -1;
    }
}

size_t getSizeOfTLVatIndex(unsigned int tlvIndex, char *tlvPointer, size_t tlvSize){
    size_t tlvLength = getLengthOfTLVatIndex(tlvIndex, tlvPointer, tlvSize);
    if (tlvSize == -1) {
        return -1;
    } else {
        return tlvLength + TYPE_LENGTH + LENGTH_LENGTH;
    }
}


size_t removeTLVatIndex(unsigned int tlvIndex, unsigned int tlvCount, char *tlvPointer, size_t tlvSize){
    /*
	Remove TLV from Dazibao at index tlvIndex
	Input: tlvIndex - index of TLV to delete (starting with 0)
	       tlvCount - total count of TLV at Dazibao file
	       tlvPointer - pointer to memory image of Dazibao file
	       tlvSize - size of memoty image of Dazibao file
	Output: new Dazibao size
    */

    if (tlvIndex >= tlvCount) {
        printf("TLV out of range");
        return -1;
    }

    size_t TLVtoDeleteSize = getSizeOfTLVatIndex(tlvIndex, tlvPointer, tlvSize);
    
    if (flock(dazibaoFD, LOCK_SH) < 0) {
	if ( errno == EWOULDBLOCK)
		perror("flock: already locked");
	else
		perror("flock: lock error");
    }

    if (tlvIndex == tlvCount-1) {
        printf("Remove TLV from end\n");
        if (ftruncate(dazibaoFD, tlvSize - TLVtoDeleteSize) == -1){
            perror("ftruncate error while removing TLV");
            return -1;
        }
	mainTLVPointer = tlvPointer;
	mainTLVSize = tlvSize - TLVtoDeleteSize;
    	if (flock(dazibaoFD, LOCK_UN) < 0)
		perror("flock: release error");

        return tlvSize - TLVtoDeleteSize;
        
    } else {
        printf("Remove TLV in the middle\n");

        int offset = getOffsetToTLVatIndex(tlvIndex, mainTLVPointer, mainTLVSize);
        char *tlvToDeletePointer = getPointerToTLVatIndex(tlvIndex, mainTLVPointer, mainTLVSize);

        struct TLV *padTLV = createPadNTLV(TLVtoDeleteSize - TYPE_LENGTH - LENGTH_LENGTH);
        char * serializedPadTLV = serializeTLV(padTLV);

        memcpy(tlvToDeletePointer, serializedPadTLV, TLVtoDeleteSize);
        
        if(lseek(dazibaoFD, offset, SEEK_SET) == -1){
            perror("lseek error while removing TLV");
            return -1;
        }
        if(write(dazibaoFD, serializedPadTLV, TLVtoDeleteSize) == -1){
            perror("write error while removing TLV");
            return -1;
        }
        

    	if (flock(dazibaoFD, LOCK_UN) < 0)
		perror("flock: release error");


        return tlvSize;
    }
}

size_t compressDazibao(char *tlvPointer, size_t tlvSize){
    /*
	Remove all PAD1 and PADN TLV from dazibao file
	Input: tlvPointer - pointer to dazibao File memory image
	       tlvSize - size of dazibao file in bytes
	Output: size of compressed dazibao
    */
    size_t compressedTLVSize = tlvSize;
    unsigned long notProcessedBytes = tlvSize;
    notProcessedBytes -= HEADER_LENGTH;
    int compressedCount = 0;

    printf("Compression started...\n");

    while (notProcessedBytes) {
        size_t offset = compressedTLVSize - notProcessedBytes;
        TLVtype type = (int)*(tlvPointer + offset);
        //printf("TLV type: %i\n", type);
        int tlvToRemoveLength = char24BitToInt(tlvPointer + offset + TYPE_LENGTH);

        int tlvToRemoveSize = tlvToRemoveLength + TYPE_LENGTH + LENGTH_LENGTH;
        
        switch (type) {
            case PAD1:
            {
                memcpy(tlvPointer+offset, tlvPointer + offset + TYPE_LENGTH, notProcessedBytes-TYPE_LENGTH);
                compressedTLVSize -= TYPE_LENGTH;
                notProcessedBytes -= TYPE_LENGTH;
                ++compressedCount;
            }
                break;

            case PADN:
            {
                memcpy(tlvPointer+offset, tlvPointer + offset + tlvToRemoveSize, notProcessedBytes-tlvToRemoveSize);
                compressedTLVSize -= tlvToRemoveSize;
                notProcessedBytes -= tlvToRemoveSize;
                ++compressedCount;
            }
                break;
            default:
            {
                notProcessedBytes -= tlvToRemoveSize;
            }
                break;
        }
    }
    
    if (compressedCount) {
        if(writeDazibaoToFile(tlvPointer, compressedTLVSize) != -1){
		printf("Dazibao file was successfully compressed, %i empty TLV removed, %lu bytes saved\n", compressedCount, tlvSize-compressedTLVSize);

	}
    } else {
	printf("No empty TLV found\n");
    }
    
    return compressedTLVSize;
}
