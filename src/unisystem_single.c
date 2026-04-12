#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//一些数量上限
#define MAX_USERS 500
#define MAX_FRIENDS 100
#define MAX_ASSETS 100
#define MAX_GAMES 200
#define MAX_MARKET_ITEMS 500

//用户资料
typedef struct {
    char username[50];
    char passwd[50];
    int id;
    int age;
    int role;//0是普通用户，1是VIP，2是管理员
    int coins;//金币数
    int friend_count;
    int asset_count;
    int friends[MAX_FRIENDS];//好友ID
    int assets[MAX_ASSETS];//道具ID
} users;

//商店道具资料
typedef struct {
    int item_id;
    char item_name[64];
    int item_price;
    int seller_id;
    char publish_time[32];
} market;

//游戏资料
typedef struct {
    int game_id;
    char game_name[64];
    char game_desc[256];
} games;

//函数声明
int crud_next_id(void);
void crud_init(void);
int crud_save_line(int option, char* json_line);
users* crud_find_user(char* username);
int crud_add_user(users* u);
int crud_edit_user_file(char* username, char* item);

int crud_read_user_line(char* line, users* out_user);
int crud_make_user_line(users* u, char* out_line, int out_size);
int crud_load_users(users* list, int max_count);
int crud_save_users(users* list, int num);
int crud_update_user(users* u);
int crud_delete_user_file(char* username);

int crud_load_games(games* list, int max_count);
int crud_load_market(market* list, int max_count);
int crud_save_market(market* list, int num);
int crud_add_market_item(market* item);

int manage_user_register(char* username, char* passwd, int age);
int manage_user_login(char* username, char* passwd);
void manage_user_list_all(void);
int manage_user_upgrade(void);
int manage_user_degrade(void);
int manage_user_delete(void);
void manage_user_show_info(char* username);
int manage_user_edit_info(char* username);
int manage_friend_add(char* username);
int manage_friend_remove(char* username);
void manage_friend_list(char* username);

void trade_menu(char* username);

//数据文件名
#define USER_FILE "users.jsonl"
#define MARKET_FILE "market.jsonl"
#define GAME_FILE "games.jsonl"
#define ID_FILE "id"

//基本小函数
void crud_parse_int_list(char* text, int* crud_list, int* crud_num, int max_count)
{
    char crud_buffer[512];
    char* crud_token;

    *crud_num = 0;//先默认没有数据
    if (text == NULL || text[0] == '\0') {
        return;
    }

    strncpy(crud_buffer, text, sizeof(crud_buffer) - 1);//先把原字符串复制出来
    crud_buffer[sizeof(crud_buffer) - 1] = '\0';

    crud_token = strtok(crud_buffer, ",");//按逗号一个一个拆开
    while (crud_token != NULL && *crud_num < max_count) {
        while (*crud_token == ' ') {
            crud_token++;
        }
        if (*crud_token != '\0') {
            crud_list[*crud_num] = atoi(crud_token);
            (*crud_num)++;
        }
        crud_token = strtok(NULL, ",");
    }
}

void crud_build_int_list(int* crud_list, int crud_num, char* out, int out_size)
{
    int i;
    out[0] = '\0';

    for (i = 0; i < crud_num; i++) {//把整数数组重新拼成字符串
        char part[32];
        int written;

        if (i > 0) {
            if ((int)strlen(out) + 1 >= out_size) {
                break;
            }
            strcat(out, ",");
        }

        written = snprintf(part, sizeof(part), "%d", crud_list[i]);
        if (written <= 0) {
            continue;
        }

        if ((int)strlen(out) + written >= out_size) {
            break;
        }
        strcat(out, part);
    }
}

void crud_extract_json_string_field(char* crud_line, char* key, char* out, int out_size)
{
    char* start;
    char* end;
    int len;

    out[0] = '\0';//先清空输出
    start = strstr(crud_line, key);
    if (start == NULL) {
        return;
    }

    start += (int)strlen(key);
    end = strchr(start, '"');
    if (end == NULL) {
        return;
    }

    len = (int)(end - start);
    if (len >= out_size) {
        len = out_size - 1;
    }

    strncpy(out, start, len);
    out[len] = '\0';
}

//检查数据文件
int crud_check_data_files(void)
{
    FILE* crud_fp;

    crud_fp = fopen(USER_FILE, "a");//检查用户文件
    if (crud_fp == NULL) {
        printf("用户文件创建失败。\n");
        return -1;
    }
    fclose(crud_fp);

    crud_fp = fopen(MARKET_FILE, "a");//检查商店文件
    if (crud_fp == NULL) {
        printf("找不到 market.jsonl 文件。\n");
        return -1;
    }
    fclose(crud_fp);

    crud_fp = fopen(GAME_FILE, "a");//检查游戏文件
    if (crud_fp == NULL) {
        printf("找不到 games.jsonl 文件。\n");
        return -1;
    }
    fclose(crud_fp);

    crud_fp = fopen(ID_FILE, "r");//检查编号文件
    if (crud_fp == NULL) {
        crud_fp = fopen(ID_FILE, "w");
        if (crud_fp == NULL) {
            printf("找不到 id 文件。\n");
            return -1;
        }
        fprintf(crud_fp, "0");
    }

    if (crud_fp != NULL) {
        fclose(crud_fp);
    }

    return 0;
}

//写入默认游戏数据
void crud_write_default_games(void)
{
    FILE* crud_fp = fopen(GAME_FILE, "r");
    int ch;
    int crud_has_content = 0;

    if (crud_fp == NULL) {//先看看文件能不能打开
        return;
    }

    while ((ch = fgetc(crud_fp)) != EOF) {//只要有内容就不再重复写入
        if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t') {
            crud_has_content = 1;
            break;
        }
    }
    fclose(crud_fp);

    if (crud_has_content) {
        return;
    }

    crud_fp = fopen(GAME_FILE, "w");
    if (crud_fp == NULL) {
        return;
    }

    fputs("{\"game_id\": 1, \"game_name\": \"Sky War\", \"game_desc\": \"快节奏多人竞技射击游戏\"}\n", crud_fp);
    fputs("{\"game_id\": 2, \"game_name\": \"Farm World\", \"game_desc\": \"休闲经营类种田社交游戏\"}\n", crud_fp);
    fputs("{\"game_id\": 3, \"game_name\": \"Magic Chess\", \"game_desc\": \"策略回合制卡牌对战游戏\"}\n", crud_fp);
    fputs("{\"game_id\": 4, \"game_name\": \"Hero Road\", \"game_desc\": \"剧情向角色扮演冒险游戏\"}\n", crud_fp);
    fclose(crud_fp);
}

