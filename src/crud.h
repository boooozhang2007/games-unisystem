#ifndef CRUD_H
#define CRUD_H

#include "models.h"

int id(void);
void crud_init(void);
int crud_create_line(int option, const char* json_line);
users* crud_search_user(const char* username);
int crud_create_user(const users* u);
int crud_edit_user(const char* username, const char* item);

int crud_parse_user_line(const char* line, users* out_user);
int crud_user_to_json(const users* u, char* out_line, int out_size);
int crud_load_users(users* arr, int max_count);
int crud_save_users(const users* arr, int count);
int crud_update_user(const users* u);
int crud_delete_user(const char* username);

int crud_load_games(games* arr, int max_count);
int crud_load_market(market* arr, int max_count);
int crud_save_market(const market* arr, int count);
int crud_append_market_item(const market* item);

#endif // CRUD_H
