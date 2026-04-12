#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crud.h"

#if defined(_WIN32)
#include <direct.h>
#define MAKE_DIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MAKE_DIR(path) mkdir(path, 0777)
#endif

#define USER_FILE "data/users.jsonl"
#define MARKET_FILE "data/market.jsonl"
#define GAME_FILE "data/games.jsonl"
#define ID_FILE "data/id"

static void parse_int_list(const char* text, int* arr, int* count, int max_count)
{
    char buffer[512];
    char* token;

    *count = 0;
    if (text == NULL || text[0] == '\0') {
        return;
    }

    strncpy(buffer, text, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    token = strtok(buffer, ",");
    while (token != NULL && *count < max_count) {
        while (*token == ' ') {
            token++;
        }
        if (*token != '\0') {
            arr[*count] = atoi(token);
            (*count)++;
        }
        token = strtok(NULL, ",");
    }
}

static void build_int_list(const int* arr, int count, char* out, int out_size)
{
    int i;
    out[0] = '\0';

    for (i = 0; i < count; i++) {
        char part[32];
        int written;

        if (i > 0) {
            if ((int)strlen(out) + 1 >= out_size) {
                break;
            }
            strcat(out, ",");
        }

        written = snprintf(part, sizeof(part), "%d", arr[i]);
        if (written <= 0) {
            continue;
        }

        if ((int)strlen(out) + written >= out_size) {
            break;
        }
        strcat(out, part);
    }
}

static void extract_json_string_field(const char* line, const char* key, char* out, int out_size)
{
    const char* start;
    const char* end;
    int len;

    out[0] = '\0';
    start = strstr(line, key);
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

static void ensure_data_dir(void)
{
    MAKE_DIR("data");
}

static void ensure_file_exists(const char* path, const char* default_content)
{
    FILE* fp = fopen(path, "r");
    if (fp != NULL) {
        fclose(fp);
        return;
    }

    fp = fopen(path, "w");
    if (fp == NULL) {
        return;
    }

    if (default_content != NULL) {
        fputs(default_content, fp);
    }
    fclose(fp);
}

static void ensure_default_games(void)
{
    FILE* fp = fopen(GAME_FILE, "r");
    int ch;
    int has_content = 0;

    if (fp == NULL) {
        return;
    }

    while ((ch = fgetc(fp)) != EOF) {
        if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t') {
            has_content = 1;
            break;
        }
    }
    fclose(fp);

    if (has_content) {
        return;
    }

    fp = fopen(GAME_FILE, "w");
    if (fp == NULL) {
        return;
    }

    fputs("{\"game_id\": 1, \"game_name\": \"Sky War\", \"game_desc\": \"快节奏多人竞技射击游戏\"}\n", fp);
    fputs("{\"game_id\": 2, \"game_name\": \"Farm World\", \"game_desc\": \"休闲经营类种田社交游戏\"}\n", fp);
    fputs("{\"game_id\": 3, \"game_name\": \"Magic Chess\", \"game_desc\": \"策略回合制卡牌对战游戏\"}\n", fp);
    fputs("{\"game_id\": 4, \"game_name\": \"Hero Road\", \"game_desc\": \"剧情向角色扮演冒险游戏\"}\n", fp);
    fclose(fp);
}

int id(void)
{
    FILE* fp;
    int current_id = 0;

    crud_init();

    fp = fopen(ID_FILE, "r");
    if (fp != NULL) {
        if (fscanf(fp, "%d", &current_id) != 1) {
            current_id = 0;
        }
        fclose(fp);
    }

    current_id++;

    fp = fopen(ID_FILE, "w");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "%d", current_id);
    fclose(fp);

    return current_id;
}

void crud_init(void)
{
    ensure_data_dir();
    ensure_file_exists(USER_FILE, "");
    ensure_file_exists(MARKET_FILE, "");
    ensure_file_exists(GAME_FILE, "");
    ensure_file_exists(ID_FILE, "0");
    ensure_default_games();
}