int crud_have_user(void)
{
    FILE* crud_fp;
    char crud_line[256];

    crud_fp = fopen(USER_FILE, "r");//打开用户文件
    if (crud_fp == NULL) {
        return 0;
    }

    while (fgets(crud_line, sizeof(crud_line), crud_fp) != NULL) {//只要读到一行内容就说明已经有用户了
        if (crud_line[0] != '\n' && crud_line[0] != '\r' && crud_line[0] != '\0') {
            fclose(crud_fp);
            return 1;
        }
    }

    fclose(crud_fp);
    return 0;
}

//第一次运行时先建管理员账号
void crud_create_first_user(void)
{
    users admin = {0};
    FILE* crud_fp;

    if (crud_have_user()) {//如果已经有用户，就不用再创建
        return;
    }

    admin.id = 1;//默认创建一个管理员
    strcpy(admin.username, "admin");
    strcpy(admin.passwd, "123456");
    admin.age = 18;
    admin.role = 2;
    admin.coins = 1000;

    crud_add_user(&admin);

    crud_fp = fopen(ID_FILE, "w");
    if (crud_fp != NULL) {
        fprintf(crud_fp, "1");
        fclose(crud_fp);
    }

    printf("第一次运行，系统里还没有用户。\n");
    printf("已自动创建管理员账号：admin  密码：123456\n");
}

//生成新编号
int crud_next_id(void)
{
    FILE* crud_fp;
    int crud_id_num = 0;

    crud_init();//先保证数据文件都准备好了

    crud_fp = fopen(ID_FILE, "r");//读出当前编号
    if (crud_fp != NULL) {
        if (fscanf(crud_fp, "%d", &crud_id_num) != 1) {
            crud_id_num = 0;
        }
        fclose(crud_fp);
    }

    crud_id_num++;//编号加1再写回去

    crud_fp = fopen(ID_FILE, "w");
    if (crud_fp == NULL) {
        return -1;
    }

    fprintf(crud_fp, "%d", crud_id_num);
    fclose(crud_fp);

    return crud_id_num;
}

//程序开始时先初始化
void crud_init(void)
{
    if (crud_check_data_files() != 0) {
        return;
    }

    crud_write_default_games();
    crud_create_first_user();
}

int crud_save_line(int option, char* json_line)
{
    char* crud_path;
    FILE* crud_fp;

    switch (option) {//根据选项决定写到哪个文件
    case 1:
        crud_path = USER_FILE;
        break;
    case 2:
        crud_path = MARKET_FILE;
        break;
    case 3:
        crud_path = GAME_FILE;
        break;
    default:
        return -2;
    }

    crud_fp = fopen(crud_path, "a");
    if (crud_fp == NULL) {
        return -3;
    }

    fprintf(crud_fp, "%s\n", json_line);
    fclose(crud_fp);
    return 0;
}

int crud_read_user_line(char* crud_line, users* out_user)
{
    char crud_assets_text[512] = "";
    char crud_friends_text[512] = "";
    int crud_matched;
    users temp = {0};

    if (crud_line == NULL || out_user == NULL) {
        return -1;
    }

    *out_user = temp;//先清空结构体

    crud_matched = sscanf(crud_line,//先按现在的格式读
                     "{\"id\": %d, \"username\": \"%49[^\"]\", \"passwd\": \"%49[^\"]\", \"age\": %d, \"role\": %d, \"coins\": %d",
                     &out_user->id,
                     out_user->username,
                     out_user->passwd,
                     &out_user->age,
                     &out_user->role,
                     &out_user->coins);

    if (crud_matched != 6) {
        return -1;
    }

    crud_extract_json_string_field(crud_line, "\"assets\": \"", crud_assets_text, sizeof(crud_assets_text));
    crud_extract_json_string_field(crud_line, "\"friends\": \"", crud_friends_text, sizeof(crud_friends_text));

    crud_parse_int_list(crud_assets_text, out_user->assets, &out_user->asset_count, MAX_ASSETS);
    crud_parse_int_list(crud_friends_text, out_user->friends, &out_user->friend_count, MAX_FRIENDS);
    return 0;
}

int crud_make_user_line(users* u, char* out_line, int out_size)
{
    char crud_assets_text[512];
    char crud_friends_text[512];
    int written;

    if (u == NULL || out_line == NULL || out_size <= 0) {
        return -1;
    }

    crud_build_int_list(u->assets, u->asset_count, crud_assets_text, sizeof(crud_assets_text));
    crud_build_int_list(u->friends, u->friend_count, crud_friends_text, sizeof(crud_friends_text));

    written = snprintf(out_line,
                       out_size,
                       "{\"id\": %d, \"username\": \"%s\", \"passwd\": \"%s\", \"age\": %d, \"role\": %d, \"coins\": %d, \"assets\": \"%s\", \"friends\": \"%s\"}",
                       u->id,
                       u->username,
                       u->passwd,
                       u->age,
                       u->role,
                       u->coins,
                       crud_assets_text,
                       crud_friends_text);

    if (written < 0 || written >= out_size) {
        return -2;
    }
    return 0;
}

int crud_load_users(users* crud_list, int max_count)
{
    FILE* crud_fp;
    char crud_line[2048];
    int crud_num = 0;

    if (crud_list == NULL || max_count <= 0) {
        return -1;
    }

    crud_init();

    crud_fp = fopen(USER_FILE, "r");
    if (crud_fp == NULL) {
        return 0;
    }

    while (fgets(crud_line, sizeof(crud_line), crud_fp) != NULL) {//逐行读取用户数据
        users u;

        if (crud_read_user_line(crud_line, &u) == 0) {
            if (crud_num < max_count) {
                crud_list[crud_num++] = u;
            }
        }
    }

    fclose(crud_fp);
    return crud_num;
}

