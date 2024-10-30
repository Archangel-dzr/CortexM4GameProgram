#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>
#include <linux/input.h> 

void home();


//打开屏幕
void lcd_open(void);
//关闭屏幕
void lcd_close(void);
//描点
void lcd_draw_point(int x, int y, int color);
//画矩形
void lcd_draw_rectanle(int x0, int y0, int w, int h, int color);
//清屏
void lcd_clear_screen(int color);

void lcd_draw_word(char *ch, int len, int w, int color, int x0, int y0);
void lcd_draw_rect(unsigned int x0, unsigned int y0, unsigned int w, unsigned int h, int color);
void lcd_draw_digit(int value,int x0, int y0, int color);

//棋盘左上角坐标
#define startx0  50
#define statry0  200
//棋盘棋子宽度
#define Size 80
//棋子间隙
#define space 5
//棋子数量
#define number 4

//游戏主循环
void game_2048();






void bmp_display(const char *filename, int x0, int y0);




#define MOVE_UP 1
#define MOVE_DOWN 2
#define MOVE_LEFT 3
#define MOVE_RIGHT 4

int get_user_input();




int lcd_fd = -1;
//帧缓冲首地址
int *plcd = NULL;

char w_digit[][58] = {
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x1E,0xF0,
0x3C,0x78,0x38,0x38,0x78,0x3C,0x78,0x3C,0x70,0x1C,0x70,0x1C,0x70,0x1C,0x70,0x1C,
0x70,0x1C,0x70,0x1C,0x70,0x3C,0x78,0x3C,0x78,0x38,0x38,0x38,0x3C,0x70,0x1E,0xF0,
0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x03,0x80,
0x1F,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,
0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0x80,0x03,0xC0,
0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xE0,0x1C,0x70,
0x38,0x38,0x78,0x3C,0x78,0x3C,0x78,0x3C,0x38,0x38,0x00,0x78,0x00,0x70,0x00,0xE0,
0x01,0xC0,0x03,0x80,0x07,0x00,0x0E,0x00,0x1C,0x0C,0x38,0x1C,0x70,0x3C,0x7F,0xF8,
0x7F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xC0,0x38,0xF0,
0x38,0x78,0x78,0x78,0x38,0x78,0x00,0x78,0x00,0x70,0x00,0xE0,0x07,0xC0,0x00,0xF0,
0x00,0x78,0x00,0x38,0x00,0x3C,0x00,0x3C,0x78,0x3C,0x78,0x38,0x78,0x38,0x38,0xF0,
0x0F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0x01,0xE0,
0x03,0xE0,0x03,0xE0,0x07,0xE0,0x0E,0xE0,0x0C,0xE0,0x1C,0xE0,0x18,0xE0,0x30,0xE0,
0x70,0xE0,0x60,0xE0,0xFF,0xFC,0x00,0xE0,0x00,0xE0,0x00,0xE0,0x00,0xE0,0x01,0xF0,
0x07,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xF8,0x3F,0xF8,
0x38,0x00,0x38,0x00,0x38,0x00,0x38,0x00,0x30,0x00,0x37,0xE0,0x3C,0x70,0x38,0x38,
0x10,0x3C,0x00,0x3C,0x00,0x3C,0x78,0x3C,0x78,0x3C,0x78,0x38,0x38,0x78,0x38,0xF0,
0x0F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xF0,0x0E,0x78,
0x1C,0x78,0x38,0x78,0x38,0x00,0x78,0x00,0x70,0x00,0x77,0xE0,0x7E,0x78,0x78,0x38,
0x78,0x3C,0x70,0x3C,0x70,0x1C,0x70,0x1C,0x78,0x3C,0x38,0x3C,0x3C,0x38,0x1E,0x70,
0x0F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x3F,0xF8,
0x78,0x38,0x70,0x30,0x60,0x60,0x00,0x60,0x00,0xC0,0x01,0xC0,0x01,0xC0,0x03,0x80,
0x03,0x80,0x03,0x80,0x07,0x80,0x07,0x00,0x07,0x00,0x07,0x00,0x0F,0x00,0x0F,0x00,
0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xE0,0x3C,0x70,
0x38,0x38,0x70,0x3C,0x70,0x1C,0x70,0x1C,0x38,0x38,0x3E,0x70,0x0F,0xE0,0x1F,0xE0,
0x38,0xF0,0x70,0x78,0x70,0x3C,0x70,0x1C,0x70,0x1C,0x70,0x1C,0x70,0x38,0x3C,0x70,
0x0F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xC0,0x3C,0x70,
0x38,0x38,0x78,0x38,0x70,0x3C,0x70,0x3C,0x70,0x1C,0x70,0x3C,0x70,0x3C,0x78,0x7C,
0x3C,0xFC,0x1F,0xFC,0x00,0x3C,0x00,0x38,0x00,0x38,0x38,0x70,0x38,0x70,0x3D,0xE0,
0x1F,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};


