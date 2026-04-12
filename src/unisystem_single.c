#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//数量上限
#define MAX_USERS 500
#define MAX_GAMES 100
#define MAX_MARKET 200
#define MAX_FRIENDS 100
#define MAX_ASSETS 100

//数据文件
#define USER_FILE "users.dat"
#define GAME_FILE "games.dat"
#define MARKET_FILE "market.dat"
#define TRADE_LOG_FILE "trade_log.dat"
#define INVITE_LOG_FILE "invite_log.dat"
#define ID_FILE "id.dat"
#define SEED_FILE "seed.dat"

//用户资料
typedef struct {
    int id;
    char username[50];
    char passwd[50];
    int age;
    char identity[30];
    char hobby[30];
    int role;//0普通用户 1VIP 2管理员
    int coins;
    int game_hours;
    int play_count;
    int win_count;
    int friend_count;
    int asset_count;
    int friends[MAX_FRIENDS];
    int assets[MAX_ASSETS];
} users;

//游戏资料
typedef struct {
    int game_id;
    char game_name[50];
    char game_type[30];
    char game_desc[100];
    int need_age;
} games;

//市场资料
typedef struct {
    int item_id;
    char item_name[50];
    char item_type[30];
    int item_price;
    int seller_id;
} market;

//交易记录
typedef struct {
    int buyer_id;
    int seller_id;
    char buyer_name[50];
    char seller_name[50];
    int item_id;
    char item_name[50];
    char item_type[30];
    int item_price;
} trade_log;

//好友邀约记录
typedef struct {
    int send_id;
    int recv_id;
    char send_name[50];
    char recv_name[50];
    char game_name[50];
} invite_log;

//crud部分
void crud_init(void);
void crud_write_default_games(void);
int crud_next_id(void);
int crud_load_users(users list[]);
void crud_save_users(users list[], int num);
int crud_load_games(games list[]);
int crud_load_market(market list[]);
void crud_save_market(market list[], int num);
int crud_load_trade_logs(trade_log list[]);
void crud_add_trade_log(trade_log* log_data);
int crud_load_invite_logs(invite_log list[]);
void crud_add_invite_log(invite_log* log_data);
int crud_find_user_index(users list[], int num, char username[]);
int crud_find_user_id_index(users list[], int num, int id);
int crud_find_market_index(market list[], int num, int item_id);

//manage部分
void manage_create_first_admin(void);
int manage_make_captcha(char username[], int id);
int manage_user_register(users* user);
int manage_user_login(char username[], char passwd[]);
int manage_get_win_rate(users* user);
void manage_user_auto_upgrade(users* user);
void manage_user_show_info(char username[]);
int manage_user_edit_info(char username[]);
void manage_user_list_all(void);
int manage_user_upgrade(void);
int manage_user_degrade(void);
int manage_user_delete(void);
int manage_friend_add(char username[]);
int manage_friend_remove(char username[]);
void manage_friend_list(char username[]);
int manage_friend_invite(char username[]);
void manage_friend_show_invite(char username[]);
void manage_show_recommend_games(char username[]);
void manage_show_friend_menu(char username[]);
void manage_show_user_info_menu(char username[]);
void manage_show_admin_menu(void);

//trade部分
void trade_show_market(void);
void trade_show_log(char username[]);
int trade_publish_item(char username[]);
int trade_buy_item(char username[]);
void trade_menu(char username[]);

//main部分
int main_show_menu(int role);
void main_show_ranking(void);
void main_first_register(char username[]);

//这个函数是程序一开始要先调用的
//主要就是把后面要用到的数据文件先准备好
void crud_init(void)
{
    FILE* fp;
    int id_value = 0;

    fp = fopen(USER_FILE, "ab");
    if (fp != NULL) {
        fclose(fp);
    }

    fp = fopen(MARKET_FILE, "ab");
    if (fp != NULL) {
        fclose(fp);
    }

    fp = fopen(TRADE_LOG_FILE, "ab");
    if (fp != NULL) {
        fclose(fp);
    }

    fp = fopen(INVITE_LOG_FILE, "ab");
    if (fp != NULL) {
        fclose(fp);
    }

    fp = fopen(GAME_FILE, "ab");
    if (fp != NULL) {
        fclose(fp);
    }

    fp = fopen(ID_FILE, "rb");
    if (fp == NULL) {
        fp = fopen(ID_FILE, "wb");
        if (fp != NULL) {
            fwrite(&id_value, sizeof(int), 1, fp);
            fclose(fp);
        }
    } else {
        fclose(fp);
    }

    id_value = 1357;
    fp = fopen(SEED_FILE, "rb");
    if (fp == NULL) {
        fp = fopen(SEED_FILE, "wb");
        if (fp != NULL) {
            fwrite(&id_value, sizeof(int), 1, fp);
            fclose(fp);
        }
    } else {
        fclose(fp);
    }

    crud_write_default_games();
}