int crud_save_users(users* crud_list, int crud_num)
{
    FILE* crud_fp;
    int i;

    crud_init();

    crud_fp = fopen(USER_FILE, "w");
    if (crud_fp == NULL) {
        return -1;
    }

    for (i = 0; i < crud_num; i++) {//重新把所有用户写回文件
        char crud_line[2048];

        if (crud_make_user_line(&crud_list[i], crud_line, sizeof(crud_line)) == 0) {
            fprintf(crud_fp, "%s\n", crud_line);
        }
    }

    fclose(crud_fp);
    return 0;
}

users* crud_find_user(char* username)
{
    static users target_user;
    users crud_list[MAX_USERS];
    int crud_num;
    int i;

    if (username == NULL) {
        return NULL;
    }

    crud_num = crud_load_users(crud_list, MAX_USERS);//把文件里的用户全部读出来再查找
    if (crud_num < 0) {
        return NULL;
    }

    for (i = 0; i < crud_num; i++) {
        if (strcmp(crud_list[i].username, username) == 0) {
            target_user = crud_list[i];
            return &target_user;
        }
    }

    return NULL;
}

int crud_add_user(users* u)
{
    char crud_line[2048];

    if (u == NULL) {
        return -1;
    }
    //先把结构体转成一行字符串
    if (crud_make_user_line(u, crud_line, sizeof(crud_line)) != 0) {
        return -2;
    }

    return crud_save_line(1, crud_line);
}

int crud_update_user(users* u)
{
    users crud_list[MAX_USERS];
    int crud_num;
    int i;

    if (u == NULL) {
        return -1;
    }

    crud_num = crud_load_users(crud_list, MAX_USERS);
    if (crud_num < 0) {
        return -2;
    }

    for (i = 0; i < crud_num; i++) {//找到这个用户后再覆盖保存
        if (crud_list[i].id == u->id || strcmp(crud_list[i].username, u->username) == 0) {
            crud_list[i] = *u;
            return crud_save_users(crud_list, crud_num);
        }
    }

    return -3;
}

int crud_delete_user_file(char* username)
{
    users crud_list[MAX_USERS];
    int crud_num;
    int i;

    if (username == NULL) {
        return -1;
    }

    crud_num = crud_load_users(crud_list, MAX_USERS);
    if (crud_num < 0) {
        return -2;
    }

    for (i = 0; i < crud_num; i++) {
        if (strcmp(crud_list[i].username, username) == 0) {
            int j;
            for (j = i; j < crud_num - 1; j++) {
                crud_list[j] = crud_list[j + 1];
            }
            crud_num--;
            return crud_save_users(crud_list, crud_num);
        }
    }

    return -3;
}

int crud_edit_user_file(char* username, char* item)
{
    users* u;
    users updated;

    if (username == NULL || item == NULL) {
        return -1;
    }

    u = crud_find_user(username);
    if (u == NULL) {
        return -2;
    }

    updated = *u;

    if (strncmp(item, "age=", 4) == 0) {
        updated.age = atoi(item + 4);
    } else if (strncmp(item, "coins=", 6) == 0) {
        updated.coins = atoi(item + 6);
    } else if (strncmp(item, "role=", 5) == 0) {
        updated.role = atoi(item + 5);
    } else if (strncmp(item, "passwd=", 7) == 0) {
        strncpy(updated.passwd, item + 7, sizeof(updated.passwd) - 1);
        updated.passwd[sizeof(updated.passwd) - 1] = '\0';
    } else {
        return -3;
    }

    return crud_update_user(&updated);
}

int crud_load_games(games* crud_list, int max_count)
{
    FILE* crud_fp;
    char crud_line[2048];
    int crud_num = 0;

    if (crud_list == NULL || max_count <= 0) {
        return -1;
    }

    crud_init();

    crud_fp = fopen(GAME_FILE, "r");
    if (crud_fp == NULL) {
        return 0;
    }

    while (fgets(crud_line, sizeof(crud_line), crud_fp) != NULL) {
        games g = {0};
        int crud_matched = sscanf(crud_line,
                             "{\"game_id\": %d, \"game_name\": \"%63[^\"]\", \"game_desc\": \"%255[^\"]\"}",
                             &g.game_id,
                             g.game_name,
                             g.game_desc);
        if (crud_matched == 3 && crud_num < max_count) {
            crud_list[crud_num++] = g;
        }
    }

    fclose(crud_fp);
    return crud_num;
}

int crud_load_market(market* crud_list, int max_count)
{
    FILE* crud_fp;
    char crud_line[1024];
    int crud_num = 0;

    if (crud_list == NULL || max_count <= 0) {
        return -1;
    }

    crud_init();

    crud_fp = fopen(MARKET_FILE, "r");
    if (crud_fp == NULL) {
        return 0;
    }

    while (fgets(crud_line, sizeof(crud_line), crud_fp) != NULL) {
        market m = {0};
        int crud_matched = sscanf(crud_line,
                             "{\"item_id\": %d, \"item_name\": \"%63[^\"]\", \"item_price\": %d, \"seller_id\": %d, \"publish_time\": \"%31[^\"]\"}",
                             &m.item_id,
                             m.item_name,
                             &m.item_price,
                             &m.seller_id,
                             m.publish_time);

        if (crud_matched != 5) {
            crud_matched = sscanf(crud_line,
                             "{\"item_id\": %d, \"item_price\": %d, \"publish_time\": \"%31[^\"]\"}",
                             &m.item_id,
                             &m.item_price,
                             m.publish_time);
            if (crud_matched == 3) {
                strcpy(m.item_name, "未知道具");
                m.seller_id = 0;
            }
        }

        if ((crud_matched == 5 || crud_matched == 3) && crud_num < max_count) {
            crud_list[crud_num++] = m;
        }
    }

    fclose(crud_fp);
    return crud_num;
}

