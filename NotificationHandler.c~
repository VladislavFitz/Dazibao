//
//  NotificationHandler.c
//  Dazibao
//
//  Created by Владислав Фиц on 15.12.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#include <stdio.h>
#include "NotificationHandler.h"

int initNotifications(){
    return notify_init("Dazibao");
}

void postNotification(char *header, char *message, int timeout){
    NotifyNotification *n;  

    n = notify_notification_new (header, 
                                 message,
                                  NULL);
    notify_notification_set_timeout (n, timeout);

    if (!notify_notification_show (n, NULL)) 
    {
	fprintf(stderr, "failed to send notification\n");
    }

    g_object_unref(G_OBJECT(n));
    //GError *error = NULL;
}