//打开屏幕
void lcd_open(void)
{
	int fd;
	// fd = open("/dev/ubuntu_lcd",O_RDWR);
	fd = open("/dev/fb0",O_RDWR);
	if(fd == -1)
	{
		printf("sorry,open fail\n");
	}
	printf("open success\n");
	lcd_fd=fd;
	plcd=mmap(NULL, 480*800*4, PROT_READ | PROT_WRITE, MAP_SHARED,fd,0);   
	if(plcd==MAP_FAILED)
	{
		printf("sorry,mmap open fail\n");
	}
}
//关闭屏幕
void lcd_close(void)
{
	munmap(plcd,480*800*4);
	close(lcd_fd);
}
//描点
void lcd_draw_point(int x, int y, int color)
{
	if (x >= 0 &&  x < 800 &&  y >= 0 &&  y < 480)
	{
		*(plcd + 800*y + x) = color;
	}
}
//画矩形
void lcd_draw_rectanle(int x0, int y0, int w, int h, int color)
{
	int i,j;
	for(i=0;i<w;i++)
	{	
		for(j=0;j<h;j++)
		lcd_draw_point( x0+i,y0+j,color);
	}
}
//清屏
void lcd_clear_screen(int color)
{
	lcd_draw_rectanle(0, 0, 800, 480,  color);
}

void lcd_draw_word(char *ch, int len, int w, int color, int x0, int y0)
{
    int i, j;
    int x, y;
    int flag = w / 8;
    // 遍历整个点阵数组
    for (i = 0; i < len; i++)
    {
        for (j = 7; j >= 0; j--)
        {
            if ((ch[i] >> j) & 1)
            {
                x = 7 - j + 8 * (i % flag) + x0;
                y = i / flag + y0;
                lcd_draw_point(x, y, color);
            }
        }
    }
}


void lcd_draw_digit(int value,int x0, int y0, int color)
{
	int i = 0;
	if (value == 0)
	{
		lcd_draw_word(w_digit[0],sizeof(w_digit[0]),16,color,x0+48-16*i,y0);
	}
	
	while (value)
	{
		int x;
		x = value%10;
		lcd_draw_word(w_digit[x],sizeof(w_digit[x]),16,color,x0+48-16*i,y0);
		value = value/10;
		i++;
	}
}

int value_max = 0; //最高分
int value = 0; //分数


int matrix_2048[number][number] = 
{
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};
//游戏主循环

int matrix_2048_copy[number][number];
int get_zero_num(void)
{
	int i, j;
	int n = 0;

	for (i = 0; i < number ; i++)
	{
		for (j = 0; j < number ;j++)
		{
			if (matrix_2048[i][j] == 0)
			{
				n++;
			}
		}
	}
	return n;
}