//这里是写默认游戏数据的
//如果游戏文件里还没有内容，就先放几款热门游戏进去
void crud_write_default_games(void)
{
    FILE* fp;
    long size;
    games first_game;
    games list[6] = {
        {1, "王者荣耀", "竞技", "5V5多人在线竞技游戏", 12},
        {2, "原神", "冒险", "开放世界角色扮演游戏", 12},
        {3, "第五人格", "竞技", "非对称对抗竞技游戏", 12},
        {4, "和平精英", "竞技", "多人战术射击游戏", 12},
        {5, "蛋仔派对", "休闲", "多人闯关休闲派对游戏", 6},
        {6, "阴阳师", "养成", "和风卡牌养成策略游戏", 12}
    };
    int num;

    fp = fopen(GAME_FILE, "rb");
    if (fp == NULL) {
        return;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    if (size > 0) {
        rewind(fp);
        if (fread(&first_game, sizeof(games), 1, fp) == 1) {
            if (strcmp(first_game.game_name, "王者荣耀") == 0) {
                fclose(fp);
                return;
            }
        }
    }
    fclose(fp);

    fp = fopen(GAME_FILE, "wb");
    if (fp == NULL) {
        return;
    }

    num = sizeof(list) / sizeof(list[0]);
    fwrite(list, sizeof(games), num, fp);
    fclose(fp);
}

//这个函数专门用来生成新的编号
//每次注册用户或者上架道具时都会用到
int crud_next_id(void)
{
    FILE* fp;
    int id_value = 0;

    fp = fopen(ID_FILE, "rb");
    if (fp == NULL) {
        return -1;
    }

    if (fread(&id_value, sizeof(int), 1, fp) != 1) {
        id_value = 0;
    }
    fclose(fp);

    id_value++;

    fp = fopen(ID_FILE, "wb");
    if (fp == NULL) {
        return -1;
    }

    fwrite(&id_value, sizeof(int), 1, fp);
    fclose(fp);

    return id_value;
}

//这个函数负责把所有用户从文件里读到数组里
//后面查找、修改、登录基本都要先调用它
int crud_load_users(users list[])
{
    FILE* fp;
    int num = 0;
    users one_user;

    fp = fopen(USER_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&one_user, sizeof(users), 1, fp) == 1) {
        if (num < MAX_USERS) {
            list[num] = one_user;
            num++;
        }
    }

    fclose(fp);
    return num;
}

//这个函数负责把用户数组重新写回文件
//当用户信息有变化时就保存一次
void crud_save_users(users list[], int num)
{
    FILE* fp;

    fp = fopen(USER_FILE, "wb");
    if (fp == NULL) {
        return;
    }

    if (num > 0) {
        fwrite(list, sizeof(users), num, fp);
    }

    fclose(fp);
}

//读取全部游戏数据
//推荐游戏和好友邀约时会用到
int crud_load_games(games list[])
{
    FILE* fp;
    int num = 0;
    games one_game;

    fp = fopen(GAME_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&one_game, sizeof(games), 1, fp) == 1) {
        if (num < MAX_GAMES) {
            list[num] = one_game;
            num++;
        }
    }

    fclose(fp);
    return num;
}

//读取市场里的全部道具
//进入交易菜单时会先读这里的数据
int crud_load_market(market list[])
{
    FILE* fp;
    int num = 0;
    market one_item;

    fp = fopen(MARKET_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&one_item, sizeof(market), 1, fp) == 1) {
        if (num < MAX_MARKET) {
            list[num] = one_item;
            num++;
        }
    }

    fclose(fp);
    return num;
}

//读取交易记录
//这样用户就可以看到自己买过和卖过什么
int crud_load_trade_logs(trade_log list[])
{
    FILE* fp;
    int num = 0;
    trade_log log_data;

    fp = fopen(TRADE_LOG_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&log_data, sizeof(trade_log), 1, fp) == 1) {
        if (num < MAX_MARKET) {
            list[num] = log_data;
            num++;
        }
    }

    fclose(fp);
    return num;
}

//读取好友邀约记录
//查看邀约历史的时候会用到
int crud_load_invite_logs(invite_log list[])
{
    FILE* fp;
    int num = 0;
    invite_log log_data;

    fp = fopen(INVITE_LOG_FILE, "rb");
    if (fp == NULL) {
        return 0;
    }

    while (fread(&log_data, sizeof(invite_log), 1, fp) == 1) {
        if (num < MAX_MARKET) {
            list[num] = log_data;
            num++;
        }
    }

    fclose(fp);
    return num;
}

//把市场数组重新保存到文件里
//上架和购买后都要更新这里
void crud_save_market(market list[], int num)
{
    FILE* fp;

    fp = fopen(MARKET_FILE, "wb");
    if (fp == NULL) {
        return;
    }

    if (num > 0) {
        fwrite(list, sizeof(market), num, fp);
    }

    fclose(fp);
}

//这里是追加一条交易记录
//每次购买成功后就往文件后面加一条
void crud_add_trade_log(trade_log* log_data)
{
    FILE* fp;

    if (log_data == NULL) {
        return;
    }

    fp = fopen(TRADE_LOG_FILE, "ab");
    if (fp == NULL) {
        return;
    }

    fwrite(log_data, sizeof(trade_log), 1, fp);
    fclose(fp);
}

