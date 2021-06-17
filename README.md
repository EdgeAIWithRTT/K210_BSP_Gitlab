# RT-AK 之 K210 插件 Demo-Mnist

[[_TOC_]]

## 1. 简介

基于Tensorflow训练第一个AI模型：MNIST手写数字识别模型。

MNIST手写数字识别模型的主要任务是：输入一张手写数字的图像，然后识别图像中手写的是哪个数字。

该模型的目标明确、任务简单，数据集规范、统一，数据量大小适中，在普通的PC电脑上都能训练和识别，堪称是深度学习领域的“Hello World!”，学习嵌入式AI的入门必备模型。

![image-20210609111539182](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210609111540.png)

基于神经网络实现手写字符识别，掌握神经网络图像分类识别的基本原理。

当模型训练完成之后，使用RT-AK框架，将模型一键部署到RT-Thread系统中。

## 2. 元件

无

## 3. 步骤

K210 核心图：

![image-20210609101050522](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210609101051.png)

### 3.1 代码流程

**系统内部初始化**：

- 系统时钟初始化

**RT-AK Lib 模型加载并运行**：

- 注册模型（代码自动注册，无需修改）
- 找到注册模型
- 初始化模型，挂载模型信息，准备运行环境
- 运行（推理）模型
- 获取输出结果

**Mnist 业务逻辑层**：

- 找出输出最大值

### 3.2 核心代码

```c
// main.c
/* Set CPU clock */
sysctl_clock_enable(SYSCTL_CLOCK_AI);  // 使能系统时钟（系统时钟初始化）

/* LCD init */
lcd_init();
...

// 注册模型的代码在 rt_ai_mnist_model.c 文件下的第31行，代码自动执行
// 模型的相关信息在 rt_ai_mnist_model.h 文件
/* AI modol inference */
mymodel = rt_ai_find(MY_MODEL_NAME);  // 找到注册模型
if (rt_ai_init(mymodel, (rt_ai_buffer_t *)IMG9_CHW) != 0)  // 初始化模型，传入输入数据
...
if(rt_ai_run(mymodel, ai_done, NULL) != 0)    // 模型推理一次
...
output = (float *)rt_ai_output(mymodel, 0);  // 获取模型输出结果

/* 对模型输出结果进行处理，该实验是Mnist，输出结果为10个概率值，选出其中最大概率即可 */
for(int i = 0; i < 10 ; i++)
{
    // printf("pred: %d, scores: %.2f%%\n", i, output[i]*100);
    if(output[i] > scores  && output[i] > 0.2)
    {
        prediction = i;
        scores = output[i];
    }
}

```

---

**如何更换模型输入数据补充说明**：

示例数据在 `applications/test_data` 文件夹下，模型不用重新训练

**数据准备**

1. 将图片保存成 CHW 格式，作为 K210 模型推理时的输入数据，使用的是 `utils/save_chw_img.py` ，文件中有使用该文件的用例

**main.c 代码修改**：

1. 修改第 19 行代码，改为新的文件名，其中带 chw 后缀的是模型输入文件

   ![image-20210617103033189](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210617103033.png)

2. 修改第 52 行代码，如果使用脚本生成的文件，应该是文件名的大写变量

   ![image-20210617103116275](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210617103124.png)

4. 重新编译烧录即可

### 3.3 编译说明

**需要准备好交叉编译工具链** | [下载](https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases/tag/v8.3.0-1.2)

设置编译环境：

```shell
set RTT_EXEC_PATH=your_toolchains
# 或者修改rtconfig.py 文件第22行，新增 os.environ['RTT_EXEC_PATH'] = r'your_toolchains'
scons -j 6	
```

如果编译正确无误，会产生rtthread.elf、rtthread.bin文件。

其中 rtthread.bin 需要烧写到设备中进行运行。

### 3.4 烧写及执行

连接好串口，然后使用 [K-Flash](https://kendryte.com/downloads/) 工具进行烧写bin文件。

![K-Flash](images/flash.png)

### 3.5 运行结果

如果编译 & 烧写无误，LCD 显示器先会显示图片 logo 和文字，一秒后显示预测的图片和预测结果

![image-20210609103738327](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210609103739.png)

可以用如下命令来打开串口：

```shell
# 遇到没有这个 serial 这个包的话，需要先安装一下
python -m serial.tools.miniterm --rts 0 --dtr 0 COM9 115200
```

## 4. 驱动支持情况及计划

| 驱动 | 支持情况  |  备注  |
| ------ | ----  | :------:  |
| UART | 支持 | 高速UART，用于shell |

## 5. 联系人信息

维护人：lebhoryi@gmail.com

## 6. 注意事项

在进行系统menuconfig配置时，以下项是必选的

| 配置项 | 强制配置值 |
| -----  | --------- |
| Alignment size for CPU architecture data access | 8 |
| The stack size of idle thread | > 1024 |
| the device name for console | "uarths" |
| Set main thread stack size | 4096 |

## 7. 参考

* 芯片[datasheet][2]

  [1]: https://www.rt-thread.org/page/download.html
  [2]: https://s3.cn-north-1.amazonaws.com.cn/dl.kendryte.com/documents/kendryte_datasheet_20180919020633.pdf

