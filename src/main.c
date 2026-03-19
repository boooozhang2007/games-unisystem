#include <stdio.h>
#include <string.h>
#include "../include/manage.h"
#include "../include/crud.h"
#include "../include/trade.h"

// 定义参数
char username[50];
int age;
char passwd[50];

int menu()
{
    printf("请选择操作：\n1. 游戏排行榜\n2. 推荐游戏\n3. 交易道具\n4. 我的好友\n5. 个人信息\n6. 退出系统\n");
    int choice;
    scanf("%d", &choice);
    return choice;
}

int initial(char username[50])
{
    printf("请输入您的年龄：");
    scanf("%d", &age);
    printf("请输入您的密码：");
    scanf("%s", passwd);
    user_register(username, passwd, age);
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
    int choice;
    printf("欢迎来到unisystem，请输入您的用户名：");
    scanf("%s", username);
    if(crud_search_user(username) != NULL){
        printf("请输入您的密码：");
        scanf("%s", passwd);
        user_login(username, passwd);
    }
    else initial(username);

    while(1)
    {
        // TODO:获取用户角色数据，根据不同角色展示不同的菜单
        menu();
    }
    return 0;
}