#include <stdio.h>
#include <string.h>
#include "../include/manage.h"
#include "../include/crud.h"
#include "../include/trade.h"

// 定义参数
char username[50];
int age;
char passwd[20];

int initial(char username[50])
{
    printf("请输入您的年龄：");
    scanf("%d", &age);
    printf("请输入您的密码：");
    scanf("%s", passwd);
    user_register(username, passwd, age);

    printf("欢迎来到unisystem，%s！\n", username);
    printf("这是一个基于C语言的游戏推荐和交易系统。\n");
    printf("请按照提示进行操作，享受您的unisystem之旅！\n");
    return 0;
}

void ranking()
{
    printf("排行榜功能正在开发中，敬请期待！\n");
    return;
}

int main()
{
    printf("请输入您的用户名：");
    scanf("%s", username);
    // user_login(username, passwd);
    if(crud_search_user(username) == -1) printf("欢迎回来，%s！\n", username);
    else initial(username);
    return 0;
}