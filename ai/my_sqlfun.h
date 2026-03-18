#ifndef _MY_SQLFUN_H_
#define _MY_SQLFUN_H_

#include <sqlite3.h>

// #define FILEPATH "/home/xauat/rv1126/sqlite/test_for_Linux/demo/"
#define FILEPATH "/userdata/media/www/"
#define FILENAME "highlook.sqlite"

#define SQLMAXSIZE 2048

#define INSERTPARKINGDATA "INSERT INTO parking (name,number,createtime) VALUES('%s','%s',%f);"
#define INSERTPLACEDATA "INSERT INTO place (pindex,name,number,points,status,createtime) VALUES(%d,'%s','%s','%s',%d,%f);"
#define INSERTUSERDATA "INSERT INTO user (username,password,type,nickname,createtime) VALUES('%s','%s',%d,'%s',%f);"

#define DELETEALLPARKINGDATA "DELETE from parking;"
#define DELETEALLPLACEDATA "DELETE from place;"
#define DELETEALLUSERDATA "DELETE from user;"

#define DELETEPARKINGDATA "DELETE from parking where id = %d;"
#define DELETEPLACEDATA "DELETE from place where id = %d;"
#define DELETEUSERDATA "DELETE from user where id = %d;"

#define SELECTPARKINGALLDATA "SELECT * FROM parking;"
#define SELECTPARKINGIDDATA "SELECT * FROM parking where id = %d;"
#define SELECTPARKINGNAMEDATA "SELECT * FROM parking where name = '%s';"
#define SELECTPARKINGNUMBERDATA "SELECT * FROM parking where number = '%s';"
#define SELECTPARKINGCREATETIMEDATA "SELECT * FROM parking where createtime = %f;"

#define SELECTPLACEALLDATA "SELECT * FROM place order by number;"
#define SELECTPLACEIDDATA "SELECT * FROM place where id = %d;"
#define SELECTPLACEPINDEXDATA "SELECT * FROM place where pindex = %d;"
#define SELECTPLACENAMEDATA "SELECT * FROM place where name = '%s';"
#define SELECTPLACENUMBERDATA "SELECT * FROM place where number = '%s';"
#define SELECTPLACEPOINTSDATA "SELECT * FROM place where points = '%s';"
#define SELECTPLACESTATUSDATA "SELECT * FROM place where status = %d;"
#define SELECTPLACECREATETIMEDATA "SELECT * FROM place where createtime = %f;"

#define SELECTUSERALLDATA "SELECT * FROM user;"
#define SELECTUSERIDDATA "SELECT * FROM user where id = %d;"
#define SELECTUSERUSERNAMEDATA "SELECT * FROM user where username = '%s';"
#define SELECTUSERPASSWORDDATA "SELECT * FROM user where password = '%s';"
#define SELECTUSERTYPEDATA "SELECT * FROM user where type = %d;"
#define SELECTUSERNICKNAMEDATA "SELECT * FROM user where nickname = '%s';"
#define SELECTUSERCREATETIMEDATA "SELECT * FROM user where createtime = %f;"

#define UPDATEPARKINGIDDATA "update parking set id = %d where id = %d;"
#define UPDATEPARKINGNAMEDATA "update parking set name = '%s' where id = %d;"
#define UPDATEPARKINGNUMBERDATA "update parking set number = '%s' where id = %d;"
#define UPDATEPARKINGCREATETIMEDATA "update parking set createtime = %f where id = %d;"

#define UPDATEPLACEIDDATA "update place set id = %d where id = %d;"
#define UPDATEPLACEPINDEXDATA "update place set pindex = %d where id = %d;"
#define UPDATEPLACENAMEDATA "update place set name = '%s' where id = %d;"
#define UPDATEPLACENUMBERDATA "update place set number = '%s' where id = %d;"
#define UPDATEPLACEPOINTSDATA "update place set points = '%s' where id = %d;"
#define UPDATEPLACESTATUSDATA "update place set status = %d where id = %d;"
#define UPDATEPLACECREATETIMEDATA "update place set createtime = %f where id = %d;"

#define UPDATEUSERIDDATA "update user set id = %d where id = %d;"
#define UPDATEUSERUSERNAMEDATA "update user set username = '%s' where id = %d;"
#define UPDATEUSERPASSWORDDATA "update user set password = '%s' where id = %d;"
#define UPDATEUSERTYPEDATA "update user set type = %d where id = %d;"
#define UPDATEUSERNICKNAMEDATA "update user set nickname = '%s' where id = %d;"
#define UPDATEUSERCREATETIMEDATA "update user set createtime = %f where id = %d;"

