#include <stdio.h>
#include <string.h>
#include <time.h>
#include "trade.h"
#include "crud.h"
#include "models.h"

static int trade_find_user_index_by_name(const users* arr, int count, const char* username)
{
	int i;
	for (i = 0; i < count; i++) {
		if (strcmp(arr[i].username, username) == 0) {
			return i;
		}
	}
	return -1;
}

static int trade_find_user_index_by_id(const users* arr, int count, int id_value)
{
	int i;
	for (i = 0; i < count; i++) {
		if (arr[i].id == id_value) {
			return i;
		}
	}
	return -1;
}

static void trade_show_market(void)
{
	market items[MAX_MARKET_ITEMS];
	users user_list[MAX_USERS];
	int item_count;
	int user_count;
	int i;

	item_count = crud_load_market(items, MAX_MARKET_ITEMS);
	user_count = crud_load_users(user_list, MAX_USERS);

	if (item_count <= 0) {
		printf("市场暂无上架道具。\n");
		return;
	}

	printf("\n市场列表：\n");
	printf("道具ID\t名称\t价格\t卖家\t上架时间\n");
	for (i = 0; i < item_count; i++) {
		const char* seller_name = "未知卖家";
		int seller_index = trade_find_user_index_by_id(user_list, user_count, items[i].seller_id);
		if (seller_index >= 0) {
			seller_name = user_list[seller_index].username;
		}
		printf("%d\t%s\t%d\t%s\t%s\n",
			   items[i].item_id,
			   items[i].item_name,
			   items[i].item_price,
			   seller_name,
			   items[i].publish_time);
	}
}

static int trade_publish_item(const char* username)
{
	users user_list[MAX_USERS];
	int user_count;
	int self_index;
	market item;
	char item_name[64];
	int item_price;
	time_t now;
	struct tm* tm_info;

	if (username == NULL) {
		return -1;
	}

	user_count = crud_load_users(user_list, MAX_USERS);
	if (user_count <= 0) {
		printf("用户数据为空。\n");
		return -2;
	}

	self_index = trade_find_user_index_by_name(user_list, user_count, username);
	if (self_index < 0) {
		printf("当前用户不存在。\n");
		return -3;
	}

	printf("请输入道具名称（不含空格）：");
	scanf("%63s", item_name);
	printf("请输入道具价格：");
	scanf("%d", &item_price);

	if (item_price <= 0) {
		printf("价格必须大于0。\n");
		return -4;
	}

	memset(&item, 0, sizeof(item));
	item.item_id = id();
	strncpy(item.item_name, item_name, sizeof(item.item_name) - 1);
	item.item_price = item_price;
	item.seller_id = user_list[self_index].id;

	now = time(NULL);
	tm_info = localtime(&now);
	if (tm_info != NULL) {
		strftime(item.publish_time, sizeof(item.publish_time), "%Y-%m-%d", tm_info);
	} else {
		strcpy(item.publish_time, "unknown");
	}

	if (crud_append_market_item(&item) != 0) {
		printf("上架失败。\n");
		return -5;
	}

	if (user_list[self_index].asset_count < MAX_ASSETS) {
		user_list[self_index].assets[user_list[self_index].asset_count++] = item.item_id;
		crud_save_users(user_list, user_count);
	}

	printf("上架成功！道具ID：%d\n", item.item_id);
	return 0;
}

static int trade_buy_item(const char* username)
{
	market items[MAX_MARKET_ITEMS];
	users user_list[MAX_USERS];
	int item_count;
	int user_count;
	int buyer_index;
	int target_id;
	int item_index = -1;
	int seller_index;
	int bought_price;
	int i;

	if (username == NULL) {
		return -1;
	}

	item_count = crud_load_market(items, MAX_MARKET_ITEMS);
	if (item_count <= 0) {
		printf("市场暂无上架道具。\n");
		return -2;
	}

	user_count = crud_load_users(user_list, MAX_USERS);
	if (user_count <= 0) {
		printf("用户数据为空。\n");
		return -3;
	}

	buyer_index = trade_find_user_index_by_name(user_list, user_count, username);
	if (buyer_index < 0) {
		printf("当前用户不存在。\n");
		return -4;
	}

	trade_show_market();
	printf("请输入要购买的道具ID：");
	scanf("%d", &target_id);

	for (i = 0; i < item_count; i++) {
		if (items[i].item_id == target_id) {
			item_index = i;
			break;
		}
	}

	if (item_index < 0) {
		printf("未找到该道具。\n");
		return -5;
	}

	if (items[item_index].seller_id == user_list[buyer_index].id) {
		printf("不能购买自己上架的道具。\n");
		return -6;
	}

	if (user_list[buyer_index].coins < items[item_index].item_price) {
		printf("金币不足，购买失败。\n");
		return -7;
	}

	seller_index = trade_find_user_index_by_id(user_list, user_count, items[item_index].seller_id);
	bought_price = items[item_index].item_price;

	user_list[buyer_index].coins -= bought_price;
	if (seller_index >= 0) {
		user_list[seller_index].coins += bought_price;
	}

	if (user_list[buyer_index].asset_count < MAX_ASSETS) {
		user_list[buyer_index].assets[user_list[buyer_index].asset_count++] = items[item_index].item_id;
	}

	for (i = item_index; i < item_count - 1; i++) {
		items[i] = items[i + 1];
	}
	item_count--;

	if (crud_save_users(user_list, user_count) != 0) {
		printf("用户数据保存失败。\n");
		return -8;
	}

	if (crud_save_market(items, item_count) != 0) {
		printf("市场数据保存失败。\n");
		return -9;
	}

	printf("购买成功！你花费了 %d 金币。\n", bought_price);
	return 0;
}

void trade_menu(const char* username)
{
	int choice;

	while (1) {
		printf("\n交易菜单\n");
		printf("1. 查看市场\n");
		printf("2. 发布道具\n");
		printf("3. 购买道具\n");
		printf("0. 返回上级菜单\n");
		printf("请输入选择：");
		scanf("%d", &choice);

		if (choice == 1) {
			trade_show_market();
		} else if (choice == 2) {
			trade_publish_item(username);
		} else if (choice == 3) {
			trade_buy_item(username);
		} else if (choice == 0) {
			return;
		} else {
			printf("无效选择，请重新输入。\n");
		}
	}
}
