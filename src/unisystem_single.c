#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//数量上限
#define MAX_USERS 500 //最多能存500个用户
#define MAX_GAMES 100 //最多能存100个游戏
#define MAX_MARKET 200 //市场里最多200个道具
#define MAX_FRIENDS 100 //每个人最多100个好友
#define MAX_ASSETS 100 //每个人最多拥有100个资产

//数据文件
#define USER_FILE "users.dat" //用户数据
#define GAME_FILE "games.dat" //游戏数据
#define MARKET_FILE "market.dat" //市场道具数据
#define TRADE_LOG_FILE "trade_log.dat" //交易记录
#define INVITE_LOG_FILE "invite_log.dat" //好友邀约记录
#define ID_FILE "id.dat" //自增编号

//用户资料
typedef struct { //定义一个结构体，用来装用户的所有信息
    int id; //用户的编号
    char username[50]; //用户名
    char passwd[50]; //密码
    int age; //年龄
    char identity[30]; //身份或者职业，比如学生、老师
    char hobby[30]; //爱好，比如竞技、休闲
    int role;//0普通用户 1VIP 2管理员
    int coins; //金币数量
    int game_hours; //游戏时长
    int play_count; //总对局数
    int win_count; //赢的局数
    int friend_count; //好友数量
    int asset_count; //资产数量
    int friends[MAX_FRIENDS]; //好友列表
    int assets[MAX_ASSETS]; //资产列表
} users;

//游戏资料
typedef struct {
    int game_id; //游戏编号
    char game_name[50]; //游戏名称
    char game_type[30]; //游戏类型，比如竞技、冒险
    char game_desc[100]; //游戏简介
    int need_age; //最低年龄要求
} games;

//市场资料
typedef struct {
    int item_id; //道具编号
    char item_name[50]; //道具名称
    char item_type[30]; //道具类型
    int item_price; //道具价格
    int seller_id; //卖家的用户id
} market;

//交易记录
typedef struct {
    int buyer_id; //买家的用户id
    int seller_id; //卖家的用户id
    char buyer_name[50]; //买家用户名
    char seller_name[50]; //卖家用户名
    int item_id; //交易的道具编号
    char item_name[50]; //交易的道具名称
    char item_type[30]; //交易的道具类型
    int item_price; //交易价格
} trade_log;

//好友邀约记录
typedef struct {
    int send_id; //发起邀约的人的id
    int recv_id; //被邀约的人的id
    char send_name[50]; //发起人的用户名
    char recv_name[50]; //被邀约人的用户名
    char game_name[50]; //邀约玩的游戏名称
} invite_log;

//crud部分
void crud_init(void); //初始化函数
void crud_write_default_games(void); //写默认游戏数据
int crud_next_id(void); //获取下一个编号
int crud_load_users(users list[]); //读取所有用户
void crud_save_users(users list[], int num); //保存所有用户
int crud_load_games(games list[]); //读取所有游戏
int crud_load_market(market list[]); //读取市场道具
void crud_save_market(market list[], int num); //保存市场道具
int crud_load_trade_logs(trade_log list[]); //读取交易记录
void crud_add_trade_log(trade_log* log_data); //添加交易记录
int crud_load_invite_logs(invite_log list[]); //读取邀约记录
void crud_add_invite_log(invite_log* log_data); //添加邀约记录
int crud_find_user_index(users list[], int num, char username[]); //按用户名找位置
int crud_find_user_id_index(users list[], int num, int id); //按id找位置
int crud_find_market_index(market list[], int num, int item_id); //按道具id找位置

//manage部分
void manage_create_first_admin(void); //创建默认管理员
int manage_user_register(users* user); //用户注册
int manage_user_login(char username[], char passwd[]); //用户登录
int manage_get_win_rate(users* user); //计算胜率
void manage_user_auto_upgrade(users* user); //自动升级
void manage_user_show_info(char username[]); //显示用户信息
int manage_user_edit_info(char username[]); //修改用户信息
void manage_user_list_all(void); //列出所有用户
int manage_user_upgrade(void); //手动升级用户
int manage_user_degrade(void); //手动降级用户
int manage_user_delete(void); //删除用户
int manage_friend_add(char username[]); //添加好友
int manage_friend_remove(char username[]); //删除好友
void manage_friend_list(char username[]); //查看好友列表
int manage_friend_invite(char username[]); //好友邀约
void manage_friend_show_invite(char username[]); //查看邀约记录
void manage_show_recommend_games(char username[]); //推荐游戏
void manage_show_friend_menu(char username[]); //好友菜单
void manage_show_user_info_menu(char username[]); //个人信息菜单
void manage_show_admin_menu(void); //管理员菜单

//trade部分
void trade_show_market(void); //展示市场
void trade_show_log(char username[]); //展示交易记录
int trade_publish_item(char username[]); //上架道具
int trade_buy_item(char username[]); //购买道具
void trade_menu(char username[]); //交易菜单

//main部分
int main_show_menu(int role); //主菜单
void main_show_ranking(void); //排行榜
void main_first_register(char username[]); //第一次注册

//初始化函数
void crud_init(void)
{
    FILE* fp;
    int id_value = 0; //初始编号设成0

    fp = fopen(USER_FILE, "ab"); //用追加模式打开用户文件，文件不存在会自动创建
    if (fp != NULL) fclose(fp);

    fp = fopen(MARKET_FILE, "ab"); //同样的方式打开市场文件
    if (fp != NULL) fclose(fp);

    fp = fopen(TRADE_LOG_FILE, "ab"); //打开交易记录文件
    if (fp != NULL) fclose(fp);

    fp = fopen(INVITE_LOG_FILE, "ab"); //打开邀约记录文件
    if (fp != NULL) fclose(fp);

    fp = fopen(GAME_FILE, "ab"); //打开游戏数据文件
    if (fp != NULL) fclose(fp);

    fp = fopen(ID_FILE, "rb"); //试着用读模式打开编号文件
    if (fp == NULL) { //如果打开失败说明文件不存在
        fp = fopen(ID_FILE, "wb"); //那就用写模式创建一个新的
        if (fp != NULL) { //如果创建成功了
            fwrite(&id_value, sizeof(int), 1, fp); //把初始编号0写进去
            fclose(fp);
        }
    } else { //如果文件已经存在了
        fclose(fp); //那就直接关掉不用管了
    }


    crud_write_default_games(); //调用函数写入默认的游戏数据
}

