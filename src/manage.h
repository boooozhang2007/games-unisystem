#ifndef MANAGE_H
#define MANAGE_H

int user_register(char* username, char* passwd, int age);
int user_login(char* username, char* passwd);
void user_list_all(void);
int user_upgrade(void);
int user_degrade(void);
int user_edit_info(void);
int friend_add(void);
int friend_remove(void);
void friend_list(void);

#endif // MANAGE_H
