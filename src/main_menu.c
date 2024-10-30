// main_menu.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <pthread.h>
#include <time.h>
#include "font.h"

char buf[50]={0};
void game_main();
int gomoku_main();
void mine_main();

// 播放音乐函数
void* play_sound_thread(void* arg) 
{
    system("madplay /mnt/music/touch.mp3 &");
    sleep(0.5);
    system("killall -SIGKILL madplay");
    printf("播放结束\n");
    pthread_exit(NULL); // 结束线程
}

void play_touch_sound() 
{
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, play_sound_thread, NULL) != 0) 
    {
        perror("创建播放线程失败");
    }

}

// 获取坐标函数
void touch_xy(int *x, int *y) 
{
    struct input_event touch;
    int touch_fd = open("/dev/input/event0", O_RDONLY);
    if (touch_fd == -1) 
    {
        perror("无法打开输入设备！");
        return;
    }
    while (1) {
        ssize_t ret = read(touch_fd, &touch, sizeof(touch));
        if (ret != sizeof(touch)) 
        {
            perror("读取输入事件失败！");
            break;
        }
        if (touch.type == EV_ABS) 
        {
            if (touch.code == ABS_X) 
            {
                *x = touch.value*800/1024;
            } 
            else if (touch.code == ABS_Y) 
            {
                *y = touch.value*480/600;
            }
        } 
        else if (touch.type == EV_KEY && touch.code == BTN_TOUCH && touch.value == 0) 
        {
            printf("( %d , %d )\n", *x, *y);
            // 播放触屏音乐
            play_touch_sound();
            break;
        }
    }
    close(touch_fd);
}


// 显示图片函数
int show_bmp(const char *pathname) 
{
    int bmp_fd = open(pathname, O_RDONLY);
    if (bmp_fd == -1) 
    {
        perror("图片打开失败");
        return -1;
    }
    
    if (lseek(bmp_fd, 54, SEEK_SET) == -1) 
    {
        perror("设置文件偏移失败");
        close(bmp_fd);
        return -1;
    }

    char bmp_buf[800 * 480 * 3];
    ssize_t bytes_read = read(bmp_fd, bmp_buf, sizeof(bmp_buf));
    if (bytes_read != sizeof(bmp_buf)) 
    {
        perror("读取位图数据失败");
        close(bmp_fd);
        return -1;
    }

    char new_buf[800 * 480 * 4] = {0};
    for (int i = 0, j = 0; i < sizeof(bmp_buf); i += 3, j += 4) 
    {
        new_buf[j] = bmp_buf[i];
        new_buf[j + 1] = bmp_buf[i + 1];
        new_buf[j + 2] = bmp_buf[i + 2];
        new_buf[j + 3] = 0;  // 用于填充 Alpha
    }

    int lcd_fd = open("/dev/fb0", O_RDWR);
    if (lcd_fd == -1) 
    {
        perror("显示屏打开失败");
        close(bmp_fd);
        return -1;
    }

    char *p = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (p == MAP_FAILED) 
    {
        perror("内存映射失败");
        close(bmp_fd);
        close(lcd_fd);
        return -1;
    }

    for (int i = 0; i < 480; i++) 
    {
        memcpy(p + 800 * 4 * i, new_buf + 800 * 4 * (479 - i), 800 * 4);
    }

    munmap(p, 800 * 480 * 4);
    close(bmp_fd);
    close(lcd_fd);
    return 0;
}

void Time_char(int x, int y,char *p)
{
	Init_Font();

	Clean_Area(x,  //x坐标起始点
               y,  //y坐标起始点
				360, //绘制的宽度
				30,//绘制的高度
				0xffffff); //往屏幕指定区域填充颜色

	Display_characterX(x,          //x坐标起始点
					   y,           //y坐标起始点
						p,   //GB2312 中文字符串
						   0x000000,     //字体颜色值
						   2);	

	UnInit_Font();		


}

void Time()
{
    char buf[50]={0};
    time_t ret=time(NULL);
    
    struct tm time_stru1;
    struct tm *time_stru=localtime(&ret);
    gmtime_r(&ret, &time_stru1);  
    sprintf(buf,"%d:%d:%d %d:%d:%d",(1900+time_stru->tm_year),
                             (1+time_stru->tm_mon),
                             (time_stru->tm_mday),
                             (time_stru->tm_hour),
                             (time_stru->tm_min),
                             (time_stru->tm_sec));    
                            
    Time_char(440,450,buf);
}
void *clock_thread() // 修正函数名
{
    while(1)
    {
        Time();
        usleep(100);
    }
}



// 选择游戏
void home() 
{
    while(1){
    int x, y;
    if (show_bmp("/mnt/image/main/menu.bmp") == -1) 
    {
        return;  // 处理显示图片失败的情况
    }
    sleep(1);
    touch_xy(&x, &y);
    if(x >= 50 && x <= 250 && y >= 100 && y <= 380)
    {
        gomoku_main();
    }
    if (x >= 300 && x <= 500 && y >= 100 && y <= 380) 
    {
        game_main();
    }
    if(x>=550 && x<=750 && y>=100 && y<=380)
    {
        mine_main();
    }
    } 
}

int main() 
{   
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, clock_thread, NULL) != 0) 
    {
        perror("线程创建失败");
        return -1;
    }

        home();
 

}
