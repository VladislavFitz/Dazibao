#include "gtk/gtk.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <fcntl.h>

#include "Structures.h"
#include "string.h"
#include "TLVBuilder.h"
#include "DazibaoProcessing.h"


GtkBuilder *getBuilder();

void on_butAdd_clicked(GtkWidget *TopWindow, gpointer data);
void on_buttonAdd_clicked(GtkWidget *TopWindow, gpointer data);
void on_butQuit_clicked(GtkWidget *TopWindow, gpointer data);
void on_butDelete_clicked(GtkButton *remove, gpointer data);
void on_buttonCancel_clicked(GtkWidget *TopWindow, gpointer data);

void on_cellrenderertoggleSelected_toggled(GtkCellRendererToggle *render, gchar *path, gpointer data);
void on_cellrenderertextName_edited(GtkCellRendererText *renderer, gchar *path, gchar *new_text, gpointer data);
void on_cellrenderertextCount_edited(GtkCellRendererText *renderer, gchar *path, gchar *new_text, gpointer data);
void on_treeviewTLV_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data);
void on_newTLVWindow_destroy_event();

void showTLVWindow(char *tlvPointer, size_t tlvLength, TLVtype tlvType, gchar *title);
void showWelcomeWindow();
void showDialog(char *header, char *message);
int extractCompoundToList(char *compoundPointer, size_t compoundSize, GtkListStore *listStoreTLV);
