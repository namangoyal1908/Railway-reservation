#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "structs.h"

void encryptText(char *str);
void decryptText(char *str);
void encryptPhone(char *phone);
void sign_up();
void log_in();

// This holds the currently logged in user so other files can use it
extern struct User currentUser;

#endif