//这里是追加一条好友邀约记录
//有人发起邀约时就在这里保存
void crud_add_invite_log(invite_log* log_data)
{
    FILE* fp;

    if (log_data == NULL) {
        return;
    }

    fp = fopen(INVITE_LOG_FILE, "ab");
    if (fp == NULL) {
        return;
    }

    fwrite(log_data, sizeof(invite_log), 1, fp);
    fclose(fp);
}

//按用户名在数组里找用户位置
//找到就返回下标，找不到就返回-1
int crud_find_user_index(users list[], int num, char username[])
{
    int i;

    for (i = 0; i < num; i++) {
        if (strcmp(list[i].username, username) == 0) {
            return i;
        }
    }

    return -1;
}

//按用户编号查找位置
//显示好友名字和市场卖家时会用到
int crud_find_user_id_index(users list[], int num, int id)
{
    int i;

    for (i = 0; i < num; i++) {
        if (list[i].id == id) {
            return i;
        }
    }

    return -1;
}

//按道具ID查市场里的位置
//购买道具时要先找到是哪一个道具
int crud_find_market_index(market list[], int num, int item_id)
{
    int i;

    for (i = 0; i < num; i++) {
        if (list[i].item_id == item_id) {
            return i;
        }
    }

    return -1;
}

//如果系统里还没有任何用户
//这里会自动建一个默认管理员，方便第一次使用
void manage_create_first_admin(void)
{
    users list[MAX_USERS];
    int num;
    users admin = {0};

    num = crud_load_users(list);
    if (num > 0) {
        return;
    }

    admin.id = crud_next_id();
    strcpy(admin.username, "admin");
    strcpy(admin.passwd, "123456");
    strcpy(admin.identity, "manager");
    strcpy(admin.hobby, "竞技");
    admin.age = 20;
    admin.role = 2;
    admin.coins = 1000;
    admin.game_hours = 200;
    admin.play_count = 50;
    admin.win_count = 30;

    list[0] = admin;
    crud_save_users(list, 1);

    printf("第一次运行，已自动创建管理员账号。\n");
    printf("账号：admin  密码：123456\n");
}

//这里做一个简单的伪随机验证码
//不是完全随机，但每次登录生成的数字会变化
int manage_make_captcha(char username[], int id)
{
    FILE* fp;
    int seed = 1357;
    int code = 0;
    int i;

    fp = fopen(SEED_FILE, "rb");
    if (fp != NULL) {
        if (fread(&seed, sizeof(int), 1, fp) != 1) {
            seed = 1357;
        }
        fclose(fp);
    }

    for (i = 0; username[i] != '\0'; i++) {
        seed = seed * 17 + username[i] + id;
        seed = seed % 10000;
    }

    seed = seed * 31 + id * 7 + 97;
    seed = seed % 10000;
    code = seed % 9000;
    code = code + 1000;

    fp = fopen(SEED_FILE, "wb");
    if (fp != NULL) {
        fwrite(&seed, sizeof(int), 1, fp);
        fclose(fp);
    }

    return code;
}

//根据总对局和胜场算胜率
//这里直接返回整数百分比，方便显示
int manage_get_win_rate(users* user)
{
    if (user == NULL) {
        return 0;
    }

    if (user->play_count <= 0) {
        return 0;
    }

    return user->win_count * 100 / user->play_count;
}

//用户注册函数
//把输入的资料整理好以后保存到用户文件里
int manage_user_register(users* user)
{
    users list[MAX_USERS];
    int num;

    if (user == NULL) {
        return -1;
    }

    num = crud_load_users(list);
    if (crud_find_user_index(list, num, user->username) >= 0) {
        printf("用户名已存在。\n");
        return -1;
    }

    user->id = crud_next_id();
    user->role = 0;
    user->coins = 100;
    user->friend_count = 0;
    user->asset_count = 0;
    user->game_hours = 0;
    user->play_count = 0;
    user->win_count = 0;

    list[num] = *user;
    num++;
    crud_save_users(list, num);

    printf("注册成功。\n");
    return 0;
}

//用户登录函数
//先检查密码，再检查验证码
int manage_user_login(char username[], char passwd[])
{
    users list[MAX_USERS];
    int num;
    int pos;
    int code;
    int input_code;

    num = crud_load_users(list);
    pos = crud_find_user_index(list, num, username);
    if (pos < 0) {
        return -1;
    }

    if (strcmp(list[pos].passwd, passwd) != 0) {
        printf("密码错误。\n");
        return -1;
    }

    code = manage_make_captcha(username, list[pos].id);
    printf("验证码：%d\n", code);
    printf("请输入验证码：");
    scanf("%d", &input_code);
    if (input_code != code) {
        printf("验证码错误。\n");
        return -1;
    }

    printf("登录成功。\n");
    return 0;
}

//这里做自动升级
//普通用户只要游戏时长够或者胜率够，就会升成VIP
void manage_user_auto_upgrade(users* user)
{
    int rate;

    if (user == NULL) {
        return;
    }

    rate = manage_get_win_rate(user);

    if (user->role == 0) {
        if (user->game_hours >= 100 || rate >= 60) {
            user->role = 1;
        }
    }
}

