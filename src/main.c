#include <stdio.h>
#include <string.h>
#include "manage.h"
#include "crud.h"
#include "trade.h"

static void recommend_games(const char* username)
{
    games game_list[MAX_GAMES];
    users* current_user;
    int count;
    int i;
    int shown = 0;

    if (username == NULL) {
        return;
    }

    current_user = crud_search_user(username);
    if (current_user == NULL) {
        printf("用户不存在。\n");
        return;
    }

    count = crud_load_games(game_list, MAX_GAMES);
    if (count <= 0) {
        printf("当前没有可推荐的游戏数据。\n");
        return;
    }

    printf("\n推荐游戏（按年龄做简单推荐）：\n");
    for (i = 0; i < count; i++) {
        int matched = 0;
        if (current_user->age < 18 && (game_list[i].game_id % 2 == 0)) {
            matched = 1;
        }
        if (current_user->age >= 18 && (game_list[i].game_id % 2 == 1)) {
            matched = 1;
        }

        if (matched) {
            printf("- %s：%s\n", game_list[i].game_name, game_list[i].game_desc);
            shown++;
        }
    }

    if (shown == 0) {
        for (i = 0; i < count && i < 3; i++) {
            printf("- %s：%s\n", game_list[i].game_name, game_list[i].game_desc);
        }
    }
}

static void friend_menu(const char* username)
{
    int choice;

    while (1) {
        printf("\n好友菜单\n");
        printf("1. 查看好友\n");
        printf("2. 添加好友\n");
        printf("3. 删除好友\n");
        printf("0. 返回\n");
        printf("请输入选择：");
        scanf("%d", &choice);

        if (choice == 1) {
            friend_list(username);
        } else if (choice == 2) {
            friend_add(username);
        } else if (choice == 3) {
            friend_remove(username);
        } else if (choice == 0) {
            return;
        } else {
            printf("无效选择，请重新输入！\n");
        }
    }
}

static void user_info_menu(const char* username)
{
    int choice;

    while (1) {
        printf("\n个人信息菜单\n");
        printf("1. 查看个人信息\n");
        printf("2. 修改个人信息\n");
        printf("0. 返回\n");
        printf("请输入选择：");
        scanf("%d", &choice);

        if (choice == 1) {
            user_show_info(username);
        } else if (choice == 2) {
            user_edit_info(username);
        } else if (choice == 0) {
            return;
        } else {
            printf("无效选择，请重新输入！\n");
        }
    }
}

static void admin_menu(void)
{
    int choice;

    while (1) {
        printf("\n管理员菜单\n");
        printf("1. 查看所有用户\n");
        printf("2. 升级用户\n");
        printf("3. 降级用户\n");
        printf("4. 删除用户\n");
        printf("0. 返回\n");
        printf("请输入选择：");
        scanf("%d", &choice);

        if (choice == 1) {
            user_list_all();
        } else if (choice == 2) {
            user_upgrade();
        } else if (choice == 3) {
            user_degrade();
        } else if (choice == 4) {
            user_delete();
        } else if (choice == 0) {
            return;
        } else {
            printf("无效选择，请重新输入！\n");
        }
    }
}


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
    scanf("%49s", passwd);

    crud_init();
    user_register(username, passwd, age);

    printf("这是一个基于C语言的游戏推荐和交易系统。\n");
    printf("请按照提示进行操作，享受您的unisystem之旅！\n");

    return 0;
}

void ranking()
{
    users list[MAX_USERS];
    int count;
    int i;

    count = crud_load_users(list, MAX_USERS);
    if (count <= 0) {
        printf("暂无用户数据，无法显示排行。\n");
        return;
    }

    for (i = 0; i < count - 1; i++) {
        int j;
        for (j = 0; j < count - 1 - i; j++) {
            if (list[j].coins < list[j + 1].coins) {
                users temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }

    printf("\n金币排行榜：\n");
    printf("名次\t用户名\t金币\n");
    for (i = 0; i < count && i < 10; i++) {
        printf("%d\t%s\t%d\n", i + 1, list[i].username, list[i].coins);
    }
    return;
}

int main()
{
    char username[50];
    char passwd[50];

    crud_init();

    printf("欢迎来到unisystem，请输入您的用户名：");
    scanf("%49s", username);

    if(crud_search_user(username) != NULL){
        printf("请输入您的密码：");
        scanf("%49s", passwd);
        while(user_login(username, passwd)!=0){
            printf("\n请重新输入您的密码：");
            scanf("%49s", passwd);
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
            recommend_games(username);
            break;
        case 3:
            trade_menu(username);
            break;
        case 4:
            friend_menu(username);
            break;
        case 5:
            user_info_menu(username);
            break;
        case 6:
            if(role==2){
                admin_menu();
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