//
//  NotificationHandler.h
//  Dazibao
//
//  Created by Владислав Фиц on 15.12.13.
//  Copyright (c) 2013 Vladislav Fitc. All rights reserved.
//

#ifndef Dazibao_NotificationHandler_h
#define Dazibao_NotificationHandler_h

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <libnotify/notify.h>

int initNotifications();
void postNotification(char *header, char *message, int timeout);


#endif
