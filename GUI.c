#include "GUI.h"
unsigned int windowCount;

GtkBuilder *getBuilder(){
    GtkBuilder *builder;
    GError *error = NULL;
    builder = gtk_builder_new();

    if( ! gtk_builder_add_from_file( builder, UI_FILE, &error ) )
    {
     g_warning( "%s\n", error->message );
     g_free( error );
     exit(EXIT_FAILURE);
    }
    return builder;
}


#pragma mark - TreeView rows handling
static void remove_row (GtkTreeRowReference *ref, GtkTreeModel *model)
{
    GtkTreeIter iter;
    GtkTreePath *path;
    path = gtk_tree_row_reference_get_path (ref);
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
}

void removeAllRows(GtkTreeView *treeView){
    GList *rows, *ptr, *references = NULL;
    GtkTreeRowReference *ref;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeView);
    GtkTreeModel *model = gtk_tree_view_get_model (treeView);
    gtk_tree_selection_select_all(selection);
    rows = gtk_tree_selection_get_selected_rows (selection, &model);
    ptr = rows;

    while (ptr != NULL)
    {
        ref = gtk_tree_row_reference_new (model, (GtkTreePath*) ptr->data);
        references = g_list_prepend (references, gtk_tree_row_reference_copy (ref));
        gtk_tree_row_reference_free (ref);
        ptr = ptr->next;
    }
    
    g_list_foreach ( references, (GFunc) remove_row, model );
    g_list_foreach ( references, (GFunc) gtk_tree_row_reference_free, NULL );
    g_list_foreach ( rows, (GFunc) gtk_tree_path_free, NULL );
    g_list_free ( references );
    g_list_free ( rows );
}

unsigned int getRowCount(GtkTreeModel *model){
    unsigned int rowCount = 0;
    GtkTreeIter iter;

    gboolean reader = gtk_tree_model_get_iter_first(model, &iter);
    while ( reader )
    {	
	++rowCount;
        reader = gtk_tree_model_iter_next(model, &iter);
    }

    return rowCount;
}

int extractCompoundToList(char *compoundPointer, size_t compoundSize, GtkListStore *listStoreTLV){
    /* Extracts and process all innerTLVs of compound TLV */
    /* Returns number of compound's inner TLV which were finely processed, else returns -1 */
    unsigned long notProcessedBytes = compoundSize - HEADER_LENGTH;
    int TLVcount = 0;
    int padCount = 0;
    
    struct TLV * nTLV = malloc(sizeof(struct TLV));
    GtkTreeIter iter;
    while (notProcessedBytes) {
        nTLV = deserializeTLV(compoundPointer + (compoundSize-notProcessedBytes));
	if(nTLV->type == PAD1 || nTLV->type == PADN){
		++padCount;
		++TLVcount;
	} else {
   		gtk_list_store_append(GTK_LIST_STORE( listStoreTLV ), &iter);
    		gtk_list_store_set(GTK_LIST_STORE(listStoreTLV), &iter, 
               	NAME, getTLVTypeName(*nTLV),
	       	COUNT, ++TLVcount,
	       	SELECTED, 0,
		LENGTH, nTLV->length,
		-1);
	}
        notProcessedBytes -= TLVSize(nTLV);
        free(nTLV);
    }

    printf("Compound TLV was extracted: %i TLVs found, %i PAD TLVs\n", TLVcount, padCount);
    
    return TLVcount;
}

#pragma mark - Main TLV window event handlers

/* Button "Delete" click event handler for main TLV window */
void on_buttonDeletePost_clicked(GtkButton *deleteButton, gpointer data){
    struct MainWindowObjects* mwo = data;
    GtkTreeIter   iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection ( mwo->treeviewTLV );

    gboolean reader = gtk_tree_model_get_iter_first(GTK_TREE_MODEL( mwo->liststoreTLV ), &iter);
    while ( reader )
    {
        gboolean selected;
	selected = gtk_tree_selection_iter_is_selected(selection, &iter);
        if ( selected )
        {
            gchar* name;
            gint count;
            gtk_tree_model_get (GTK_TREE_MODEL( mwo->liststoreTLV ), &iter, 
                NAME, &name,
                COUNT, &count,
                -1);
	    guint TLVCount = getDazibaoTLVCount();
            g_print( "TLV #%i will be removed. Number of TLVs: %i\n", count, TLVCount);

	    removeTLVatIndex(count-1, TLVCount, mainTLVPointer, mainTLVSize);
        }
        reader = gtk_tree_model_iter_next(GTK_TREE_MODEL( mwo->liststoreTLV ), &iter);
    }
    removeAllRows(mwo->treeviewTLV);
    int tlvCount = extractCompoundToList(mainTLVPointer, mainTLVSize, mwo->liststoreTLV);
}