//在二维数组matrix_2048的随机的空白位置上
//填充一个随机的数字 
void fill_a_number(void)
{
	int i, j;
	int n =0;
	int zero_num = get_zero_num();
	if (zero_num == 0)
	{
		return ;
	}
	int pos; //第pos个0的位置为填充位置
	pos =	random() % zero_num  + 1 ; //[1, zero_num]
	for (i = 0; i < number ; i++)
	{
		for (j = 0; j < number; j++)
		{
			if (matrix_2048[i][j] == 0)
			{
				n++;
				if (n == pos)
				{
					//定义一个数组来用于随机出现2,4,8
					int num[4]={2,2,4,8};
					pos = random()%4;
					matrix_2048[i][j] =num[pos];
					return ;
				}
			}
		}
	}

}
//画棋盘矩阵
void draw_matrix(void)
{
	int i,j;
	int x=0;
	int y=0;
	for (i = 0; i < number;i++)
	{
		y=Size*i+space*i+startx0;
		for(j=0;j<number;j++)
		{
			x=Size*j+space*j+statry0;
			if (matrix_2048[i][j] == 0)
			lcd_draw_rectanle(x,y, Size, Size,0xffffff);
			else
			{
				char filename[256];
				sprintf(filename, "/mnt/image/2048/color_x80_%d.bmp", matrix_2048[i][j]);				
				bmp_display(filename, x, y);
			}
		}
	}	
}
void Change_up(int a)
{
	int i;
	int b[number];
	for(i=0;i<number;i++)
	{
		b[i]=matrix_2048[i][a];
	}
	int j = 1;
	int k = 0;
	for (; j < 4; j++) 
	{  
		//找出k第一个不为0的块，之后分为三种情况
		if (b[j] > 0) 
		{
			//移动
			 if (b[k] == 0) 
			{ 
				b[k] = b[j];
	            b[j] = 0;
			}
			 //合并
			else if (b[k] == b[j])
			{
				b[k] = 2 * b[k];
				b[j] = 0;
				k = k + 1;

				value += 2;
			}
			 //碰撞
			else
			{ 
				b[k + 1] = b[j];
				if (j != k + 1) 
				{ 
					b[j] = 0;
				}
				k = k + 1;
			}
		}
	}
	for(i=0;i<number;i++)
	{
		matrix_2048[i][a]=b[i];
	}
}
void Change_down(int a)
{
	int i;
	int b[number];
	for(i=0;i<number;i++)
	{
		b[i]=matrix_2048[i][a];
	}
	int j = 2;
	int k =3;
	for (; j>=0; j--) 
	{  
		//找出k第一个不为0的块，之后分为三种情况
		if (b[j] > 0) 
		{
			//移动
			 if (b[k] == 0) 
			{ 
				b[k] = b[j];
	            b[j] = 0;
			}
			 //合并
			else if (b[k] == b[j])
			{
				b[k] = 2 * b[k];
				b[j] = 0;
				k = k - 1;

				value += 2;
			}
			 //碰撞
			else
			{ 
				b[k - 1] = b[j];
				if (j != k - 1) 
				{ 
					b[j] = 0;
				}
				k = k - 1;
			}
		}
	}
	for(i=0;i<number;i++)
	{
		matrix_2048[i][a]=b[i];
	}
}
void Change_left(int a)
{
	int i;
	int b[number];
	for(i=0;i<number;i++)
	{
		b[i]=matrix_2048[a][i];
	}
	int j = 1;
	int k = 0;
	for (; j < 4; j++) 
	{  
		//找出k第一个不为0的块，之后分为三种情况
		if (b[j] > 0) 
		{
			//移动
			 if (b[k] == 0) 
			{ 
				b[k] = b[j];
	            b[j] = 0;
			}
			 //合并
			else if (b[k] == b[j])
			{
				b[k] = 2 * b[k];
				b[j] = 0;
				k = k + 1;

				value += 2;
			}
			 //碰撞
			else
			{ 
				b[k + 1] = b[j];
				if (j != k + 1) 
				{ 
					b[j] = 0;
				}
				k = k + 1;
			}
		}
	}
	for(i=0;i<number;i++)
	{
		matrix_2048[a][i]=b[i];
	}
}
void Change_right(int a)
{
	int i;
	int b[number];
	for(i=0;i<number;i++)
	{
		b[i]=matrix_2048[a][i];
	}
	int j = 2;
	int k =3;
	for (; j>=0; j--) 
	{  
		//找出k第一个不为0的块，之后分为三种情况
		if (b[j] > 0) 
		{
			//移动
			 if (b[k] == 0) 
			{ 
				b[k] = b[j];
	            b[j] = 0;
			}
			 //合并
			else if (b[k] == b[j])
			{
				b[k] = 2 * b[k];
				b[j] = 0;
				k = k - 1;

				value += 2;
			}
			 //碰撞
			else
			{ 
				b[k - 1] = b[j];
				if (j != k - 1) 
				{ 
					b[j] = 0;
				}
				k = k - 1;
			}
		}
	}
	for(i=0;i<number;i++)
	{
		matrix_2048[a][i]=b[i];
	}
}