//显示当前用户的详细信息
//包括画像、游戏时长、胜率、好友数这些内容
void manage_user_show_info(char username[])
{
    users list[MAX_USERS];
    int num;
    int pos;
    int rate;

    num = crud_load_users(list);
    pos = crud_find_user_index(list, num, username);
    if (pos < 0) {
        printf("用户不存在。\n");
        return;
    }

    rate = manage_get_win_rate(&list[pos]);

    printf("\n个人信息\n");
    printf("编号：%d\n", list[pos].id);
    printf("用户名：%s\n", list[pos].username);
    printf("年龄：%d\n", list[pos].age);
    printf("身份/职业：%s\n", list[pos].identity);
    printf("爱好：%s\n", list[pos].hobby);
    printf("角色：%d\n", list[pos].role);
    printf("金币：%d\n", list[pos].coins);
    printf("游戏时长：%d\n", list[pos].game_hours);
    printf("总对局：%d\n", list[pos].play_count);
    printf("胜场：%d\n", list[pos].win_count);
    printf("胜率：%d%%\n", rate);
    printf("好友数：%d\n", list[pos].friend_count);
    printf("资产数：%d\n", list[pos].asset_count);
}

//这个函数让用户自己修改信息
//除了基本资料，也可以顺便录入游戏时长和胜负情况
int manage_user_edit_info(char username[])
{
    users list[MAX_USERS];
    int num;
    int pos;
    int choice;
    int add_hours;
    int add_play;
    int add_win;
    char text[50];

    num = crud_load_users(list);
    pos = crud_find_user_index(list, num, username);
    if (pos < 0) {
        printf("用户不存在。\n");
        return -1;
    }

    printf("\n1.改密码\n");
    printf("2.改年龄\n");
    printf("3.改身份\n");
    printf("4.改爱好\n");
    printf("5.录入游戏时长\n");
    printf("6.录入胜负记录\n");
    printf("7.充值金币\n");
    printf("0.返回\n");
    printf("请输入选择：");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("请输入新密码：");
        scanf("%49s", text);
        strcpy(list[pos].passwd, text);
    } else if (choice == 2) {
        printf("请输入新年龄：");
        scanf("%d", &list[pos].age);
    } else if (choice == 3) {
        printf("请输入新身份或职业：");
        scanf("%29s", text);
        strcpy(list[pos].identity, text);
    } else if (choice == 4) {
        printf("请输入新爱好：");
        scanf("%29s", text);
        strcpy(list[pos].hobby, text);
    } else if (choice == 5) {
        printf("请输入新增游戏时长：");
        scanf("%d", &add_hours);
        if (add_hours > 0) {
            list[pos].game_hours += add_hours;
        }
    } else if (choice == 6) {
        printf("请输入新增对局数：");
        scanf("%d", &add_play);
        printf("请输入新增胜场数：");
        scanf("%d", &add_win);
        if (add_play > 0 && add_win >= 0 && add_win <= add_play) {
            list[pos].play_count += add_play;
            list[pos].win_count += add_win;
        }
    } else if (choice == 7) {
        printf("请输入充值金币：");
        scanf("%d", &add_hours);
        if (add_hours > 0) {
            list[pos].coins += add_hours;
        }
    } else {
        return 0;
    }

    manage_user_auto_upgrade(&list[pos]);
    crud_save_users(list, num);
    printf("修改完成。\n");
    return 0;
}

//管理员查看全部用户
//这里主要是为了管理方便
void manage_user_list_all(void)
{
    users list[MAX_USERS];
    int num;
    int i;

    num = crud_load_users(list);
    if (num <= 0) {
        printf("没有用户数据。\n");
        return;
    }

    printf("\n用户列表\n");
    printf("ID\t用户名\t年龄\t角色\t金币\n");
    for (i = 0; i < num; i++) {
        printf("%d\t%s\t%d\t%d\t%d\n",
               list[i].id,
               list[i].username,
               list[i].age,
               list[i].role,
               list[i].coins);
    }
}

//管理员手动升级用户
//和自动升级不同，这里是直接改角色等级
int manage_user_upgrade(void)
{
    users list[MAX_USERS];
    int num;
    int pos;
    char username[50];

    num = crud_load_users(list);
    printf("请输入要升级的用户名：");
    scanf("%49s", username);
    pos = crud_find_user_index(list, num, username);
    if (pos < 0) {
        printf("用户不存在。\n");
        return -1;
    }

    if (list[pos].role < 2) {
        list[pos].role++;
    }

    crud_save_users(list, num);
    printf("升级完成。\n");
    return 0;
}

//管理员手动降级用户
//如果需要恢复普通权限就可以在这里操作
int manage_user_degrade(void)
{
    users list[MAX_USERS];
    int num;
    int pos;
    char username[50];

    num = crud_load_users(list);
    printf("请输入要降级的用户名：");
    scanf("%49s", username);
    pos = crud_find_user_index(list, num, username);
    if (pos < 0) {
        printf("用户不存在。\n");
        return -1;
    }

    if (list[pos].role > 0) {
        list[pos].role--;
    }

    crud_save_users(list, num);
    printf("降级完成。\n");
    return 0;
}