//写默认游戏数据
void crud_write_default_games(void)
{
    FILE* fp;
    long size;
    games list[6] = { //预设6款游戏的数据
        {1, "王者荣耀", "竞技", "5V5多人在线竞技游戏", 12}, //王者荣耀，竞技类，12岁以上
        {2, "原神", "冒险", "开放世界角色扮演游戏", 12}, //原神，冒险类
        {3, "第五人格", "竞技", "非对称对抗竞技游戏", 12}, //第五人格，竞技类
        {4, "和平精英", "竞技", "多人战术射击游戏", 12}, //和平精英，竞技类
        {5, "蛋仔派对", "休闲", "多人闯关休闲派对游戏", 6}, //蛋仔派对，休闲类，6岁以上
        {6, "阴阳师", "养成", "和风卡牌养成策略游戏", 12} //阴阳师，养成类
    };
    int num; //记录游戏数量

    fp = fopen(GAME_FILE, "rb"); //用读模式打开游戏文件
    if (fp == NULL) return;

    fseek(fp, 0, SEEK_END); //把文件指针移到文件末尾
    size = ftell(fp); //看看现在指针在哪，就知道文件多大了
    fclose(fp);

    if (size > 0) return;

    fp = fopen(GAME_FILE, "wb"); //用写模式打开游戏文件
    if (fp == NULL) return;

    num = 6; //一共6个游戏
    fwrite(list, sizeof(games), num, fp); //把所有游戏数据一次性写进文件
    fclose(fp);
}

//生成新的编号
int crud_next_id(void)
{
    FILE* fp;
    int id_value = 0; //先给编号一个初始值0

    fp = fopen(ID_FILE, "rb"); //用读模式打开编号文件
    if (fp == NULL) return -1;

    if (fread(&id_value, sizeof(int), 1, fp) != 1) { //试着从文件读出当前编号
        id_value = 0; //读失败就当成0
    }
    fclose(fp);

    id_value++; //编号加1，这样每次调用编号都不一样

    fp = fopen(ID_FILE, "wb"); //用写模式打开编号文件
    if (fp == NULL) return -1;

    fwrite(&id_value, sizeof(int), 1, fp); //把新编号写回文件
    fclose(fp);

    return id_value; //返回这个新编号
}

//加载用户数据，返回读取到的用户数量
int crud_load_users(users list[])
{
    FILE* fp;
    int num = 0; //计数器，记录读了几个用户
    users one_user; //临时变量，每次读一个用户先放这里

    fp = fopen(USER_FILE, "rb"); //用读模式打开用户文件
    if (fp == NULL) return 0;

    while (fread(&one_user, sizeof(users), 1, fp) == 1) { //一个一个读用户，读成功就继续
        if (num < MAX_USERS) { //只要还没超过上限
            list[num] = one_user; //就把这个用户放到数组里
            num++; //计数加1
        }
    }

    fclose(fp);
    return num; //返回读取到的用户数量
}

//把用户数组重新写回文件
void crud_save_users(users list[], int num)
{
    FILE* fp; //文件指针

    fp = fopen(USER_FILE, "wb"); //用写模式打开用户文件，会覆盖原来的内容
    if (fp == NULL) return; //打不开就不保存了

    if (num > 0) { //如果确实有用户要保存
        fwrite(list, sizeof(users), num, fp); //把整个数组写进文件
    }

    fclose(fp); //关文件
}

//读取全部游戏数据
int crud_load_games(games list[])
{
    FILE* fp; //文件指针
    int num = 0; //计数器
    games one_game; //临时变量存一个游戏

    fp = fopen(GAME_FILE, "rb"); //打开游戏文件
    if (fp == NULL) return 0; //打不开就返回0

    while (fread(&one_game, sizeof(games), 1, fp) == 1) { //循环读游戏
        if (num < MAX_GAMES) { //没超上限就
            list[num] = one_game; //放进数组
            num++; //计数加1
        }
    }

    fclose(fp); //关文件
    return num; //返回读了几个
}

//读取市场里的全部道具
int crud_load_market(market list[])
{
    FILE* fp; //文件指针
    int num = 0; //计数器
    market one_item; //临时变量存一个道具

    fp = fopen(MARKET_FILE, "rb"); //打开市场数据文件
    if (fp == NULL) return 0; //打不开就返回0

    while (fread(&one_item, sizeof(market), 1, fp) == 1) { //循环读道具
        if (num < MAX_MARKET) { //没超限
            list[num] = one_item; //放进数组
            num++; //加1
        }
    }

    fclose(fp); //关文件
    return num; //返回数量
}

//读取交易记录
int crud_load_trade_logs(trade_log list[])
{
    FILE* fp; //文件指针
    int num = 0; //计数器
    trade_log log_data; //临时变量存一条交易记录

    fp = fopen(TRADE_LOG_FILE, "rb"); //打开交易记录文件
    if (fp == NULL) return 0; //打不开就返回0

    while (fread(&log_data, sizeof(trade_log), 1, fp) == 1) { //循环读记录
        if (num < MAX_MARKET) { //没超限
            list[num] = log_data; //放进数组
            num++; //加1
        }
    }

    fclose(fp); //关文件
    return num; //返回数量
}

//读取好友邀约记录
int crud_load_invite_logs(invite_log list[])
{
    FILE* fp; //文件指针
    int num = 0; //计数器
    invite_log log_data; //临时变量存一条邀约记录

    fp = fopen(INVITE_LOG_FILE, "rb"); //打开邀约记录文件
    if (fp == NULL) return 0; //打不开就返回0

    while (fread(&log_data, sizeof(invite_log), 1, fp) == 1) { //循环读记录
        if (num < MAX_MARKET) { //没超限
            list[num] = log_data; //放进数组
            num++; //加1
        }
    }

    fclose(fp); //关文件
    return num; //返回数量
}

//把市场数组重新保存到文件里
void crud_save_market(market list[], int num)
{
    FILE* fp; //文件指针

    fp = fopen(MARKET_FILE, "wb"); //用写模式打开市场文件
    if (fp == NULL) return; //打不开就不保存了

    if (num > 0) { //有道具要保存的话
        fwrite(list, sizeof(market), num, fp); //一次性写入
    }

    fclose(fp); //关文件
}

//追加一条交易记录
void crud_add_trade_log(trade_log* log_data)
{
    FILE* fp; //文件指针

    fp = fopen(TRADE_LOG_FILE, "ab"); //用追加模式打开，这样不会覆盖之前的记录
    if (fp == NULL) return; //打不开就返回

    fwrite(log_data, sizeof(trade_log), 1, fp); //把这条记录追加到文件末尾
    fclose(fp); //关文件
}

