
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#include "rknn_api.h"

#include <sys/time.h>


#include "lprnet_detect_demo.h"
#include "rknn_api.h"

using namespace cv;
using namespace std;



char CHARS[][20] = {"京", "沪", "津", "渝", "冀", "晋", "蒙", "辽", "吉", "黑",
						"苏", "浙", "皖", "闽", "赣", "鲁", "豫", "鄂", "湘", "粤",
						"桂", "琼", "川", "贵", "云", "藏", "陕", "甘", "青", "宁",
						"新",
						"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
						"A", "B", "C", "D", "E", "F", "G", "H", "J", "K",
						"L", "M", "N", "P", "Q", "R", "S", "T", "U", "V",
						"W", "X", "Y", "Z", "I", "O", "-"};


        // char CHARS[100][20] = {"这", "是", "一", "个", "数", "组"};



int lprnet_detect_release(rknn_context ctx)
{
    rknn_destroy(ctx);
	return 0;
}

static int letter_box(cv::Mat input_image, cv::Mat *output_image, int model_input_size)
{
	int input_width, input_height;

	input_width = input_image.cols;
	input_height = input_image.rows;
	float ratio;
	ratio = min((float)model_input_size / input_width, (float)model_input_size / input_height);

	int new_width, new_height;
	new_width = round(ratio * input_width );
	new_height = round(ratio * input_height);


	int height_padding = 0;
	int width_padding = 0;
	int top = 0;
	int bottom = 0;
	int left = 0;
	int right = 0;
	if( new_width >= new_height)
	{
		height_padding = new_width - new_height;
		if( (height_padding % 2) == 0 )
		{
			top = (int)((float)(height_padding/2));
			bottom = (int)((float)(height_padding/2));
		}
		else
		{
			top = (int)((float)(height_padding/2));
			bottom = (int)((float)(height_padding/2))+1;
		}
	}
	else
	{
		width_padding = new_height - new_width;
		if( (width_padding % 2) == 0 )
		{
			left = (int)((float)(width_padding/2));
			right = (int)((float)(width_padding/2));
		}
		else
		{
			left = (int)((float)(width_padding/2));
			right = (int)((float)(width_padding/2))+1;
		}

	}

	cv::Mat resize_img;

	cv::resize(input_image, resize_img, cv::Size(new_width, new_height));
	cv::copyMakeBorder(resize_img, *output_image, top, bottom, left, right, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

	return 0;
}


float Float16ToFloat( short fltInt16 )
{
    int fltInt32    =  ((fltInt16 & 0x8000) << 16);
    fltInt32        |= ((fltInt16 & 0x7fff) << 13) + 0x38000000;

    float fRet;
    memcpy( &fRet, &fltInt32, sizeof( float ) );
    return fRet;
 }

static float deqnt_affine_to_f32(uint16_t qnt, uint16_t zp, float scale)
{
    return ((float)qnt - (float)zp) * scale;
}



static int checkProvince(float *input,int province){


    int max3Index[]={0,1,2};


    for(int i=0;i<3;i++){
        int max=input[i];
        int maxI=i;
        for(int j=i;j<10;j++){
           if(input[j]>max){
               maxI=j;
               max=input[j];
           }
        }
        int tmp=input[maxI];
        input[maxI]=input[i];
        input[i]=tmp;
        max3Index[i]=maxI;
    }//插入排序,获取最大的三个值

//    for(int i=0;i< 10;i++){
//        printf("%f ",input[i]);
//    }
//    printf("\n%d %d %d\n",max3Index[0],max3Index[1],max3Index[2]);

    if(province == max3Index[0] ||province==max3Index[1] || province==max3Index[2]){
        return province;
    }else{
        return -1;
    }
}




static int process_f16(short *input, int sequence[10],int *num,int province)
{

    *num=0;
	int first=1;
	int j=0;
	for(int i=0;i<18;i++){
		j=0;
		float max=Float16ToFloat( input[i+j*18]);
		int argmaxI=0;
		for(;j<68;j++){
			float _fp32=Float16ToFloat( input[i+j*18]);
			if(_fp32>max){
				max=_fp32;
				argmaxI=j;
			}
		}//获取到argmax index


		if (argmaxI!=67){
			if(first==1 && province!=-1){

				float tmpf16[68];
				for(int m=0;m<68;m++){
					tmpf16[m] = Float16ToFloat(input[i+m*18]);
				}

				int new_argmaxI=checkProvince(tmpf16,province);
				if(new_argmaxI!=-1){
					argmaxI=new_argmaxI;
				}
				first=2;
			}//如果是第一次,并且需要省份矫正,执行条件

			sequence[*num]=argmaxI;
			*num = *num+1;

		}//保证是有效数字

	}

    return 1;
}





int lprnet_process_f16(short *input0, int buf[7] ,int province)
{

	int sequenceNum=0;
	int sequence[15];


// 1. argmax
    process_f16(input0,sequence,&sequenceNum,province);

	for(int i=0;i<sequenceNum ; i++){
		// printf("%d " ,sequence[i]);
	}
	// printf("\n");



    if(sequenceNum<7){
        return -1;
    }

    int tmp=sequence[0];
    int j=1;
    for(int i=1;i<sequenceNum;i++){
        if(sequence[i] == tmp){
            continue;
        }
        tmp=sequence[i];
        sequence[j]=sequence[i];
        j++;
    }

    for(int i=0;i<j ; i++){
        printf("%d \n" ,sequence[i]);
		buf[i] = sequence[i];
		// cout << CHARS[sequence[i]] <<endl;
    }

    return 0;
}




static void printRKNNTensor(rknn_tensor_attr *attr)
{
    printf("index=%d name=%s n_dims=%d dims=[%d %d %d %d] n_elems=%d size=%d "
           "fmt=%d type=%d qnt_type=%d fl=%d zp=%d scale=%f\n",
           attr->index, attr->name, attr->n_dims, attr->dims[3], attr->dims[2],
           attr->dims[1], attr->dims[0], attr->n_elems, attr->size, 0, attr->type,
           attr->qnt_type, attr->fl, attr->zp, attr->scale);
}





int lprnet_detect_run(rknn_context ctx, cv::Mat input_image,int buf[7])
{

	int ret;
	size_t actual_size = 0;

	// int img_width = 0;
	// int img_height = 0;
	// int img_channel = 0;


	// const float vis_threshold = 0.1;
	// const float nms_threshold = 0.5;
	// const float conf_threshold = 0.2;


	// img_width = input_image.cols;
	// img_height = input_image.rows;




/***********************/
// 1.  查看版本
/***********************/
	rknn_sdk_version version;
	ret = rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &version,
					 sizeof(rknn_sdk_version));
	if (ret < 0)
	{
		printf("rknn_init error ret=%d\n", ret);
		return -1;
	}
	/*
	printf("sdk version: %s driver version: %s\n", version.api_version,
		   version.drv_version);
	*/

/***********************/
//  2. 查询模型输入输出 Tensor 的个数
/***********************/
	rknn_input_output_num io_num;
	ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
	if (ret < 0)
	{
		printf("rknn_init error ret=%d\n", ret);
		return -1;
	}

	printf("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);

/***********************/
//  3. 查询输入 in Tensor 属性
/***********************/


	rknn_tensor_attr input_attrs[io_num.n_input];
	memset(input_attrs, 0, sizeof(input_attrs));
	for (int i = 0; i < io_num.n_input; i++)
	{
		input_attrs[i].index = i;
		ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]),
						 sizeof(rknn_tensor_attr));
		if (ret < 0)
		{
			printf("rknn_init error ret=%d\n", ret);
			return -1;
		}

		// printRKNNTensor(&(input_attrs[i]));
	}


