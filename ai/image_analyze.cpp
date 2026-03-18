#include <time.h>
#include <sys/time.h>
#include "my_http.h"
#include "common.h"

#include "comm_type.h"
#include "param.h"
#include "comm_codec.h"
//#include "comm_isp.h"
#include <rga/rga.h>
#include <rga/im2d.h>

#include "codec2_process.h"
#include "rkaiq/sample_common.h"

#include "osd_if.h"
#include "snapshot.h"
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>

#include "yolov5_detect.h"
#include "lprnet_detect_demo.h"

#include "parking_analyze.h"
#include "image_analyze.h"

using namespace cv;
using namespace std;

CarInfo carinfo;

extern rknn_context yolo_ctx;
extern rknn_context lprnet_ctx;

typedef struct avcodec_osd_point_s
{
	int x;
	int y;
}avcodec_osd_point_t;

void plot_box(coco_detect_result_t det_result_car)
{
	avcodec_osd_rect_t orect_car,orect_center,orect_left,orect_right;
	orect_car.index = 0;
	orect_car.enable = 0;
	orect_center.index = 1;
	orect_center.enable = 0;
	orect_left.index = 2;
	orect_left.enable = 0;
	orect_right.index = 3;
	orect_right.enable = 0;
	app_set_stream_osd_rect(CAM0_STREAM_MAIN, &orect_car);
	app_set_stream_osd_rect(CAM0_STREAM_MAIN, &orect_center);
	app_set_stream_osd_rect(CAM0_STREAM_MAIN, &orect_left);
	app_set_stream_osd_rect(CAM0_STREAM_MAIN, &orect_right);
	// os_dbg("----------avcodec_osd_point_t start------------");

	os_dbg("-----------%s---(%d %d %d %d)----------------\n",det_result_car.name,
															det_result_car.box.left,
															det_result_car.box.top,
															det_result_car.box.right,
															det_result_car.box.bottom);

	avcodec_osd_point_t point_center,point_left,point_right;
	point_center.x = (det_result_car.box.left + det_result_car.box.right)/2;
	point_center.y = (det_result_car.box.top + det_result_car.box.bottom)/2;
	point_left.x = (3 * det_result_car.box.left + det_result_car.box.right)/4;
	point_left.y = (3 * det_result_car.box.bottom + det_result_car.box.top)/4;
	point_right.x = (3 * det_result_car.box.right + det_result_car.box.left)/4;
	point_right.y = (3 * det_result_car.box.bottom + det_result_car.box.top)/4;
	// os_dbg("----------avcodec_osd_point_t end------------");

	// os_dbg("----------avcodec_osd_rect_t start------------");

	orect_car.enable = 1;
	orect_car.x = det_result_car.box.left;
	orect_car.y = det_result_car.box.top;
	orect_car.w = (det_result_car.box.right - det_result_car.box.left);
	orect_car.h = (det_result_car.box.bottom - det_result_car.box.top);
	orect_car.thick = 3;
	orect_car.color = 0xff0000;

	orect_center.enable = 1;
	orect_center.x = point_center.x;
	orect_center.y = point_center.y;
	orect_center.w = 3;
	orect_center.h = 3;
	orect_center.thick = 5;
	orect_center.color = 0x00ff00;

	orect_left.enable = 1;
	orect_left.x = point_left.x;
	orect_left.y = point_left.y;
	orect_left.w = 3;
	orect_left.h = 3;
	orect_left.thick = 5;
	orect_left.color = 0x0000ff;

	orect_right.enable = 1;
	orect_right.x = point_right.x;
	orect_right.y = point_right.y;
	orect_right.w = 3;
	orect_right.h = 3;
	orect_right.thick = 5;
	orect_right.color = 0xff0000;

	app_set_stream_osd_rect(CAM0_STREAM_MAIN, &orect_car);
	app_set_stream_osd_rect(CAM0_STREAM_MAIN, &orect_center);
	app_set_stream_osd_rect(CAM0_STREAM_MAIN, &orect_left);
	app_set_stream_osd_rect(CAM0_STREAM_MAIN, &orect_right);

	// os_dbg("----------avcodec_osd_rect_t end------------");
}