//追加一条好友邀约记录
void crud_add_invite_log(invite_log* log_data)
{
    FILE* fp; //文件指针

    fp = fopen(INVITE_LOG_FILE, "ab"); //追加模式打开邀约记录文件
    if (fp == NULL) return; //打不开就返回

    fwrite(log_data, sizeof(invite_log), 1, fp); //写一条记录进去
    fclose(fp); //关文件
}

//按用户名在数组里找用户位置
int crud_find_user_index(users list[], int num, char username[])
{
    int i; //循环变量

    for (i = 0; i < num; i++) { //从头到尾遍历每个用户
        if (strcmp(list[i].username, username) == 0) { //如果用户名和要找的一样
            return i; //返回这个位置
        }
    }

    return -1; //循环结束都没找到，返回-1表示没有
}

//按用户编号查找位置
int crud_find_user_id_index(users list[], int num, int id)
{
    int i; //循环变量

    for (i = 0; i < num; i++) { //遍历所有用户
        if (list[i].id == id) { //找到编号一样的
            return i; //返回下标
        }
    }

    return -1; //没找到返回-1
}

//按道具ID查市场里的位置
int crud_find_market_index(market list[], int num, int item_id)
{
    int i; //循环变量

    for (i = 0; i < num; i++) { //遍历市场里所有道具
        if (list[i].item_id == item_id) { //找到道具ID匹配的
            return i; //返回位置
        }
    }

    return -1; //没找到返回-1
}

//如果系统里还没有任何用户，就创建一个默认管理员
void manage_create_first_admin(void)
{
    users list[MAX_USERS]; //声明一个用户数组
    int num; //用户数量
    users admin = {0}; //声明一个管理员变量，先全部初始化为0

    num = crud_load_users(list); //从文件读取所有用户
    if (num > 0) return; //已经有用户了就不用创建管理员了

    admin.id = crud_next_id(); //给管理员分配一个新编号
    strcpy(admin.username, "admin"); //用户名设成admin
    strcpy(admin.passwd, "123456"); //密码设成123456
    strcpy(admin.identity, "manager"); //身份设成manager
    strcpy(admin.hobby, "竞技"); //爱好设成竞技
    admin.age = 20; //年龄设成20
    admin.role = 2; //角色设成2，就是管理员
    admin.coins = 1000; //给1000个金币
    admin.game_hours = 200; //游戏时长200小时
    admin.play_count = 50; //打了50把
    admin.win_count = 30; //赢了30把

    list[0] = admin; //把管理员放到数组第一个位置
    crud_save_users(list, 1); //保存到文件里，总共1个用户

    printf("第一次运行，已自动创建管理员账号。\n"); //提示一下用户
    printf("账号：admin  密码：123456\n"); //告诉用户默认的账号密码
}


//根据总对局和胜场算胜率
int manage_get_win_rate(users* user)
{
    if (user->play_count <= 0) return 0; //没打过就是0

    return user->win_count * 100 / user->play_count; //胜场乘以100再除以总场次就是胜率百分比
}

//用户注册函数
//把输入的资料整理好以后保存到用户文件里
int manage_user_register(users* user)
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量

    num = crud_load_users(list); //先把现有用户读出来
    if (crud_find_user_index(list, num, user->username) >= 0) { //看看用户名有没有重复的
        printf("用户名已存在。\n"); //有重复就提示一下
        return -1; //返回失败
    }

    user->id = crud_next_id(); //给新用户分配编号
    user->role = 0; //默认是普通用户
    user->coins = 100; //送100个金币当初始资金
    user->friend_count = 0; //好友数量初始为0
    user->asset_count = 0; //资产数量初始为0
    user->game_hours = 0; //游戏时长初始为0
    user->play_count = 0; //对局数初始为0
    user->win_count = 0; //胜场数初始为0

    list[num] = *user; //把新用户放到数组末尾
    num++; //用户总数加1
    crud_save_users(list, num); //保存到文件

    printf("注册成功。\n"); //提示注册成功
    return 0; //返回0表示成功
}

//用户登录函数
//先检查密码，再检查验证码
int manage_user_login(char username[], char passwd[])
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int pos; //用户在数组里的位置
    int input_code; //用户输入的验证码

    num = crud_load_users(list); //读取所有用户
    pos = crud_find_user_index(list, num, username); //按用户名查找位置
    if (pos < 0) return -1; //找不到这个用户就登录失败

    if (strcmp(list[pos].passwd, passwd) != 0) { //比较密码，不一样的话
        printf("密码错误。\n"); //提示密码错误
        return -1; //返回失败
    }

    printf("验证码：1234\n"); //显示固定的验证码
    printf("请输入验证码："); //让用户输入验证码
    scanf("%d", &input_code); //读取用户输入的验证码
    if (input_code != 1234) { //如果输入的不是1234
        printf("验证码错误。\n"); //提示验证码错误
        return -1; //返回失败
    }

    printf("登录成功。\n"); //登录成功提示
    return 0; //返回0表示成功
}

//自动升级
//普通用户只要游戏时长够或者胜率够，就会升成VIP
void manage_user_auto_upgrade(users* user)
{
    int rate; //胜率

    rate = manage_get_win_rate(user); //算一下胜率

    if (user->role == 0) { //如果当前是普通用户
        if (user->game_hours >= 100 || rate >= 60) { //游戏时长满100小时或者胜率60%以上
            user->role = 1; //就升级成VIP
        }
    }
}

//显示当前用户的详细信息
void manage_user_show_info(char username[])
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int pos; //用户位置
    int rate; //胜率

    num = crud_load_users(list); //读取所有用户
    pos = crud_find_user_index(list, num, username); //按名字找到位置
    if (pos < 0) { //没找到
        printf("用户不存在。\n"); //提示一下
        return; //返回
    }

    rate = manage_get_win_rate(&list[pos]); //算一下这个用户的胜率

    printf("\n个人信息\n"); //打印标题
    printf("编号：%d\n", list[pos].id); //显示用户编号
    printf("用户名：%s\n", list[pos].username); //显示用户名
    printf("年龄：%d\n", list[pos].age); //显示年龄
    printf("身份/职业：%s\n", list[pos].identity); //显示身份
    printf("爱好：%s\n", list[pos].hobby); //显示爱好
    printf("角色：%d\n", list[pos].role); //显示角色等级
    printf("金币：%d\n", list[pos].coins); //显示金币
    printf("游戏时长：%d\n", list[pos].game_hours); //显示游戏时间
    printf("总对局：%d\n", list[pos].play_count); //显示打了几把
    printf("胜场：%d\n", list[pos].win_count); //显示赢了几把
    printf("胜率：%d%%\n", rate); //显示胜率百分比
    printf("好友数：%d\n", list[pos].friend_count); //显示好友有几个
    printf("资产数：%d\n", list[pos].asset_count); //显示有几个道具
}

