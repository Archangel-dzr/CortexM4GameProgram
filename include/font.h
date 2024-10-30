/*** 
 * @ ┌─────────────────────────────────────────────────────────────┐
 * @ │┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐│
 * @ ││Esc│!1 │@2 │#3 │$4 │%5 │^6 │&7 │*8 │(9 │)0 │_- │+= │|\ │`~ ││
 * @ │├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┤│
 * @ ││ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │{[ │}] │ BS  ││
 * @ │├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤│
 * @ ││ Ctrl │ A │ S │ D │ F │ G │ H │ J │ K │ L │: ;│" '│ Enter  ││
 * @ │├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┤│
 * @ ││ Shift  │ Z │ X │ C │ V │ B │ N │ M │< ,│> .│? /│Shift │Fn ││
 * @ │└─────┬──┴┬──┴──┬┴───┴───┴───┴───┴───┴──┬┴───┴┬──┴┬─────┴───┘│
 * @ │      │Fn │ Alt │         Space         │ Alt │Win│   HHKB   │
 * @ │      └───┴─────┴───────────────────────┴─────┴───┘          │
 * @ └─────────────────────────────────────────────────────────────┘
 * @
 * @Author: Archangel-dzr 781446156@qq.com
 * @Date: 2022-07-15 09:06:16
 * @LastEditTime: 2024-10-30 18:34:35
 * @LastEditors: Archangel-dzr 781446156@qq.com
 * @Description: 
 * @FilePath: \新建文件夹\include\font.h
 * @Copyright (c) 2024 by Archangel-dzr email: 781446156@qq.com, All Rights Reserved. 
 */


#ifndef __FONT__
#define __FONT__


int   Init_Font(void);  //显示字体库前先调用本函数进行初始化
void  UnInit_Font(void); //程序退出前，调用本函数
int   Clean_Area(int X,  //x坐标起始点
                 int Y,  //y坐标起始点
				 int width, //绘制的宽度
				 int height,//绘制的高度
				 unsigned long color); //往屏幕指定区域填充颜色
				 
int   Display_characterX(unsigned int x,          //x坐标起始点
                         unsigned int y,          //y坐标起始点
						 unsigned char *string,   //GB2312 中文字符串
						 unsigned int color ,     //字体颜色值
						 int size);               //字体放大倍数 1~8


#endif