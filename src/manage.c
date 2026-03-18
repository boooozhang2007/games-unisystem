#include <stdio.h>
#include "../include/manage.h"
#include "../include/crud.h"

int user_register(char* username, char* passwd, int age)
{
        if (crud_search_user(username) == -1)
        {
            printf("用户名已存在，请选择其他用户名。\n");
            return -1; // 用户名已存在，注册失败
        }
        else
        {
            // 将新用户信息写入文件
            FILE *fp = fopen("data/users.json", "a");
            if (fp == NULL) return -1; // 文件打开失败

            fprintf(fp, "{\"username\": \"%s\", \"passwd\": \"%s\", \"age\": %d}\n", username, passwd, age);
            fclose(fp);
            printf("注册成功！欢迎加入unisystem，%s！\n", username);
            return 0; // 注册成功
        }
}

int user_login(void)
{
    // 这里应该添加用户登录的逻辑，例如验证用户名和密码
    printf("登录功能正在开发中，敬请期待！\n");
    return 0; // 返回0表示登录成功，非0表示失败
}

void user_list_all(void)
{
    // 这里应该添加列出所有用户的逻辑，例如从文件或数据库中读取用户信息并打印
    printf("列出所有用户功能正在开发中，敬请期待！\n");
}

int user_upgrade(void)
{
    // 这里应该添加用户升级的逻辑，例如提升用户权限或等级
    printf("用户升级功能正在开发中，敬请期待！\n");
    return 0; // 返回0表示升级成功，非0表示失败
}

int user_degrade(void)
{
    // 这里应该添加用户降级的逻辑，例如降低用户权限或等级
    printf("用户降级功能正在开发中，敬请期待！\n");
    return 0; // 返回0表示降级成功，非0表示失败
}

int user_edit_info(void)
{
    // 这里应该添加编辑用户信息的逻辑，例如更新用户的年龄或密码
    printf("编辑用户信息功能正在开发中，敬请期待！\n");
    return 0; // 返回0表示编辑成功，非0表示失败
}

int friend_add(void)
{
    // 这里应该添加添加好友的逻辑，例如将好友关系保存到文件或数据库中
    printf("添加好友功能正在开发中，敬请期待！\n");
    return 0; // 返回0表示添加成功，非0表示失败
}

int friend_remove(void)
{
    // 这里应该添加删除好友的逻辑，例如从文件或数据库中删除好友关系
    printf("删除好友功能正在开发中，敬请期待！\n");
    return 0; // 返回0表示删除成功，非0表示失败
}

void friend_list(void)
{
    // 这里应该添加列出好友的逻辑，例如从文件或数据库中读取好友信息并打印
    printf("列出好友功能正在开发中，敬请期待！\n");
}