int change_matrix(int mv)
{
	int i,j;
	int key =1;
	for (i = 0; i < number; ++i)
	{
		for (j = 0; j < number; ++j)
		{
			matrix_2048_copy[i][j]=matrix_2048[i][j];
		}
	}
	if(mv == MOVE_UP)
	{
		for (j = 0; j < number;j++)
		{
			for(i=0;i<number-1;i++)
			{
				Change_up(j);
			}
			
		}
	}
	if(mv==MOVE_DOWN)
	{
		for (j = 0; j < number;j++)
		{
			for(i=0;i<number-1;i++)
			{
				Change_down(j);
			}
			
		}
	}
	if(mv==MOVE_LEFT)
	{
		for (i = 0; i < number;i++)
		{
			for(j=0;j<number-1;j++)
			{
				Change_left(i);
			}
			
		}
	}
	if(mv==MOVE_RIGHT)
	{
		for (i = 0; i < number;i++)
		{
			for(j=0;j<number-1;j++)
			{
				Change_right(i);
			}
			
		}
	}
	draw_matrix();

	for (i = 0; i < number; ++i)
	{
		for (j = 0; j < number; ++j)
		{
			if(matrix_2048_copy[i][j]!=matrix_2048[i][j])
			{
				return 0;
			}	
		}
	}
	return 1;
}

int game_is_over()
{
    if(get_zero_num()>0)
    {
    	return 0;
    }
	int i,j;
	for (i = 0; i < number; ++i)
	{
		for (j = 0; j < number; ++j)
		{
			if(matrix_2048[i][j]==matrix_2048[i+1][j]&&i!=3)return 0;		
			else if(matrix_2048[i][j]==matrix_2048[i][j+1]&&j!=3)return 0;
			else if(matrix_2048[i][j]==matrix_2048[i-1][j]&&i!=0)return 0;
			else if(matrix_2048[i][j]==matrix_2048[i][j-1]&&j!=0)return 0;
		}
	}
	return 1;
}

void game_2048() {
    bmp_display("/mnt/image/2048/background.bmp", 0, 0);

    // 初始化matrix_2048并显示
    int pos;
    pos = random() % 3 + 1;
    int i, j;
    for (i = 0; i < pos; i++) {
        fill_a_number();
        draw_matrix();
    }

    // 游戏主循环
    while (1) {
        // 等待用户输入(手指滑动，或 键盘按键)
        int mv = get_user_input();
        if (mv == -1) {
            break; // 返回主菜单
        }
        bmp_display("/mnt/image/2048/background_score.bmp", 0, 0);

        change_matrix(mv);

        fill_a_number();
        draw_matrix();

        // 显示分数和最高分
        lcd_draw_digit(value, 0, 0, 0x00ff00); // 显示当前分数
        lcd_draw_digit(value_max, 0, 100, 0x00ff00); // 显示最高分

        // 判断游戏是否结束
        if (game_is_over()) {
            char filename[256] = "/mnt/image/2048/game_over.bmp";
            bmp_display(filename, 200, 85);
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    matrix_2048[i][j] = 0;
                }
            }
            if (value > value_max) {
                value_max = value;
            }
            value = 0;
            break;
        }
    }
}