//让用户自己修改信息
int manage_user_edit_info(char username[])
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int pos; //用户位置
    int choice; //用户选择的选项
    int add_hours; //新增的小时数
    int add_play; //新增的对局数
    int add_win; //新增的胜场数
    char text[50]; //临时字符串存用户输入

    num = crud_load_users(list); //读取所有用户
    pos = crud_find_user_index(list, num, username); //找到当前用户
    if (pos < 0) { //没找到
        printf("用户不存在。\n"); //提示
        return -1; //返回失败
    }

    printf("\n1.改密码\n"); //显示可以改密码
    printf("2.改年龄\n"); //显示可以改年龄
    printf("3.改身份\n"); //显示可以改身份
    printf("4.改爱好\n"); //显示可以改爱好
    printf("5.录入游戏时长\n"); //显示可以录入时长
    printf("6.录入胜负记录\n"); //显示可以录入胜负
    printf("7.充值金币\n"); //显示可以充值
    printf("0.返回\n"); //显示返回选项
    printf("请输入选择："); //提示用户选择
    scanf("%d", &choice); //读取选择

    if (choice == 1) { //选了改密码
        printf("请输入新密码："); //提示输入
        scanf("%s", text); //读取新密码
        strcpy(list[pos].passwd, text); //把新密码复制过去
    } else if (choice == 2) { //选了改年龄
        printf("请输入新年龄："); //提示
        scanf("%d", &list[pos].age); //直接读到结构体里
    } else if (choice == 3) { //选了改身份
        printf("请输入新身份或职业："); //提示
        scanf("%s", text); //读取
        strcpy(list[pos].identity, text); //复制过去
    } else if (choice == 4) { //选了改爱好
        printf("请输入新爱好："); //提示
        scanf("%s", text); //读取
        strcpy(list[pos].hobby, text); //复制过去
    } else if (choice == 5) { //选了录入游戏时长
        printf("请输入新增游戏时长："); //提示
        scanf("%d", &add_hours); //读取新增的小时
        if (add_hours > 0) { //输入的数大于0才有效
            list[pos].game_hours += add_hours; //加到原来的时长上
        }
    } else if (choice == 6) { //选了录入胜负记录
        printf("请输入新增对局数："); //提示输入对局
        scanf("%d", &add_play); //读取
        printf("请输入新增胜场数："); //提示输入胜场
        scanf("%d", &add_win); //读取
        if (add_play > 0 && add_win >= 0 && add_win <= add_play) { //数据合理才更新
            list[pos].play_count += add_play; //加到总对局
            list[pos].win_count += add_win; //加到总胜场
        }
    } else if (choice == 7) { //选了充值金币
        printf("请输入充值金币："); //提示
        scanf("%d", &add_hours); //读取充值数量（借用了add_hours这个变量）
        if (add_hours > 0) { //大于0才有效
            list[pos].coins += add_hours; //加到金币上
        }
    } else { //其他选项比如0
        return 0; //直接返回
    }

    manage_user_auto_upgrade(&list[pos]); //改完了检查一下是不是该自动升级了
    crud_save_users(list, num); //把修改后的数据保存到文件
    printf("修改完成。\n"); //提示修改完成
    return 0; //返回成功
}

//管理员查看全部用户
void manage_user_list_all(void)
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int i; //循环变量

    num = crud_load_users(list); //读出所有用户
    if (num <= 0) { //一个用户都没有
        printf("没有用户数据。\n"); //提示
        return; //返回
    }

    printf("\n用户列表\n"); //打印标题
    printf("ID\t用户名\t年龄\t角色\t金币\n"); //打印表头
    for (i = 0; i < num; i++) { //遍历每个用户
        printf("%d\t%s\t%d\t%d\t%d\n", //逐个打印用户信息
               list[i].id, //编号
               list[i].username, //用户名
               list[i].age, //年龄
               list[i].role, //角色
               list[i].coins); //金币
    }
}

//管理员手动升级用户
int manage_user_upgrade(void)
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int pos; //用户位置
    char username[50]; //要升级的用户名

    num = crud_load_users(list); //读取所有用户
    printf("请输入要升级的用户名："); //提示管理员输入
    scanf("%s", username); //读取用户名
    pos = crud_find_user_index(list, num, username); //找到这个用户
    if (pos < 0) { //没找到
        printf("用户不存在。\n"); //提示
        return -1; //返回失败
    }

    if (list[pos].role < 2) { //如果还不是最高等级
        list[pos].role++; //角色等级加1
    }

    crud_save_users(list, num); //保存修改
    printf("升级完成。\n"); //提示
    return 0; //返回成功
}

//管理员手动降级用户
int manage_user_degrade(void)
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int pos; //用户位置
    char username[50]; //要降级的用户名

    num = crud_load_users(list); //读取所有用户
    printf("请输入要降级的用户名："); //提示
    scanf("%s", username); //读取
    pos = crud_find_user_index(list, num, username); //找到这个用户
    if (pos < 0) { //没找到
        printf("用户不存在。\n"); //提示
        return -1; //返回失败
    }

    if (list[pos].role > 0) { //如果不是最低等级
        list[pos].role--; //角色等级减1
    }

    crud_save_users(list, num); //保存修改
    printf("降级完成。\n"); //提示
    return 0; //返回成功
}

//管理员删除用户
int manage_user_delete(void)
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int pos; //要删除的用户位置
    char username[50]; //要删除的用户名
    int i; //循环变量

    num = crud_load_users(list); //读取所有用户
    printf("请输入要删除的用户名："); //提示
    scanf("%s", username); //读取用户名
    pos = crud_find_user_index(list, num, username); //找到位置
    if (pos < 0) { //没找到
        printf("用户不存在。\n"); //提示
        return -1; //返回失败
    }

    for (i = pos; i < num - 1; i++) { //从删除位置开始，逐个往前移
        list[i] = list[i + 1]; //后面一个覆盖前面一个
    }
    num--; //总数减1
    crud_save_users(list, num); //保存修改后的数组
    printf("删除完成。\n"); //提示
    return 0; //返回成功
}