/***********************/
//  4. 查询输出 out Tensor 属性
/***********************/
	rknn_tensor_attr output_attrs[io_num.n_output];
	memset(output_attrs, 0, sizeof(output_attrs));
	for (int i = 0; i < io_num.n_output; i++)
	{
		output_attrs[i].index = i;
		ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[i]),
						 sizeof(rknn_tensor_attr));

		// printf("model input num: %d, output num: %d\n", output_attrs[i].n_input,
		// output_attrs[i].n_output);
		// printRKNNTensor(&(output_attrs[i]));
	}


	int input_channel = 3;
	int input_width = 0;
	int input_height = 0;
	if (input_attrs[0].fmt == RKNN_TENSOR_NCHW)
	{
		// printf("model is NCHW input fmt\n");
		input_width = input_attrs[0].dims[0];
		input_height = input_attrs[0].dims[1];
	}
	else
	{
		// printf("model is NHWC input fmt\n");
		input_width = input_attrs[0].dims[1];
		input_height = input_attrs[0].dims[2];
	}


	printf("model input height=%d, width=%d, channel=%d\n", input_height, input_width,
		   input_channel);

/***********************/
//  5. 定义输入张量 设置属性
/***********************/

	/* Init input tensor */
	rknn_input inputs[1];
	memset(inputs, 0, sizeof(inputs));
	inputs[0].index = 0;
	inputs[0].type = RKNN_TENSOR_UINT8;
	inputs[0].size = input_width * input_height * input_channel;
	inputs[0].fmt = RKNN_TENSOR_NHWC;
	inputs[0].pass_through = 0;


