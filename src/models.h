#ifndef MODELS_H
#define MODELS_H

typedef struct {
    char username[50];
    char passwd[50];
    int id;
    int age;
    int role;            // 0: 普通用户, 1: VIP, 2: 管理员
    int coins;           // 持有金钱
    int friends[1000];   // 好友ID列表
    int assets[1000];    // 物品ID列表
} users;

typedef struct {
    int item_id;
    int item_price;
    char publish_time[32];
} market;

typedef struct {
    int game_id;
    char game_name[64];
    char game_desc[256];
} games;

#endif // MODELS_H
