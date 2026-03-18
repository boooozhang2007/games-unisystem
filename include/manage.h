#ifndef MANAGE_H
#define MANAGE_H

// 公开数据结构
typedef struct {
    int id;
    char username[50];
    int age;
    char passwd[20];
} User;



int  user_register(char* username, char* passwd, int age);
int  user_login(void);
void user_list_all(void);
int  user_upgrade(void);
int  user_degrade(void);
int  user_edit_info(void);

int  friend_add(void);
int  friend_remove(void);
void friend_list(void);

#endif // MANAGE_H