#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <linux/input.h>
#include <time.h>


#define ROW 11
#define COL 17
#define MINES 20
#define TOUCHPATH  ("/dev/input/event0") 
#define LCDPATH ("/dev/fb0")
#define LCDSIZE (800*480*4)

//初始化棋盘
void Creat_board(char board[ROW][COL]);

//获取该格子周围雷的数量
int countMines(char board[ROW][COL], int row, int col);

//打印棋盘
void print_board(char board[ROW][COL]);

//随机落雷(在第一次选点的3×3范围外)
void DownMines(char board[ROW][COL],int row, int col);

//将数据计算并加入棋盘
void InsertData(char board[ROW][COL],int row,int col);

//获取棋盘中字符a的数量
int Find_sign(char board[ROW][COL],char a);

//判断游戏是否结束
int Judge_func(char board[ROW][COL]);

//将内部棋盘可展示部分赋值给展示棋盘，并递归找空地
void revealCell(char board[ROW][COL], char displayBoard[ROW][COL], int row, int col);

int LCD_INIT();

int LCD_UNINIT();

void DrawPixel(int h,int w,int color);

//画棋盘线
void Drawboard(int color);

//画背景
void DrawBackgournd(int color);

//显示图片
int DisplayBMPPicture(int posx,int posy,const char *pic_path);

//显示棋盘
void Darw_board(int first,char board[ROW][COL]);

//获取手指点击屏幕的坐标,超出范围返回1,未超出范围返回-1
void Get_XY(int *x,int *y);

//获取手指点击屏幕的坐标之后进行开始游戏的操作
void Start_game(char board[ROW][COL],char displayBoard[ROW][COL],int Y,int X);

//初始化界面
int Initial();

//重新开始游戏与退出游戏程序
int Restart();

static int  lcd_fd    = -1;
static int *lcd_point = NULL;

void home();

//初始化棋盘
void Creat_board(char board[ROW][COL])
{
    // 初始化二维数组空间
    for(int h = 0;h < ROW;h++)
    {  
        for(int w = 0;w < COL;w++)
        {
            board[h][w] = '#';//初始化二维矩阵
        }
    }
}

//打印棋盘
void print_board(char board[ROW][COL])
{
    for(int h = 0;h < ROW;h++)
    {  
        for(int w = 0; w < COL; w++)
        {
            if (board[h-1][w] != '#')
            {
                if (board[h-1][w] == 'X')
                {   //将旗子颜色设置为蓝色
                    printf("%c  ",board[h-1][w]);
                }
                else
                //为了美观将边界打印为红色
                printf("%c  ",board[h-1][w]);
            }
            else 
            printf("%c  ",board[h][w]);//打印元素
        }
        printf("\n\n\n");
    }
}

//随机落雷(在第一次选点的3×3范围外)
void DownMines(char board[ROW][COL],int row, int col)
{
    srand(time(0));//随机种子
    int count = 0;
    //先将第一次输入的点的周围变成雷
    for (int i = row - 1; i <= row + 1; i++)
    {
        for (int j = col - 1; j <= col + 1; j++)
        {
            board[i][j] = '*';//随机生成雷时就不会在这个点生成
        }
    }
    //随机落十个雷
    while (count < MINES)
    {
        int x = rand() % ROW;
        int y = rand() % COL;
        if (board[x][y] != '*')
        {
            board[x][y] = '*';//布置雷
            count++;
        }
    }
    //然后再将刚刚的区域消去
    for (int i = row - 1; i <= row + 1; i++)
    {
        for (int j = col - 1; j <= col + 1; j++)
        {
            board[i][j] = '#';//起到保护作用
        }
    }
}

//获取该格子周围雷的数量
int countMines(char board[ROW][COL], int row, int col)
{
    int count = 0;
    int i, j;
    for (i = row - 1; i <= row + 1; i++)
    {
        for (j = col - 1; j <= col + 1; j++)
        {
            if (i >= 0 && i < ROW && j >= 0 && j < COL && board[i][j] == '*')
            {
                count++;//如果有雷则+1，统计周围雷的总数
            }
        }
    }
    return count;//返回雷数
}

