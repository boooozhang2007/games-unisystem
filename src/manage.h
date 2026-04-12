#ifndef MANAGE_H
#define MANAGE_H

int user_register(const char* username, const char* passwd, int age);
int user_login(const char* username, const char* passwd);
void user_list_all(void);
int user_upgrade(void);
int user_degrade(void);
int user_delete(void);
void user_show_info(const char* username);
int user_edit_info(const char* username);
int friend_add(const char* username);
int friend_remove(const char* username);
void friend_list(const char* username);

#endif // MANAGE_H
