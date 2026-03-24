#include <stdio.h>
#include <string.h>
#include "manage.h"
#include "crud.h"
#include "trade.h"


int menu(int role)
{
    char* title = "普通用户菜单";

    if (role == 1) {
        title = "VIP用户菜单";
    } else if (role == 2) {
        title = "管理员菜单";
    }

    printf("\n+--------------------------------------+\n");
    printf("|            %s              |\n", title);
    printf("+--------------------------------------+\n");

    if(role==0){
        printf("| 1. 游戏排行        2. 推荐游戏      |\n");
        printf("| 3. 交易道具        4. 我的好友      |\n");
        printf("| 5. 个人信息        6. 退出系统      |\n");
    }
    else if(role==1){
        printf("| 1. 游戏排行        2. 推荐游戏      |\n");
        printf("| 3. 交易道具        4. 我的好友      |\n");
        printf("| 5. 个人信息        6. 退出系统      |\n");
    }
    else if(role==2){
        printf("| 1. 游戏排行        2. 推荐游戏      |\n");
        printf("| 3. 交易道具        4. 我的好友      |\n");
        printf("| 5. 个人信息        6. 用户管理      |\n");
        printf("| 7. 退出系统                        |\n");
    }
    printf("+--------------------------------------+\n");
    printf("请输入您的选择: ");

    int choice;
    scanf("%d", &choice);
    return choice;
}

int initial(char username[50])
{
    int age;
    char passwd[50];

    printf("请输入您的年龄：");
    scanf("%d", &age);

    while(age<=0){
        printf("年龄有误，请重新输入：");
        scanf("%d", &age);  
    }

    printf("请输入您的密码：");
    scanf("%s", passwd);

    crud_init();
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
    char username[50];
    char passwd[50];

    printf("欢迎来到unisystem，请输入您的用户名：");
    scanf("%s", username);

    if(crud_search_user(username) != NULL){
        printf("请输入您的密码：");
        scanf("%s", passwd);
        while(user_login(username, passwd)!=0){
            printf("\n请重新输入您的密码：");
            scanf("%s", passwd);
        }
    }
    else initial(username);

    while(1)
    {
        int role = crud_search_user(username)->role;
        switch (menu(role))
        {
        case 1:
            ranking();
            break;
        case 2:
            printf("推荐游戏功能正在开发中，敬请期待！\n");
            break;
        case 3:
            printf("交易道具功能正在开发中，敬请期待！\n");
            break;
        case 4:
            printf("好友管理功能正在开发中，敬请期待！\n");
            break;
        case 5:
            printf("个人信息功能正在开发中，敬请期待！\n");
            break;
        case 6:
            if(role==2){
                printf("用户管理功能正在开发中，敬请期待！\n");
                break;
            }
            printf("感谢使用unisystem，期待您的下次光临！\n");
            return 0;
        case 7:
            printf("感谢使用unisystem，期待您的下次光临！\n");
            return 0;
        
        default:
            printf("无效的选择，请重新输入！\n");
            break;
        }
    }
    return 0;
}