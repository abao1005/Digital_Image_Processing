#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <cmath>
#include <time.h>
using namespace std;
#define MAXPIX 512 * 512

int randint(int n);

int main()
{
    BITMAPFILEHEADER FileHeader; //宣告BMP檔案標頭
    BITMAPINFOHEADER InfoHeader; //宣告BMP資訊標頭
    FILE *fp;                    //讀取檔案FILE
    FILE *fpw;                   //寫入檔案FILE

    int ImageX, ImageY; //圖檔內容寬高
    uint8_t gray_value[MAXPIX];

    double MSE = 0.0;
    double PSNR = 0.0;

    /** ##### 讀取圖檔 #####  */
    fp = fopen("DIPpic1_gray.bmp", "rb"); //開啟檔案
    fpw = fopen("PicWithNoice.bmp", "wb");

    fread(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
    for (int i = 0; i < 1024; i++)
    {
        uint8_t palette;
        fread(&palette, sizeof(uint8_t), 1, fp);
    }

    ImageX = InfoHeader.biWidth; // 取得寬高
    ImageY = InfoHeader.biHeight;

    for (int i = 0; i < ImageY; i++) //逐列掃描
    {
        for (int j = 0; j < ImageX; j++) //逐行掃描
        {
            uint8_t rawData;
            fread(&rawData, sizeof(uint8_t), 1, fp); //將每格的pixel BGR 寫入 color matrix 中
            gray_value[i * ImageX + j] = rawData;
        }
    }
    fclose(fp); //檔案fp讀取完成，關閉

    /** ##### 輸出圖檔 ##### */
    fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fpw); //輸出檔案
    fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fpw);

    for (int i = 0; i < 256; i++)
    {
        uint8_t palette = uint8_t(i);
        for (int j = 0; j < 4; j++)
        {
            fwrite(&palette, sizeof(uint8_t), 1, fpw); // 將 palette 寫入輸出圖檔中
        }
    }

    for (int i = 0; i < ImageY; i++) //逐列掃描
    {
        for (int j = 0; j < ImageX; j++) //逐行掃描
        {
            uint8_t result = gray_value[i * ImageX + j];
            int tmp = (int)result + (-20) + randint(20 - (-20) + 1); //原圖加上 Uniform noise
            tmp = tmp > 255 ? 255 : tmp;                             //灰階值 >255 設為255
            tmp = tmp < 0 ? 0 : tmp;                                 //灰階值 <0 設為0
            MSE += pow(tmp - (int)result, 2);
            result = (uint8_t)tmp;
            fwrite(&result, sizeof(uint8_t), 1, fpw); // 將 gray value with noise added 寫入輸出圖檔中
        }
    }

    cout << "MSE = " << (MSE = MSE / (ImageX * ImageY)) << endl
         << "PSNR = " << (PSNR = 10 * log10(pow(255, 2) / MSE)) << endl
         << endl;

    fclose(fpw);

    return 0;
}

/* 產生 [0, n) 均勻分布的整數亂數 */
int randint(int n)
{
    if ((n - 1) == RAND_MAX)
    {
        return rand();
    }
    else
    {
        /* 計算可以被整除的長度 */
        long end = RAND_MAX / n;
        end *= n;

        /* 將尾端會造成偏差的幾個亂數去除，
       若產生的亂數超過 limit，則將其捨去 */
        int r;
        while ((r = rand()) >= end)
            ;

        return r % n;
    }
}