//管理员删除用户
//这里是把数组后面的内容往前移一位
int manage_user_delete(void)
{
    users list[MAX_USERS];
    int num;
    int pos;
    char username[50];
    int i;

    num = crud_load_users(list);
    printf("请输入要删除的用户名：");
    scanf("%49s", username);
    pos = crud_find_user_index(list, num, username);
    if (pos < 0) {
        printf("用户不存在。\n");
        return -1;
    }

    for (i = pos; i < num - 1; i++) {
        list[i] = list[i + 1];
    }
    num--;
    crud_save_users(list, num);
    printf("删除完成。\n");
    return 0;
}

//添加好友
//双方都会互相进入对方的好友列表
int manage_friend_add(char username[])
{
    users list[MAX_USERS];
    int num;
    int my_pos;
    int friend_pos;
    char friend_name[50];

    num = crud_load_users(list);
    my_pos = crud_find_user_index(list, num, username);
    if (my_pos < 0) {
        return -1;
    }

    printf("请输入好友用户名：");
    scanf("%49s", friend_name);
    friend_pos = crud_find_user_index(list, num, friend_name);
    if (friend_pos < 0) {
        printf("好友不存在。\n");
        return -1;
    }

    if (my_pos == friend_pos) {
        printf("不能加自己。\n");
        return -1;
    }

    if (list[my_pos].friend_count >= MAX_FRIENDS || list[friend_pos].friend_count >= MAX_FRIENDS) {
        printf("好友已满。\n");
        return -1;
    }

    list[my_pos].friends[list[my_pos].friend_count] = list[friend_pos].id;
    list[my_pos].friend_count++;
    list[friend_pos].friends[list[friend_pos].friend_count] = list[my_pos].id;
    list[friend_pos].friend_count++;
    crud_save_users(list, num);
    printf("添加好友成功。\n");
    return 0;
}

//删除好友
//这里也是双向删除，不是只删一边
int manage_friend_remove(char username[])
{
    users list[MAX_USERS];
    int num;
    int my_pos;
    int friend_pos;
    char friend_name[50];
    int i;
    int j;

    num = crud_load_users(list);
    my_pos = crud_find_user_index(list, num, username);
    if (my_pos < 0) {
        return -1;
    }

    printf("请输入要删除的好友用户名：");
    scanf("%49s", friend_name);
    friend_pos = crud_find_user_index(list, num, friend_name);
    if (friend_pos < 0) {
        printf("好友不存在。\n");
        return -1;
    }

    for (i = 0; i < list[my_pos].friend_count; i++) {
        if (list[my_pos].friends[i] == list[friend_pos].id) {
            for (j = i; j < list[my_pos].friend_count - 1; j++) {
                list[my_pos].friends[j] = list[my_pos].friends[j + 1];
            }
            list[my_pos].friend_count--;
            break;
        }
    }

    for (i = 0; i < list[friend_pos].friend_count; i++) {
        if (list[friend_pos].friends[i] == list[my_pos].id) {
            for (j = i; j < list[friend_pos].friend_count - 1; j++) {
                list[friend_pos].friends[j] = list[friend_pos].friends[j + 1];
            }
            list[friend_pos].friend_count--;
            break;
        }
    }

    crud_save_users(list, num);
    printf("删除好友成功。\n");
    return 0;
}

//显示当前用户的好友列表
//这里会把好友ID再对应回好友名字
void manage_friend_list(char username[])
{
    users list[MAX_USERS];
    int num;
    int my_pos;
    int i;
    int pos;

    num = crud_load_users(list);
    my_pos = crud_find_user_index(list, num, username);
    if (my_pos < 0) {
        return;
    }

    if (list[my_pos].friend_count == 0) {
        printf("还没有好友。\n");
        return;
    }

    printf("\n好友列表\n");
    for (i = 0; i < list[my_pos].friend_count; i++) {
        pos = crud_find_user_id_index(list, num, list[my_pos].friends[i]);
        if (pos >= 0) {
            printf("%s\n", list[pos].username);
        }
    }
}

