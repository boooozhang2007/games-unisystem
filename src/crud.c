#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

// 解决id问题，每次调用id函数时会返回一个递增的整数
int id(void)
{
    FILE* fp = fopen("data/id", "r+");
    if(fp == NULL) return -1; // 文件打开失败
    int current_id;
    fscanf(fp, "%d", &current_id);
    fwrite(&current_id, sizeof(int), 1, fp);
    current_id++; // 递增id
    fclose(fp);
    return current_id; // 返回当前id
}

// 初始化crud系统，若不存在数据文件则创建

void crud_init()
{
    FILE *fp;

    // 检查数据文件是否存在
    fp = fopen("data/users.jsonl", "a");
    if(fp != NULL) fclose(fp);
    else{
        // 如果文件不存在则创建，后续同理
        fp = fopen("data/users.jsonl", "w");
        if (fp) fclose(fp);
    }

    fp = fopen("data/market.jsonl", "a");
    if(fp != NULL) fclose(fp);
    else{
        fp = fopen("data/market.jsonl", "w");
        if (fp) fclose(fp);
    }

    fp = fopen("data/games.jsonl", "a");
    if(fp != NULL) fclose(fp);
    else{
        fp = fopen("data/games.jsonl", "w");
        if(fp) fclose(fp);
    }

    fp = fopen("data/id", "a");
    if(fp != NULL){
        fprintf(fp, "%d", 0); // 初始化id为0
        fclose(fp);
    }
    else{
        fp = fopen("data/id", "w");
        if(fp) fclose(fp);
    }
    printf("CRUD初始化完成！\n");
}

// -----------------------------
// 新增行函数

int crud_create_line(int option, char* json_line)
{
    char* path;
    switch (option)//1用户，2商店，3游戏
    {
    case 1:
        // 用户文件
        path = "data/users.jsonl";
        break;
    
    case 2:
        // 商店文件
        path = "data/market.jsonl";
        break;
    
    case 3:
        // 游戏文件
        path = "data/games.jsonl";
        break;

    default:
        return -2;
    }
    
    FILE* fp = fopen(path, "a");
    if (fp == NULL) return -3;

    fprintf(fp, "%s\n", json_line);
    fclose(fp);

    return 0;
}

// -----------------------------
// 查询用户函数
// 返回值为用户数据结构体指针，若用户不存在则返回NULL

users* crud_search_user(char* username)
{
    static char buffer[1024];
    static users u;
    FILE *backup = fopen("data/users.jsonl", "r");
    if(backup == NULL) return NULL; // 文件打开失败

    char searched_username[256];
    snprintf(searched_username, sizeof(searched_username), "\"username\": \"%s\"", username);

    while(fgets(buffer, sizeof(buffer), backup) != NULL)
    {
        char *found = strstr(buffer, searched_username);
        if (found != NULL)
        {
            users tmp = {0};
            sscanf(buffer, "{\"id\": %d, \"username\": \"%49[^\"]\", \"passwd\": \"%49[^\"]\", \"age\": %d, \"role\": %d, \"coins\": %d", &tmp.id, tmp.username, tmp.passwd, &tmp.age, &tmp.role, &tmp.coins);

            u = tmp; // 
            fclose(backup);
            return &u; // 用户名已存在
        }
    }

    fclose(backup);
    return NULL; // 用户名不存在
}

// -----------------------------
// 新增用户函数
// 传入注册用户信息，默认role为0

int crud_create_user(users* u)
{
    if(u == NULL) return -1;

    char jsonline[2048];
    
    snprintf(jsonline,sizeof(jsonline),"{\"id\": %d, \"username\": \"%s\", \"passwd\": \"%s\", \"age\": %d, \"role\": %d, \"coins\": %d, \"assets\": \"\", \"friends\": \"\"}",u->id,u->username,u->passwd,u->age,u->role,u->coins);

    return crud_create_line(1, jsonline);
}

// -----------------------------
// 编辑用户信息函数
// username:用户名
// item:要编辑的项

int crud_edit_user(char* username, char* item)
{
    users* u = crud_search_user(username);
    if(u == NULL) return -1;

    return 0;
}