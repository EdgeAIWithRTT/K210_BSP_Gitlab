/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/09/30     Bernard      The first version
 * 2021/06/16     Lebhoryi     The first AI Demo app
 */

#include <rtthread.h>
#include <stdio.h>
#include "sysctl.h"

/****************************
 * please update this
 **************************/
#include "img9_chw.h"
#include "rt_ai.h"
#include "rt_ai_mnist_model.h"

#define MY_MODEL_NAME           RT_AI_MNIST_MODEL_NAME
#define MY_MODEL_IN_1_SIZE      RT_AI_MNIST_IN_1_SIZE
#define MY_MODEL_OUT_1_SIZE     RT_AI_MNIST_OUT_1_SIZE

volatile rt_ai_uint32_t g_ai_done_flag;  // 判断模型推理一次是否完成
char *label[] = {"pred: 0", "pred: 1", "pred: 2", "pred: 3", "pred: 4", "pred: 5", "pred: 6", "pred: 7", "pred: 8", "pred: 9"};

static int ai_done(void *ctx);
/********************************************************/


int main(void)
{
    rt_kprintf("Hello RT-Thread!\r\n");
    rt_kprintf("Demo: Mnist\r\n");

    /* Set CPU clock */
    sysctl_clock_enable(SYSCTL_CLOCK_AI);  // 使能系统时钟（系统时钟初始化）

    rt_ai_t mymodel = NULL;  // 初始化模型信息

    /* AI modol inference */
    mymodel = rt_ai_find(MY_MODEL_NAME);  // 找到模型
    if(!mymodel)
    {
        rt_kprintf("\nmodel find error\n");
        while (1) {};
    }

    if (rt_ai_init(mymodel, (rt_ai_buffer_t *)IMG9_CHW) != 0)  // 初始化模型，传入输入数据
    {
        rt_kprintf("\nmodel init error\n");
        while (1) {};
    }

    rt_kprintf("rt_ai_init complete..\n");

    g_ai_done_flag = 0;
    if(rt_ai_run(mymodel, (void *)ai_done, NULL) != 0)
    {    // 模型推理一次
        rt_kprintf("rtak run fail!\n");
        while (1) {};
    }
    rt_kprintf("AI model inference done.\r\n");
    while(!g_ai_done_flag) {};  // 等待kpu运算完成
    
    float *output;  // 模型输出结果存放
    int prediction = -1;  // 模型预测结果
    float scores = 0.;  // 模型预测概率
    output = (float *)rt_ai_output(mymodel, 0);  // 获取模型输出结果

    /* 对模型输出结果进行处理，该实验是Mnist，输出结果为10个概率值，选出其中最大概率即可 */
    for(int i = 0; i < 10 ; i++)
    {
        // printf("pred: %d, scores: %.2f%%\n", i, output[i]*100);
        if(output[i] > scores)
        {
            prediction = i;
            scores = output[i];
        }
    }

    /* 如果预测的概率值都没有超过0.2的，则视为预测失败*/
    if (scores == 0.)
    {
        rt_kprintf("no predictions here\n");
        while (1) {};
    }
    printf("The prediction: %d, scores: %.2f%%\n", prediction, scores*100);   

    return 0;
}

static int ai_done(void *ctx)
{
    g_ai_done_flag = 1;
    return 0;
}