//好友邀约功能
//只有已经加成好友的人才能发邀约
int manage_friend_invite(char username[])
{
    users list[MAX_USERS];
    games game_list[MAX_GAMES];
    invite_log invite = {0};
    int num;
    int game_num;
    int my_pos;
    int friend_pos;
    int has_friend = 0;
    int i;
    char friend_name[50];
    char game_name[50];

    num = crud_load_users(list);
    my_pos = crud_find_user_index(list, num, username);
    if (my_pos < 0) {
        return -1;
    }

    printf("请输入要邀约的好友用户名：");
    scanf("%49s", friend_name);
    friend_pos = crud_find_user_index(list, num, friend_name);
    if (friend_pos < 0) {
        printf("好友不存在。\n");
        return -1;
    }

    for (i = 0; i < list[my_pos].friend_count; i++) {
        if (list[my_pos].friends[i] == list[friend_pos].id) {
            has_friend = 1;
            break;
        }
    }

    if (has_friend == 0) {
        printf("对方不是你的好友，不能发邀约。\n");
        return -1;
    }

    game_num = crud_load_games(game_list);
    printf("\n可邀约游戏\n");
    for (i = 0; i < game_num; i++) {
        printf("%s  %s\n", game_list[i].game_name, game_list[i].game_type);
    }

    printf("请输入邀约游戏名称：");
    scanf("%49s", game_name);

    invite.send_id = list[my_pos].id;
    invite.recv_id = list[friend_pos].id;
    strcpy(invite.send_name, list[my_pos].username);
    strcpy(invite.recv_name, list[friend_pos].username);
    strcpy(invite.game_name, game_name);
    crud_add_invite_log(&invite);
    printf("邀约发送成功。\n");
    return 0;
}

//查看和自己有关的邀约记录
//不管是你发出去的还是别人发给你的都会显示
void manage_friend_show_invite(char username[])
{
    invite_log logs[MAX_MARKET];
    int num;
    int i;
    int show_count = 0;

    num = crud_load_invite_logs(logs);
    if (num <= 0) {
        printf("还没有邀约记录。\n");
        return;
    }

    printf("\n好友邀约记录\n");
    for (i = 0; i < num; i++) {
        if (strcmp(logs[i].send_name, username) == 0 || strcmp(logs[i].recv_name, username) == 0) {
            printf("发起人：%s  接收人：%s  游戏：%s\n",
                   logs[i].send_name,
                   logs[i].recv_name,
                   logs[i].game_name);
            show_count++;
        }
    }

    if (show_count == 0) {
        printf("你还没有邀约记录。\n");
    }
}

//推荐游戏
//这里按年龄、身份、爱好和游戏时长做一个简单推荐
void manage_show_recommend_games(char username[])
{
    users list[MAX_USERS];
    games game_list[MAX_GAMES];
    int user_num;
    int game_num;
    int pos;
    int i;
    int show_count = 0;

    user_num = crud_load_users(list);
    pos = crud_find_user_index(list, user_num, username);
    if (pos < 0) {
        return;
    }

    game_num = crud_load_games(game_list);
    printf("\n推荐游戏\n");
    for (i = 0; i < game_num; i++) {
        if (list[pos].age >= game_list[i].need_age) {
            if (strstr(game_list[i].game_type, list[pos].hobby) != NULL ||
                strstr(list[pos].hobby, game_list[i].game_type) != NULL ||
                (strstr(list[pos].identity, "学生") != NULL && strstr(game_list[i].game_type, "竞技") != NULL) ||
                (strstr(list[pos].identity, "教师") != NULL && strstr(game_list[i].game_type, "策略") != NULL) ||
                (list[pos].game_hours >= 100 && strstr(game_list[i].game_type, "竞技") != NULL)) {
                printf("%s  %s  %s\n",
                       game_list[i].game_name,
                       game_list[i].game_type,
                       game_list[i].game_desc);
                show_count++;
            }
        }
    }

    if (show_count == 0) {
        for (i = 0; i < game_num; i++) {
            if (list[pos].age >= game_list[i].need_age) {
                printf("%s  %s  %s\n",
                       game_list[i].game_name,
                       game_list[i].game_type,
                       game_list[i].game_desc);
            }
        }
    }
}

//好友菜单
//把好友相关功能集中放在这里
void manage_show_friend_menu(char username[])
{
    int choice;

    while (1) {
        printf("\n1.查看好友\n");
        printf("2.添加好友\n");
        printf("3.删除好友\n");
        printf("4.发起邀约\n");
        printf("5.查看邀约记录\n");
        printf("0.返回\n");
        printf("请输入选择：");
        scanf("%d", &choice);

        if (choice == 1) {
            manage_friend_list(username);
        } else if (choice == 2) {
            manage_friend_add(username);
        } else if (choice == 3) {
            manage_friend_remove(username);
        } else if (choice == 4) {
            manage_friend_invite(username);
        } else if (choice == 5) {
            manage_friend_show_invite(username);
        } else {
            return;
        }
    }
}

//个人信息菜单
//查看和修改个人资料都从这里进入
void manage_show_user_info_menu(char username[])
{
    int choice;

    while (1) {
        printf("\n1.查看个人信息\n");
        printf("2.修改个人信息\n");
        printf("0.返回\n");
        printf("请输入选择：");
        scanf("%d", &choice);

        if (choice == 1) {
            manage_user_show_info(username);
        } else if (choice == 2) {
            manage_user_edit_info(username);
        } else {
            return;
        }
    }
}

//管理员菜单
//管理员可以在这里管用户
void manage_show_admin_menu(void)
{
    int choice;

    while (1) {
        printf("\n1.查看所有用户\n");
        printf("2.升级用户\n");
        printf("3.降级用户\n");
        printf("4.删除用户\n");
        printf("0.返回\n");
        printf("请输入选择：");
        scanf("%d", &choice);

        if (choice == 1) {
            manage_user_list_all();
        } else if (choice == 2) {
            manage_user_upgrade();
        } else if (choice == 3) {
            manage_user_degrade();
        } else if (choice == 4) {
            manage_user_delete();
        } else {
            return;
        }
    }
}

