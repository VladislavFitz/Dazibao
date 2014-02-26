gcc main.c Structures.c GUI.c Conversions.c DazibaoProcessing.c TLVBuilder.c TLVProcessing.c -o Dazibao -export-dynamic  `pkg-config --cflags --libs gtk+-3.0`
gcc notifier.c NotificationHandler.c -Wall -o Notifier `pkg-config --cflags --libs libnotify`
gcc -Wall -o Listener listener.c