//添加好友
int manage_friend_add(char username[])
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int my_pos; //自己在数组里的位置
    int friend_pos; //好友在数组里的位置
    char friend_name[50]; //好友的用户名

    num = crud_load_users(list); //读取所有用户
    my_pos = crud_find_user_index(list, num, username); //找到自己
    if (my_pos < 0) return -1; //找不到自己就返回失败

    printf("请输入好友用户名："); //提示输入好友名字
    scanf("%s", friend_name); //读取好友名字
    friend_pos = crud_find_user_index(list, num, friend_name); //在数组里找这个好友
    if (friend_pos < 0) { //找不到
        printf("好友不存在。\n"); //提示
        return -1; //返回失败
    }

    if (my_pos == friend_pos) { //如果输入的就是自己
        printf("不能加自己。\n"); //提示不能加自己
        return -1; //返回失败
    }

    if (list[my_pos].friend_count >= MAX_FRIENDS || list[friend_pos].friend_count >= MAX_FRIENDS) { //任何一方好友满了
        printf("好友已满。\n"); //提示
        return -1; //返回失败
    }

    list[my_pos].friends[list[my_pos].friend_count] = list[friend_pos].id; //把好友的id加到自己的好友列表末尾
    list[my_pos].friend_count++; //自己的好友数加1
    list[friend_pos].friends[list[friend_pos].friend_count] = list[my_pos].id; //同时把自己的id也加到对方列表
    list[friend_pos].friend_count++; //对方好友数也加1
    crud_save_users(list, num); //保存
    printf("添加好友成功。\n"); //提示成功
    return 0; //返回成功
}

//删除好友
int manage_friend_remove(char username[])
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int my_pos; //自己的位置
    int friend_pos; //好友的位置
    char friend_name[50]; //好友用户名
    int i; //外层循环变量
    int j; //内层循环变量

    num = crud_load_users(list); //读取所有用户
    my_pos = crud_find_user_index(list, num, username); //找到自己
    if (my_pos < 0) return -1; //找不到就返回失败

    printf("请输入要删除的好友用户名："); //提示
    scanf("%s", friend_name); //读取
    friend_pos = crud_find_user_index(list, num, friend_name); //找这个好友
    if (friend_pos < 0) { //找不到
        printf("好友不存在。\n"); //提示
        return -1; //返回失败
    }

    for (i = 0; i < list[my_pos].friend_count; i++) { //遍历自己的好友列表
        if (list[my_pos].friends[i] == list[friend_pos].id) { //找到要删的好友id
            for (j = i; j < list[my_pos].friend_count - 1; j++) { //从这个位置开始往前移
                list[my_pos].friends[j] = list[my_pos].friends[j + 1]; //后面的覆盖前面的
            }
            list[my_pos].friend_count--; //好友数减1
            break; //找到了就不用继续了
        }
    }

    for (i = 0; i < list[friend_pos].friend_count; i++) { //同样遍历对方的好友列表
        if (list[friend_pos].friends[i] == list[my_pos].id) { //找到自己的id
            for (j = i; j < list[friend_pos].friend_count - 1; j++) { //往前移
                list[friend_pos].friends[j] = list[friend_pos].friends[j + 1]; //覆盖
            }
            list[friend_pos].friend_count--; //对方好友数减1
            break; //跳出循环
        }
    }

    crud_save_users(list, num); //保存修改
    printf("删除好友成功。\n"); //提示成功
    return 0; //返回成功
}

//显示当前用户的好友列表
void manage_friend_list(char username[])
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int my_pos; //自己的位置
    int i; //循环变量
    int pos; //好友在数组里的位置

    num = crud_load_users(list); //读取所有用户
    my_pos = crud_find_user_index(list, num, username); //找到自己
    if (my_pos < 0) return; //没找到就返回

    if (list[my_pos].friend_count == 0) { //如果一个好友都没有
        printf("还没有好友。\n"); //提示
        return; //返回
    }

    printf("\n好友列表\n"); //打印标题
    for (i = 0; i < list[my_pos].friend_count; i++) { //遍历好友列表
        pos = crud_find_user_id_index(list, num, list[my_pos].friends[i]); //用好友id找到对应的用户
        if (pos >= 0) { //找到了
            printf("%s\n", list[pos].username); //打印好友名字
        }
    }
}

//好友邀约功能
//只有已经加成好友的人才能发邀约
int manage_friend_invite(char username[])
{
    users list[MAX_USERS]; //用户数组
    games game_list[MAX_GAMES]; //游戏数组
    invite_log invite = {0}; //邀约记录，先全部初始化成0
    int num; //用户数量
    int game_num; //游戏数量
    int my_pos; //自己的位置
    int friend_pos; //好友的位置
    int has_friend = 0; //标记对方是不是自己好友，0表示不是
    int i; //循环变量
    char friend_name[50]; //好友用户名
    char game_name[50]; //要邀约的游戏名称

    num = crud_load_users(list); //读取所有用户
    my_pos = crud_find_user_index(list, num, username); //找到自己
    if (my_pos < 0) return -1; //找不到就返回失败

    printf("请输入要邀约的好友用户名："); //提示输入
    scanf("%s", friend_name); //读取好友名
    friend_pos = crud_find_user_index(list, num, friend_name); //找到好友
    if (friend_pos < 0) { //找不到
        printf("好友不存在。\n"); //提示
        return -1; //返回失败
    }

    for (i = 0; i < list[my_pos].friend_count; i++) { //遍历自己好友列表
        if (list[my_pos].friends[i] == list[friend_pos].id) { //看看对方在不在里面
            has_friend = 1; //在的话标记成1
            break; //找到了就不用继续了
        }
    }

    if (has_friend == 0) { //如果不是好友
        printf("对方不是你的好友，不能发邀约。\n"); //提示
        return -1; //返回失败
    }

    game_num = crud_load_games(game_list); //读取所有游戏
    printf("\n可邀约游戏\n"); //打印可选游戏标题
    for (i = 0; i < game_num; i++) { //遍历游戏列表
        printf("%s  %s\n", game_list[i].game_name, game_list[i].game_type); //显示游戏名和类型
    }

    printf("请输入邀约游戏名称："); //提示输入游戏名
    scanf("%s", game_name); //读取

    invite.send_id = list[my_pos].id; //记录发起人id
    invite.recv_id = list[friend_pos].id; //记录接收人id
    strcpy(invite.send_name, list[my_pos].username); //记录发起人名字
    strcpy(invite.recv_name, list[friend_pos].username); //记录接收人名字
    strcpy(invite.game_name, game_name); //记录邀约的游戏
    crud_add_invite_log(&invite); //把邀约记录保存到文件
    printf("邀约发送成功。\n"); //提示成功
    return 0; //返回成功
}