//把市场里现在所有在售的道具列出来
//用户买东西之前会先看这里
void trade_show_market(void)
{
    market list[MAX_MARKET];
    users user_list[MAX_USERS];
    int num;
    int user_num;
    int i;
    int pos;

    num = crud_load_market(list);
    user_num = crud_load_users(user_list);
    if (num <= 0) {
        printf("市场里还没有道具。\n");
        return;
    }

    printf("\n市场列表\n");
    printf("ID\t名称\t类型\t价格\t卖家\n");
    for (i = 0; i < num; i++) {
        pos = crud_find_user_id_index(user_list, user_num, list[i].seller_id);
        printf("%d\t%s\t%s\t%d\t",
               list[i].item_id,
               list[i].item_name,
               list[i].item_type,
               list[i].item_price);
        if (pos >= 0) {
            printf("%s\n", user_list[pos].username);
        } else {
            printf("未知\n");
        }
    }
}

//显示交易记录
//这里只展示和当前用户有关的记录
void trade_show_log(char username[])
{
    trade_log logs[MAX_MARKET];
    int num;
    int i;
    int show_count = 0;

    num = crud_load_trade_logs(logs);
    if (num <= 0) {
        printf("还没有交易记录。\n");
        return;
    }

    printf("\n交易记录\n");
    for (i = 0; i < num; i++) {
        if (strcmp(logs[i].buyer_name, username) == 0 || strcmp(logs[i].seller_name, username) == 0) {
            printf("道具：%s  类型：%s  价格：%d  买家：%s  卖家：%s\n",
                   logs[i].item_name,
                   logs[i].item_type,
                   logs[i].item_price,
                   logs[i].buyer_name,
                   logs[i].seller_name);
            show_count++;
        }
    }

    if (show_count == 0) {
        printf("你还没有相关交易记录。\n");
    }
}

//上架道具
//输入道具名字、类型和价格以后保存到市场里
int trade_publish_item(char username[])
{
    market list[MAX_MARKET];
    users user_list[MAX_USERS];
    int num;
    int user_num;
    int user_pos;
    market item = {0};

    num = crud_load_market(list);
    user_num = crud_load_users(user_list);
    user_pos = crud_find_user_index(user_list, user_num, username);
    if (user_pos < 0) {
        return -1;
    }

    printf("请输入道具名称：");
    scanf("%49s", item.item_name);
    printf("请输入道具类型：");
    scanf("%29s", item.item_type);
    printf("请输入价格：");
    scanf("%d", &item.item_price);

    item.item_id = crud_next_id();
    item.seller_id = user_list[user_pos].id;
    list[num] = item;
    num++;

    if (user_list[user_pos].asset_count < MAX_ASSETS) {
        user_list[user_pos].assets[user_list[user_pos].asset_count] = item.item_id;
        user_list[user_pos].asset_count++;
    }

    crud_save_market(list, num);
    crud_save_users(user_list, user_num);
    printf("上架成功。\n");
    return 0;
}

//购买道具
//买家扣钱，卖家加钱，同时还会写交易记录
int trade_buy_item(char username[])
{
    market list[MAX_MARKET];
    users user_list[MAX_USERS];
    trade_log one_log = {0};
    int num;
    int user_num;
    int buyer_pos;
    int item_pos;
    int seller_pos;
    int target_id;
    int i;

    num = crud_load_market(list);
    user_num = crud_load_users(user_list);
    buyer_pos = crud_find_user_index(user_list, user_num, username);
    if (buyer_pos < 0) {
        return -1;
    }

    if (num <= 0) {
        printf("市场里还没有道具。\n");
        return -1;
    }

    trade_show_market();
    printf("请输入要购买的道具ID：");
    scanf("%d", &target_id);
    item_pos = crud_find_market_index(list, num, target_id);
    if (item_pos < 0) {
        printf("没有这个道具。\n");
        return -1;
    }

    if (list[item_pos].seller_id == user_list[buyer_pos].id) {
        printf("不能买自己的道具。\n");
        return -1;
    }

    if (user_list[buyer_pos].coins < list[item_pos].item_price) {
        printf("金币不够。\n");
        return -1;
    }

    seller_pos = crud_find_user_id_index(user_list, user_num, list[item_pos].seller_id);
    user_list[buyer_pos].coins -= list[item_pos].item_price;
    if (seller_pos >= 0) {
        user_list[seller_pos].coins += list[item_pos].item_price;
    }

    if (user_list[buyer_pos].asset_count < MAX_ASSETS) {
        user_list[buyer_pos].assets[user_list[buyer_pos].asset_count] = list[item_pos].item_id;
        user_list[buyer_pos].asset_count++;
    }

    one_log.buyer_id = user_list[buyer_pos].id;
    strcpy(one_log.buyer_name, user_list[buyer_pos].username);
    one_log.seller_id = list[item_pos].seller_id;
    if (seller_pos >= 0) {
        strcpy(one_log.seller_name, user_list[seller_pos].username);
    } else {
        strcpy(one_log.seller_name, "未知");
    }
    one_log.item_id = list[item_pos].item_id;
    strcpy(one_log.item_name, list[item_pos].item_name);
    strcpy(one_log.item_type, list[item_pos].item_type);
    one_log.item_price = list[item_pos].item_price;

    for (i = item_pos; i < num - 1; i++) {
        list[i] = list[i + 1];
    }
    num--;

    crud_save_market(list, num);
    crud_save_users(user_list, user_num);
    crud_add_trade_log(&one_log);
    printf("购买成功。\n");
    return 0;
}