int crud_save_market(market* crud_list, int crud_num)
{
    FILE* crud_fp;
    int i;

    crud_init();

    crud_fp = fopen(MARKET_FILE, "w");
    if (crud_fp == NULL) {
        return -1;
    }

    for (i = 0; i < crud_num; i++) {
        fprintf(crud_fp,
                "{\"item_id\": %d, \"item_name\": \"%s\", \"item_price\": %d, \"seller_id\": %d, \"publish_time\": \"%s\"}\n",
                crud_list[i].item_id,
                crud_list[i].item_name,
                crud_list[i].item_price,
                crud_list[i].seller_id,
                crud_list[i].publish_time);
    }

    fclose(crud_fp);
    return 0;
}

int crud_add_market_item(market* item)
{
    char crud_line[1024];

    if (item == NULL) {
        return -1;
    }

    snprintf(crud_line,
             sizeof(crud_line),
             "{\"item_id\": %d, \"item_name\": \"%s\", \"item_price\": %d, \"seller_id\": %d, \"publish_time\": \"%s\"}",
             item->item_id,
             item->item_name,
             item->item_price,
             item->seller_id,
             item->publish_time);

    return crud_save_line(2, crud_line);
}

int manage_find_user_pos_by_name(users* crud_list, int crud_num, char* username)
{
    int i;
    for (i = 0; i < crud_num; i++) {
        if (strcmp(crud_list[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

int manage_find_user_pos_by_id(users* crud_list, int crud_num, int id_value)
{
    int i;
    for (i = 0; i < crud_num; i++) {
        if (crud_list[i].id == id_value) {
            return i;
        }
    }
    return -1;
}

int manage_is_friend(users* u, int friend_id)
{
    int i;
    for (i = 0; i < u->friend_count; i++) {
        if (u->friends[i] == friend_id) {
            return 1;
        }
    }
    return 0;
}

//用户和好友功能
char* manage_role_name(int role)
{
    if (role == 2) {
        return "管理员";
    }
    if (role == 1) {
        return "VIP";
    }
    return "普通用户";
}

//用户注册
int manage_user_register(char* username, char* manage_passwd, int manage_age)
{
    users u = {0};

    if (username == NULL || manage_passwd == NULL || manage_age <= 0) {//先检查输入
        printf("注册信息无效。\n");
        return -1;
    }

    if (crud_find_user(username) != NULL) {
        printf("用户名已存在，请选择其他用户名。\n");
        return -2;
    }

    u.id = crud_next_id();//给新用户设置初始信息
    if (u.id < 0) {
        printf("系统初始化失败，注册失败。\n");
        return -3;
    }

    strncpy(u.username, username, sizeof(u.username) - 1);
    strncpy(u.passwd, manage_passwd, sizeof(u.passwd) - 1);
    u.age = manage_age;
    u.role = 0;
    u.coins = 100;
    u.friend_count = 0;
    u.asset_count = 0;

    if (crud_add_user(&u) == 0) {
        printf("注册成功！欢迎加入unisystem，%s！\n", username);
        printf("系统赠送 100 金币，祝你玩得开心！\n");
        return 0;
    }

    printf("注册失败，请稍后重试。\n");
    return -4;
}

//用户登录
int manage_user_login(char* username, char* manage_passwd)
{
    users* manage_target_user = crud_find_user(username);

    if (manage_target_user == NULL) {//先找有没有这个人
        return -1;
    }

    if (strcmp(manage_target_user->passwd, manage_passwd) != 0) {
        printf("密码错误。\n");
        return 1;
    }

    printf("登录成功！欢迎回来，%s！\n", username);
    return 0;
}

void manage_user_list_all(void)
{
    users manage_list[MAX_USERS];
    int manage_num;
    int i;

    manage_num = crud_load_users(manage_list, MAX_USERS);
    if (manage_num <= 0) {
        printf("当前没有用户数据。\n");
        return;
    }

    printf("\n用户列表：\n");
    printf("ID\t用户名\t年龄\t角色\t金币\n");
    for (i = 0; i < manage_num; i++) {
        printf("%d\t%s\t%d\t%s\t%d\n",
               manage_list[i].id,
               manage_list[i].username,
               manage_list[i].age,
               manage_role_name(manage_list[i].role),
               manage_list[i].coins);
    }
}

int manage_user_upgrade(void)
{
    char username[50];
    users manage_list[MAX_USERS];
    int manage_num;
    int manage_pos;

    printf("请输入要升级的用户名：");
    scanf("%49s", username);

    manage_num = crud_load_users(manage_list, MAX_USERS);
    if (manage_num <= 0) {
        printf("用户数据为空。\n");
        return -1;
    }

    manage_pos = manage_find_user_pos_by_name(manage_list, manage_num, username);
    if (manage_pos < 0) {
        printf("用户不存在。\n");
        return -2;
    }

    if (manage_list[manage_pos].role >= 2) {
        printf("该用户已经是最高等级。\n");
        return -3;
    }

    manage_list[manage_pos].role++;//把角色往上升一级
    if (crud_save_users(manage_list, manage_num) == 0) {
        printf("升级成功：%s 当前角色：%s\n", username, manage_role_name(manage_list[manage_pos].role));
        return 0;
    }

    printf("升级失败。\n");
    return -4;
}

int manage_user_degrade(void)
{
    char username[50];
    users manage_list[MAX_USERS];
    int manage_num;
    int manage_pos;

    printf("请输入要降级的用户名：");
    scanf("%49s", username);

    manage_num = crud_load_users(manage_list, MAX_USERS);
    if (manage_num <= 0) {
        printf("用户数据为空。\n");
        return -1;
    }

    manage_pos = manage_find_user_pos_by_name(manage_list, manage_num, username);
    if (manage_pos < 0) {
        printf("用户不存在。\n");
        return -2;
    }

    if (manage_list[manage_pos].role <= 0) {
        printf("该用户已经是最低等级。\n");
        return -3;
    }

    manage_list[manage_pos].role--;//把角色往下降一级
    if (crud_save_users(manage_list, manage_num) == 0) {
        printf("降级成功：%s 当前角色：%s\n", username, manage_role_name(manage_list[manage_pos].role));
        return 0;
    }

    printf("降级失败。\n");
    return -4;
}

int manage_user_delete(void)
{
    char username[50];
    users manage_list[MAX_USERS];
    int manage_num;
    int manage_pos;
    int manage_target_id;
    int i;

    printf("请输入要删除的用户名：");
    scanf("%49s", username);

    manage_num = crud_load_users(manage_list, MAX_USERS);
    if (manage_num <= 0) {
        printf("用户数据为空。\n");
        return -1;
    }

    manage_pos = manage_find_user_pos_by_name(manage_list, manage_num, username);
    if (manage_pos < 0) {
        printf("用户不存在。\n");
        return -2;
    }

    manage_target_id = manage_list[manage_pos].id;

    for (i = manage_pos; i < manage_num - 1; i++) {//先把这个用户从数组里删掉
        manage_list[i] = manage_list[i + 1];
    }
    manage_num--;

    for (i = 0; i < manage_num; i++) {//再把其他人的好友列表里对应的id也删掉
        int j;
        for (j = 0; j < manage_list[i].friend_count; j++) {
            if (manage_list[i].friends[j] == manage_target_id) {
                int k;
                for (k = j; k < manage_list[i].friend_count - 1; k++) {
                    manage_list[i].friends[k] = manage_list[i].friends[k + 1];
                }
                manage_list[i].friend_count--;
                j--;
            }
        }
    }

    if (crud_save_users(manage_list, manage_num) == 0) {
        printf("用户 %s 删除成功。\n", username);
        return 0;
    }

    printf("删除失败。\n");
    return -3;
}

void manage_user_show_info(char* username)
{
    users* u;

    if (username == NULL) {
        return;
    }

    u = crud_find_user(username);
    if (u == NULL) {
        printf("用户不存在。\n");
        return;
    }

    printf("\n个人信息：\n");
    printf("用户名：%s\n", u->username);
    printf("年龄：%d\n", u->age);
    printf("角色：%s\n", manage_role_name(u->role));
    printf("金币：%d\n", u->coins);
    printf("好友数：%d\n", u->friend_count);
    printf("资产数：%d\n", u->asset_count);
}

int manage_user_edit_info(char* username)
{
    users manage_list[MAX_USERS];
    int manage_num;
    int manage_pos;
    int manage_choice;

    if (username == NULL) {
        return -1;
    }

    manage_num = crud_load_users(manage_list, MAX_USERS);
    if (manage_num <= 0) {
        printf("用户数据为空。\n");
        return -2;
    }

    manage_pos = manage_find_user_pos_by_name(manage_list, manage_num, username);
    if (manage_pos < 0) {
        printf("当前用户不存在。\n");
        return -3;
    }

    printf("\n个人信息修改\n");
    printf("1. 修改密码\n");
    printf("2. 修改年龄\n");
    printf("3. 充值金币\n");
    printf("0. 返回\n");
    printf("请输入选择：");
    scanf("%d", &manage_choice);

    if (manage_choice == 1) {
        char manage_passwd[50];
        printf("请输入新密码：");
        scanf("%49s", manage_passwd);
        strncpy(manage_list[manage_pos].passwd, manage_passwd, sizeof(manage_list[manage_pos].passwd) - 1);
        manage_list[manage_pos].passwd[sizeof(manage_list[manage_pos].passwd) - 1] = '\0';
    } else if (manage_choice == 2) {
        int manage_age;
        printf("请输入新年龄：");
        scanf("%d", &manage_age);
        if (manage_age <= 0) {
            printf("年龄不合法。\n");
            return -4;
        }
        manage_list[manage_pos].age = manage_age;
    } else if (manage_choice == 3) {
        int manage_add_coins;
        printf("请输入充值金币数量：");
        scanf("%d", &manage_add_coins);
        if (manage_add_coins <= 0) {
            printf("充值数量不合法。\n");
            return -5;
        }
        manage_list[manage_pos].coins += manage_add_coins;
    } else if (manage_choice == 0) {
        return 0;
    } else {
        printf("无效选择。\n");
        return -6;
    }

    if (crud_save_users(manage_list, manage_num) == 0) {
        printf("信息修改成功。\n");
        return 0;
    }

    printf("信息修改失败。\n");
    return -7;
}

int manage_friend_add(char* username)
{
    char manage_friend_name[50];
    users manage_list[MAX_USERS];
    int manage_num;
    int manage_my_pos;
    int manage_friend_pos;

    if (username == NULL) {
        return -1;
    }

    printf("请输入要添加的好友用户名：");
    scanf("%49s", manage_friend_name);

    if (strcmp(username, manage_friend_name) == 0) {
        printf("不能添加自己为好友。\n");
        return -2;
    }

    manage_num = crud_load_users(manage_list, MAX_USERS);
    if (manage_num <= 0) {
        printf("用户数据为空。\n");
        return -3;
    }

    manage_my_pos = manage_find_user_pos_by_name(manage_list, manage_num, username);
    manage_friend_pos = manage_find_user_pos_by_name(manage_list, manage_num, manage_friend_name);

    if (manage_my_pos < 0 || manage_friend_pos < 0) {
        printf("好友不存在。\n");
        return -4;
    }

    if (manage_is_friend(&manage_list[manage_my_pos], manage_list[manage_friend_pos].id)) {
        printf("你们已经是好友了。\n");
        return -5;
    }

    if (manage_list[manage_my_pos].friend_count >= MAX_FRIENDS || manage_list[manage_friend_pos].friend_count >= MAX_FRIENDS) {
        printf("好友数量已达上限。\n");
        return -6;
    }

    manage_list[manage_my_pos].friends[manage_list[manage_my_pos].friend_count++] = manage_list[manage_friend_pos].id;//双方互相加到好友列表里
    manage_list[manage_friend_pos].friends[manage_list[manage_friend_pos].friend_count++] = manage_list[manage_my_pos].id;

    if (crud_save_users(manage_list, manage_num) == 0) {
        printf("添加好友成功：%s\n", manage_friend_name);
        return 0;
    }

    printf("添加好友失败。\n");
    return -7;
}

int manage_friend_remove(char* username)
{
    char manage_friend_name[50];
    users manage_list[MAX_USERS];
    int manage_num;
    int manage_my_pos;
    int manage_friend_pos;
    int i;

    if (username == NULL) {
        return -1;
    }

    printf("请输入要删除的好友用户名：");
    scanf("%49s", manage_friend_name);

    manage_num = crud_load_users(manage_list, MAX_USERS);
    if (manage_num <= 0) {
        printf("用户数据为空。\n");
        return -2;
    }

    manage_my_pos = manage_find_user_pos_by_name(manage_list, manage_num, username);
    manage_friend_pos = manage_find_user_pos_by_name(manage_list, manage_num, manage_friend_name);

    if (manage_my_pos < 0 || manage_friend_pos < 0) {
        printf("好友不存在。\n");
        return -3;
    }

    if (!manage_is_friend(&manage_list[manage_my_pos], manage_list[manage_friend_pos].id)) {
        printf("你们当前不是好友关系。\n");
        return -4;
    }

    for (i = 0; i < manage_list[manage_my_pos].friend_count; i++) {//先从自己这边删
        if (manage_list[manage_my_pos].friends[i] == manage_list[manage_friend_pos].id) {
            int j;
            for (j = i; j < manage_list[manage_my_pos].friend_count - 1; j++) {
                manage_list[manage_my_pos].friends[j] = manage_list[manage_my_pos].friends[j + 1];
            }
            manage_list[manage_my_pos].friend_count--;
            break;
        }
    }

    for (i = 0; i < manage_list[manage_friend_pos].friend_count; i++) {//再从对方那边删
        if (manage_list[manage_friend_pos].friends[i] == manage_list[manage_my_pos].id) {
            int j;
            for (j = i; j < manage_list[manage_friend_pos].friend_count - 1; j++) {
                manage_list[manage_friend_pos].friends[j] = manage_list[manage_friend_pos].friends[j + 1];
            }
            manage_list[manage_friend_pos].friend_count--;
            break;
        }
    }

    if (crud_save_users(manage_list, manage_num) == 0) {
        printf("删除好友成功：%s\n", manage_friend_name);
        return 0;
    }

    printf("删除好友失败。\n");
    return -5;
}

void manage_friend_list(char* username)
{
    users manage_list[MAX_USERS];
    int manage_num;
    int manage_my_pos;
    int i;

    if (username == NULL) {
        return;
    }

    manage_num = crud_load_users(manage_list, MAX_USERS);
    if (manage_num <= 0) {
        printf("用户数据为空。\n");
        return;
    }

    manage_my_pos = manage_find_user_pos_by_name(manage_list, manage_num, username);
    if (manage_my_pos < 0) {
        printf("当前用户不存在。\n");
        return;
    }

    if (manage_list[manage_my_pos].friend_count == 0) {
        printf("你还没有好友。\n");
        return;
    }

    printf("\n我的好友：\n");
    for (i = 0; i < manage_list[manage_my_pos].friend_count; i++) {
        int manage_pos = manage_find_user_pos_by_id(manage_list, manage_num, manage_list[manage_my_pos].friends[i]);
        if (manage_pos >= 0) {
            printf("- %s\n", manage_list[manage_pos].username);
        }
    }
}

void manage_show_recommend_games(char* manage_username)
{
    games manage_game_list[MAX_GAMES];
    users* manage_now_user;
    int manage_num;
    int i;
    int manage_shown = 0;

    if (manage_username == NULL) {
        return;
    }

    manage_now_user = crud_find_user(manage_username);
    if (manage_now_user == NULL) {
        printf("用户不存在。\n");
        return;
    }

    manage_num = crud_load_games(manage_game_list, MAX_GAMES);
    if (manage_num <= 0) {
        printf("当前没有可推荐的游戏数据。\n");
        return;
    }

    printf("\n推荐游戏（按年龄做简单推荐）：\n");//这里只做一个很简单的推荐
    for (i = 0; i < manage_num; i++) {
        int manage_matched = 0;
        if (manage_now_user->age < 18 && (manage_game_list[i].game_id % 2 == 0)) {
            manage_matched = 1;
        }
        if (manage_now_user->age >= 18 && (manage_game_list[i].game_id % 2 == 1)) {
            manage_matched = 1;
        }

        if (manage_matched) {
            printf("- %s：%s\n", manage_game_list[i].game_name, manage_game_list[i].game_desc);
            manage_shown++;
        }
    }

    if (manage_shown == 0) {
        for (i = 0; i < manage_num && i < 3; i++) {
            printf("- %s：%s\n", manage_game_list[i].game_name, manage_game_list[i].game_desc);
        }
    }
}

void manage_show_friend_menu(char* manage_username)
{
    int manage_choice;

    while (1) {
        printf("\n好友菜单\n");
        printf("1. 查看好友\n");
        printf("2. 添加好友\n");
        printf("3. 删除好友\n");
        printf("0. 返回\n");
        printf("请输入选择：");
        scanf("%d", &manage_choice);

        if (manage_choice == 1) {
            manage_friend_list(manage_username);
        } else if (manage_choice == 2) {
            manage_friend_add(manage_username);
        } else if (manage_choice == 3) {
            manage_friend_remove(manage_username);
        } else if (manage_choice == 0) {
            return;
        } else {
            printf("无效选择，请重新输入！\n");
        }
    }
}

void manage_show_user_info_menu(char* manage_username)
{
    int manage_choice;

    while (1) {
        printf("\n个人信息菜单\n");
        printf("1. 查看个人信息\n");
        printf("2. 修改个人信息\n");
        printf("0. 返回\n");
        printf("请输入选择：");
        scanf("%d", &manage_choice);

        if (manage_choice == 1) {
            manage_user_show_info(manage_username);
        } else if (manage_choice == 2) {
            manage_user_edit_info(manage_username);
        } else if (manage_choice == 0) {
            return;
        } else {
            printf("无效选择，请重新输入！\n");
        }
    }
}

void manage_show_admin_menu(void)
{
    int manage_choice;

    while (1) {
        printf("\n管理员菜单\n");
        printf("1. 查看所有用户\n");
        printf("2. 升级用户\n");
        printf("3. 降级用户\n");
        printf("4. 删除用户\n");
        printf("0. 返回\n");
        printf("请输入选择：");
        scanf("%d", &manage_choice);

        if (manage_choice == 1) {
            manage_user_list_all();
        } else if (manage_choice == 2) {
            manage_user_upgrade();
        } else if (manage_choice == 3) {
            manage_user_degrade();
        } else if (manage_choice == 4) {
            manage_user_delete();
        } else if (manage_choice == 0) {
            return;
        } else {
            printf("无效选择，请重新输入！\n");
        }
    }
}

//交易功能

void trade_show_market(void)
{
    market trade_items[MAX_MARKET_ITEMS];
    users trade_user_list[MAX_USERS];
    int trade_item_count;
    int trade_user_count;
    int i;

    trade_item_count = crud_load_market(trade_items, MAX_MARKET_ITEMS);
    trade_user_count = crud_load_users(trade_user_list, MAX_USERS);

    if (trade_item_count <= 0) {
        printf("市场暂无上架道具。\n");
        return;
    }

    printf("\n市场列表：\n");
    printf("道具ID\t名称\t价格\t卖家\t上架时间\n");
    for (i = 0; i < trade_item_count; i++) {//把市场里的内容列出来
        char* trade_seller_name = "未知卖家";
        int trade_seller_pos = manage_find_user_pos_by_id(trade_user_list, trade_user_count, trade_items[i].seller_id);
        if (trade_seller_pos >= 0) {
            trade_seller_name = trade_user_list[trade_seller_pos].username;
        }
        printf("%d\t%s\t%d\t%s\t%s\n",
               trade_items[i].item_id,
               trade_items[i].item_name,
               trade_items[i].item_price,
               trade_seller_name,
               trade_items[i].publish_time);
    }
}

int trade_publish_item(char* trade_username)
{
    users trade_user_list[MAX_USERS];
    int trade_user_count;
    int trade_my_pos;
    market trade_item = {0};
    char trade_item_name[64];
    int trade_item_price;
    time_t trade_now;
    struct tm* trade_tm_info;

    if (trade_username == NULL) {
        return -1;
    }

    trade_user_count = crud_load_users(trade_user_list, MAX_USERS);
    if (trade_user_count <= 0) {
        printf("用户数据为空。\n");
        return -2;
    }

    trade_my_pos = manage_find_user_pos_by_name(trade_user_list, trade_user_count, trade_username);
    if (trade_my_pos < 0) {
        printf("当前用户不存在。\n");
        return -3;
    }

    printf("请输入道具名称（不含空格）：");//输入要上架的道具信息
    scanf("%63s", trade_item_name);
    printf("请输入道具价格：");
    scanf("%d", &trade_item_price);

    if (trade_item_price <= 0) {
        printf("价格必须大于0。\n");
        return -4;
    }

    trade_item.item_id = crud_next_id();//给道具补上编号和卖家
    strncpy(trade_item.item_name, trade_item_name, sizeof(trade_item.item_name) - 1);
    trade_item.item_price = trade_item_price;
    trade_item.seller_id = trade_user_list[trade_my_pos].id;

    trade_now = time(NULL);
    trade_tm_info = localtime(&trade_now);
    if (trade_tm_info != NULL) {
        strftime(trade_item.publish_time, sizeof(trade_item.publish_time), "%Y-%m-%d", trade_tm_info);
    } else {
        strcpy(trade_item.publish_time, "unknown");
    }

    if (crud_add_market_item(&trade_item) != 0) {
        printf("上架失败。\n");
        return -5;
    }

    if (trade_user_list[trade_my_pos].asset_count < MAX_ASSETS) {
        trade_user_list[trade_my_pos].assets[trade_user_list[trade_my_pos].asset_count++] = trade_item.item_id;
        crud_save_users(trade_user_list, trade_user_count);
    }

    printf("上架成功！道具ID：%d\n", trade_item.item_id);
    return 0;
}

int trade_buy_item(char* trade_username)
{
    market trade_items[MAX_MARKET_ITEMS];
    users trade_user_list[MAX_USERS];
    int trade_item_count;
    int trade_user_count;
    int trade_buyer_pos;
    int trade_target_id;
    int trade_item_pos = -1;
    int trade_seller_pos;
    int trade_bought_price;
    int i;

    if (trade_username == NULL) {
        return -1;
    }

    trade_item_count = crud_load_market(trade_items, MAX_MARKET_ITEMS);
    if (trade_item_count <= 0) {
        printf("市场暂无上架道具。\n");
        return -2;
    }

    trade_user_count = crud_load_users(trade_user_list, MAX_USERS);
    if (trade_user_count <= 0) {
        printf("用户数据为空。\n");
        return -3;
    }

    trade_buyer_pos = manage_find_user_pos_by_name(trade_user_list, trade_user_count, trade_username);
    if (trade_buyer_pos < 0) {
        printf("当前用户不存在。\n");
        return -4;
    }

    trade_show_market();//先显示市场再让用户输入
    printf("请输入要购买的道具ID：");
    scanf("%d", &trade_target_id);

    for (i = 0; i < trade_item_count; i++) {//找到要买的是哪一个
        if (trade_items[i].item_id == trade_target_id) {
            trade_item_pos = i;
            break;
        }
    }

    if (trade_item_pos < 0) {
        printf("未找到该道具。\n");
        return -5;
    }

    if (trade_items[trade_item_pos].seller_id == trade_user_list[trade_buyer_pos].id) {
        printf("不能购买自己上架的道具。\n");
        return -6;
    }

    if (trade_user_list[trade_buyer_pos].coins < trade_items[trade_item_pos].item_price) {
        printf("金币不足，购买失败。\n");
        return -7;
    }

    trade_seller_pos = manage_find_user_pos_by_id(trade_user_list, trade_user_count, trade_items[trade_item_pos].seller_id);
    trade_bought_price = trade_items[trade_item_pos].item_price;

    trade_user_list[trade_buyer_pos].coins -= trade_bought_price;//买家扣钱，卖家加钱
    if (trade_seller_pos >= 0) {
        trade_user_list[trade_seller_pos].coins += trade_bought_price;
    }

    if (trade_user_list[trade_buyer_pos].asset_count < MAX_ASSETS) {
        trade_user_list[trade_buyer_pos].assets[trade_user_list[trade_buyer_pos].asset_count++] = trade_items[trade_item_pos].item_id;
    }

    for (i = trade_item_pos; i < trade_item_count - 1; i++) {//买走后从市场里删掉
        trade_items[i] = trade_items[i + 1];
    }
    trade_item_count--;

    if (crud_save_users(trade_user_list, trade_user_count) != 0) {
        printf("用户数据保存失败。\n");
        return -8;
    }

    if (crud_save_market(trade_items, trade_item_count) != 0) {
        printf("市场数据保存失败。\n");
        return -9;
    }

    printf("购买成功！你花费了 %d 金币。\n", trade_bought_price);
    return 0;
}

void trade_menu(char* trade_username)
{
    int trade_choice;

    while (1) {
        printf("\n交易菜单\n");
        printf("1. 查看市场\n");
        printf("2. 发布道具\n");
        printf("3. 购买道具\n");
        printf("0. 返回上级菜单\n");
        printf("请输入选择：");
        scanf("%d", &trade_choice);

        if (trade_choice == 1) {
            trade_show_market();
        } else if (trade_choice == 2) {
            trade_publish_item(trade_username);
        } else if (trade_choice == 3) {
            trade_buy_item(trade_username);
        } else if (trade_choice == 0) {
            return;
        } else {
            printf("无效选择，请重新输入。\n");
        }
    }
}

//菜单和主程序
int main_show_menu(int main_role)
{
    char* main_title = "普通用户菜单";

    if (main_role == 1) {//不同身份显示不同菜单标题
        main_title = "VIP用户菜单";
    } else if (main_role == 2) {
        main_title = "管理员菜单";
    }

    printf("\n+--------------------------------------+\n");
    printf("|            %s              |\n", main_title);
    printf("+--------------------------------------+\n");

    if (main_role == 0) {
        printf("| 1. 游戏排行        2. 推荐游戏      |\n");
        printf("| 3. 交易道具        4. 我的好友      |\n");
        printf("| 5. 个人信息        6. 退出系统      |\n");
    }
    else if (main_role == 1) {
        printf("| 1. 游戏排行        2. 推荐游戏      |\n");
        printf("| 3. 交易道具        4. 我的好友      |\n");
        printf("| 5. 个人信息        6. 退出系统      |\n");
    }
    else if (main_role == 2) {
        printf("| 1. 游戏排行        2. 推荐游戏      |\n");
        printf("| 3. 交易道具        4. 我的好友      |\n");
        printf("| 5. 个人信息        6. 用户管理      |\n");
        printf("| 7. 退出系统                        |\n");
    }
    printf("+--------------------------------------+\n");
    printf("请输入您的选择：");

    int main_choice;
    scanf("%d", &main_choice);
    return main_choice;
}

int main_first_register(char main_username[50])
{
    int main_age;
    char main_passwd[50];

    printf("请输入您的年龄：");//第一次登录就当作注册
    scanf("%d", &main_age);

    while (main_age <= 0) {
        printf("年龄有误，请重新输入：");
        scanf("%d", &main_age);
    }

    printf("请输入您的密码：");
    scanf("%49s", main_passwd);

    crud_init();
    manage_user_register(main_username, main_passwd, main_age);

    printf("这是一个基于C语言的游戏推荐和交易系统。\n");
    printf("请按照提示进行操作，享受您的unisystem之旅！\n");

    return 0;
}

void main_show_ranking()
{
    users main_list[MAX_USERS];
    int main_num;
    int i;

    main_num = crud_load_users(main_list, MAX_USERS);
    if (main_num <= 0) {
        printf("暂无用户数据，无法显示排行。\n");
        return;
    }

    for (i = 0; i < main_num - 1; i++) {//用简单冒泡排序按金币排
        int j;
        for (j = 0; j < main_num - 1 - i; j++) {
            if (main_list[j].coins < main_list[j + 1].coins) {
                users temp = main_list[j];
                main_list[j] = main_list[j + 1];
                main_list[j + 1] = temp;
            }
        }
    }

    printf("\n金币排行榜：\n");
    printf("名次\t用户名\t金币\n");
    for (i = 0; i < main_num && i < 10; i++) {
        printf("%d\t%s\t%d\n", i + 1, main_list[i].username, main_list[i].coins);
    }
    return;
}

int main()
{
    char main_username[50];
    char main_passwd[50];

    crud_init();//程序启动先准备数据文件

    printf("欢迎来到unisystem，请输入您的用户名：");
    scanf("%49s", main_username);

    if (crud_find_user(main_username) != NULL) {//有账号就登录，没有就注册
        printf("请输入您的密码：");
        scanf("%49s", main_passwd);
        while (manage_user_login(main_username, main_passwd) != 0) {
            printf("\n请重新输入您的密码：");
            scanf("%49s", main_passwd);
        }
    }
    else {
        main_first_register(main_username);
    }

    while (1)//进入主菜单循环
    {
        int main_role = crud_find_user(main_username)->role;
        switch (main_show_menu(main_role))
        {
        case 1:
            main_show_ranking();
            break;
        case 2:
            manage_show_recommend_games(main_username);
            break;
        case 3:
            trade_menu(main_username);
            break;
        case 4:
            manage_show_friend_menu(main_username);
            break;
        case 5:
            manage_show_user_info_menu(main_username);
            break;
        case 6:
            if (main_role == 2) {
                manage_show_admin_menu();
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