int crud_create_line(int option, const char* json_line)
{
    const char* path;
    FILE* fp;

    switch (option) {
    case 1:
        path = USER_FILE;
        break;
    case 2:
        path = MARKET_FILE;
        break;
    case 3:
        path = GAME_FILE;
        break;
    default:
        return -2;
    }

    fp = fopen(path, "a");
    if (fp == NULL) {
        return -3;
    }

    fprintf(fp, "%s\n", json_line);
    fclose(fp);
    return 0;
}

int crud_parse_user_line(const char* line, users* out_user)
{
    char assets_text[512] = "";
    char friends_text[512] = "";
    int matched;

    if (line == NULL || out_user == NULL) {
        return -1;
    }

    memset(out_user, 0, sizeof(users));

    matched = sscanf(line,
                     "{\"id\": %d, \"username\": \"%49[^\"]\", \"passwd\": \"%49[^\"]\", \"age\": %d, \"role\": %d, \"coins\": %d",
                     &out_user->id,
                     out_user->username,
                     out_user->passwd,
                     &out_user->age,
                     &out_user->role,
                     &out_user->coins);

    if (matched != 6) {
        return -1;
    }

    extract_json_string_field(line, "\"assets\": \"", assets_text, sizeof(assets_text));
    extract_json_string_field(line, "\"friends\": \"", friends_text, sizeof(friends_text));

    parse_int_list(assets_text, out_user->assets, &out_user->asset_count, MAX_ASSETS);
    parse_int_list(friends_text, out_user->friends, &out_user->friend_count, MAX_FRIENDS);
    return 0;
}

int crud_user_to_json(const users* u, char* out_line, int out_size)
{
    char assets_text[512];
    char friends_text[512];
    int written;

    if (u == NULL || out_line == NULL || out_size <= 0) {
        return -1;
    }

    build_int_list(u->assets, u->asset_count, assets_text, sizeof(assets_text));
    build_int_list(u->friends, u->friend_count, friends_text, sizeof(friends_text));

    written = snprintf(out_line,
                       out_size,
                       "{\"id\": %d, \"username\": \"%s\", \"passwd\": \"%s\", \"age\": %d, \"role\": %d, \"coins\": %d, \"assets\": \"%s\", \"friends\": \"%s\"}",
                       u->id,
                       u->username,
                       u->passwd,
                       u->age,
                       u->role,
                       u->coins,
                       assets_text,
                       friends_text);

    if (written < 0 || written >= out_size) {
        return -2;
    }
    return 0;
}

int crud_load_users(users* arr, int max_count)
{
    FILE* fp;
    char line[2048];
    int count = 0;

    if (arr == NULL || max_count <= 0) {
        return -1;
    }

    crud_init();

    fp = fopen(USER_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        users u;

        if (crud_parse_user_line(line, &u) == 0) {
            if (count < max_count) {
                arr[count++] = u;
            }
        }
    }

    fclose(fp);
    return count;
}

int crud_save_users(const users* arr, int count)
{
    FILE* fp;
    int i;

    crud_init();

    fp = fopen(USER_FILE, "w");
    if (fp == NULL) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        char line[2048];

        if (crud_user_to_json(&arr[i], line, sizeof(line)) == 0) {
            fprintf(fp, "%s\n", line);
        }
    }

    fclose(fp);
    return 0;
}

users* crud_search_user(const char* username)
{
    static users found;
    users arr[MAX_USERS];
    int count;
    int i;

    if (username == NULL) {
        return NULL;
    }

    count = crud_load_users(arr, MAX_USERS);
    if (count < 0) {
        return NULL;
    }

    for (i = 0; i < count; i++) {
        if (strcmp(arr[i].username, username) == 0) {
            found = arr[i];
            return &found;
        }
    }

    return NULL;
}

int crud_create_user(const users* u)
{
    char line[2048];

    if (u == NULL) {
        return -1;
    }
    if (crud_user_to_json(u, line, sizeof(line)) != 0) {
        return -2;
    }

    return crud_create_line(1, line);
}

