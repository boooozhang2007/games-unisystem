# 网络游戏推荐和交易系统

## 概述

网络游戏是人们非常喜欢的一种娱乐方式。目前市面上有很多种游戏，适合不同年龄阶段和兴趣的用户。人们在玩游戏的过程中常进行交友，甚至可以交易游戏装备、积分等。本项目设计一个推荐系统，能够通过用户画像进行分类推荐，通过用户的游戏时间或胜率进行升级，支持游戏资产交易，以及其他游戏娱乐相关任务。该系统面向游戏玩家、网络游戏管理者等不同角色，能够适应多种用户需求。

## 1. 题目描述

### 功能需求

- **用户注册**：注册新用户，支持游戏管理者、玩家等不同角色
- **用户登录**：通过账号、密码、验证码登录系统
- **修改用户信息**：用户登录后可对自己信息进行增删改查
- **游戏推荐**：根据用户的历史操作，推荐新款游戏、游戏新装备等
- **游戏好友管理**：管理用户好友，进行游戏邀约，查看好友记录等
- **游戏相关交易**：进行游戏财富管理（积分、装备等），支持交易功能
- **其他相关功能**：游戏排行榜、论坛交流等

## 2. 题目要求

- 按照分析、设计、编码、调试和测试过程完成应用程序
- 学习并使用流程图等工具，并在撰写报告中使用
- 程序各项功能在运行时以菜单方式选择并执行
- 用户输入数据时给出清晰、明确的提示（包括内容、格式及结束方式）
- 所有信息存储在文件中，实现文件读写操作
- 使用复杂的数据结构

## 3. 题目提示

- 参考37互娱等相关游戏网站进行资料查询
- 从游戏管理、用户管理、交易管理、游戏交流等角度思考和设计系统

## 4. 架构设计

- main：主程序入口
    - ranking
    - initial
    - menu
- recommend：推荐系统
    - recommend
    - analyse
- trade：交易系统
    - store
    - publish_items
    - remove_items
    - purchase
    - transfer
- manage：统一管理系统
    - users：用户注册、登录方法
        - register
        - login
        - list_users
        - upgrade_user
        - degrade_user
        - edit_user_info
    - friends：好友管理
        - add_friends
        - remove_friends
        - list_friends
- crud：增删改查等通用操作
    - create
    - read
    - update
    - delete
- data：数据
    - user_jsons：用户数据库文件
        - user_id
        - passwd(加密保存)
        - age
        - role
        - friends
        - wealth
        - history
        - game_hours
    - games_jsons：游戏信息
        - games
            - game_id
            - game_description
        - game_assets
            - asset_belong
            - asset_id
            - asset_owner_id
            - asset_price
            - asset_type
            - asset_description
    - market_json：商店物品数据
        - items
        - 