/* Button "New post" click event handler for main TLV window */
void on_buttonNewPost_clicked(GtkButton *newPostButton, gpointer data){
    struct MainWindowObjects* mwo = data;
    GtkBuilder *builder = getBuilder();
    GtkWindow *window; 
    struct AddWindowObjects *awo = malloc(sizeof(struct AddWindowObjects));
    awo->addWindow = GTK_WINDOW(gtk_builder_get_object(builder, "newTLVWindow"));
    awo->dateCheckButton = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "tlvDateCheckbox"));
    awo->tlvTextView = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "tlvTextview"));
    awo->tlvFileChooser = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "tlvFileChooser"));
    awo->treeviewTLV = mwo->treeviewTLV;
    awo->liststoreTLV = mwo->liststoreTLV;

    gtk_builder_connect_signals (builder, awo);
    gtk_window_set_title( GTK_WINDOW(awo->addWindow), "Create TLV");
    gtk_window_set_default_size (GTK_WINDOW (awo->addWindow), 220, 200);
    gtk_widget_show_all( GTK_WIDGET(awo->addWindow) );
}

/* Row activated event handler for main TLV window */
void on_treeviewMain_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data){
    GtkTreeIter   iter;
    struct MainWindowObjects *mwo = data;
    GtkTreeModel *model = gtk_tree_view_get_model( treeview );

    if ( gtk_tree_model_get_iter(model, &iter, path) )
    {
    	gint count;
        gtk_tree_model_get(model, &iter, 
               		   COUNT, &count,
               		   -1);

	char *tlvPointer = getPointerToTLVatIndex(count-1, mainTLVPointer, mainTLVSize);
	//currentTLVoffset = getOffsetToTLVatIndex(count-1, mainTLVPointer, mainTLVSize);
	struct TLV *myTLV = deserializeTLV(tlvPointer);
	currentTLVsize = TLVSize(myTLV);
	showTLVWindow(tlvPointer, TLVSize(myTLV), myTLV->type, NULL);
    }
}

/* Main window destruction event handler */

void on_mainWindow_destroy(GtkWidget *mainWindow, GdkEvent  *event, gpointer gdata){
	--windowCount;
	compressDazibao(mainTLVPointer, mainTLVSize);
        close(dazibaoFD);
	gtk_main_quit ();
	exit(EXIT_SUCCESS);
}

#pragma mark - Compound TLV window event handlers

/* Row activated event handler for compound TLV window */
void on_treeviewCompound_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data){
    GtkTreeModel *model;
    GtkTreeIter   iter;
    struct MainWindowObjects *mwo = data;

    model = gtk_tree_view_get_model( treeview );

    if ( gtk_tree_model_get_iter(model, &iter, path) )
    {
        	gint count;
        	gtk_tree_model_get(model, &iter, 
               			COUNT, &count,
               			-1);
		char *tlvPointer;
		tlvPointer = getPointerToTLVatIndex(count-1, mwo->compoundPointer, mwo->compoundSize);
		//currentTLVoffset = getOffsetToTLVatIndex(count-1, mwo->compoundPointer, mwo->compoundSize);

        	struct TLV *myTLV = deserializeTLV(tlvPointer);

		currentTLVsize = TLVSize(myTLV);
		showTLVWindow(tlvPointer, TLVSize(myTLV), myTLV->type, NULL);
    }
}

/* Compound window destruction event handler */
void on_compoundWindow_destroy(GtkWidget *compoundWindow, GdkEvent  *event, gpointer gdata){
	--windowCount;
	if(windowCount == 0){
 		gtk_main_quit ();
		exit(EXIT_SUCCESS);
	}
}

#pragma mark - New TLV window event handlers