/***********************/
//  6. 定义输出张量 设置属性
/***********************/
	/* Init output tensor */
	rknn_output outputs[io_num.n_output];
	memset(outputs, 0, sizeof(outputs));
	for (int i = 0; i < io_num.n_output; i++)
	{
		outputs[i].want_float = 0;
	}


	inputs[0].buf = input_image.data;


/***********************/
//  7. 设置输入
/***********************/
	printf("rknn_inputs_set  \n");
	rknn_inputs_set(ctx, io_num.n_input, inputs);
	ret = rknn_run(ctx, NULL);
	if(ret<0){
		printf("[error] :%d\n",ret);
		return -1;
	}

/***********************/
//  8. 获取输出
/***********************/
	ret = rknn_outputs_get(ctx, io_num.n_output, outputs, NULL);
	if(ret<0){
		printf("[error] :%d\n",ret);
		return -1;
	}


	printf("size %d\n",outputs[0].size);

	// Post process
	std::vector<float> out_scales;
	std::vector<uint16_t> out_zps;
	for (int i = 0; i < io_num.n_output; ++i)
	{
		out_scales.push_back(output_attrs[i].scale);
		out_zps.push_back(output_attrs[i].zp);
	}

	// printf("output type :%d  ,%d\n",output_attrs[0].type  ,RKNN_TENSOR_FLOAT16);

	// printf( "%d", length(outputs[0].buf) );


	// float res= *( (short *)outputs[0].buf);
	// float ff=Float16ToFloat( *( (short *)outputs[0].buf) );

	// printf("outputs %f  ff: %f \n", res  ,ff);

/***********************/
//  8. 获取结果
/***********************/

	int province=-1;
	ret = lprnet_process_f16((short *)outputs[0].buf,buf,province);
	if (ret < 0)
	{
		printf("[error] :%d\n",ret);
		return -1;
	}

	/*
	yolov5_post_process_fp((float *)outputs[0].buf, (float *)outputs[1].buf, (float *)outputs[2].buf, input_height, input_width,
			            conf_threshold, nms_threshold, &detect_result_group);
	*/

	// rknn_outputs_release(ctx, io_num.n_output, outputs);

	// scale_coords(detect_result_group, img_width, img_height, input_width);

	return 0;
}





int lprnet_detect_init(rknn_context *ctx, const char * path)
{
	int ret;

	// Load model
	FILE *fp = fopen(path, "rb");
	if(fp == NULL)
	{
		printf("fopen %s fail!\n", path);
		return -1;
	}
	fseek(fp, 0, SEEK_END);   //fp指向end,fseek(FILE *stream, long offset, int fromwhere);
	int model_len = ftell(fp);   //相对文件首偏移
	unsigned char *model_data = (unsigned char*)malloc(model_len);

	fseek(fp, 0, SEEK_SET);   //SEEK_SET为文件头
	if(model_len != fread(model_data, 1, model_len, fp))
	{
		printf("fread %s fail!\n", path);
		free(model_data);
		return -1;
	}
	fclose(fp);

	//init
	ret = rknn_init(ctx, model_data, model_len, RKNN_FLAG_PRIOR_MEDIUM);
	if(ret < 0)
	{
		printf("rknn_init fail! ret=%d\n", ret);
		return -1;
	}

	free(model_data);

	return 0;
}





int test(int argc, char **argv)
{
	/* 参数初始化 */
	// int output_nboxes_left =0;
	// coco_detect_result_group_t detect_result_group;

	/* 算法模型初始化 */
	rknn_context ctx;
	lprnet_detect_init(&ctx, "./lprnet.rknn");



	/* 算法运行 */
	cv::Mat src, rgb_img;
	src = cv::imread("test.jpg");
	// cv::cvtColor(src, rgb_img, CV_BGR2RGB);


	lprnet_detect_run(ctx, src,NULL);




	/* 算法模型空间释放 */
	lprnet_detect_release(ctx);

	return 0;
}

