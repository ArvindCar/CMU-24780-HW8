#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <string>
#include "fssimplewindow.h"
#include "yspng.h"

class Histogram
{
protected:
    unsigned int hist[256];
    bool isinit =false;
public:
    Histogram()
    {
        Clear();
    }

    void Clear()
    {
        isinit = false;
        for (int i = 0; i < 256; ++i)
        {
            hist[i] = 0;
        }
    }

    void Make(const YsRawPngDecoder &png)
    {
        Clear();
        isinit = true;
        for (int i = 0; i < png.wid * png.hei; ++i)
        {
            auto r = png.rgba[i * 4];
            auto g = png.rgba[i * 4 + 1];
            auto b = png.rgba[i * 4 + 2];
            hist[r] += 1;
            hist[g] += 1;
            hist[b] += 1;
        }
    }

    void Print() const
    {
        if (!isinit)
        {
            std::cout<<"Histogram has not been initialized";
        }
        else
        {
            int maxNum = 0;
            for (int i = 0; i < 256; ++i)
            {
                maxNum = std::max(maxNum, static_cast<int>(hist[i]));
            }

            for (int i = 0; i < 256; ++i)
            {
                printf("%3d:", i);

                int len = hist[i] * 80 / maxNum;
                for (int j = 0; j < len; ++j)
                {
                    printf("#");
                }
                printf("\n");
            }
        }
        
    }

    void Draw(const int windowHeight) const
    {
         if (!isinit)
        {
            std::cout<<"Histogram has not been initialized";
        }
        else
        {
            int maxNum = 0;
            for (int i = 0; i < 256; ++i)
            {
                maxNum = std::max(maxNum, static_cast<int>(hist[i]));
            }

            glColor3ub(255, 255, 255); 
            for (int i = 0; i < 256; ++i)
            {
                int height = hist[i] * 80 / maxNum;
                glBegin(GL_LINES);
                glVertex2i(i, windowHeight);
                glVertex2i(i, windowHeight - height);
                glEnd();
            }
        }
    }
};

int main(void)
{
    std::string filename;
    std::cout << "Enter the PNG filename: ";
    std::cin >> filename;

    FsChangeToProgramDir();

    YsRawPngDecoder png;
    if (YSOK == png.Decode(filename.c_str()))
    {
        printf("Width: %d, Height: %d\n", png.wid, png.hei);
    }
    else
    {
        printf("Failed to open file.\n");
        return 1;
    }
    png.Flip();

    Histogram histogram;
    histogram.Make(png);
    histogram.Print();

    FsOpenWindow(0, 0, png.wid, png.hei, 1);
    for (;;)
    {
        FsPollDevice();
        if (FSKEY_ESC == FsInkey())
        {
            break;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glRasterPos2i(0, png.hei - 1);
        glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);
        
        histogram.Draw(png.hei);

        FsSwapBuffers();
    }

    return 0;
}