/* Button "Save" of new TLV window */
void on_buttonAdd_clicked(GtkWidget *TopWindow, gpointer data){
	struct AddWindowObjects* awo = data;
	gboolean withText;
	gboolean withFile;
	gboolean compound;

	struct TLV *textTLV = NULL;
	struct TLV *imageTLV = NULL;
	struct TLV *compoundTLV = NULL;
	struct TLV *tlvToAdd = NULL;

	char *fileContent = NULL;
	size_t fileSize;
	TLVtype fileType;

	/* Extract text */
	GtkTextBuffer* textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(awo->tlvTextView));
    	GtkTextIter start;
    	GtkTextIter end;
	gtk_text_buffer_get_start_iter (textBuffer, &start);
	gtk_text_buffer_get_end_iter (textBuffer, &end); 
	gchar* tlvText = gtk_text_buffer_get_text(textBuffer, &start, &end,FALSE);
	g_print("Text: %s Length: %lu\n",tlvText, strlen(tlvText));
	if(strlen(tlvText)) 
		withText = TRUE;
	else
		withText = FALSE;

	/* Extract dated flag */	
	gboolean dated = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(awo->dateCheckButton));
	if(dated) 
		g_print("Dated\n");
	
	/* Extract file path */
	gchar* filePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(awo->tlvFileChooser));
	g_print("File path: %s\n", filePath);
	if(filePath){
		withFile = TRUE;
		fileType = getFileType(filePath);
		int fd;


    		if( (fd = open(filePath, O_RDONLY)) == -1){
			printf("This file: %s\n", DAZIBAO_PATH);
       	 		perror("File open error");
    		}
    
    		struct stat fileFS;
    		memset(&fileFS, 0, sizeof(fileFS));
    
   		if(fstat(fd, &fileFS) == -1){
        		perror("File fstat error");
    		}
   	 	fileSize = fileFS.st_size;
    		fileContent = malloc(fileSize);

    		if (read(fd, fileContent, fileSize) == -1){
        		perror("File read error");
    		}
	}
	else{
		withFile = FALSE;
	}
	compound = withText && withFile;
	
	/* TLV creation */
	if(withText) 
		textTLV = createTextTLV(tlvText);

	if(withFile){
		if(fileType == JPEG){
			imageTLV = createJPEGTLV(fileSize, fileContent);
		} else 
		if(fileType == PNG){
			imageTLV = createPNGTLV(fileSize, fileContent);
		} else 
		if(fileType == GIF){
			imageTLV = createGIFTLV(fileSize, fileContent);
		}
	}
	
	if(compound)
	{
		struct TLV *innerTLVs[2];
		innerTLVs[0] = textTLV;
		innerTLVs[1] = imageTLV;
		compoundTLV = createCompoundTLV(innerTLVs, 2);
		tlvToAdd = compoundTLV;
	} else 
	if(withText){
		tlvToAdd = textTLV;
	} else 
	if(withFile){
		tlvToAdd = imageTLV;
	}

	if(tlvToAdd){
		if(dated)
			tlvToAdd = createDatedTLV(time(0),tlvToAdd);
	
		processTLV(tlvToAdd);
		mainTLVPointer = addTLVtoDazibao(tlvToAdd, mainTLVPointer, mainTLVSize, 0);
		mainTLVSize = extendedDazibaoLength(mainTLVSize, tlvToAdd);

	    	removeAllRows(awo->treeviewTLV);
	    	int tlvCount = extractCompoundToList(mainTLVPointer, mainTLVSize, awo->liststoreTLV);

		gtk_widget_destroy(GTK_WIDGET(awo->addWindow));
	} else {
		showDialog("Error", "No data for TLV adding");
	}
}

/* Button "Cancel" of new TLV window */
void on_buttonCancel_clicked(GtkWidget *TopWindow, gpointer data){
	struct AddWindowObjects* awo = data;
	gtk_widget_destroy(GTK_WIDGET(awo->addWindow)); 
}

#pragma mark - Construct window

