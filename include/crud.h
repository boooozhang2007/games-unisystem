#ifndef CRUD_H
#define CRUD_H

void crud_init();
void crud_add_game();
void crud_list_all_games();
char* crud_search_user(char* username);
void crud_login_user();
void crud_list_all_users();
void crud_upgrade_user();
void crud_degrade_user();
void crud_edit_user_info();
void crud_add_friend();
void crud_remove_friend();
void crud_list_friends();

#endif // CRUD_H