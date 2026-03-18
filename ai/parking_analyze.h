#ifndef _PARKING_ANALYZE_H_
#define _PARKING_ANALYZE_H_

enum CarStatus
{
    CarStatus_Empty = 0,
    CarStatus_IN = 1,
    CarStatus_PARKING,
    CarStatus_OUT,
};

typedef struct
{
    int left_up_y;
    int left_up_x;
    int left_down_y;
    int left_down_x;
    char number_plate[7][20];
}CarObject;

typedef struct
{
    CarObject Object[8];
    int CarCount;
}CarInfo;

typedef struct
{
    int ParkingID;
    int ParkingStatus;
    float point1_x;
    float point1_y;
    float point2_x;
    float point2_y;
    float point3_x;
    float point3_y;
    float point4_x;
    float point4_y;
    CarObject carobject;
}ParkingObject;

typedef struct
{
    ParkingObject *Object;
    int ParkingCount;
    char ParkingName[256];
}ParkingInfo;

extern ParkingInfo parkinginfo;

int select_parkinginfo(void);
int CrossOrNotNew(int left_top_x,int left_top_y,int right_bottom_x,int right_bottom_y);

#endif
