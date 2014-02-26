//
//  Structures.h
//  Dazibao
//
//  Created by Владислав Фиц on 01.11.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#ifndef Dazibao_Structures_h

#define Dazibao_Structures_h

#define DAZIBAO_PATH "/home/coolermaster/Dazibao/exemple.dzb"
#define UI_FILE "mainform.ui"
#define SOCKET_NAME "/Users/CoolerMaster/demo_socket"
#define BUFFER_SIZE 1024

#define HEADER_LENGTH 4
#define TYPE_LENGTH 1
#define TLV_LENGTH 3
#define LENGTH_LENGTH 3
#define DATE_LENGTH 4
#define TLV_DESCRIPTION_LENGTH 4
#include "gtk/gtk.h"

#define PAD1 (unsigned char)0
#define PADN (unsigned char)1
#define TEXT (unsigned char)2
#define PNG (unsigned char)3
#define JPEG (unsigned char)4
#define COMPOUND (unsigned char)5
#define DATED (unsigned char)6
#define MAIN (unsigned char)250
#define UNKNOWN (unsigned char)251
#define GIF (unsigned char)252

extern char *mainTLVPointer;
extern size_t mainTLVSize;

extern int dazibaoFD;
extern char *currentTLVpointer;
extern int currentTLVoffset;
extern size_t currentTLVsize;
extern char *dazibaoPath;
extern int DRDONLY;

typedef unsigned char TLVtype ;

struct Header {
    char magic;
    char version;
    short mbz;
};

struct TLV {
    TLVtype type;
    unsigned int length;
    char * content;
};

enum {
 NAME = 1,
 COUNT = 0,
 SELECTED = 2,
 LENGTH = 3
};

struct MainWindowObjects
{
    GtkWindow      *topWindow;
    GtkTreeView    *treeviewTLV;
    GtkListStore   *liststoreTLV;
    char 	   *compoundPointer;
    size_t	   compoundSize;
    char	   isMain;
};

struct WelcomeWindowObjects
{
    	GtkWindow      *welcomeWindow;
        GtkCheckButton *readonlyCheckbox;
    	GtkFileChooserButton *fileChooser;
};

struct AddWindowObjects
{
	GtkWindow	*addWindow;
	GtkCheckButton	*dateCheckButton;
	GtkTextView	*tlvTextView;
	GtkFileChooserButton *tlvFileChooser;
    	GtkTreeView    *treeviewTLV;
    	GtkListStore   *liststoreTLV;
};

#endif
