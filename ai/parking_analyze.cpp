#include<stdlib.h>
#include<stdio.h>
#include "common.h"
#include "codec2_process.h"
#include "my_sqlfun.h"

#include "parking_analyze.h"

typedef struct Point{
    int x;
    int y;
}Point;

typedef struct PeriArea
{
    /* data */
    Point points[4];
    int stopStateFun;
    int stopIn1;
    int stopIn2;
}PeriArea;

static CROP_RECT crops[MAX_STREAM_NUM] = {
	{0, 0, 2592, 1944,},  // CAM0_STREAM_AI ifdef AI_FROM_STREAM0
	{100, 100, 640, 480,},
	{0, 0, 1920, 1080,},  // CAM1_STREAM_AI ifdef AI_FROM_STREAM0
	{100, 100, 640, 480,},
	{100, 100, 640, 480,},  // CAM0_STREAM_AI if not def AI_FROM_STREAM0
	{100, 100, 640, 480,},  // CAM1_STREAM_AI if not def AI_FROM_STREAM0

};

ParkingInfo parkinginfo;

/**
  * @name: select_parkinginfo
  * @description: 测试解析车位信息数据
  * @return -1:错误 0:正常
  */
int select_parkinginfo(void)
{
    sqlite3 *db=NULL;
    char *errmsg;
    char sql[SQLMAXSIZE];

    int ret = -1;
    int i;

    //打开指定的数据库文件,如果不存在将创建一个同名的数据库文件
    char file[500] = {'\0'};

    sprintf(file,"%s%s",FILEPATH,FILENAME);

    ret = sqlite3_open(file, &db);
    if( ret != SQLITE_OK )
    {
        os_dbg("Can't open(creat) database: %s\n", sqlite3_errmsg(db));

        return -1;
    }

    //查询Place数据
    PlaceData placedata[10];
    char str_points[1024] = {'\0'};

    ret = SelectPlaceData(db,placedata,"",NULL);

    parkinginfo.Object = (ParkingObject *)malloc(sizeof(ParkingObject) * ret);
    parkinginfo.ParkingCount = ret;
    for (i = 0; i < parkinginfo.ParkingCount; i++)
    {
        memset(str_points,'\0',1024);
        memcpy(str_points,placedata[i].points,strlen(placedata[i].points));
        sscanf(str_points,"%f,%f_%f,%f_%f,%f_%f,%f",&parkinginfo.Object[i].point1_x,&parkinginfo.Object[i].point1_y,
                                                    &parkinginfo.Object[i].point2_x,&parkinginfo.Object[i].point2_y,
                                                    &parkinginfo.Object[i].point3_x,&parkinginfo.Object[i].point3_y,
                                                    &parkinginfo.Object[i].point4_x,&parkinginfo.Object[i].point4_y);
        parkinginfo.Object[i].ParkingID = placedata[i].pindex;


        parkinginfo.Object[i].point1_x*=crops[0].w;
        parkinginfo.Object[i].point1_y*=crops[0].h;

        parkinginfo.Object[i].point2_x*=crops[0].w;
        parkinginfo.Object[i].point2_y*=crops[0].h;


        parkinginfo.Object[i].point3_x*=crops[0].w;
        parkinginfo.Object[i].point3_y*=crops[0].h;


        parkinginfo.Object[i].point4_x*=crops[0].w;
        parkinginfo.Object[i].point4_y*=crops[0].h;
    }

    //查询Parking数据

    ParkingData parkingdata;
    ret = SelectParkingData(db,&parkingdata,"",NULL);
    sprintf(parkinginfo.ParkingName,"%s%s",parkingdata.name,parkingdata.number);

    ret = sqlite3_close(db); //关闭数据库
    if( ret != SQLITE_OK )
    {
        os_dbg("Can't close database: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

/**
  * @name: judgeCros
  * @description: 判断车辆是否在车位上
  * @return -1:代表该车没有在任何车位里 大于0:代表在某个车位上
  */
int judgeCros(int x,int y,int parking_id)
{
    int i;
    for(i=0;i < parkinginfo.ParkingCount;i++){
        if (parking_id == parkinginfo.Object[i].ParkingID)
        {
            int a, b, c,d;//分别存四个向量的计算结果;

            a = (parkinginfo.Object[i].point2_x - parkinginfo.Object[i].point1_x)*(y - parkinginfo.Object[i].point1_y) - (parkinginfo.Object[i].point2_y - parkinginfo.Object[i].point1_x)*(x - parkinginfo.Object[i].point1_x);
            b = (parkinginfo.Object[i].point3_x - parkinginfo.Object[i].point2_x)*(y - parkinginfo.Object[i].point2_y) - (parkinginfo.Object[i].point3_y - parkinginfo.Object[i].point2_x)*(x - parkinginfo.Object[i].point2_x);
            c = (parkinginfo.Object[i].point4_x - parkinginfo.Object[i].point3_x)*(y - parkinginfo.Object[i].point3_y) - (parkinginfo.Object[i].point4_y - parkinginfo.Object[i].point3_x)*(x - parkinginfo.Object[i].point3_x);
            d = (parkinginfo.Object[i].point1_x - parkinginfo.Object[i].point4_x)*(y - parkinginfo.Object[i].point4_y) - (parkinginfo.Object[i].point1_y - parkinginfo.Object[i].point4_x)*(x - parkinginfo.Object[i].point4_x);

            if((a > 0 && b > 0 && c > 0 && d > 0) || (a < 0 && b < 0 && c < 0 && d < 0))
            {
                return parkinginfo.Object[i].ParkingID;
            }
        }
    }

    return 0;
}

/**
  * @name: CrossOrNotNew
  * @description: 判断车辆状态
  * @return 0:代表该车没有停稳、入库、出库 其他:返回所在的车位
  */
int CrossOrNotNew(int left_top_x,int left_top_y,int right_bottom_x,int right_bottom_y)
{
    int i;
    int stop_status1 = -1,stop_status2 = -1;

    for(i = 0;i < parkinginfo.ParkingCount;i++){
        int x1 = 0,x2 = 0,y1 = 0,y2 = 0;
        int StateFun = parkinginfo.Object[i].ParkingID;
        os_dbg("\n----------StateFun :%d----------------\n\n",StateFun);

/*******************************************************/
        //选择一个合适的停车位 停稳 好的判断方法
/*******************************************************/
        if(StateFun == 1)
        {
            x1=(3 * left_top_x + right_bottom_x)/4;
            y1=(left_top_y + 3 * right_bottom_y)/4;
            x2=(left_top_x + 3 * right_bottom_x)/4;
            y2=(left_top_y + 3 * right_bottom_y)/4;
        }
        else if(StateFun == 2 || StateFun == 3 || StateFun == 4){
            x1=(3 * left_top_x + right_bottom_x)/4;
            y1=(left_top_y + right_bottom_y)/2;
            x2=(3 * left_top_x + right_bottom_x)/4;
            y2=(left_top_y + 3 * right_bottom_y)/4;
        }

        stop_status1 = judgeCros(x1,y1,StateFun);
        stop_status2 = judgeCros(x2,y2,StateFun);

        if(stop_status1 > 0 && stop_status2 > 0)
        {
            if (parkinginfo.Object[i].ParkingStatus == CarStatus_IN)
            {
                parkinginfo.Object[i].ParkingStatus = CarStatus_PARKING;
                return parkinginfo.Object[i].ParkingID;
            }
        }

/*******************************************************/
        //选择一个合适的停车 出,入 位方法
/*******************************************************/
        if(StateFun == 1)
        {
            x1=(3 * left_top_x + right_bottom_x)/4;
            y1=(3 * left_top_y + right_bottom_y)/4;
            x2=(3 * left_top_x + right_bottom_x)/4;
            y2=(left_top_y + 3 * right_bottom_y)/4;
        }
        else if(StateFun == 2 || StateFun == 3 || StateFun == 4){
            x1=(3 * left_top_x + right_bottom_x)/4;
            y1=(left_top_y + right_bottom_y)/2;
            x2=(3 * left_top_x + right_bottom_x)/4;
            y2=(left_top_y + 3 * right_bottom_y)/4;
        }

        stop_status1 = judgeCros(x1,y1,StateFun);
        stop_status2 = judgeCros(x2,y2,StateFun);

        if(stop_status1 > 0 && stop_status2 > 0)
        {
            if(parkinginfo.Object[i].ParkingStatus == CarStatus_PARKING)
            {
                parkinginfo.Object[i].ParkingStatus = CarStatus_OUT;
                return parkinginfo.Object[i].ParkingID;
            }
            else if (parkinginfo.Object[i].ParkingStatus == CarStatus_Empty)
            {
                parkinginfo.Object[i].ParkingStatus = CarStatus_IN;
                return parkinginfo.Object[i].ParkingID;
            }
        }
    }

    return 0;
}