void showTLVWindow(char *tlvPointer, size_t tlvSize, TLVtype tlvType, gchar *title){
	GtkBuilder *builder = getBuilder();
	GtkWindow *window; 

	switch(tlvType){
	 case TEXT:{
    		window = GTK_WINDOW(gtk_builder_get_object(builder, "textTLVwindow"));
		GtkTextView *textView = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "tlvText"));
		GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);

		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), tlvPointer+TYPE_LENGTH+LENGTH_LENGTH, tlvSize - TYPE_LENGTH - LENGTH_LENGTH);
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textView), GTK_WRAP_WORD);
		gtk_text_view_set_buffer(GTK_TEXT_VIEW(textView), buffer);
		
		if(!title)
		 title = "Text TLV";

		gtk_window_set_title( GTK_WINDOW(window), title);
		gtk_window_set_default_size (GTK_WINDOW (window), 220, 200);
		gtk_widget_show_all( GTK_WIDGET(window) );
	 }
	 break;

         case PNG:
	 case GIF:
	 case JPEG:{
		window = GTK_WINDOW(gtk_builder_get_object(builder, "imageTLVwindow"));
		GtkImage *image = GTK_IMAGE(gtk_builder_get_object(builder, "tlvImage"));

		GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
		GError *error = NULL;

		if( !gdk_pixbuf_loader_write(loader, tlvPointer+TYPE_LENGTH+LENGTH_LENGTH, tlvSize - TYPE_LENGTH - LENGTH_LENGTH, &error) )
    		{
     			g_warning( "%s\n", error->message );
     			g_free( error );
     			exit(EXIT_FAILURE);
    		}
		if(tlvType == GIF){
			GdkPixbufAnimation *pixbufa = gdk_pixbuf_loader_get_animation(loader);
			if( !pixbufa )
    			{
     				g_warning( "%s\n", error->message );
     				g_free( error );
     				exit(EXIT_FAILURE);
    			}
			gtk_image_set_from_animation(image, pixbufa);
		} else {

			GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
			if( !pixbuf )
    			{
     				g_warning( "%s\n", error->message );
     				g_free( error );
     				exit(EXIT_FAILURE);
    			}	

			gtk_image_set_from_pixbuf(image, pixbuf);
		}
		
		if(!title)
		 title = "Image TLV";

		gtk_window_set_title(GTK_WINDOW(window), title);
		gtk_widget_show_all( GTK_WIDGET (window));
	 }
	 break;

         case DATED:{
    	    time_t date = deserializeDate(tlvPointer+TYPE_LENGTH+LENGTH_LENGTH);
            char * timeString = ctime(&date);
            char * innerTLV = tlvPointer + TYPE_LENGTH + LENGTH_LENGTH + DATE_LENGTH;
            struct TLV *datedTLV = deserializeTLV(innerTLV);
	    showTLVWindow(innerTLV, TLVSize(datedTLV), datedTLV->type, timeString);
	 } 
	 break;
	 
	 case MAIN:
	 case COMPOUND:{
	    struct MainWindowObjects *mwo = malloc(sizeof(struct MainWindowObjects));
	    ++windowCount;

	    if(tlvType == MAIN && !DRDONLY){
	    	mwo->topWindow = GTK_WINDOW(gtk_builder_get_object(builder, "mainWindow"));
	    	mwo->isMain = 1;
	    } else {
	    	mwo->topWindow = GTK_WINDOW(gtk_builder_get_object(builder, "compoundWindow"));
	    	mwo->isMain = 0;
	    }

    	    mwo->treeviewTLV = GTK_TREE_VIEW( gtk_builder_get_object( builder, "treeviewMain" ) );
            mwo->liststoreTLV = GTK_LIST_STORE( gtk_builder_get_object(builder, "liststoreTLV") );
	    mwo->compoundPointer = tlvPointer;
	    mwo->compoundSize = tlvSize;

    	    gtk_builder_connect_signals (builder, mwo);
	    int tlvCount = extractCompoundToList(tlvPointer, tlvSize, mwo->liststoreTLV);

	    gtk_window_set_title(GTK_WINDOW(mwo->topWindow), title);
    	    gtk_widget_show_all ( GTK_WIDGET (mwo->topWindow) );

	 }
	 break;
	}
	g_object_unref( G_OBJECT( builder ) );
}

#pragma mark - Welcome window signal handlers
void on_buttonOK_clicked(GtkButton *newPostButton, gpointer data){
	struct WelcomeWindowObjects* wwo = data;

	gboolean readOnly = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wwo->readonlyCheckbox));
	dazibaoPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wwo->fileChooser));

	if(!dazibaoPath){
		showDialog("Error", "Dazibao is not choosen");
	} else {
   		dazibaoFD = -1;
    		mainTLVPointer = NULL;
      		if(readOnly){
	  		DRDONLY = 1;
		} else {
	  		DRDONLY = 0;	
		}
    		int flag;
	 	if(DRDONLY)
			flag = O_RDONLY;
    	 	else 
			flag = O_RDWR;
    	 	dazibaoLoad(flag);
 		char *name = "Main window";
    		currentTLVoffset = 0;
    		currentTLVsize = mainTLVSize;
    		showTLVWindow(mainTLVPointer, mainTLVSize, MAIN, name);
        	gtk_widget_destroy(GTK_WIDGET(wwo->welcomeWindow));
	}
}

void showWelcomeWindow(){
	windowCount = 0;
	GtkBuilder *builder = getBuilder();
	struct WelcomeWindowObjects *wwo = malloc(sizeof(struct WelcomeWindowObjects));

	wwo->welcomeWindow = GTK_WINDOW(gtk_builder_get_object(builder, "welcomeWindow"));
        wwo->readonlyCheckbox = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "checkbuttonReadonly"));
        wwo->fileChooser = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "dazibaoChooser"));

    	gtk_builder_connect_signals (builder, wwo);

	gtk_window_set_title(GTK_WINDOW(wwo->welcomeWindow), "Dazibao");
    	gtk_widget_show_all ( GTK_WIDGET (wwo->welcomeWindow) );

	g_object_unref( G_OBJECT( builder ) );
}

void on_welcomeWindow_destroy(GtkWidget *welcomeWindow, GdkEvent  *event, gpointer gdata){
	if(!dazibaoPath){
		gtk_main_quit ();
		exit(EXIT_SUCCESS);
	}
}

void showDialog(char *header, char *message){
        GtkWidget *dialog;
        GtkWidget *label;
        GtkWidget *content_area;

        dialog = gtk_dialog_new_with_buttons(header,
                                             NULL,
                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_STOCK_OK,
                                             GTK_RESPONSE_ACCEPT,
                                             NULL);

        content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

        label = gtk_label_new(message);
        gtk_container_add(GTK_CONTAINER(content_area), label);
        gtk_widget_show(label);

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
}