//将数据计算并加入棋盘
void InsertData(char board[ROW][COL],int row,int col)
{
    //便利整个棋盘
    for (row = 0; row < ROW; row++)
    {
        for (col = 0; col < COL; col++)
        {
            if (board[row][col] != '*' && board[row][col] != '\0')//判断是否为数字
            {
                int count = countMines(board, row, col);//获取旁边雷的数量
                if (count == 0)
                {
                    board[row][col] = '\0';//周围没雷的格子消去
                }
                else
                board[row][col] = count + '0';//周围有雷的格子加入数字
            }
        }
    }
}

//获取棋盘中字符a的数量
int Find_sign(char board[ROW][COL],char a)
{
    int count = 0;
    //遍历整个棋盘
    for (int row = 0; row < ROW; row++)
    {
        for (int col = 0; col < COL; col++)
        {
            if (board[row][col] == a)
            {
                count ++;//统计字符a的数量
            }
        }
    }
    return count;//返回数量
}

int Judge_func(char board[ROW][COL])
{
    //将雷全部标记完或者将其他空全部排完只剩雷则成功
    if (Find_sign(board,'#') == MINES)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//将内部棋盘可展示部分赋值给展示棋盘，并递归找空地
void revealCell(char board[ROW][COL], char displayBoard[ROW][COL], int row, int col)
{
    if (row < 0 || row >= ROW || col < 0 || col >= COL || displayBoard[row][col] != '#' || board[row][col] == '*') 
    {
        return;
    }
    //将输入点坐标对应的内部棋盘数据传递给展示棋盘
    displayBoard[row][col] = board[row][col];
    //如果为空地，则递归该指定坐标周围的八个格子，coner
    if (board[row][col] == '\0')
    {
        for (int i = row - 1; i <= row + 1; i++) 
        {
            for (int j = col - 1; j <= col + 1; j++) 
            {
                revealCell(board, displayBoard,i,j);
            }
        }
    }
}

int LCD_INIT()
{
    // 打开屏幕文件
    lcd_fd = open(LCDPATH,O_RDWR);
    if(lcd_fd == -1)
        return -1;
    
    lcd_point = mmap(NULL,LCDSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0);
    if(lcd_point == MAP_FAILED)
    {
        close(lcd_fd);
        lcd_fd = -1;
        return -1;
    }
    return 0;
}

int LCD_UNINIT()
{
    if(lcd_fd == -1)
        return -1;
    munmap(lcd_point,LCDSIZE);
    close(lcd_fd);
    return 0;
}

void DrawPixel(int h,int w,int color)
{
    // 约束一下坐标范围
    if(h>=0&&h<480&&w>=0&&w<800)
        *(lcd_point+h*800+w) = color;
}

//画棋盘线
void Drawboard_line(int color)
{
    int h = 0;
    int w = 1;
    //画竖线
    for (h = 1; h < 480; h++)
    {
        if (h % 40 == 0)
        {
            for (w = 0; w < 680; w++)
            {
                DrawPixel(h,w,color);
            }
        }
    }
    //画横线
    for (w = 0; w < 700; w++)
    {
        if (w % 40 == 0)
        {
            for (h = 0; h <= 440; h++)
            {
                DrawPixel(h,w,color);
            }
        }
    }
}

//画背景
void DrawBackgournd(int color)
{
    for(int h = 0;h < 480;h++)
    {
        for(int w = 0;w < 800;w++)
        {
            DrawPixel(h,w,color);
        }
    }
}

int DisplayBMPPicture(int posx,int posy,const char *pic_path)
{
    // 第一步：打开图片
    int pic_fd = open(pic_path,O_RDONLY);
    if(pic_fd == -1)
    {
        perror("Open bmp file failed");
        return 1;
    }

    // 第二步读取数据，判断文件类型
    unsigned char data[4]={0};
    read(pic_fd,data,2);

    // 判断是否为BMP文件
    if(data[0]!=0x42||data[1]!=0x4d)
    {
        puts("this picture not BMP file");
        close(pic_fd);
        return 1;
    }

    // 读取图片的偏移量
    lseek(pic_fd,0x0a,SEEK_SET);
    read(pic_fd,data,4);
    int offset = data[3]<<24|data[2]<<16|data[1]<<8|data[0];

    lseek(pic_fd,0x12,SEEK_SET);
    read(pic_fd,data,4);
    int width = data[3]<<24|data[2]<<16|data[1]<<8|data[0];

    read(pic_fd,data,4);
    int height = data[3]<<24|data[2]<<16|data[1]<<8|data[0];

    lseek(pic_fd,0x1C,SEEK_SET);
    read(pic_fd,data,2);
    int depth = data[1]<<8|data[0];

    // 如果图片的宽度不为4的倍数，那么图片显示将会异常。字节对齐引起
    int rand_num = 0; // 填充随机数
    if((width*(depth / 8))%4)
    {
        rand_num = 4 - ((width*(depth / 8))%4);
    }

    int real_width_bytes = (width*depth/8)+rand_num;

    // 像素数组的数据

    // 开辟了一个动态可变数组
    unsigned char *color=calloc(sizeof(char),real_width_bytes*abs(height));
    lseek(pic_fd,offset,SEEK_SET);
    read(pic_fd,color,real_width_bytes*abs(height));

    // 处理数据
    // 注意： BMP图片的像素数组对于三原色的存储是反着来的，也就是像素数组的第一个字节保存的是第一个像素点的Blue值

    unsigned char *color_p = color;
    // 显示整幅图像
    for(int h = 0;h < abs(height);h++)
    {
        for(int w = 0;w < width;w++)
        {
            unsigned char a,r,g,b;
            b =  *color_p++;
            g =  *color_p++;
            r =  *color_p++;
            a = (depth == 24)?0:*color_p++;
            //int color_var = color[2] << 16|color[1]<<8|color[0];
            int color_var = a<<24|r<<16|g<<8|b;
            DrawPixel(((height < 0)?h:height-1-h)+posy,w+posx,color_var);
        }
        
        // 更新指针
        color_p+= rand_num;
    }

    // 收尾工作
    free(color);
    close(pic_fd);
    return 0;
}

//显示棋盘
void Darw_board(int first,char board[ROW][COL])
{
    //最开始不显示结束游戏的图标
    if (first)
    {
        DisplayBMPPicture(680,5,"/mnt/image/mine/over.bmp");
    }
    for(int h = 0;h < ROW;h++)
    { 
        for(int w = 0; w < COL; w++)
        {
            //根据棋盘元素在对应位置打印相应的图片
            switch (board[h][w])
            {
            case '\0':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/0.bmp");
                break;
            case '1':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/1.bmp");
                break;
            case '2':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/2.bmp");
                break;
            case '3':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/3.bmp");
                break;
            case '4':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/4.bmp");
                break;
            case '5':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/5.bmp");
                break;
            case '6':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/6.bmp");
                break;
            case '7':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/7.bmp");
                break;
            case '8':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/8.bmp");
                break;
            case '*':
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/mines.bmp");
                break;
            default:
                DisplayBMPPicture(w*40,h*40,"/mnt/image/mine/initial.bmp");
                break;
            }
        }
    }
    //画棋盘线
    Drawboard_line(0x000000);
}

//获取手指点击屏幕的坐标,超出范围返回1,未超出范围返回-1
void Get_XY(int *x,int *y)
{
    int x1 = -1,y1 = -1;//滑动开始坐标

    int fd = open("/dev/input/event0",O_RDWR);
    if(fd == -1)
    {
        printf("open error\n");
        return;
    }
    struct input_event ev;

    while(1)
    {
        int ret = read(fd,&ev,sizeof(ev));
        if(ret != sizeof(ev))
        {
            continue;
        }
        //将获取到的坐标值赋值给x1,y1
        if(ev.type == EV_ABS && ev.code == ABS_X)
        {
            x1 = ev.value*800/1024;
        }
        if(ev.type == EV_ABS && ev.code == ABS_Y)
        {
            y1 = ev.value*480/600;
        }
        //手指离开时才会满足if条件
        if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
        {
            *x = x1 / 40;//用指针来更新传入函数的坐标
            *y = y1 / 40;
            close(fd);
            return;
        }
    }
}

//获取手指点击屏幕的坐标之后进行开始游戏的操作
void Start_game(char board[ROW][COL],char displayBoard[ROW][COL],int Y,int X)
{
    int first = 1;
    while(1)
    {
        //获取手指点击坐标
        Get_XY(&Y,&X);
        if (Y < 17 && X < 11)
        {
            //为了避免第一次就寄掉所以在第一次输入坐标之后再对内部棋盘进行落雷并插入数字
            if (first)//第一次选完点
            {
                DownMines(board,X,Y);//落雷

                InsertData(board,X,Y);//在内部棋盘插入数字

                first = 0;//之后输入坐标不再随机落雷与插数字
            }
            if (board[X][Y] == '*') //游戏以踩雷结束
            {
                DisplayBMPPicture(0,0,"/mnt/image/mine/gameover.bmp");
                sleep(1);
                DrawBackgournd(0xffffff);
                Darw_board(1,board);//打印出内部棋盘
                break;
            }
            else
            {
                revealCell(board,displayBoard,X,Y);//扩散
            }
            //显示展示棋盘
            Darw_board(1,displayBoard);
        }
        //主动结束游戏
        if (Y > 17 && X <2)
        {
            DisplayBMPPicture(0,0,"/mnt/image/mine/gameover.bmp");
            sleep(1);
            DrawBackgournd(0xffffff);
            Darw_board(1,board);//打印出内部棋盘
            break;
        }
        
        if (Judge_func(displayBoard))//雷被全部排完，成功通关
        {
            sleep(1);
            DisplayBMPPicture(0,0,"/mnt/image/mine/VICTORY.bmp");//展示成功通关图片
            sleep(1);
            DrawBackgournd(0xffffff);
            Darw_board(1,board);//打印出内部棋盘
            break;
        }
    }
    //出循环代表本局游戏结束，显示再来一局以及退出游戏程序的图片
    DisplayBMPPicture(680,80,"/mnt/image/mine/ag.bmp");
    DisplayBMPPicture(680,230,"/mnt/image/mine/exit.bmp");
    return;
}

//初始化界面
int Initial()
{
    int Y = -1;
    int X = -1;
    //展示界面图片
    DisplayBMPPicture(0,0,"/mnt/image/mine/start2.bmp");
    sleep(3);
    DisplayBMPPicture(0,0,"/mnt/image/mine/start.bmp");
    while(1)
    {   //获取点击坐标
        Get_XY(&Y,&X);
        //限制进入游戏区域
        if (Y < 13 && Y > 8 && X < 8 && X > 4)
        {
            return 1;
        }
    }
}

//重新开始游戏与退出游戏程序
int Restart()
{
    int X = -1;
    int Y = -1;
    while(1)
    {   //获取点击坐标
        Get_XY(&Y,&X);
        //限制重新开始的区域
        if (Y > 17 && X > 2 && X < 5)
        {

            return 1;
        }
        else if (Y >17 && X >5 && X <8)
        {
            return 0;
        }
    }
}


int mine_main()
{
    int first = 1;
    char displayBoard[ROW][COL];//展示棋盘
    char board[ROW][COL];//内部数据棋盘

    //初始化屏幕
    LCD_INIT();

    //初始化游戏界面,并限制进入游戏区域
    Initial();

    //第一次无条件开始游戏，first跟Restart不能替换位置，否则需要点两下
    while (first || Restart())
    {
        Creat_board(board);//初始化内部棋盘
        Creat_board(displayBoard);//初始化展示棋盘

        //设置背景板
        DrawBackgournd(0xffffff);
        
        int X = -1;//初始化坐标
        int Y = -1;

        //显示棋盘,最开始的时候无法结束游戏
        Darw_board(0,board);

        //感应触摸屏幕将XY值修改成点击的坐标
        Get_XY(&Y,&X);
        
        //获取到坐标之后开始进行游戏
        Start_game(board,displayBoard,Y,X);

        //将frist置零，之后的循环条件都是判断Restart
        first = 0;
    }
    //游戏结束
    DisplayBMPPicture(0,0,"/mnt/image/mine/gameover.bmp");
    sleep(2);
    //关闭屏幕
    LCD_UNINIT();
    sleep(2);    
    return 0;
}