static int Get_Time(char *time_str)
{
    time_t local_time;
    struct tm *p;

	local_time = time(NULL);//获取当前时间sec
	p = localtime(&local_time);//本地时间

    snprintf(time_str,40,"%d/%02d/%02d %02d:%02d:%02d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

    printf("open log current time is %s\n",time_str);//打印当前时间

	return 0;
}

int Freetype_PutText(cv::Mat image,cv::Point org,cv::Scalar color,const char* str)
{
    os_dbg("-------------Freetype_PutText start--------------\n");

    cv::Ptr<cv::freetype::FreeType2> ft2;
    ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData( "/userdata/media/rv1126_app/simhei.ttf", 0 );

    os_dbg("-------------Freetype_PutText content %s--------------\n",str);

    // img表示将要添加文字的图像;
    // text表示需要添加的文本
    // org表示文件左下角坐标位置;
    // fontFace表示字体类型
    // fontScale表示问题缩放尺度,有一个默认的大小;
    // color表示图像颜色,其他如前述一致;
    // void putText( InputOutputArray img, const String& text, Point org,int fontFace, double fontScale, Scalar color,int thickness = 1, int lineType = LINE_8,bool bottomLeftOrigin = false );
    ft2->putText(image, str, org, 30, color, -1, 8, true );

    os_dbg("-------------Freetype_PutText end--------------\n");

    return 0;
}

//将场景图片与汽车图片拼接
int Splice_pic(int parking_id,int parking_status)
{
    vector<Mat> input(2);
    vector<Mat> temp(2);

    cv::Mat output_image;
    char imagename_input[256] = {'\0'};
    char imagename_car[256] = {'\0'};
    int  i;

    sprintf(imagename_input,"./output/parking_%d_input_%d.jpg",parking_id,parking_status);
    input[0] = cv::imread(imagename_input,1);
    sprintf(imagename_car,"./output/parking_%d_car_%d.jpg",parking_id,parking_status);
    input[1] = cv::imread(imagename_car,1);

    Size bigsize(input[0].cols * 2, input[0].rows*1);//合并后图片size

    output_image.create(bigsize, CV_MAKETYPE(input[0].depth(), 3));//rgb 3通道
    output_image = Scalar::all(0);

    for (i = 0; i < 2;i++)
    {
        temp[i] = output_image(Rect(i*input[0].cols, 0, input[0].cols, input[0].rows));
        input[i].copyTo(temp[i]); //copy图片到对应位置
    }

    char image_name[256] = {'\0'};

    memset(image_name,'\0',256);
    sprintf(image_name,"./output/parking_%d_whole_%d.jpg",parking_id,parking_status);
    cv::imwrite(image_name, output_image);

    return 0;
}

int Snapshot_once(cv::Mat input_image,cv::Mat car_image,int parking_index)
{
    char time_str[256] = {'\0'};

    char file_str_1[256] = {'\0'};
    char file_str_2[256] = {'\0'};
    char text_str_1[256] = {'\0'};
    char text_str_2[256] = {'\0'};

    Get_Time(time_str);

    sprintf(text_str_1,"%s 车牌信息[%s]",time_str,
                                        parkinginfo.Object[parking_index].carobject.number_plate);
    sprintf(text_str_2,"停车地点[%s] 状态[%d] 停车场[%d]",parkinginfo.ParkingName,
                                                        parkinginfo.Object[parking_index].ParkingStatus,
                                                        parkinginfo.Object[parking_index].ParkingID);

    os_dbg("-------------%s--------------\n",text_str_1);
    os_dbg("-------------%s--------------\n",text_str_2);

    cv::Mat resize_image;
    resize_image = car_image;
    resize(car_image, resize_image, Size(input_image.cols, input_image.rows), (0, 0), (0, 0), 3);
    line(resize_image, Point(0, 0), Point(2592, 0), Scalar(0, 0, 0), 150);
    Freetype_PutText(resize_image,Point(10, 25),Scalar(255, 255, 255),text_str_1);
    Freetype_PutText(resize_image,Point(10, 65),Scalar(255, 255, 255),text_str_2);

    sprintf(file_str_2,"./output/parking_%d_car_%d.jpg",parkinginfo.Object[parking_index].ParkingID,parkinginfo.Object[parking_index].ParkingStatus);
    cv::imwrite(file_str_2, resize_image);

    resize_image.release();

    cv::Rect rect;
    rect.x = parkinginfo.Object[parking_index].carobject.left_up_x;
	rect.y = parkinginfo.Object[parking_index].carobject.left_up_y;
	rect.width = parkinginfo.Object[parking_index].carobject.left_down_x - parkinginfo.Object[parking_index].carobject.left_up_x;
	rect.height = parkinginfo.Object[parking_index].carobject.left_down_y - parkinginfo.Object[parking_index].carobject.left_up_y;

    rectangle(input_image, rect, Scalar(0, 0,255), 3, 8,0);
    line(input_image, Point(0, 0), Point(2592, 0), Scalar(0, 0, 0), 150);
    Freetype_PutText(input_image,Point(10, 25),Scalar(255, 255, 255),text_str_1);
    Freetype_PutText(input_image,Point(10, 65),Scalar(255, 255, 255),text_str_2);

    sprintf(file_str_1,"./output/parking_%d_input_%d.jpg",parkinginfo.Object[parking_index].ParkingID,parkinginfo.Object[parking_index].ParkingStatus);
    cv::imwrite(file_str_1, input_image);

    Splice_pic(parkinginfo.Object[parking_index].ParkingID,parkinginfo.Object[parking_index].ParkingStatus);

    return 0;
}

//将图片上传
int Post_pic(int parking_id)
{
    vector<Mat> input(3);
    vector<Mat> temp(3);

    cv::Mat output_image;
    char imagename_whole[256] = {'\0'};
    int  i;

    //读取三张图片
    for (i = 0; i < 3; i++)
    {
        memset(imagename_whole,'\0',256);
        sprintf(imagename_whole,"./output/parking_%d_whole_%d.jpg",parking_id,i+1);
        input[i] = cv::imread(imagename_whole,1);
    }

    Size bigsize(input[0].cols * 1, input[0].rows * 3);//合并后图片size

    output_image.create(bigsize, CV_MAKETYPE(input[0].depth(), 3));//rgb 3通道
    output_image = Scalar::all(0);

    for (i = 0; i < 3;i++)
    {
        temp[i] = output_image(Rect(0 , i * input[0].rows, input[0].cols, input[0].rows));
        input[i].copyTo(temp[i]); //copy图片到对应位置
    }

    char image_name[256] = {'\0'};
    memset(image_name,'\0',256);
    sprintf(image_name,"./output/parking_%d_post.jpg",parking_id);
    cv::imwrite(image_name, output_image);

    http_post("http://120.48.16.252:32888/predict",image_name);

    return 0;
}

int Image_Plate_detect(cv::Mat car_image,int parking_index)
{
    int i = 0,j = 0,ret = -1;
    coco_detect_result_group_t detect_result_group_plate;

    ret = coco_detect_run(yolo_ctx,car_image,&detect_result_group_plate);
    if (ret == -1)
    {
        // 检测错误
        os_dbg("----------Image_Plate_detect failed----------------\n");
        return -1;
    }

    for (i = 0; i < detect_result_group_plate.count; i++)
    {
    	coco_detect_result_t *det_result_plate = &(detect_result_group_plate.results[i]);

    	if (det_result_plate->prop < 0.4)
    	{
    		continue;
    	}
    	if (strstr(det_result_plate->name,"plate"))
    	{
            os_dbg("-----------plate---(%d %d %d %d)----------------\n",det_result_plate->box.left,
                                                                        det_result_plate->box.top,
                                                                        det_result_plate->box.right,
                                                                        det_result_plate->box.bottom);

            cv::Mat bgr_plate_image,rgb_plate_image;
            rgb_plate_image = car_image(Range(det_result_plate->box.top,det_result_plate->box.bottom) , Range(det_result_plate->box.left,det_result_plate->box.right));
            cv::cvtColor(rgb_plate_image, bgr_plate_image, CV_RGB2BGR);

            // LPRNet_detect_run
            int Number_Index[7];
            ret = lprnet_detect_run(lprnet_ctx,bgr_plate_image,Number_Index);//做车牌检测

            if (ret != -1)
            {
                for (j = 0; j < 7; j++)
                {
                    memset(parkinginfo.Object[parking_index].carobject.number_plate[j],'\0',20);
                    strcpy(parkinginfo.Object[parking_index].carobject.number_plate[j],CHARS[Number_Index[j]]);
                    cout << parkinginfo.Object[parking_index].carobject.number_plate[j] << endl;
                }
            }
    	}
    }
    return 0;
}

int Image_Car_detect(cv::Mat input_image)
{
    int i = 0,j = 0,ret = -1;
    coco_detect_result_group_t detect_result_group_car;

    ret = coco_detect_run(yolo_ctx,input_image,&detect_result_group_car);
    if (ret == -1)
    {
        // 检测错误
        os_dbg("----------Image_Car_detect failed----------------\n");
        return -1;
    }
    else
    {
        // os_dbg("----------Image_Car_detect start----------------\n");
        j = 0;
        for(i = 0; i <  detect_result_group_car.count; i++)
        {
            coco_detect_result_t *det_result_car = &(detect_result_group_car.results[i]);

            // plot_box(*det_result_car);

            if (det_result_car->prop < 0.7)
            {
                continue;
            }
            if (strstr(det_result_car->name,"car"))
            {
                os_dbg("-----------car---(%d %d %d %d)----------------\n",det_result_car->box.left,
                                                                        det_result_car->box.top,
                                                                        det_result_car->box.right,
                                                                        det_result_car->box.bottom);

                ret  = CrossOrNotNew(det_result_car->box.left,det_result_car->box.top,det_result_car->box.right,det_result_car->box.bottom);
                os_dbg("----------ret:%d----------------\n",ret);

                if (ret > 0)
                {
                    for (int i = 0; i < parkinginfo.ParkingCount; i++)
                    {
                        if (parkinginfo.Object[i].ParkingID == ret)
                        {
                            parkinginfo.Object[i].carobject.left_up_x = det_result_car->box.left;
                            parkinginfo.Object[i].carobject.left_up_y = det_result_car->box.top;
                            parkinginfo.Object[i].carobject.left_down_x = det_result_car->box.right;
                            parkinginfo.Object[i].carobject.left_down_y = det_result_car->box.bottom;

                            cv::Mat car_image;
                            car_image = input_image(Range(det_result_car->box.top,det_result_car->box.bottom) , Range(det_result_car->box.left,det_result_car->box.right));

                            ret = Image_Plate_detect(car_image,i);
                            if (ret != -1)
                            {
                                Snapshot_once(input_image,car_image,i);

                                if (parkinginfo.Object[i].ParkingStatus == 3)
                                {
                                    Post_pic(parkinginfo.Object[i].ParkingID);
                                }
                            }
                        }
                    }
                }
            }
        }
        // os_dbg("----------Image_Car_detect end----------------\n");
    }
    return 0;
}


