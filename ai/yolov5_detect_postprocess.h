#ifndef _YOLOV5_DETECT_POSTPROCESS_H_
#define _YOLOV5_DETECT_POSTPROCESS_H_

#include <stdint.h>

#define COCO_NAME_MAX_SIZE 16
#define COCO_NUMB_MAX_SIZE 200
#define COCO_CLASS_NUM     4
#define COCO_PROP_BOX_SIZE     (5+COCO_CLASS_NUM)

typedef struct _COCO_BOX_RECT
{
    int left;
    int right;
    int top;
    int bottom;
} COCO_BOX_RECT;

typedef struct __coco_detect_result_t
{
    char name[COCO_NAME_MAX_SIZE];
    int class_index;
    COCO_BOX_RECT box;
    float prop;
} coco_detect_result_t;

typedef struct _detect_result_group_t
{
    int id;
    int count;
    coco_detect_result_t results[COCO_NUMB_MAX_SIZE];
} coco_detect_result_group_t;

int yolov5_post_process_u8(uint8_t *input0, uint8_t *input1, uint8_t *input2, int model_in_h, int model_in_w,
                 float conf_threshold, float nms_threshold,
                 std::vector<uint8_t> &qnt_zps, std::vector<float> &qnt_scales,
                 coco_detect_result_group_t *group);

int yolov5_post_process_fp(float *input0, float *input1, float *input2, int model_in_h, int model_in_w,
                 float conf_threshold, float nms_threshold, 
                 coco_detect_result_group_t *group);

#endif //_RKNN_ZERO_COPY_DEMO_POSTPROCESS_H_