typedef struct
{
    int id;
    char name[50];
    char number[30];
    float createtime;
}ParkingData;

typedef struct
{
    int id;
    int pindex;
    char name[50];
    char number[30];
    char points[1024];
    int status;
    float createtime;
}PlaceData;

typedef struct
{
    int id;
    char username[30];
    char password[30];
    int type;
    char nickname[30];
    float createtime;
}UserData;

/**
  * @name: InsertParkingData
  * @description: 向parking插入数据
  * @param db : 数据库文件指针
  * @param parkingdata : 数据结构体
  * @return -1:错误 0:正常
  */
int InsertParkingData(sqlite3 *db,ParkingData parkingdata);

/**
  * @name: InsertPlaceData
  * @description: 向place插入数据
  * @param db : 数据库文件指针
  * @param parkingdata : 数据结构体
  * @return -1:错误 0:正常
  */
int InsertPlaceData(sqlite3 *db,PlaceData placedata);

/**
  * @name: InsertUserData
  * @description: 向user插入数据
  * @param db : 数据库文件指针
  * @param parkingdata : 数据结构体
  * @return -1:错误 0:正常
  */
int InsertUserData(sqlite3 *db,UserData userdata);

/**
  * @name: UpdateParkingData
  * @description: 更新parking的数据
  * @param db : 数据库文件指针
  * @param updatename : 需要修改的对象名
  * @param updatecontent : 需要修改的内容
  * @param id : 需要修改的对象id
  * @return -1:错误 0:正常
  */
int UpdateParkingData(sqlite3 *db,char *updatename,void *updatecontent,int id);

/**
  * @name: UpdatePlaceData
  * @description: 更新place的数据
  * @param db : 数据库文件指针
  * @param updatename : 需要修改的对象名
  * @param updatecontent : 需要修改的内容
  * @param id : 需要修改的对象id
  * @return -1:错误 0:正常
  */
int UpdatePlaceData(sqlite3 *db,char *updatename,void *updatecontent,int id);

/**
  * @name: UpdateUserData
  * @description: 更新user的数据
  * @param db : 数据库文件指针
  * @param updatename : 需要修改的对象名
  * @param updatecontent : 需要修改的内容
  * @param id : 需要修改的对象id
  * @return -1:错误 0:正常
  */
int UpdateUserData(sqlite3 *db,char *updatename,void *updatecontent,int id);

/**
  * @name: DeleteParkingData
  * @description: 删除parking的数据
  * @param db : 数据库文件指针
  * @param id : 需要删除的对象id
  * @return -1:错误 0:正常
  */
int DeleteParkingData(sqlite3 *db,int id);

/**
  * @name: DeletePlaceData
  * @description: 删除place的数据
  * @param db : 数据库文件指针
  * @param id : 需要删除的对象id
  * @return -1:错误 0:正常
  */
int DeletePlaceData(sqlite3 *db,int id);

/**
  * @name: DeleteUserData
  * @description: 删除user的数据
  * @param db : 数据库文件指针
  * @param id : 需要删除的对象id
  * @return -1:错误 0:正常
  */
int DeleteUserData(sqlite3 *db,int id);

/**
  * @name: SelectParkingData
  * @description: 查找parking的数据
  * @param db : 数据库文件指针
  * @param parkingdata : 接受查询结果的结构体数组
  * @param selectname : 查询的对象名
  * @param selectcontent : 查询的对象内容
  * @return -1:错误 大于0:查询到的对象个数
  */
int SelectParkingData(sqlite3 *db,ParkingData *parkingdata,char *selectname,void *selectcontent);

/**
  * @name: SelectPlaceData
  * @description: 查找place的数据
  * @param db : 数据库文件指针
  * @param parkingdata : 接受查询结果的结构体数组
  * @param selectname : 查询的对象名
  * @param selectcontent : 查询的对象内容
  * @return -1:错误 大于0:查询到的对象个数
  */
int SelectPlaceData(sqlite3 *db,PlaceData *placedata,char *selectname,void *selectcontent);

/**
  * @name: SelectUserData
  * @description: 查找user的数据
  * @param db : 数据库文件指针
  * @param parkingdata : 接受查询结果的结构体数组
  * @param selectname : 查询的对象名
  * @param selectcontent : 查询的对象内容
  * @return -1:错误 大于0:查询到的对象个数
  */
int SelectUserData(sqlite3 *db,UserData *userdata,char *selectname,void *selectcontent);

#endif
