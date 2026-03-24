#ifndef CRUD_H
#define CRUD_H

#include "models.h"

int id(void);
void crud_init(void);
int crud_create_line(int option,char* json_line);
users* crud_search_user(char* username);
int crud_create_user(users* u);
int crud_edit_user(char* username, char* item);

#endif // CRUD_H
