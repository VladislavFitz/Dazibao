#include <stdlib.h>
#include <gtk/gtk.h>

#define UI_FILE "mainform.ui"

enum {
 NAME = 1,
 COUNT = 0,
 SELECTED = 2
};
struct MainWindowObjects
{
    GtkWindow      *topWindow;
    GtkTreeView    *treeviewTLV;
    GtkListStore   *liststoreTLV;
    GtkEntry       *entryTLV;
} mainWindowObjects;


void on_butQuit_clicked(GtkWidget *TopWindow, gpointer data){}
void on_butDelete_clicked(GtkButton *remove, gpointer data){}
void on_butAdd_clicked(GtkWidget *TopWindow, gpointer data){
    struct MainWindowObjects *mwo = data;
    gtk_entry_set_text(mwo->entryTLV,"Hello!");
    GtkTreeIter iter;
    gtk_list_store_append(GTK_LIST_STORE( mwo->liststoreTLV ), &iter);
    gtk_list_store_set(GTK_LIST_STORE(mwo->liststoreTLV), &iter, 
               NAME, gtk_entry_get_text(mwo->entryTLV),
	       COUNT, 55,
		-1);
}
void on_treeviewTLV_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data){}
void on_cellrenderertoggleSelected_toggled(GtkCellRendererToggle *render, gchar *path, gpointer data){}
void on_cellrenderertextName_edited(GtkCellRendererText *renderer, gchar *path, gchar *new_text, gpointer data){}
void on_cellrenderertextCount_edited(GtkCellRendererText *renderer, gchar *path, gchar *new_text, gpointer data){}



int main(int argc, char** argv)
{
    GtkBuilder *builder;
    GError *error = NULL;
    gtk_init( &argc, &argv );

    builder = gtk_builder_new();

    if( ! gtk_builder_add_from_file( builder, UI_FILE, &error ) )
    {
     g_warning( "%s\n", error->message );
     g_free( error );
     return( 1 );
    }

    mainWindowObjects.topWindow = GTK_WINDOW(gtk_builder_get_object(builder, "topWindow"));
    mainWindowObjects.treeviewTLV = GTK_TREE_VIEW( gtk_builder_get_object( builder, "treeviewTLV" ) );
    mainWindowObjects.liststoreTLV = GTK_LIST_STORE( gtk_builder_get_object(builder, "liststoreTLV") );
    mainWindowObjects.entryTLV = GTK_ENTRY( gtk_builder_get_object(builder, "entryTLV") );

    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(mainWindowObjects.treeviewTLV) );
    gtk_tree_selection_set_mode( selection, GTK_SELECTION_MULTIPLE );

    gtk_builder_connect_signals (builder, &mainWindowObjects);
    
    g_object_unref( G_OBJECT( builder ) );
    gtk_widget_show_all ( GTK_WIDGET (mainWindowObjects.topWindow) );
    gtk_main ();

}