//查看和自己有关的邀约记录
void manage_friend_show_invite(char username[])
{
    invite_log logs[MAX_MARKET]; //邀约记录数组
    int num; //记录数量
    int i; //循环变量
    int show_count = 0; //展示了几条记录

    num = crud_load_invite_logs(logs); //读取所有邀约记录
    if (num <= 0) { //一条记录都没有
        printf("还没有邀约记录。\n"); //提示
        return; //返回
    }

    printf("\n好友邀约记录\n"); //打印标题
    for (i = 0; i < num; i++) { //遍历所有记录
        if (strcmp(logs[i].send_name, username) == 0 || strcmp(logs[i].recv_name, username) == 0) { //只要发送人或接收人是自己
            printf("发起人：%s  接收人：%s  游戏：%s\n", //就打印这条记录
                   logs[i].send_name, //发起人
                   logs[i].recv_name, //接收人
                   logs[i].game_name); //游戏名
            show_count++; //展示计数加1
        }
    }

    if (show_count == 0) { //如果一条相关的都没有
        printf("你还没有邀约记录。\n"); //提示
    }
}

//推荐游戏
//这里按年龄、身份、爱好和游戏时长做一个简单推荐
void manage_show_recommend_games(char username[])
{
    users list[MAX_USERS]; //用户数组
    games game_list[MAX_GAMES]; //游戏数组
    int user_num; //用户数量
    int game_num; //游戏数量
    int pos; //当前用户位置
    int i; //循环变量
    int show_count = 0; //推荐了几个游戏

    user_num = crud_load_users(list); //读取用户
    pos = crud_find_user_index(list, user_num, username); //找到当前用户
    if (pos < 0) return; //没找到就返回

    game_num = crud_load_games(game_list); //读取所有游戏
    printf("\n推荐游戏\n"); //打印标题
    for (i = 0; i < game_num; i++) { //遍历每个游戏
        if (list[pos].age >= game_list[i].need_age) { //先看年龄够不够
            if (strstr(game_list[i].game_type, list[pos].hobby) != NULL || //游戏类型包含用户爱好
                strstr(list[pos].hobby, game_list[i].game_type) != NULL || //或者用户爱好包含游戏类型
                (strstr(list[pos].identity, "学生") != NULL && strstr(game_list[i].game_type, "竞技") != NULL) || //学生推荐竞技游戏
                (strstr(list[pos].identity, "教师") != NULL && strstr(game_list[i].game_type, "策略") != NULL) || //老师推荐策略游戏
                (list[pos].game_hours >= 100 && strstr(game_list[i].game_type, "竞技") != NULL)) { //老玩家推荐竞技
                printf("%s  %s  %s\n", //打印推荐的游戏信息
                       game_list[i].game_name, //游戏名
                       game_list[i].game_type, //类型
                       game_list[i].game_desc); //简介
                show_count++; //推荐计数加1
            }
        }
    }

    if (show_count == 0) { //如果一个都没推荐到
        for (i = 0; i < game_num; i++) { //那就把年龄合适的全推荐了
            if (list[pos].age >= game_list[i].need_age) { //年龄够就行
                printf("%s  %s  %s\n", //打印游戏信息
                       game_list[i].game_name, //游戏名
                       game_list[i].game_type, //类型
                       game_list[i].game_desc); //简介
            }
        }
    }
}

//好友菜单
void manage_show_friend_menu(char username[])
{
    int choice; //用户选择

    while (1) { //一直循环直到用户选择返回
        printf("\n1.查看好友\n"); //选项1
        printf("2.添加好友\n"); //选项2
        printf("3.删除好友\n"); //选项3
        printf("4.发起邀约\n"); //选项4
        printf("5.查看邀约记录\n"); //选项5
        printf("0.返回\n"); //选项0返回
        printf("请输入选择："); //提示输入
        scanf("%d", &choice); //读取选择

        if (choice == 1) { //选了查看好友
            manage_friend_list(username); //调用查看好友函数
        } else if (choice == 2) { //选了添加好友
            manage_friend_add(username); //调用添加好友函数
        } else if (choice == 3) { //选了删除好友
            manage_friend_remove(username); //调用删除好友函数
        } else if (choice == 4) { //选了发起邀约
            manage_friend_invite(username); //调用邀约函数
        } else if (choice == 5) { //选了查看邀约记录
            manage_friend_show_invite(username); //调用查看邀约记录函数
        } else { //其他输入比如0
            return; //退出菜单
        }
    }
}

//个人信息菜单
void manage_show_user_info_menu(char username[])
{
    int choice; //用户选择

    while (1) { //循环直到返回
        printf("\n1.查看个人信息\n"); //选项1
        printf("2.修改个人信息\n"); //选项2
        printf("0.返回\n"); //选项0
        printf("请输入选择："); //提示
        scanf("%d", &choice); //读取

        if (choice == 1) { //选了查看
            manage_user_show_info(username); //调用显示信息函数
        } else if (choice == 2) { //选了修改
            manage_user_edit_info(username); //调用修改信息函数
        } else { //其他输入
            return; //退出
        }
    }
}

//管理员菜单
void manage_show_admin_menu(void)
{
    int choice; //用户选择

    while (1) { //循环直到返回
        printf("\n1.查看所有用户\n"); //选项1
        printf("2.升级用户\n"); //选项2
        printf("3.降级用户\n"); //选项3
        printf("4.删除用户\n"); //选项4
        printf("0.返回\n"); //选项0
        printf("请输入选择："); //提示
        scanf("%d", &choice); //读取

        if (choice == 1) { //选了查看全部用户
            manage_user_list_all(); //调用列出所有用户函数
        } else if (choice == 2) { //选了升级
            manage_user_upgrade(); //调用升级函数
        } else if (choice == 3) { //选了降级
            manage_user_degrade(); //调用降级函数
        } else if (choice == 4) { //选了删除
            manage_user_delete(); //调用删除函数
        } else { //其他输入
            return; //退出
        }
    }
}

//把市场里现在所有在售的道具列出来
void trade_show_market(void)
{
    market list[MAX_MARKET]; //市场道具数组
    users user_list[MAX_USERS]; //用户数组，用来查卖家名字
    int num; //道具数量
    int user_num; //用户数量
    int i; //循环变量
    int pos; //卖家在用户数组里的位置

    num = crud_load_market(list); //读取市场数据
    user_num = crud_load_users(user_list); //读取用户数据
    if (num <= 0) { //市场里啥都没有
        printf("市场里还没有道具。\n"); //提示
        return; //返回
    }

    printf("\n市场列表\n"); //打印标题
    printf("ID\t名称\t类型\t价格\t卖家\n"); //打印表头
    for (i = 0; i < num; i++) { //遍历每个道具
        pos = crud_find_user_id_index(user_list, user_num, list[i].seller_id); //通过卖家id找到卖家
        printf("%d\t%s\t%s\t%d\t", //打印道具基本信息
               list[i].item_id, //道具编号
               list[i].item_name, //道具名称
               list[i].item_type, //道具类型
               list[i].item_price); //道具价格
        if (pos >= 0) { //如果找到了卖家
            printf("%s\n", user_list[pos].username); //打印卖家名字
        } else { //找不到卖家
            printf("未知\n"); //显示未知
        }
    }
}