void bmp_display(const char *filename, int x0, int y0)
{
	int fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		//printf("failed to open %s\n", filename);
		return ;
	}

	int width, height;
	short depth;

	//width
	lseek(fd, 0x12, SEEK_SET);
	read(fd, &width, 4);

	//height
	lseek(fd, 0x16, SEEK_SET);
	read(fd, &height, 4);

	//depth
	lseek(fd, 0x1c, SEEK_SET);
	read(fd, &depth, 2);


	//printf("%d * %d \n", width, height);
	//printf("depth: %d\n", depth);

	if ( !(depth == 24 || depth == 32))
	{
		printf("Sorry, Non-standard Bmp\n");
		return ;
	}


	//
	int total_bytes; //图片中整个像素数组的字节数
	int line_bytes_valid; //每一行所占的像素字节数
	int line_bytes; //一行所占的字节数
	int laizi = 0; //每一行末尾赖子数量
	
	line_bytes_valid = abs(width) * (depth/8) ;

	if (line_bytes_valid % 4)
	{
		laizi = 4 - line_bytes_valid%4; 

	}
	line_bytes = line_bytes_valid + laizi;

	total_bytes = line_bytes * abs(height) ;

	unsigned char *pixel = malloc( total_bytes ); 
	lseek(fd, 54, SEEK_SET );
	read(fd, pixel, total_bytes);

	int i = 0;
	unsigned char a,r,g,b;
	int color ;

	int x, y;


	for (y = 0; y < abs(height); y++)
	{
		//这一行的第x点的解析
		for (x = 0; x < abs(width); x++)
		{
			b = pixel[i++];
			g = pixel[i++];
			r = pixel[i++];

			if (depth == 32)
			{
				a = pixel[i++];
			}
			else
			{
				a = 0;
			}
			color = (a << 24) | (r << 16) | (g << 8) | (b);
			lcd_draw_point( x0 + x,	y0 + abs(height) - 1 - y, color);
		}
		i = i + laizi;

	}
			
	free(pixel);

	close(fd);


}

int get_user_input()
{
    // int fd = open("/dev/ubuntu_event", O_RDONLY);
    int fd = open("/dev/input/event0", O_RDONLY);
    if (fd == -1)
    {
        printf("failed to open /dev/input/event0\n");
        return -1;
    }

    int x1 = -1, y1 = -1; //一次滑动过程中的，起点的坐标
    int x2, y2; //一次滑动过程中的，终点的坐标
    struct input_event ev;
    int quit = 0; // 添加退出标志

    while (1)
    {
        int r = read(fd, &ev, sizeof(ev));
        if (r != sizeof(ev))
        {
            continue;
        }

        //ev是一个x轴坐标事件
        if (ev.type == EV_ABS &&  ev.code == ABS_X)
        {   
            if (x1 == -1)
            {   
                x1 = ev.value*800/1024 ; //就是x轴的坐标值
            }
            x2 = ev.value*800/1024;
        }

        
        //ev是一个y轴坐标事件
        if (ev.type == EV_ABS &&  ev.code == ABS_Y)
        {   
            if (y1 == -1)
            {   
                y1 = ev.value*480/600 ; //就是x轴的坐标值
            }
            y2 = ev.value*480/600;
        }

        // 检测点击事件
        if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
        {
            // 点击退出
            if (x1 >= 700 && x1 <= 800 && y1 >= 380 && y1 <= 480)
            {
                quit = 1; // 设置退出标志
                close(fd);
                for (int i = 0; i < 4; i++) 
                {
                    for (int j = 0; j < 4; j++) 
                    {
                        matrix_2048[i][j] = 0;
                    }
                }
                if (value > value_max) 
                {
                    value_max = value;
                }
                value = 0;
                return -1; // 返回-1表示退出
            }

            int delt_x = abs(x2 - x1);
            int delt_y = abs(y2 - y1);

            if (delt_x > 2 *delt_y)
            {
                close(fd);
                if (x2 > x1)
                {
                    return MOVE_RIGHT;
                }
                else
                {
                    return MOVE_LEFT;
                }
            }
            else if (delt_y > 2*delt_x)
            {
                close(fd);
                if (y2 > y1)
                {
                    return MOVE_DOWN;
                }
                else
                {
                    return MOVE_UP;
                }
            }
            else
            {
                x1 = -1;
                y1 = -1;
            }
        }
    }
}



void game_main() {
    srandom(time(NULL));

   //1. 打开屏幕
   lcd_open();
   restart: lcd_clear_screen(0x555555);
   // 关闭屏幕
   game_2048();
   int mv = get_user_input();
   if (mv != -1) // 如果返回值不是-1，则重新开始游戏
   {   
       goto restart;
   }
       
   lcd_close();
   return ;
}
