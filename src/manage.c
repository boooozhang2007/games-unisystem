#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manage.h"
#include "crud.h"
#include "models.h"

static int manage_find_user_index_by_name(const users* arr, int count, const char* username)
{
    int i;
    for (i = 0; i < count; i++) {
        if (strcmp(arr[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

static int manage_find_user_index_by_id(const users* arr, int count, int id_value)
{
    int i;
    for (i = 0; i < count; i++) {
        if (arr[i].id == id_value) {
            return i;
        }
    }
    return -1;
}

static int manage_has_friend(const users* u, int friend_id)
{
    int i;
    for (i = 0; i < u->friend_count; i++) {
        if (u->friends[i] == friend_id) {
            return 1;
        }
    }
    return 0;
}

static const char* manage_role_text(int role)
{
    if (role == 2) {
        return "管理员";
    }
    if (role == 1) {
        return "VIP";
    }
    return "普通用户";
}


// 用户注册函数，需提供基本信息
int user_register(const char* username, const char* passwd, int age)
{
    users u;

    if (username == NULL || passwd == NULL || age <= 0) {
        printf("注册信息无效。\n");
        return -1;
    }

    if (crud_search_user(username) != NULL) {
        printf("用户名已存在，请选择其他用户名。\n");
        return -2;
    }

    memset(&u, 0, sizeof(u));
    u.id = id();
    if (u.id < 0) {
        printf("系统初始化失败，注册失败。\n");
        return -3;
    }

    strncpy(u.username, username, sizeof(u.username) - 1);
    strncpy(u.passwd, passwd, sizeof(u.passwd) - 1);
    u.age = age;
    u.role = 0;
    u.coins = 100;
    u.friend_count = 0;
    u.asset_count = 0;

    if (crud_create_user(&u) == 0) {
        printf("注册成功！欢迎加入unisystem，%s！\n", username);
        printf("系统赠送 100 金币，祝你玩得开心！\n");
        return 0;
    }

    printf("注册失败，请稍后重试。\n");
    return -4;
}

// 用户登录函数，提供用户名和密码
int user_login(const char* username, const char* passwd)
{
    users* found = crud_search_user(username);

    if(found == NULL) {
        return -1;
    }

    if(strcmp(found->passwd, passwd) != 0) {
        printf("密码错误。\n");
        return 1;
    }

    printf("登录成功！欢迎回来，%s！\n", username);
    return 0;
}


void user_list_all(void)
{
    users list[MAX_USERS];
    int count;
    int i;

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("当前没有用户数据。\n");
        return;
    }

    printf("\n用户列表：\n");
    printf("ID\t用户名\t年龄\t角色\t金币\n");
    for (i = 0; i < count; i++) {
        printf("%d\t%s\t%d\t%s\t%d\n",
               list[i].id,
               list[i].username,
               list[i].age,
               manage_role_text(list[i].role),
               list[i].coins);
    }
}


int user_upgrade(void)
{
    char username[50];
    users list[MAX_USERS];
    int count;
    int index;

    printf("请输入要升级的用户名：");
    scanf("%49s", username);

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("用户数据为空。\n");
        return -1;
    }

    index = manage_find_user_index_by_name(list, count, username);
    if (index < 0) {
        printf("用户不存在。\n");
        return -2;
    }

    if (list[index].role >= 2) {
        printf("该用户已经是最高等级。\n");
        return -3;
    }

    list[index].role++;
    if (crud_save_users(list, count) == 0) {
        printf("升级成功，%s 当前角色：%s\n", username, manage_role_text(list[index].role));
        return 0;
    }

    printf("升级失败。\n");
    return -4;
}


int user_degrade(void)
{
    char username[50];
    users list[MAX_USERS];
    int count;
    int index;

    printf("请输入要降级的用户名：");
    scanf("%49s", username);

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("用户数据为空。\n");
        return -1;
    }

    index = manage_find_user_index_by_name(list, count, username);
    if (index < 0) {
        printf("用户不存在。\n");
        return -2;
    }

    if (list[index].role <= 0) {
        printf("该用户已经是最低等级。\n");
        return -3;
    }

    list[index].role--;
    if (crud_save_users(list, count) == 0) {
        printf("降级成功，%s 当前角色：%s\n", username, manage_role_text(list[index].role));
        return 0;
    }

    printf("降级失败。\n");
    return -4;
}


int user_delete(void)
{
    char username[50];
    users list[MAX_USERS];
    int count;
    int index;
    int target_id;
    int i;

    printf("请输入要删除的用户名：");
    scanf("%49s", username);

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("用户数据为空。\n");
        return -1;
    }

    index = manage_find_user_index_by_name(list, count, username);
    if (index < 0) {
        printf("用户不存在。\n");
        return -2;
    }

    target_id = list[index].id;

    for (i = index; i < count - 1; i++) {
        list[i] = list[i + 1];
    }
    count--;

    for (i = 0; i < count; i++) {
        int j;
        for (j = 0; j < list[i].friend_count; j++) {
            if (list[i].friends[j] == target_id) {
                int k;
                for (k = j; k < list[i].friend_count - 1; k++) {
                    list[i].friends[k] = list[i].friends[k + 1];
                }
                list[i].friend_count--;
                j--;
            }
        }
    }

    if (crud_save_users(list, count) == 0) {
        printf("用户 %s 删除成功。\n", username);
        return 0;
    }

    printf("删除失败。\n");
    return -3;
}


void user_show_info(const char* username)
{
    users* u;

    if (username == NULL) {
        return;
    }

    u = crud_search_user(username);
    if (u == NULL) {
        printf("用户不存在。\n");
        return;
    }

    printf("\n个人信息：\n");
    printf("用户名：%s\n", u->username);
    printf("年龄：%d\n", u->age);
    printf("角色：%s\n", manage_role_text(u->role));
    printf("金币：%d\n", u->coins);
    printf("好友数：%d\n", u->friend_count);
    printf("资产数：%d\n", u->asset_count);
}


int user_edit_info(const char* username)
{
    users list[MAX_USERS];
    int count;
    int index;
    int choice;

    if (username == NULL) {
        return -1;
    }

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("用户数据为空。\n");
        return -2;
    }

    index = manage_find_user_index_by_name(list, count, username);
    if (index < 0) {
        printf("当前用户不存在。\n");
        return -3;
    }

    printf("\n个人信息修改\n");
    printf("1. 修改密码\n");
    printf("2. 修改年龄\n");
    printf("3. 充值金币\n");
    printf("0. 返回\n");
    printf("请输入选择：");
    scanf("%d", &choice);

    if (choice == 1) {
        char passwd[50];
        printf("请输入新密码：");
        scanf("%49s", passwd);
        strncpy(list[index].passwd, passwd, sizeof(list[index].passwd) - 1);
        list[index].passwd[sizeof(list[index].passwd) - 1] = '\0';
    } else if (choice == 2) {
        int age;
        printf("请输入新年龄：");
        scanf("%d", &age);
        if (age <= 0) {
            printf("年龄不合法。\n");
            return -4;
        }
        list[index].age = age;
    } else if (choice == 3) {
        int add_coins;
        printf("请输入充值金币数量：");
        scanf("%d", &add_coins);
        if (add_coins <= 0) {
            printf("充值数量不合法。\n");
            return -5;
        }
        list[index].coins += add_coins;
    } else if (choice == 0) {
        return 0;
    } else {
        printf("无效选择。\n");
        return -6;
    }

    if (crud_save_users(list, count) == 0) {
        printf("信息修改成功。\n");
        return 0;
    }

    printf("信息修改失败。\n");
    return -7;
}