//交易菜单
//市场查看、上架、购买、交易记录都在这里
void trade_menu(char username[])
{
    int choice;

    while (1) {
        printf("\n1.查看市场\n");
        printf("2.上架道具\n");
        printf("3.购买道具\n");
        printf("4.查看交易记录\n");
        printf("0.返回\n");
        printf("请输入选择：");
        scanf("%d", &choice);

        if (choice == 1) {
            trade_show_market();
        } else if (choice == 2) {
            trade_publish_item(username);
        } else if (choice == 3) {
            trade_buy_item(username);
        } else if (choice == 4) {
            trade_show_log(username);
        } else {
            return;
        }
    }
}

//主菜单显示函数
//根据是不是管理员，菜单内容会有一点区别
int main_show_menu(int role)
{
    int choice;

    printf("\n==============================\n");
    if (role == 2) {
        printf("管理员菜单\n");
        printf("1.排行榜\n");
        printf("2.推荐游戏\n");
        printf("3.交易市场\n");
        printf("4.好友管理\n");
        printf("5.个人信息\n");
        printf("6.用户管理\n");
        printf("7.退出\n");
    } else {
        printf("用户菜单\n");
        printf("1.排行榜\n");
        printf("2.推荐游戏\n");
        printf("3.交易市场\n");
        printf("4.好友管理\n");
        printf("5.个人信息\n");
        printf("6.退出\n");
    }
    printf("==============================\n");
    printf("请输入选择：");
    scanf("%d", &choice);
    return choice;
}

//金币排行榜
//这里用最基础的冒泡排序来排前面的用户
void main_show_ranking(void)
{
    users list[MAX_USERS];
    int num;
    int i;
    int j;
    users temp;

    num = crud_load_users(list);
    if (num <= 0) {
        printf("没有用户数据。\n");
        return;
    }

    for (i = 0; i < num - 1; i++) {
        for (j = 0; j < num - 1 - i; j++) {
            if (list[j].coins < list[j + 1].coins) {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }

    printf("\n金币排行榜\n");
    for (i = 0; i < num && i < 10; i++) {
        printf("%d. %s %d\n", i + 1, list[i].username, list[i].coins);
    }
}

//如果输入的用户名还不存在
//那这里就当作第一次注册来处理
void main_first_register(char username[])
{
    users user = {0};

    strcpy(user.username, username);
    printf("请输入密码：");
    scanf("%49s", user.passwd);
    printf("请输入年龄：");
    scanf("%d", &user.age);
    printf("请输入身份或职业：");
    scanf("%29s", user.identity);
    printf("请输入爱好：");
    scanf("%29s", user.hobby);
    manage_user_register(&user);
}

//主函数
//程序从这里开始运行，先登录或注册，再进入主菜单循环
int main(void)
{
    char username[50];
    char passwd[50];
    users list[MAX_USERS];
    int num;
    int pos;
    int role;
    int choice;

    crud_init();
    manage_create_first_admin();

    printf("欢迎来到unisystem，请输入用户名：");
    scanf("%49s", username);

    num = crud_load_users(list);
    pos = crud_find_user_index(list, num, username);

    if (pos >= 0) {
        printf("请输入密码：");
        scanf("%49s", passwd);
        while (manage_user_login(username, passwd) != 0) {
            printf("请重新输入密码：");
            scanf("%49s", passwd);
        }
    } else {
        main_first_register(username);
    }

    while (1) {
        num = crud_load_users(list);
        pos = crud_find_user_index(list, num, username);
        if (pos < 0) {
            return 0;
        }

        manage_user_auto_upgrade(&list[pos]);
        crud_save_users(list, num);
        role = list[pos].role;
        choice = main_show_menu(role);

        if (choice == 1) {
            main_show_ranking();
        } else if (choice == 2) {
            manage_show_recommend_games(username);
        } else if (choice == 3) {
            trade_menu(username);
        } else if (choice == 4) {
            manage_show_friend_menu(username);
        } else if (choice == 5) {
            manage_show_user_info_menu(username);
        } else if (choice == 6 && role == 2) {
            manage_show_admin_menu();
        } else if ((choice == 6 && role != 2) || (choice == 7 && role == 2)) {
            printf("感谢使用。\n");
            return 0;
        } else {
            printf("输入有误。\n");
        }
    }
}
