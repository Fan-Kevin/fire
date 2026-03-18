#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "common.h"
#include "my_sqlfun.h"

//添加数据

/**
  * @name: InsertParkingData
  * @description: 向parking插入数据
  * @param db : 数据库文件指针
  * @param parkingdata : 数据结构体
  * @return -1:错误 0:正常
  */
int InsertParkingData(sqlite3 *db,ParkingData parkingdata)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    sprintf(sql,INSERTPARKINGDATA,parkingdata.name,
                                parkingdata.number,
                                parkingdata.createtime);

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if(ret != SQLITE_OK)
    {
        os_dbg("Can't insert parking database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

/**
  * @name: InsertPlaceData
  * @description: 向place插入数据
  * @param db : 数据库文件指针
  * @param parkingdata : 数据结构体
  * @return -1:错误 0:正常
  */
int InsertPlaceData(sqlite3 *db,PlaceData placedata)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    sprintf(sql,INSERTPLACEDATA,placedata.pindex,
                                placedata.name,
                                placedata.number,
                                placedata.points,
                                placedata.status,
                                placedata.createtime);

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if( ret != SQLITE_OK  )
    {
        os_dbg("Can't insert place database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

/**
  * @name: InsertUserData
  * @description: 向user插入数据
  * @param db : 数据库文件指针
  * @param parkingdata : 数据结构体
  * @return -1:错误 0:正常
  */
int InsertUserData(sqlite3 *db,UserData userdata)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    sprintf(sql,INSERTUSERDATA,userdata.username,
                                userdata.password,
                                userdata.type,
                                userdata.nickname,
                                userdata.createtime);

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if( ret != SQLITE_OK  )
    {
        os_dbg("Can't insert user database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

//更新数据

/**
  * @name: UpdateParkingData
  * @description: 更新parking的数据
  * @param db : 数据库文件指针
  * @param updatename : 需要修改的对象名
  * @param updatecontent : 需要修改的内容
  * @param id : 需要修改的对象id
  * @return -1:错误 0:正常
  */
int UpdateParkingData(sqlite3 *db,char *updatename,void *updatecontent,int id)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    if (strstr(updatename,"id") != NULL)
    {
        sprintf(sql,UPDATEPARKINGIDDATA,*(int *)updatecontent,id);
    }
    else if (strstr(updatename,"name") != NULL)
    {
        sprintf(sql,UPDATEPARKINGNAMEDATA,(char *)updatecontent,id);
    }
    else if (strstr(updatename,"number") != NULL)
    {
        sprintf(sql,UPDATEPARKINGNUMBERDATA,(char *)updatecontent,id);
    }
    else if (strstr(updatename,"createtime") != NULL)
    {
        sprintf(sql,UPDATEPARKINGCREATETIMEDATA,*(float *)updatecontent,id);
    }

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if( ret != SQLITE_OK  )
    {
        os_dbg("Can't update parking database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

/**
  * @name: UpdatePlaceData
  * @description: 更新place的数据
  * @param db : 数据库文件指针
  * @param updatename : 需要修改的对象名
  * @param updatecontent : 需要修改的内容
  * @param id : 需要修改的对象id
  * @return -1:错误 0:正常
  */
int UpdatePlaceData(sqlite3 *db,char *updatename,void *updatecontent,int id)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    if (strstr(updatename,"id") != NULL)
    {
        sprintf(sql,UPDATEPLACEIDDATA,*(int *)updatecontent,id);
    }
    else if (strstr(updatename,"pindex") != NULL)
    {
        sprintf(sql,UPDATEPLACEPINDEXDATA,*(int *)updatecontent,id);
    }
    else if (strstr(updatename,"name") != NULL)
    {
        sprintf(sql,UPDATEPLACENAMEDATA,(char *)updatecontent,id);
    }
    else if (strstr(updatename,"number") != NULL)
    {
        sprintf(sql,UPDATEPLACENUMBERDATA,(char *)updatecontent,id);
    }
    else if (strstr(updatename,"points") != NULL)
    {
        sprintf(sql,UPDATEPLACEPOINTSDATA,(char *)updatecontent,id);
    }
    else if (strstr(updatename,"status") != NULL)
    {
        sprintf(sql,UPDATEPLACESTATUSDATA,*(int *)updatecontent,id);
    }
    else if (strstr(updatename,"createtime") != NULL)
    {
        sprintf(sql,UPDATEPLACECREATETIMEDATA,*(float *)updatecontent,id);
    }

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if( ret != SQLITE_OK  )
    {
        os_dbg("Can't update place database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

/**
  * @name: UpdateUserData
  * @description: 更新user的数据
  * @param db : 数据库文件指针
  * @param updatename : 需要修改的对象名
  * @param updatecontent : 需要修改的内容
  * @param id : 需要修改的对象id
  * @return -1:错误 0:正常
  */
int UpdateUserData(sqlite3 *db,char *updatename,void *updatecontent,int id)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    if (strstr(updatename,"id") != NULL)
    {
        sprintf(sql,UPDATEUSERIDDATA,*(int *)updatecontent,id);
    }
    else if (strstr(updatename,"username") != NULL)
    {
        sprintf(sql,UPDATEUSERUSERNAMEDATA,(char *)updatecontent,id);
    }
    else if (strstr(updatename,"password") != NULL)
    {
        sprintf(sql,UPDATEUSERPASSWORDDATA,(char *)updatecontent,id);
    }
    else if (strstr(updatename,"type") != NULL)
    {
        sprintf(sql,UPDATEUSERTYPEDATA,*(int *)updatecontent,id);
    }
    else if (strstr(updatename,"nickname") != NULL)
    {
        sprintf(sql,UPDATEUSERNICKNAMEDATA,(char *)updatecontent,id);
    }
    else if (strstr(updatename,"createtime") != NULL)
    {
        sprintf(sql,UPDATEUSERCREATETIMEDATA,*(float *)updatecontent,id);
    }

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if( ret != SQLITE_OK  )
    {
        os_dbg("Can't update user database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

//删除数据

/**
  * @name: DeleteParkingData
  * @description: 删除parking的数据
  * @param db : 数据库文件指针
  * @param id : 需要删除的对象id
  * @return -1:错误 0:正常
  */
int DeleteParkingData(sqlite3 *db,int id)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    if (id == 0)
    {
        sprintf(sql,DELETEALLPARKINGDATA);
    }
    else
    {
        sprintf(sql,DELETEPARKINGDATA,id);
    }

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if( ret != SQLITE_OK  )
    {
        os_dbg("Can't delet parking database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

/**
  * @name: DeletePlaceData
  * @description: 删除place的数据
  * @param db : 数据库文件指针
  * @param id : 需要删除的对象id
  * @return -1:错误 0:正常
  */
int DeletePlaceData(sqlite3 *db,int id)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    if (id == 0)
    {
        sprintf(sql,DELETEALLPLACEDATA);
    }
    else
    {
        sprintf(sql,DELETEPLACEDATA,id);
    }

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if( ret != SQLITE_OK  )
    {
        os_dbg("Can't delet place database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

/**
  * @name: DeleteUserData
  * @description: 删除user的数据
  * @param db : 数据库文件指针
  * @param id : 需要删除的对象id
  * @return -1:错误 0:正常
  */
int DeleteUserData(sqlite3 *db,int id)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];

    if (id == 0)
    {
        sprintf(sql,DELETEALLUSERDATA);
    }
    else
    {
        sprintf(sql,DELETEUSERDATA,id);
    }

    ret = sqlite3_exec( db , sql , NULL , NULL , &errmsg );

    if( ret != SQLITE_OK  )
    {
        os_dbg("Can't delet user database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    return 0;
}

//查找数据

/**
  * @name: SelectParkingData
  * @description: 查找parking的数据
  * @param db : 数据库文件指针
  * @param parkingdata : 接受查询结果的结构体数组
  * @param selectname : 查询的对象名
  * @param selectcontent : 查询的对象内容
  * @return -1:错误 大于0:查询到的对象个数
  */
int SelectParkingData(sqlite3 *db,ParkingData *parkingdata,char *selectname,void *selectcontent)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];
    char **Result;
    int row,column;

    if (strstr(selectname,"id") != NULL)
    {
        sprintf(sql,SELECTPARKINGIDDATA,*(int *)selectcontent);
    }
    else if (strstr(selectname,"name") != NULL)
    {
        sprintf(sql,SELECTPARKINGNAMEDATA,(char *)selectcontent);
    }
    else if (strstr(selectname,"number") != NULL)
    {
        sprintf(sql,SELECTPARKINGNUMBERDATA,(char *)selectcontent);
    }
    else if (strstr(selectname,"createtime") != NULL)
    {
        sprintf(sql,SELECTPARKINGCREATETIMEDATA,*(float *)selectcontent);
    }
    else
    {
        sprintf(sql,SELECTPARKINGALLDATA);
    }

    ret = sqlite3_get_table( db , sql , &Result , &row , &column , &errmsg );

    if(ret == 0)
    {
        parkingdata->id = atoi(Result[(1)*column+0]);
        strcpy(parkingdata->name,Result[(1)*column+1]);
        strcpy(parkingdata->number,Result[(1)*column+2]);
        parkingdata->createtime = strtof(Result[(1)*column+3],NULL);
    }
    else
    {
        os_dbg("Can't select parking database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    sqlite3_free_table(Result);

    return row;
}

/**
  * @name: SelectPlaceData
  * @description: 查找place的数据
  * @param db : 数据库文件指针
  * @param parkingdata : 接受查询结果的结构体数组
  * @param selectname : 查询的对象名
  * @param selectcontent : 查询的对象内容
  * @return -1:错误 大于0:查询到的对象个数
  */
int SelectPlaceData(sqlite3 *db,PlaceData *placedata,char *selectname,void *selectcontent)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];
    char **Result;
    int row,column;

    if (strstr(selectname,"id") != NULL)
    {
        sprintf(sql,SELECTPLACEIDDATA,*(int *)selectcontent);
    }
    else if (strstr(selectname,"pindex") != NULL)
    {
        sprintf(sql,SELECTPLACEPINDEXDATA,*(int *)selectcontent);
    }
    else if (strstr(selectname,"name") != NULL)
    {
        sprintf(sql,SELECTPLACENAMEDATA,(char *)selectcontent);
    }
    else if (strstr(selectname,"number") != NULL)
    {
        sprintf(sql,SELECTPLACENUMBERDATA,(char *)selectcontent);
    }
    else if (strstr(selectname,"points") != NULL)
    {
        sprintf(sql,SELECTPLACEPOINTSDATA,(char *)selectcontent);
    }
    else if (strstr(selectname,"status") != NULL)
    {
        sprintf(sql,SELECTPLACESTATUSDATA,*(int *)selectcontent);
    }
    else if (strstr(selectname,"createtime") != NULL)
    {
        sprintf(sql,SELECTPLACECREATETIMEDATA,*(float *)selectcontent);
    }
    else
    {
        sprintf(sql,SELECTPLACEALLDATA);
    }

    ret = sqlite3_get_table( db , sql , &Result , &row , &column , &errmsg );
    if(ret == 0)
    {
        int i;
        for (i = 0; i < row; i++)
        {
            placedata[i].id = atoi(Result[(i+1)*column+0]);
            placedata[i].pindex = atoi(Result[(i+1)*column+1]);
            strcpy(placedata[i].name,Result[(i+1)*column+2]);
            strcpy(placedata[i].number,Result[(i+1)*column+3]);
            strcpy(placedata[i].points,Result[(i+1)*column+4]);
            placedata[i].status = atoi(Result[(i+1)*column+5]);
            placedata[i].createtime = strtof(Result[(i+1)*column+6],NULL);
        }
    }
    else
    {
        os_dbg("Can't select place database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    sqlite3_free_table(Result);

    return row;
}

/**
  * @name: SelectUserData
  * @description: 查找user的数据
  * @param db : 数据库文件指针
  * @param parkingdata : 接受查询结果的结构体数组
  * @param selectname : 查询的对象名
  * @param selectcontent : 查询的对象内容
  * @return -1:错误 大于0:查询到的对象个数
  */
int SelectUserData(sqlite3 *db,UserData *userdata,char *selectname,void *selectcontent)
{
    int ret;
    char *errmsg;
    char sql[SQLMAXSIZE];
    char **Result;
    int row,column;

    if (strstr(selectname,"id") != NULL)
    {
        sprintf(sql,SELECTUSERIDDATA,*(int *)selectcontent);
    }
    else if (strstr(selectname,"username") != NULL)
    {
        sprintf(sql,SELECTUSERUSERNAMEDATA,(char *)selectcontent);
    }
    else if (strstr(selectname,"password") != NULL)
    {
        sprintf(sql,SELECTUSERPASSWORDDATA,(char *)selectcontent);
    }
    else if (strstr(selectname,"type") != NULL)
    {
        sprintf(sql,SELECTUSERTYPEDATA,*(int *)selectcontent);
    }
    else if (strstr(selectname,"nickname") != NULL)
    {
        sprintf(sql,SELECTUSERNICKNAMEDATA,(char *)selectcontent);
    }
    else if (strstr(selectname,"createtime") != NULL)
    {
        sprintf(sql,SELECTUSERCREATETIMEDATA,*(float *)selectcontent);
    }
    else
    {
        sprintf(sql,SELECTUSERALLDATA);
    }


    ret = sqlite3_get_table( db , sql , &Result , &row , &column , &errmsg );

    if(ret == 0)
    {
        int i;
        for (i = 0; i < row; i++)
        {
            userdata[i].id = atoi(Result[(i+1)*column+0]);
            strcpy(userdata[i].username,Result[(i+1)*column+1]);
            strcpy(userdata[i].password,Result[(i+1)*column+2]);
            userdata[i].type= atoi(Result[(i+1)*column+3]);
            strcpy(userdata[i].nickname,Result[(i+1)*column+4]);
            userdata[i].createtime = strtof(Result[(i+1)*column+5],NULL);
        }
    }
    else
    {
        os_dbg("Can't select user database: %s\n",sqlite3_errmsg(db));
        sqlite3_free(errmsg);//释放内存空间
        return -1;
    }

    sqlite3_free_table(Result);

    return row;
}

// int test(void)
// {
//     sqlite3 *db=NULL;
//     char *errmsg;
//     char sql[SQLMAXSIZE];

//     int ret = -1;

//     //打开指定的数据库文件,如果不存在将创建一个同名的数据库文件
//     char *file;
//     file = (char *)malloc(sizeof(char) * 256);
//     sprintf(file,"%s%s",FILEPATH,FILENAME);

//     ret = sqlite3_open(file, &db);
//     if( ret != SQLITE_OK )
//     {
//         printf("Can't open(creat) database: %s\n", sqlite3_errmsg(db));

//         return -1;
//     }
//     else
//     {
//         printf("You have opened a sqlite3 database %s successfully!\n",file);
//     }

//     //添加Parking数据
//     // ParkingData parkingdata;
//     // sprintf(data.name,"停车场3");
//     // sprintf(data.number,"p001");
//     // data.createtime = 4.001;
//     // InsertParkingData(db,data);

//     //查询Parking数据

//     // ParkingData parkingdata[5];
//     // ret = SelectParkingData(db,parkingdata,"name","停车场3");
//     // int i;
//     // for (i = 0; i < ret; i++)
//     // {
//     //     printf("---id=%d---name=%s---number=%s---createtime=%f---\n",parkingdata[i].id,
//     //                                                                  parkingdata[i].name,
//     //                                                                  parkingdata[i].number,
//     //                                                                  parkingdata[i].createtime);
//     // }

//     //更新Parking数据
//     // double time = 1;
//     // UpdateParkingData(db,"createtime",&time,5);

//     //删除Parking数据
//     // DeleteParkingData(db,5);


//     //添加Place数据
//     // PlaceData placedata;
//     // placedata.pindex = 1;
//     // sprintf(placedata.name,"停车场3");
//     // sprintf(placedata.number,"p001");
//     // sprintf(placedata.points,"x=1,y=2");
//     // placedata.status = 1;
//     // placedata.createtime = 4.001;
//     // InsertPlaceData(db,placedata);

//     //查询Place数据
//     // PlaceData placedata[5];
//     // int pindex = 1;
//     // ret = SelectPlaceData(db,placedata,"pindex",&pindex);
//     // int i;
//     // for (i = 0; i < ret; i++)
//     // {
//     //     printf("---id=%d---pindex=%d---name=%s---number=%s---points=%s---status=%d---createtime=%f---\n",placedata[i].id,
//     //                                                                                                      placedata[i].pindex,
//     //                                                                                                      placedata[i].name,
//     //                                                                                                      placedata[i].number,
//     //                                                                                                      placedata[i].points,
//     //                                                                                                      placedata[i].status,
//     //                                                                                                      placedata[i].createtime);
//     // }

//     //更新Place数据
//     // UpdatePlaceData(db,"number","p002",5);

//     //删除Place数据
//     // DeletePlaceData(db,5);


//     //添加User数据
//     // UserData userdata;
//     // sprintf(userdata.username,"xauat");
//     // sprintf(userdata.password,"123456");
//     // userdata.type = 1;
//     // sprintf(userdata.nickname,"管理员");
//     // userdata.createtime = 4.001;
//     // InsertUserData(db,userdata);

//     // //查询User数据
//     // UserData userdata[5];
//     // ret = SelectUserData(db,userdata,"password","123456");
//     // int i;
//     // for (i = 0; i < ret; i++)
//     // {
//     //     printf("---id=%d---username=%s---password=%s---type=%d---nickname=%s---createtime=%f---\n",userdata[i].id,
//     //                                                                                                userdata[i].username,
//     //                                                                                                userdata[i].password,
//     //                                                                                                userdata[i].type,
//     //                                                                                                userdata[i].nickname,
//     //                                                                                                userdata[i].createtime);
//     // }

//     //更新Place数据
//     // UpdateUserData(db,"password","abcdefg",4);

//     //删除Place数据
//     // DeleteUserData(db,4);

//     ret = sqlite3_close(db); //关闭数据库
//     if( ret != SQLITE_OK )
//     {
//         printf("Can't close database: %s\n", sqlite3_errmsg(db));
//         return -1;
//     }
//     else
//     {
//         printf("You have closed a sqlite3 database successfully!\n");
//     }

//     return 0;
// }