//显示交易记录
//这里只展示和当前用户有关的记录
void trade_show_log(char username[])
{
    trade_log logs[MAX_MARKET]; //交易记录数组
    int num; //记录数量
    int i; //循环变量
    int show_count = 0; //展示了几条

    num = crud_load_trade_logs(logs); //读取所有交易记录
    if (num <= 0) { //一条都没有
        printf("还没有交易记录。\n"); //提示
        return; //返回
    }

    printf("\n交易记录\n"); //打印标题
    for (i = 0; i < num; i++) { //遍历所有记录
        if (strcmp(logs[i].buyer_name, username) == 0 || strcmp(logs[i].seller_name, username) == 0) { //买家或卖家是自己的
            printf("道具：%s  类型：%s  价格：%d  买家：%s  卖家：%s\n", //打印交易详情
                   logs[i].item_name, //道具名
                   logs[i].item_type, //道具类型
                   logs[i].item_price, //价格
                   logs[i].buyer_name, //买家名
                   logs[i].seller_name); //卖家名
            show_count++; //展示计数加1
        }
    }

    if (show_count == 0) { //没有相关记录
        printf("你还没有相关交易记录。\n"); //提示
    }
}

//上架道具
//输入道具名字、类型和价格以后保存到市场里
int trade_publish_item(char username[])
{
    market list[MAX_MARKET]; //市场数组
    users user_list[MAX_USERS]; //用户数组
    int num; //道具数量
    int user_num; //用户数量
    int user_pos; //当前用户位置
    market item = {0}; //新道具，先初始化为0

    num = crud_load_market(list); //读取市场数据
    user_num = crud_load_users(user_list); //读取用户数据
    user_pos = crud_find_user_index(user_list, user_num, username); //找到当前用户
    if (user_pos < 0) return -1; //没找到就返回失败

    printf("请输入道具名称："); //提示输入
    scanf("%s", item.item_name); //读取道具名
    printf("请输入道具类型："); //提示
    scanf("%s", item.item_type); //读取类型
    printf("请输入价格："); //提示
    scanf("%d", &item.item_price); //读取价格

    item.item_id = crud_next_id(); //给道具分配一个新编号
    item.seller_id = user_list[user_pos].id; //卖家就是当前用户
    list[num] = item; //把道具放到市场数组末尾
    num++; //道具总数加1

    if (user_list[user_pos].asset_count < MAX_ASSETS) { //如果资产没超上限
        user_list[user_pos].assets[user_list[user_pos].asset_count] = item.item_id; //把道具id加到资产列表
        user_list[user_pos].asset_count++; //资产数加1
    }

    crud_save_market(list, num); //保存市场数据
    crud_save_users(user_list, user_num); //保存用户数据
    printf("上架成功。\n"); //提示成功
    return 0; //返回成功
}

//购买道具
//买家扣钱，卖家加钱，同时写交易记录
int trade_buy_item(char username[])
{
    market list[MAX_MARKET]; //市场数组
    users user_list[MAX_USERS]; //用户数组
    trade_log one_log = {0}; //交易记录初始化
    int num; //道具数量
    int user_num; //用户数量
    int buyer_pos; //买家在数组里的位置
    int item_pos; //道具在市场里的位置
    int seller_pos; //卖家在数组里的位置
    int target_id; //要买的道具编号
    int i; //循环变量

    num = crud_load_market(list); //读取市场数据
    user_num = crud_load_users(user_list); //读取用户数据
    buyer_pos = crud_find_user_index(user_list, user_num, username); //找到买家
    if (buyer_pos < 0) return -1; //找不到就返回失败

    if (num <= 0) { //市场没东西
        printf("市场里还没有道具。\n"); //提示
        return -1; //返回失败
    }

    trade_show_market(); //先把市场展示出来让用户看看
    printf("请输入要购买的道具ID："); //提示输入
    scanf("%d", &target_id); //读取要买的道具id
    item_pos = crud_find_market_index(list, num, target_id); //找到这个道具
    if (item_pos < 0) { //找不到
        printf("没有这个道具。\n"); //提示
        return -1; //返回失败
    }

    if (list[item_pos].seller_id == user_list[buyer_pos].id) { //如果卖家就是自己
        printf("不能买自己的道具。\n"); //提示不能买自己的
        return -1; //返回失败
    }

    if (user_list[buyer_pos].coins < list[item_pos].item_price) { //金币不够
        printf("金币不够。\n"); //提示
        return -1; //返回失败
    }

    seller_pos = crud_find_user_id_index(user_list, user_num, list[item_pos].seller_id); //通过卖家id找到卖家
    user_list[buyer_pos].coins -= list[item_pos].item_price; //买家扣钱
    if (seller_pos >= 0) { //如果找到了卖家
        user_list[seller_pos].coins += list[item_pos].item_price; //卖家加钱
    }

    if (user_list[buyer_pos].asset_count < MAX_ASSETS) { //买家资产没满
        user_list[buyer_pos].assets[user_list[buyer_pos].asset_count] = list[item_pos].item_id; //把道具加到买家资产里
        user_list[buyer_pos].asset_count++; //资产数加1
    }

    one_log.buyer_id = user_list[buyer_pos].id; //记录买家id
    strcpy(one_log.buyer_name, user_list[buyer_pos].username); //记录买家名
    one_log.seller_id = list[item_pos].seller_id; //记录卖家id
    if (seller_pos >= 0) { //如果找到了卖家
        strcpy(one_log.seller_name, user_list[seller_pos].username); //记录卖家名
    } else { //找不到卖家
        strcpy(one_log.seller_name, "未知"); //就写未知
    }
    one_log.item_id = list[item_pos].item_id; //记录道具id
    strcpy(one_log.item_name, list[item_pos].item_name); //记录道具名
    strcpy(one_log.item_type, list[item_pos].item_type); //记录道具类型
    one_log.item_price = list[item_pos].item_price; //记录交易价格

    for (i = item_pos; i < num - 1; i++) { //把买掉的道具从市场里删掉，后面的往前移
        list[i] = list[i + 1]; //后面覆盖前面
    }
    num--; //市场道具总数减1

    crud_save_market(list, num); //保存市场
    crud_save_users(user_list, user_num); //保存用户
    crud_add_trade_log(&one_log); //保存这条交易记录
    printf("购买成功。\n"); //提示成功
    return 0; //返回成功
}

