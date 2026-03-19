#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/crud.h"

/* 初始化crud系统，若不存在数据文件则创建 */
void crud_init()
{
    FILE *fp;

    // 检查数据文件是否存在
    fp = fopen("data/users.jsonl", "a");
    if (fp != NULL) fclose(fp);
    else
    {
        // 如果文件不存在则创建
        fp = fopen("data/users.jsonl", "w");
        if (fp) fclose(fp);
    }

    fp = fopen("data/market.jsonl", "a");
    if (fp != NULL) fclose(fp);
    else
    {
        // 如果文件不存在则创建
        fp = fopen("data/market.jsonl", "w");
        if (fp) fclose(fp);
    }

    fp = fopen("data/games.jsonl", "a");
    if (fp != NULL) fclose(fp);
    else
    {
        // 如果文件不存在则创建
        fp = fopen("data/games.jsonl", "w");
        if (fp) fclose(fp);
    }
    printf("CRUD初始化完成！\n");
}

/* crud搜索用户函数 */
/* 返回值为用户数据行地址指针 */

char* crud_search_user(char* username)
{
    static char buffer[1024];
    FILE *backup = fopen("data/users.jsonl", "r");
    if(backup == NULL) return NULL; // 文件打开失败

    char searched_username[256];
    snprintf(searched_username, sizeof(searched_username), "{\"username\": \"%s\"", username);

    while(fgets(buffer, sizeof(buffer), backup) != NULL)
    {
        char *found = strstr(buffer, searched_username);
        if (found != NULL)
        {
            fclose(backup);
            return found; // 用户名已存在
        }
    }

    fclose(backup);
    return NULL; // 用户名不存在
}