int crud_update_user(const users* u)
{
    users arr[MAX_USERS];
    int count;
    int i;

    if (u == NULL) {
        return -1;
    }

    count = crud_load_users(arr, MAX_USERS);
    if (count < 0) {
        return -2;
    }

    for (i = 0; i < count; i++) {
        if (arr[i].id == u->id || strcmp(arr[i].username, u->username) == 0) {
            arr[i] = *u;
            return crud_save_users(arr, count);
        }
    }

    return -3;
}

int crud_delete_user(const char* username)
{
    users arr[MAX_USERS];
    int count;
    int i;

    if (username == NULL) {
        return -1;
    }

    count = crud_load_users(arr, MAX_USERS);
    if (count < 0) {
        return -2;
    }

    for (i = 0; i < count; i++) {
        if (strcmp(arr[i].username, username) == 0) {
            int j;
            for (j = i; j < count - 1; j++) {
                arr[j] = arr[j + 1];
            }
            count--;
            return crud_save_users(arr, count);
        }
    }

    return -3;
}

int crud_edit_user(const char* username, const char* item)
{
    users* u;
    users updated;

    if (username == NULL || item == NULL) {
        return -1;
    }

    u = crud_search_user(username);
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

int crud_load_games(games* arr, int max_count)
{
    FILE* fp;
    char line[2048];
    int count = 0;

    if (arr == NULL || max_count <= 0) {
        return -1;
    }

    crud_init();

    fp = fopen(GAME_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        games g = {0};
        int matched = sscanf(line,
                             "{\"game_id\": %d, \"game_name\": \"%63[^\"]\", \"game_desc\": \"%255[^\"]\"}",
                             &g.game_id,
                             g.game_name,
                             g.game_desc);
        if (matched == 3 && count < max_count) {
            arr[count++] = g;
        }
    }

    fclose(fp);
    return count;
}

int crud_load_market(market* arr, int max_count)
{
    FILE* fp;
    char line[1024];
    int count = 0;

    if (arr == NULL || max_count <= 0) {
        return -1;
    }

    crud_init();

    fp = fopen(MARKET_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        market m = {0};
        int matched = sscanf(line,
                             "{\"item_id\": %d, \"item_name\": \"%63[^\"]\", \"item_price\": %d, \"seller_id\": %d, \"publish_time\": \"%31[^\"]\"}",
                             &m.item_id,
                             m.item_name,
                             &m.item_price,
                             &m.seller_id,
                             m.publish_time);

        if (matched != 5) {
            matched = sscanf(line,
                             "{\"item_id\": %d, \"item_price\": %d, \"publish_time\": \"%31[^\"]\"}",
                             &m.item_id,
                             &m.item_price,
                             m.publish_time);
            if (matched == 3) {
                strcpy(m.item_name, "未知道具");
                m.seller_id = 0;
            }
        }

        if ((matched == 5 || matched == 3) && count < max_count) {
            arr[count++] = m;
        }
    }

    fclose(fp);
    return count;
}

int crud_save_market(const market* arr, int count)
{
    FILE* fp;
    int i;

    crud_init();

    fp = fopen(MARKET_FILE, "w");
    if (fp == NULL) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        fprintf(fp,
                "{\"item_id\": %d, \"item_name\": \"%s\", \"item_price\": %d, \"seller_id\": %d, \"publish_time\": \"%s\"}\n",
                arr[i].item_id,
                arr[i].item_name,
                arr[i].item_price,
                arr[i].seller_id,
                arr[i].publish_time);
    }

    fclose(fp);
    return 0;
}

int crud_append_market_item(const market* item)
{
    char line[1024];

    if (item == NULL) {
        return -1;
    }

    snprintf(line,
             sizeof(line),
             "{\"item_id\": %d, \"item_name\": \"%s\", \"item_price\": %d, \"seller_id\": %d, \"publish_time\": \"%s\"}",
             item->item_id,
             item->item_name,
             item->item_price,
             item->seller_id,
             item->publish_time);

    return crud_create_line(2, line);
}