int friend_add(const char* username)
{
    char friend_name[50];
    users list[MAX_USERS];
    int count;
    int self_index;
    int friend_index;

    if (username == NULL) {
        return -1;
    }

    printf("请输入要添加的好友用户名：");
    scanf("%49s", friend_name);

    if (strcmp(username, friend_name) == 0) {
        printf("不能添加自己为好友。\n");
        return -2;
    }

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("用户数据为空。\n");
        return -3;
    }

    self_index = manage_find_user_index_by_name(list, count, username);
    friend_index = manage_find_user_index_by_name(list, count, friend_name);

    if (self_index < 0 || friend_index < 0) {
        printf("好友不存在。\n");
        return -4;
    }

    if (manage_has_friend(&list[self_index], list[friend_index].id)) {
        printf("你们已经是好友了。\n");
        return -5;
    }

    if (list[self_index].friend_count >= MAX_FRIENDS || list[friend_index].friend_count >= MAX_FRIENDS) {
        printf("好友数量已达上限。\n");
        return -6;
    }

    list[self_index].friends[list[self_index].friend_count++] = list[friend_index].id;
    list[friend_index].friends[list[friend_index].friend_count++] = list[self_index].id;

    if (crud_save_users(list, count) == 0) {
        printf("添加好友成功：%s\n", friend_name);
        return 0;
    }

    printf("添加好友失败。\n");
    return -7;
}


int friend_remove(const char* username)
{
    char friend_name[50];
    users list[MAX_USERS];
    int count;
    int self_index;
    int friend_index;
    int i;

    if (username == NULL) {
        return -1;
    }

    printf("请输入要删除的好友用户名：");
    scanf("%49s", friend_name);

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("用户数据为空。\n");
        return -2;
    }

    self_index = manage_find_user_index_by_name(list, count, username);
    friend_index = manage_find_user_index_by_name(list, count, friend_name);

    if (self_index < 0 || friend_index < 0) {
        printf("好友不存在。\n");
        return -3;
    }

    if (!manage_has_friend(&list[self_index], list[friend_index].id)) {
        printf("你们当前不是好友关系。\n");
        return -4;
    }

    for (i = 0; i < list[self_index].friend_count; i++) {
        if (list[self_index].friends[i] == list[friend_index].id) {
            int j;
            for (j = i; j < list[self_index].friend_count - 1; j++) {
                list[self_index].friends[j] = list[self_index].friends[j + 1];
            }
            list[self_index].friend_count--;
            break;
        }
    }

    for (i = 0; i < list[friend_index].friend_count; i++) {
        if (list[friend_index].friends[i] == list[self_index].id) {
            int j;
            for (j = i; j < list[friend_index].friend_count - 1; j++) {
                list[friend_index].friends[j] = list[friend_index].friends[j + 1];
            }
            list[friend_index].friend_count--;
            break;
        }
    }

    if (crud_save_users(list, count) == 0) {
        printf("删除好友成功：%s\n", friend_name);
        return 0;
    }

    printf("删除好友失败。\n");
    return -5;
}


void friend_list(const char* username)
{
    users list[MAX_USERS];
    int count;
    int self_index;
    int i;

    if (username == NULL) {
        return;
    }

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("用户数据为空。\n");
        return;
    }

    self_index = manage_find_user_index_by_name(list, count, username);
    if (self_index < 0) {
        printf("当前用户不存在。\n");
        return;
    }

    if (list[self_index].friend_count == 0) {
        printf("你还没有好友。\n");
        return;
    }

    printf("\n我的好友：\n");
    for (i = 0; i < list[self_index].friend_count; i++) {
        int index = manage_find_user_index_by_id(list, count, list[self_index].friends[i]);
        if (index >= 0) {
            printf("- %s\n", list[index].username);
        }
    }
}