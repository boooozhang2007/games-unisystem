#ifndef MODELS_H
#define MODELS_H

#define MAX_USERS 500
#define MAX_FRIENDS 100
#define MAX_ASSETS 100
#define MAX_GAMES 200
#define MAX_MARKET_ITEMS 500

typedef struct {
    char username[50];
    char passwd[50];
    int id;
    int age;
    int role;            // 0: 普通用户, 1: VIP, 2: 管理员
    int coins;           // 持有金钱
    int friend_count;
    int asset_count;
    int friends[MAX_FRIENDS];   // 好友ID列表
    int assets[MAX_ASSETS];     // 物品ID列表
} users;

typedef struct {
    int item_id;
    char item_name[64];
    int item_price;
    int seller_id;
    char publish_time[32];
} market;

typedef struct {
    int game_id;
    char game_name[64];
    char game_desc[256];
} games;

#endif // MODELS_H