//交易菜单
void trade_menu(char username[])
{
    int choice; //用户选择

    while (1) { //循环直到返回
        printf("\n1.查看市场\n"); //选项1
        printf("2.上架道具\n"); //选项2
        printf("3.购买道具\n"); //选项3
        printf("4.查看交易记录\n"); //选项4
        printf("0.返回\n"); //选项0
        printf("请输入选择："); //提示
        scanf("%d", &choice); //读取

        if (choice == 1) { //选了查看市场
            trade_show_market(); //调用展示市场函数
        } else if (choice == 2) { //选了上架
            trade_publish_item(username); //调用上架函数
        } else if (choice == 3) { //选了购买
            trade_buy_item(username); //调用购买函数
        } else if (choice == 4) { //选了查看交易记录
            trade_show_log(username); //调用交易记录函数
        } else { //其他输入
            return; //退出
        }
    }
}

//主菜单显示函数
//根据是不是管理员，菜单内容会有一点区别
int main_show_menu(int role)
{
    int choice; //用户选择

    printf("\n==============================\n"); //打印分割线好看一点
    if (role == 2) { //如果是管理员
        printf("管理员菜单\n"); //显示管理员菜单标题
        printf("1.排行榜\n"); //选项1
        printf("2.推荐游戏\n"); //选项2
        printf("3.交易市场\n"); //选项3
        printf("4.好友管理\n"); //选项4
        printf("5.个人信息\n"); //选项5
        printf("6.用户管理\n"); //选项6，管理员专属
        printf("7.退出\n"); //选项7退出
    } else { //如果是普通用户或VIP
        printf("用户菜单\n"); //显示用户菜单标题
        printf("1.排行榜\n"); //选项1
        printf("2.推荐游戏\n"); //选项2
        printf("3.交易市场\n"); //选项3
        printf("4.好友管理\n"); //选项4
        printf("5.个人信息\n"); //选项5
        printf("6.退出\n"); //选项6退出
    }
    printf("==============================\n"); //打印分割线
    printf("请输入选择："); //提示
    scanf("%d", &choice); //读取选择
    return choice; //返回用户选了第几个
}

//金币排行榜
//这里用最基础的冒泡排序来排前面的用户
void main_show_ranking(void)
{
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int i; //外层循环变量
    int j; //内层循环变量
    users temp; //交换用的临时变量

    num = crud_load_users(list); //读取所有用户
    if (num <= 0) { //没有用户
        printf("没有用户数据。\n"); //提示
        return; //返回
    }

    for (i = 0; i < num - 1; i++) { //冒泡排序外层循环
        for (j = 0; j < num - 1 - i; j++) { //内层循环，每轮少比一个
            if (list[j].coins < list[j + 1].coins) { //如果前面的金币比后面的少
                temp = list[j]; //交换位置，金币多的排前面
                list[j] = list[j + 1]; //把后面的放到前面
                list[j + 1] = temp; //把前面的放到后面
            }
        }
    }

    printf("\n金币排行榜\n"); //打印标题
    for (i = 0; i < num && i < 10; i++) { //最多显示前10名
        printf("%d. %s %d\n", i + 1, list[i].username, list[i].coins); //打印排名、名字和金币
    }
}

//如果输入的用户名还不存在，就当作第一次注册来处理
void main_first_register(char username[])
{
    users user = {0}; //新用户变量，初始化为0

    strcpy(user.username, username); //先把用户名存进去
    printf("请输入密码："); //提示输入密码
    scanf("%s", user.passwd); //读取密码
    printf("请输入年龄："); //提示输入年龄
    scanf("%d", &user.age); //读取年龄
    printf("请输入身份或职业："); //提示输入身份
    scanf("%s", user.identity); //读取身份
    printf("请输入爱好："); //提示输入爱好
    scanf("%s", user.hobby); //读取爱好
    manage_user_register(&user); //调用注册函数完成注册
}

//主函数
//程序从这里开始运行，先登录或注册，再进入主菜单循环
int main(void)
{
    char username[50]; //用户名
    char passwd[50]; //密码
    users list[MAX_USERS]; //用户数组
    int num; //用户数量
    int pos; //当前用户位置
    int role; //当前用户角色
    int choice; //菜单选择

    crud_init(); //先初始化所有数据文件
    manage_create_first_admin(); //如果没用户就自动创建管理员

    printf("欢迎来到unisystem，请输入用户名："); //欢迎语和提示
    scanf("%s", username); //读取用户名

    num = crud_load_users(list); //读取所有用户
    pos = crud_find_user_index(list, num, username); //看看这个用户名存不存在

    if (pos >= 0) { //如果用户存在
        printf("请输入密码："); //提示输入密码
        scanf("%s", passwd); //读取密码
        while (manage_user_login(username, passwd) != 0) { //登录失败就一直重试
            printf("请重新输入密码："); //提示重新输入
            scanf("%s", passwd); //读取
        }
    } else { //用户不存在
        main_first_register(username); //走注册流程
    }

    while (1) { //进入主循环，一直运行直到退出
        num = crud_load_users(list); //每次循环都重新读取用户数据，保证是最新的
        pos = crud_find_user_index(list, num, username); //找到当前用户
        if (pos < 0) return 0; //找不到了就退出程序

        manage_user_auto_upgrade(&list[pos]); //检查是不是该自动升级了
        crud_save_users(list, num); //保存可能升级后的数据
        role = list[pos].role; //拿到当前角色
        choice = main_show_menu(role); //显示菜单并获取选择

        if (choice == 1) { //选了排行榜
            main_show_ranking(); //显示排行榜
        } else if (choice == 2) { //选了推荐游戏
            manage_show_recommend_games(username); //显示推荐游戏
        } else if (choice == 3) { //选了交易市场
            trade_menu(username); //进入交易菜单
        } else if (choice == 4) { //选了好友管理
            manage_show_friend_menu(username); //进入好友菜单
        } else if (choice == 5) { //选了个人信息
            manage_show_user_info_menu(username); //进入个人信息菜单
        } else if (choice == 6 && role == 2) { //管理员选了用户管理
            manage_show_admin_menu(); //进入管理员菜单
        } else if ((choice == 6 && role != 2) || (choice == 7 && role == 2)) { //选了退出选项
            printf("感谢使用。\n"); //感谢语
            return 0; //退出程序
        } else { //输入了其他莫名其妙的数字
            printf("输入有误。\n"); //提示输入有误
        }
    }
}
