#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/crud.h"

/* 初始化crud系统，若不存在数据文件则创建 */
void crud_init()
{
    FILE *fp;

    // 检查数据文件是否存在
    fp = fopen("data/users.json", "a");
    if (fp != NULL) fclose(fp);
    else
    {
        // 如果文件不存在则创建
        fp = fopen("data/users.json", "w");
        if (fp) fclose(fp);
    }

    fp = fopen("data/market.json", "a");
    if (fp != NULL) fclose(fp);
    else
    {
        // 如果文件不存在则创建
        fp = fopen("data/market.json", "w");
        if (fp) fclose(fp);
    }

    fp = fopen("data/games.json", "a");
    if (fp != NULL) fclose(fp);
    else
    {
        // 如果文件不存在则创建
        fp = fopen("data/games.json", "w");
        if (fp) fclose(fp);
    }
    printf("CRUD初始化完成！\n");
}

/* crud注册用户函数 */
int crud_search_user(char* username)
{
    char buffer[1024];
    FILE *backup = fopen("data/users.json", "r");
    if(backup == NULL) return -2; // 文件打开失败

    char searched_username[256];
    snprintf(searched_username, sizeof(searched_username), "\"username\": \"%s\"", username);

    while(fgets(buffer, sizeof(buffer), backup) != NULL)
    {
        if (strstr(buffer, searched_username) != NULL)
        {
            fclose(backup);
            return -1; // 用户名已存在
        }
    }

    fclose(backup);
    return 0; // 用户名不存在
}
