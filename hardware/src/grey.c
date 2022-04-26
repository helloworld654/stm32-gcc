#include "AllHead.h"
#include "grey.h"
#include "ov7670.h"
#include "lcd.h"
#include "exti.h"
#include "string.h"
#include "move.h"

#if defined(BIKING) && BIKING
extern u8 RUNNING;
#endif

#if defined(FENCHA_TEST) && FENCHA_TEST
u8 max_black_fencha = 80;
#if defined(MAP_TESTA) && MAP_TESTA
extern u8 fencha_times;
#endif
#endif

// u8 MidGreyVal = 0x78;//可调阀值
// u8 MidGreyVal = 0x60;//可调阀值  for wet day in windows
// u8 MidGreyVal = 0x58;//可调阀值 for day test in floor
// u8 MidGreyVal = 0x36;//可调阀值 for night test in windows
// u8 MidGreyVal = 0x48;//可调阀值 for night test in floor  fail!!
u8 MidGreyVal = 0x45;//可调阀值 for night test in A4 paper

// //截取出来的图片 是原图的1/8
u8 cutImg[NEEDHEIGHT][NEEDWITH] = {0};
// /*存储左右边界的黑点位置，大小为截出来的图片高度除以隔行扫描数*/
u8 leftBlackLoc[(NEEDHEIGHT)/(SKIPLINE)]  = {0};
u8 rightBlackLoc[(NEEDHEIGHT)/(SKIPLINE)] = {0};
/*存储最为有效的段，在左右边界中挑选出来*/
u8 maxUsefulBlackLine[(NEEDHEIGHT)/(SKIPLINE)] = {0};
u8 maxUsefulLineLen = 0;
//存储最长有效段的高度位置
u8 maxUsefulBlackHeight[(NEEDHEIGHT)/(SKIPLINE)] = {0};
extern u8 ov_sta;	//在exit.c里 面定义
extern u8 ov_frame;	//在timer.c里面定义		
/*拟合以后的斜率*/
double overK = 0;
int b;
/*检测到的直线相对于中线位置的偏移*/
int  lineDeviationLoc;
u8 FLAG_BORDER = 0;
u8 devLocRes = 0;

/*
函数功能，统一调用图像采集到数据处理的函数
*/
void cameraOperation(void)
{  
	u8 res,res1,res2,i;
	u8 test_str[20] = {0};
	static u8 line2stop = 0;
	static u8 slope2stop = 0;
	i = res2 = res1 = res = 0;
    camera_refresh();//图像采集二值化以及LCD显示
	/*紧接着分析边沿，获取左右黑点位置，最后一个参数为检测时的间隔行数*/
	getLineEdge(leftBlackLoc,rightBlackLoc,1,NEEDHEIGHT-1,SKIPLINE);   //the first is all 0,showing in serial port tool by printToUart(),so start at line 1
#if defined(DEBUG_CAMERA) && DEBUG_CAMERA
	for(i=0;i<(NEEDHEIGHT)/(SKIPLINE);i++){
		printf("%d:%d_%d ",i,leftBlackLoc[i],rightBlackLoc[i]);
	}
	printf("\r\n\r\n");
#endif
#if defined(MAP_TESTB) && MAP_TESTB	
	find_fencha_move();
#endif
	/*获取最长的有效段，只取一个有效段 save in maxUsefulBlackLine and maxUsefulBlackHeight*/
    res = getUsefulLine();
	// printToUart();
    if(res == BOTHLOST)//完全丢失，需要做的动作
	{
		//printf("NULL Slope\n");
		line2stop++;
		if(line2stop > 100){
			printStopMess(1);
#if defined(BIKING) && BIKING
			RUNNING = 0;
#endif
		}
	}
	else //其他情况都有斜率，试着计算出来
	{
		line2stop = 0;
		/*使用最小二乘法计算出斜率*/
		// res1 = regression(maxUsefulBlackLine,&maxUsefulLineLen,&overK,&b);
		res1 = regression(maxUsefulBlackLine,maxUsefulBlackHeight,&maxUsefulLineLen,&overK,&b);
		// printToUart();
		if(res1 == GOTSLOPE)
		{
			slope2stop = 0;
			/*专门做左右边界偏移检测的工作*/
			res2 = getCmdByDeviLoc();//根据直线相对的偏移获取的命令
#if defined(BIKING) && BIKING
			motation();
#endif
			print2lcd();
			maxUsefulLineLen = 0;
	      	memset(maxUsefulBlackLine,0,(NEEDHEIGHT)/(SKIPLINE));
	      	memset(maxUsefulBlackHeight,0,(NEEDHEIGHT)/(SKIPLINE));
		}
		else
		{
			slope2stop++;
			if(slope2stop > 100){
				printStopMess(2);
#if defined(BIKING) && BIKING
				// RUNNING = 0;
#endif
			}
		}
	}
	memsetBothBlackLoc();//做完一次摄像头采集刷新操作都要清空！
}

/*专门根据直线的水平位置偏移提供的接口，返回值就是命令*/
int getCmdByDeviLoc()
{
	// u8 devLocRes = 0;  
	devLocRes = getLineLocCompare2MidLine(&lineDeviationLoc);	
	switch(devLocRes)
	{
		case BOTHLOST:{
			printf("Both lost\r\n");return BOTHLOST;
		}
		case TOOLEFT:{
			printf("TOO LEFT\r\n");return TOOLEFT;
		}
		case TOORIGHT:{
			printf("TOO RIGHT\r\n");return TOORIGHT;
		}
		case NOMIDLOC:{
			printf("NO MID LOC\r\n");return NOMIDLOC;
		}
		case NOLINEWIDTH:{
			printf("NO LINE WIDTH\r\n");return NOLINEWIDTH;
		}
		case GETMIDLOC:{
			printf("DEV: %d \r\n",lineDeviationLoc);
			if((lineDeviationLoc >= 0) && (lineDeviationLoc <= 10)) return RIGHTDEVI0_10;
			if((lineDeviationLoc <= 0) && (lineDeviationLoc >= -10)) return LEFTDEVI0_10;
			if((lineDeviationLoc > 10 ) && (lineDeviationLoc <= 20)) return RIGHTDEVI10_20;
			if((lineDeviationLoc < -10) && (lineDeviationLoc >= -20)) return LEFTDEVI10_20;
			if((lineDeviationLoc > 20 ) && (lineDeviationLoc <= 30)) return RIGHTDEVI20_30;
			if((lineDeviationLoc < -20) && (lineDeviationLoc >= -30)) return LEFTDEVI20_30;
			if((lineDeviationLoc > 30 ) && (lineDeviationLoc <= 40)) return RIGHTDEVI30_40;
			if((lineDeviationLoc < -30) && (lineDeviationLoc >= -40)) return LEFTDEVI30_40;
			if((lineDeviationLoc > 40 )) return RIGHTDEVMORETHAN40;
			if((lineDeviationLoc < -40 )) return LEFTDEVMORETHAN40;
		}
		default:return ERR;
	}
}

/*清空两边原始检测到的边缘*/
void memsetBothBlackLoc()
{  
	memset(leftBlackLoc,0,(NEEDHEIGHT)/(SKIPLINE));
	memset(rightBlackLoc,0,(NEEDHEIGHT)/(SKIPLINE));
}

/*
函数功能：从OV7670的FIFO中读出像素，在翻转的基础上进行二值化，并在LCD上显示调试
*/
void cameraRefresh(void)
{
	u32 m = 0;u32 n = 0;u32 mm = 0;u32 nn = 0;u16 color;
	if(ov_sta)//有帧中断更新？
	{
		// printf("get ov_sta\r\n");
#if defined(LCD_ON_OFF) && LCD_ON_OFF
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
#if defined(LCD_SHOW_INFO) && LCD_SHOW_INFO
		LCD_Set_Window(50,50,120,80);//将显示区域设置到屏幕中央
#else
		LCD_Set_Window(100,100,120,80);//将显示区域设置到屏幕中央
#endif
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
#endif
		  
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK_H;
      
		for(m = 0;m < 240;m ++)//行
		{
			for(n = 0;n < 320;n ++)//列
			{
				//读取两个字节的数据
				OV7670_RCK_L;
#if defined(CHANGE_PIN) && CHANGE_PIN
				color=GPIOF->IDR&0XFF;	//读数据，摄像头使用PC0-7作为数据输入输出，GPIOC的端口输入数据寄存器
#else
				color=GPIOA->IDR&0XFF;	//读数据，摄像头使用PC0-7作为数据输入输出，GPIOC的端口输入数据寄存器
#endif
				OV7670_RCK_H; 
				color<<=8;  
				OV7670_RCK_L;
#if defined(CHANGE_PIN) && CHANGE_PIN
				color|=GPIOF->IDR&0XFF;	//读数据，虽然用不到，但是要是不读这个送color，整个屏幕会出现闪动，所以为了调试方便，还是读一下
#else
				color|=GPIOA->IDR&0XFF;	//读数据，虽然用不到，但是要是不读这个送color，整个屏幕会出现闪动，所以为了调试方便，还是读一下
#endif
				OV7670_RCK_H; 
				
				//上面读取的U和V进行清空，舍弃。
				color &= 0xff00;
				/*
				判断阀值，进行显示二值
				但为了显示到LCD上可以看到，使用两个字节进行表示二值
				*/
				if( m % 2 == 0)//选中需要的行了，隔着两行
				{
					if( n % 4 == 0)//选中的列，隔着四列
					{
						color >>= 8;
						/*进行数组的翻转存储*/
						if( (u8)color > MidGreyVal) //左移过后，低八位为需要的Y值
						{
							// cutImg[n / 4][m / 2] = 0xff;//白色
							cutImg[NEEDHEIGHT-n / 4][NEEDWITH-m / 2] = 0xff;//白色
						}
						else 
						{
							// cutImg[n / 4][m / 2] = 0x00;//黑色									
							cutImg[NEEDHEIGHT-n / 4][NEEDWITH-m / 2] = 0x00;//黑色									
						}
					}
				}
			}
		}   	
		ov_sta=0;					//清零帧中断标记
		ov_frame++;
		/*
		将采集到的像素存在数组中以后，再向LCD输出，LCD默认使用从左往右，从上往下的方式
		那么LCD上显示正向的图片，就说明我上面的数组的翻转做的是正确的。
		接下来对图像的解析，直接可以使用这个数组中的参数，默认图片就是从左往右，从上往下
		*/
#if defined(LCD_ON_OFF) && LCD_ON_OFF
		for(mm = 0;mm < 80;mm ++)  //80行
		{
			for(nn = 0;nn < 120;nn ++)  //120列
			{
				if(cutImg[mm][nn] == 0xff)
				{
					color = 0xffff;//黑色
					LCD->LCD_RAM = color; 
				}
				else if(cutImg[mm][nn] == 0x00)
				{
					color = 0x0000;//白色
					LCD->LCD_RAM = color; 
				}
			}
		}
#endif
	 }
}	

void camera_refresh(void)
{
	u32 i,j;
 	u16 color;	 
	uint8_t midGrey = 0x58;
	if(ov_sta==2)
	{
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
		LCD_Set_Window(50,50,120,80);//将显示区域设置到屏幕中央
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK_H;  
		for(i=0;i<240;i++){
			for(j=0;j<320;j++)
			{
				OV7670_RCK_L;
				color=GPIOF->IDR&0XFF;	//读数据
				OV7670_RCK_H; 
				color<<=8;  
				OV7670_RCK_L;
				color|=GPIOF->IDR&0XFF;	//读数据
				OV7670_RCK_H; 
				// LCD->LCD_RAM=color;    
				if(i%2 == 0){
					if(j%4 == 0){
						color &= 0xff00;
						color >>= 8;
						if((uint8_t)color > midGrey){
							// LCD->LCD_RAM=0xffff;
							cutImg[NEEDHEIGHT-j/4-1][NEEDWITH-i/2-1] = 0xff;
						}
						else{
							// LCD->LCD_RAM=0x0;
							cutImg[NEEDHEIGHT-j/4-1][NEEDWITH-i/2-1] = 0x0;
						}
					}
				}
			}   			
		}
		EXTI_ClearITPendingBit(EXTI_Line12);  //清除LINE8上的中断标志位
		ov_sta=0;					//开始下一次采集
 		ov_frame++; 
		for(i=0;i<NEEDHEIGHT;i++){
			for(j=0;j<NEEDWITH;j++){
				if(cutImg[i][j] == 0xff)
					LCD->LCD_RAM=0xffff;
				else
					LCD->LCD_RAM=0x0;
			}
		}				 
	} 
}	

/*调试使用，打印到串口上*/
void printToUart()
{
	u8 res;
	u32 mm;
	u32 nn;
    u32 i,j;
# if 0
	printf("\r\n");
	for(i = 0;i < NEEDHEIGHT;i ++)
    {
		printf("****");
		for(j = 0;j < NEEDWITH;j ++)
		{
			if(cutImg[i][j] == 0xff)
			{
				printf("1");
			}
			else
			{
				printf("0");
			}
				
		}
		printf("****\n");
	}
	printf("\r\n");
#endif
	
#if 0
	/*打印左右边界点*/
	printf("Left:");
	for(mm = 0;mm < (NEEDHEIGHT)/(SKIPLINE);mm ++)
	{
		printf("%d ",leftBlackLoc[mm]);
	}
	printf("\n");
	printf("******************");
	printf("\n");
	printf("Right:");
	for(nn = 0;nn < (NEEDHEIGHT)/(SKIPLINE);nn ++)
	{
		printf("%d ",rightBlackLoc[nn]);
	}
#endif

#if 1
	printf("MAX Len:%d:----",maxUsefulLineLen);
	for(nn = 0;nn < maxUsefulLineLen;nn ++)
	{
		printf("%d:%d-%d ",nn,maxUsefulBlackLine[nn],maxUsefulBlackHeight[nn]);
	}
	nn = (int)(1000*overK);
    printf("\r\nK:%d,b:%d\n",nn,b);
#endif		
}

/*
函数功能：获得黑线的左右侧黑点位置
参数：供存储黑线左右位置的数组  开始行  结束行  间隔行数
*/
void getLineEdge(u8 *leftBlackLoc,u8 *rightBlackLoc,u16 startLine,u16 endLine,u16 skipLine)
{
	u16 i = 0;
	//u16 j = 0;
	u16 tmpHeight = 0; 
	u8 left_loc_get = 0;
	u8 right_loc_get = 0;
#if defined(MAP_TESTA) && MAP_TESTA
	u8 get_fencha = 0;
	u8 fencha_start = 0;
#endif
	FLAG_BORDER = 0;
	
    /*间隔扫描几行*/	
	for(tmpHeight = startLine;tmpHeight < endLine;tmpHeight += skipLine)
	{
		left_loc_get = 0;
		right_loc_get = 0;
#if defined(MAP_TESTA) && MAP_TESTA
		get_fencha = 0;
#endif
		/*一行中的检测跳变，每行中就检测一个左点，一个右点*/
		for(i = 0;i < NEEDWITH - 3;i ++)  //连续判断三个点，所以最后三个点舍去
		{
#if defined(MAP_TESTA) && MAP_TESTA
			if(cutImg[tmpHeight][i] == 0){
				if(get_fencha == 0)
					fencha_start = i;
				get_fencha++;
			}
#endif
			if(i==NEEDWITH-4 && cutImg[tmpHeight][NEEDWITH-3]==0 && cutImg[tmpHeight][NEEDWITH-2]==0)
			{
				right_loc_get = 1;
				*rightBlackLoc = NEEDWITH-2;
				FLAG_BORDER = 2;
			}
				/*检测到正跳变，紧接着就是相同的高电平，那么就是右边的黑色点被检测到了 i+1就是黑点的位置*/
			else if(  ((cutImg[tmpHeight][i] - cutImg[tmpHeight][i+1] ) <= UPJUMP) && 
					(cutImg[tmpHeight][i+1] == cutImg[tmpHeight][i+2])  )
			{
				right_loc_get = 1;
				*rightBlackLoc = i+1;
			}

			if(i==0 && cutImg[tmpHeight][1]==0 && cutImg[tmpHeight][2]==0)
			{
				left_loc_get = 1;
				*leftBlackLoc = 1;
				FLAG_BORDER = 1;
			}
			/*检测到负跳变，紧接着就是相同的低电平，那么就是左侧的黑点被检测到了 i+1就是黑点的位置*/
			else if( (cutImg[tmpHeight][i] - cutImg[tmpHeight][i+1] ) >= DOWNJUMP && 
					((cutImg[tmpHeight][i+1] == cutImg[tmpHeight][i+2])) )
			//存储左黑点的位置
			{
				left_loc_get = 1;
				*leftBlackLoc = i+1;
			}
		}
		if(left_loc_get == 0){
			*leftBlackLoc = 0;
		}
		if(right_loc_get == 0){
			*rightBlackLoc = 0;
		}
			//准备下一行
		rightBlackLoc ++;leftBlackLoc  ++;				
#if defined(MAP_TESTA) && MAP_TESTA
		if(get_fencha > max_black_fencha){
			fencha_times++;
			if(fencha_times < 5){
				move_for_fencha(fencha_times);
				if(fencha_times == 5)
					RUNNING = 0;
				if(fencha_start <= 20)
					printStopMess(5);
				else
					printStopMess(6);
				break;
			}
		}
#endif
	}
}

// /*将获取有效段的函数部分进行复用*/
// //参数分别是 原始段  段前0的个数  存储最长有效段的位置数组  存储最长长度  存储最长有效段的高度位置
void getOneSideUsefulLine(u8 *needBlackLoc,u8 countZero,u8 *maxUBlackLine,u8 *maxUBlackLineLen,u8 *maxUBlackHeight)
{
	int i = 0;
	int j = 0;
	/*截取最长段的临时存储变量*/
	u8 tmpMaxUsefulLineLen = 0;//实时保存当前最长的段长度
	u8 tmpMaxUsefulLine[(NEEDHEIGHT)/(SKIPLINE)]  = {0}; //用于保存有效的最长段
	u8 tmpmaxUBlackHeight[(NEEDHEIGHT)/(SKIPLINE)] = {0};//保存高度
	*maxUBlackLineLen = 0;
	//从左边第一个有效的地方往后检测，直到最后
	for(i = countZero;i < (NEEDHEIGHT)/(SKIPLINE) - 2;i ++)
	{
		/*
			如果第三个位置，直接就是0，那么认为这一段就要采集结束了，
			将临时的段存储到全局的变量中
		*/
		if(needBlackLoc[i+2] == 0)//一段采集结束
		{
			//如果段比较长的话，才选中
			if((*maxUBlackLineLen) < tmpMaxUsefulLineLen)
			{
				//存储
				(*maxUBlackLineLen) = tmpMaxUsefulLineLen;
				memset(maxUBlackLine,0,(NEEDHEIGHT)/(SKIPLINE));//清空以备存储
				memset(maxUBlackHeight,0,(NEEDHEIGHT)/(SKIPLINE));
				for(j = 0;j <= tmpMaxUsefulLineLen;j ++)
				{
					maxUBlackLine[j] = tmpMaxUsefulLine[j];
					maxUBlackHeight[j] = tmpmaxUBlackHeight[j];//存储高度
				}
			}
			//存完清空
			memset(tmpMaxUsefulLine,0,(NEEDHEIGHT)/(SKIPLINE));
			memset(tmpmaxUBlackHeight,0,(NEEDHEIGHT)/(SKIPLINE));
			tmpMaxUsefulLineLen = 0;
			/*结束*/
			//将i+2以及后续有0的位置全部跳过
			if((i + 3) < (NEEDHEIGHT)/(SKIPLINE))//放置越界
			{
				i += 3;//从i + 3位置开始
			}
			/*未到最后第三个，并且一直为0，那么就继续*/
			while((i < ((NEEDHEIGHT)/(SKIPLINE) - 2)) && (needBlackLoc[i] == 0))
			{
				i ++;
			}
			continue;
		}
		//连续三个位置的比较，实时存储
		if((needBlackLoc[i] != 0) && (needBlackLoc[i+1] != 0) && (needBlackLoc[i+2] != 0) &&
			(abs((needBlackLoc[i] - needBlackLoc[i+1]) - (needBlackLoc[i+1] - needBlackLoc[i+2])) <= DEVIATION))
		{
			/*存储进临时数组中*/
			tmpMaxUsefulLine[tmpMaxUsefulLineLen] = needBlackLoc[i];
			// tmpmaxUBlackHeight[tmpMaxUsefulLineLen] = i;//存储高度
			tmpmaxUBlackHeight[tmpMaxUsefulLineLen] = i*SKIPLINE;//存储高度
			tmpMaxUsefulLineLen ++;
		}
	}
	
	/*从有效位置开始，全部有符合的位置，做存储和清空操作*/
	//如果段比较长的话，才选中
	if((*maxUBlackLineLen) < tmpMaxUsefulLineLen)
	{
		//存储
		(*maxUBlackLineLen) = tmpMaxUsefulLineLen;
		memset(maxUBlackHeight,0,(NEEDHEIGHT)/(SKIPLINE));//清空高度，以备存储
		memset(maxUBlackLine,0,(NEEDHEIGHT)/(SKIPLINE));//清空以备存储
		for(j = 0;j <= tmpMaxUsefulLineLen;j ++)
		{
			maxUBlackLine[j] = tmpMaxUsefulLine[j];
			maxUBlackHeight[j] = tmpmaxUBlackHeight[j];//存储高度
		}
	}
	//存完清空
	memset(tmpMaxUsefulLine,0,(NEEDHEIGHT)/(SKIPLINE));
	memset(tmpmaxUBlackHeight,0,(NEEDHEIGHT)/(SKIPLINE));
	tmpMaxUsefulLineLen = 0;
}

// /*函数功能：从边界数组中，提取出有效的连续段，取出最为有效可靠的段。*/
int getUsefulLine()
{	  
	//默认边界没有找到
	u8 LeftDirect = LEFTLOST;
	u8 RightDirect = RIGHTLOST;
	/*临时变量*/
	u8 countLeftZero = 0;//统计一个边界中的0的无用数据个数的临时变量
	u8 countRightZero = 0;//统计一个边界中的0的无用数据个数的临时变量
	u8 i;

/*预先对左右边界进行处理，如果没有检测到就必须返回，结束此函数*/
	/*左边滤除数组开始的0操作*/
	while((countLeftZero < 20) && (leftBlackLoc[countLeftZero] == 0))
	{
		countLeftZero ++;//下一个数据
	}
	if(countLeftZero == 20)//如果全部为0，那么就是没有边界，直接将标志位置为lost丢失
	{
		LeftDirect = LEFTLOST;
	}
	else
	{
		LeftDirect = GETDIRECT;
	}
	
	/*右边滤除0操作*/
	while((countRightZero < 20) && (rightBlackLoc[countRightZero] == 0))
	{
		//下一个数据
		countRightZero ++;
	}
	if(countRightZero == 20)//如果全部为0，那么就是没有边界，直接将标志位置为lost丢失
	{
		RightDirect = RIGHTLOST;
	}
	else
	{
		RightDirect = GETDIRECT;
	}
	//均丢失，那么就直接结束函数
	if((LeftDirect == LEFTLOST) && (RightDirect == RIGHTLOST))
	{
		return BOTHLOST;
	}
	
	if(LeftDirect==GETDIRECT && RightDirect==GETDIRECT && FLAG_BORDER!=0){
		if(FLAG_BORDER == 1){
			getOneSideUsefulLine(rightBlackLoc,countRightZero,maxUsefulBlackLine,&maxUsefulLineLen,maxUsefulBlackHeight);
		}
		else if(FLAG_BORDER == 2){
			getOneSideUsefulLine(leftBlackLoc,countLeftZero,maxUsefulBlackLine,&maxUsefulLineLen,maxUsefulBlackHeight);
		}
	}
	else{
		/*左右边界检测开始*/
		/*如果左边边界检测到了，那么才检测这个数组*/
		if(LeftDirect == GETDIRECT)
		{
			getOneSideUsefulLine(leftBlackLoc,countLeftZero,maxUsefulBlackLine,&maxUsefulLineLen,maxUsefulBlackHeight);
#if defined(DEBUG_CAMERA) && DEBUG_CAMERA
			printf("get left line,maxusefullen:%d\r\n",maxUsefulLineLen);
			for(i=0;i<(NEEDHEIGHT)/(SKIPLINE);i++){
				printf("%d ",leftBlackLoc[i]);
			}
			printf("\r\n");
#endif
		}
		/*如果右边边界检测到了，那么才开始检测数组*/
		else if(RightDirect == GETDIRECT)
		{
			getOneSideUsefulLine(rightBlackLoc,countRightZero,maxUsefulBlackLine,&maxUsefulLineLen,maxUsefulBlackHeight);
		}
	}
	
	/*要是只有一个或者根本就没有获取到最长段数据，那么这次的采集就是失败的，返回失败的信号*/
	if((maxUsefulLineLen == 0) || (maxUsefulLineLen == 1) || (maxUsefulLineLen == 2) || (maxUsefulLineLen == 3))
	{
			/*清空返回*/
		maxUsefulLineLen = 0;
		memset(maxUsefulBlackHeight,0,(NEEDHEIGHT)/(SKIPLINE));
		memset(maxUsefulBlackLine,0,(NEEDHEIGHT)/(SKIPLINE));			
		return BOTHLOST;
	}
	return GETDIRECT;
}

u8 testLineWidth = 0;

/*获得当前线宽*/
int getLineWidth(u8 *lMaxULine,u8 *rMaxULine,u8 lMaxULen,u8 rMaxULen,u8 *lMaxLineHei,u8 *rMaxLineHei,u8 *lineWidth)
{
	u8 i;
	// u8 j;
	// u8 m,n;
	//检测一个水平位置上，两个有效段均存在这个水平位置上有位置
/*首先获得斜线的水平线宽*/
	if(lMaxULen <= rMaxULen)//左边最长有效线段较短，依据左边
	{
		for(i = 0;i < lMaxULen;i ++)//开始匹配同一水平位置左右均存在有效交点
		{
			// if(rightBlackLoc[lMaxLineHei[i]] != 0)//在左边有效位置高度，存在右边有效线段交点
			if(rightBlackLoc[lMaxLineHei[i]/SKIPLINE] != 0)//在左边有效位置高度，存在右边有效线段交点
			{
				// *lineWidth = abs(rightBlackLoc[lMaxLineHei[i]] - lMaxULine[i]);
				*lineWidth = abs(rightBlackLoc[lMaxLineHei[i]/SKIPLINE] - lMaxULine[i]);
				/*test*/
				testLineWidth = *lineWidth;
				return GOTLINEWIDTH;
			}
		}
	}
	else //右边的最小有效段较短，依据右边
	{
		for(i = 0;i < rMaxULen;i ++)//开始匹配同一水平位置左右均存在有效交点
		{
			if(leftBlackLoc[rMaxLineHei[i]] != 0)//在左边有效位置高度，存在右边有效线段交点
			if(leftBlackLoc[rMaxLineHei[i]/SKIPLINE] != 0)//在左边有效位置高度，存在右边有效线段交点
			{
				// *lineWidth = abs(leftBlackLoc[rMaxLineHei[i]] - rMaxULine[i]);
				*lineWidth = abs(leftBlackLoc[rMaxLineHei[i]/SKIPLINE] - rMaxULine[i]);
				/*test*/
				testLineWidth = *lineWidth;
				return GOTLINEWIDTH;
			}
		}
	}
	//出错处理
	*lineWidth = 0;
	/*test*/
	testLineWidth = *lineWidth;
	return NOLINEWIDTH;
}
/*
函数功能：获取的是，当前检测到的直线（不论有没有倾斜），它相对于直线在屏幕中央的相对偏移
左偏为负，正偏为正
思路：首先滤除左右边界一开始的0点
然后，使用获取最长有效段的函数接口，获得两个边界的最长有效段
然后，依据比较短的最长有效段，计算出当前的线宽
再用中点位置，和一边的距离，整合线宽就可以计算出偏移量了。
*/
int getLineLocCompare2MidLine(int *realVerticalDevationLoc)//传入用于返回的变量，为相对的偏移量
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 res = 0;
/*保存临时的左右最长有效边界的数组*/
	u8 leftMaxULineLoc[((NEEDHEIGHT)/(SKIPLINE))] = {0};
	u8 rightMaxULineLoc[((NEEDHEIGHT)/(SKIPLINE))] = {0};
	//最长长度
	u8 leftMaxULen = 0;
	u8 rightMaxULen = 0;
	//有效段的高度位置
	u8 leftMaxUBlackHeight[((NEEDHEIGHT)/(SKIPLINE))] = {0};
	u8 rightMaxUBlackHeight[((NEEDHEIGHT)/(SKIPLINE))] = {0};
	//当前线宽
	u8 lineWidth = 0;
/******预先对左右边界进行处理，如果没有检测到就必须返回，结束此函数*****/
/*首先将段开始的无用数据滤除*/
	//默认边界没有找到
	u8 LeftDirect = LEFTLOST;
	u8 RightDirect = RIGHTLOST;
	/*临时变量*/
	u8 countLeftZero = 0;//统计一个边界中的0的无用数据个数的临时变量
	u8 countRightZero = 0;//统计一个边界中的0的无用数据个数的临时变量
	u8 left_nums,right_nums;
	/*左边滤除数组开始的0操作*/
	while((countLeftZero < 20) && (leftBlackLoc[countLeftZero] == 0))
	{ countLeftZero ++;}//下一个数据
	if(countLeftZero == 20)//如果全部为0，那么就是没有边界，直接将标志位置为lost丢失
	{LeftDirect = LEFTLOST;}
	else{LeftDirect = GETDIRECT;}
	/*右边滤除0操作*/
	while((countRightZero < 20) && (rightBlackLoc[countRightZero] == 0))
	{countRightZero ++;}//下一个数据
	if(countRightZero == 20)//如果全部为0，那么就是没有边界，直接将标志位置为lost丢失
	{RightDirect = RIGHTLOST;}
	else{RightDirect = GETDIRECT;}
	//均丢失，那么就直接结束函数
	if((LeftDirect == LEFTLOST) && (RightDirect == RIGHTLOST))
	{
		return BOTHLOST;//只返回丢失信号，不做偏移量复制
	}
/***********************************************************************/
	/*没有检测到左边的边界 那么就是太偏左了，这时根本确定不了偏移量，只能返回设置的最大值*/
	if((LeftDirect == LEFTLOST) && (RightDirect == GETDIRECT))
	{
		*realVerticalDevationLoc = rightBlackLoc[countRightZero]/2-NEEDWITH/2;
		return TOOLEFT;
	}
	/*没有检测到右边的边界*/
	if((LeftDirect == GETDIRECT) && (RightDirect == RIGHTLOST))
	{
		*realVerticalDevationLoc = (leftBlackLoc[countLeftZero]+NEEDWITH)/2 - NEEDWITH/2;
		return TOORIGHT;
	}
	/*************************以下均为有两边边界的*********************************/
	
	/*如果检测到了左右边界，再分一下几种情况*/
	//首先采集一次左右边界的有效段。
	getOneSideUsefulLine(leftBlackLoc,countLeftZero,leftMaxULineLoc,&leftMaxULen,leftMaxUBlackHeight);
	getOneSideUsefulLine(rightBlackLoc,countRightZero,rightMaxULineLoc,&rightMaxULen,rightMaxUBlackHeight);
	//那么现在leftMaxULineLoc leftMaxULen 和 rightMaxULineLoc  rightMaxULen中已经可以使用了
	//过滤有效段太短的边界，直接返回太偏左偏右即可
	if((leftMaxULen <= 2)){
		*realVerticalDevationLoc = rightBlackLoc[countRightZero]/2-NEEDWITH/2;
		return TOOLEFT;
	}
	if((rightMaxULen <= 2)){
		*realVerticalDevationLoc = (leftBlackLoc[countLeftZero]+NEEDWITH)/2 - NEEDWITH/2;
		return TOORIGHT;
	}
	/*获得线宽*/
	res = getLineWidth(leftMaxULineLoc,rightMaxULineLoc,leftMaxULen,rightMaxULen,leftMaxUBlackHeight,rightMaxUBlackHeight,&lineWidth);
	if(res == NOLINEWIDTH)//如果没有获取到线宽
	{
		lineWidth = 0;
		*realVerticalDevationLoc = 0;
		return NOLINEWIDTH;
	}
	/*开始使用整个屏幕中点去计算偏移，默认肯定检测到了两边*/
	//从左边开始
	for(i = 0;i < leftMaxULen;i ++)
	{
		// if(leftMaxUBlackHeight[i] == MIDHORIHEIGHT)//发现左边与水平中线有交点
		if(leftMaxUBlackHeight[i]/SKIPLINE == MIDHORIHEIGHT)//发现左边与水平中线有交点
		{
#if defined(DEBUG_CAMERA) && DEBUG_CAMERA
			printf("%d:width/2:%d,leftloc:%d\r\n",i,(lineWidth / 2),leftMaxULineLoc[i]);
			for(k=0;k<leftMaxULen;k++){
				printf("%d:%d_%d ",k,leftMaxULineLoc[i],leftMaxUBlackHeight[k]);
			}
			printf("\r\n");
#endif
			/*根据直线位于中点的四个位置，获得的两种式子，两个式子结果互为相反数*/
		
			//则整个线都偏右，结果为正！
			if((leftMaxULineLoc[i] + (lineWidth / 2)) > MIDHORLOC)
			{
				*realVerticalDevationLoc = (lineWidth / 2) - MIDHORLOC + leftMaxULineLoc[i];
				return GETMIDLOC;
			}
			else  //整个线都偏左，结果为负
			{
				//正值的偏移量
				*realVerticalDevationLoc = MIDHORLOC - leftMaxULineLoc[i] - (lineWidth / 2);
				*realVerticalDevationLoc = 0 - *realVerticalDevationLoc;//取反以后，返回为负值，表示为左偏
				return GETMIDLOC;
			}
		}
	}
	/*再做右边*/
	for(i = 0;i < rightMaxULen;i ++)
	{
		// if(rightMaxUBlackHeight[i] == MIDHORIHEIGHT)//发现左边与水平中线有交点
		if(rightMaxUBlackHeight[i]/SKIPLINE == MIDHORIHEIGHT)//发现左边与水平中线有交点
		{
			/*根据直线位于中点的四个位置，获得的两种式子，两个式子结果互为相反数*/
			//黑线偏右
			if((rightMaxULineLoc[i] + (lineWidth / 2)) > MIDHORLOC)
			{
				*realVerticalDevationLoc = rightMaxULineLoc[i] - MIDHORLOC - (lineWidth / 2);
				return GETMIDLOC;
			}
			else
			{
				*realVerticalDevationLoc = (lineWidth / 2) - rightMaxULineLoc[i] + MIDHORLOC;
				*realVerticalDevationLoc = 0 - *realVerticalDevationLoc;
				return GETMIDLOC;
			}
		}
	}
/* 左下角、右下角优化方法  */
	// left_nums = 0;
	// right_nums = 0;
	// getOneSideUsefulLine(leftBlackLoc,countLeftZero,leftMaxULineLoc,&leftMaxULen,leftMaxUBlackHeight);
	// getOneSideUsefulLine(rightBlackLoc,countRightZero,rightMaxULineLoc,&rightMaxULen,rightMaxUBlackHeight);
	// if(leftMaxUBlackHeight[leftMaxULen] < )     //the lowest point
	// for(i=0;i<leftMaxULen;i++){
	// 	if(leftMaxULineLoc[i] < NEEDWITH/2){
	// 		left_nums++;
	// 	}
	// }
	// for(i=0;i<rightMaxULen;i++){
	// 	if(rightMaxULineLoc[i] > NEEDWITH/2){
	// 		right_nums++;
	// 	}
	// }
	// if(left_nums >= leftMaxULen*2/3){
	// 	*realVerticalDevationLoc = -NEEDWITH/2;
	// }
	// else if(right_nums >= rightMaxULen*2/3){
	// 	*realVerticalDevationLoc = NEEDWITH/2;
	// }
	return NOMIDLOC;
}

int regression(u8 *maxLine,u8 *maxLineH,u8 *maxLen,double *oK,int *ob)
{
	  /*累积变量*/
	  double x_sum = 0,y_sum = 0,xy_sum = 0,xx_sum = 0;
		double x_avr = 0,y_avr = 0,xy_avr = 0,xx_avr = 0;
		int i;
		x_sum=0;y_sum=0;xy_sum=0;xx_sum=0;
		for(i=0;i < *maxLen;i++)
		{
				x_sum+=maxLine[i];
				// y_sum+= i;
				y_sum+= (NEEDHEIGHT - maxLineH[i]);
				// xy_sum+=maxLine[i]*(i);
				xy_sum+=maxLine[i]*(NEEDHEIGHT-maxLineH[i]);
				xx_sum+=maxLine[i]*maxLine[i];
		}
		x_avr=x_sum/(*maxLen);
		y_avr=y_sum/(*maxLen);
		xy_avr=xy_sum/(*maxLen);
		xx_avr=xx_sum/(*maxLen);
		
		/*公式中分子为0*/
		if((xy_avr-x_avr*y_avr)==0)
		{			  
			  *oK = 0;
			  /*用完清除无用的最长有效段，以便下一次采集*/
			//   *maxLen = 0;
	    //   memset(maxLine,0,(NEEDHEIGHT)/(SKIPLINE));	  
				return NOSLOPE;
		}
		else
		{
				*oK = (xx_avr-x_avr*x_avr)/(xy_avr-x_avr*y_avr);//反斜率
				// *oK = (xy_avr-x_avr*y_avr)/(xx_avr-x_avr*x_avr);//正斜率
				// *oK = 0 - (*oK);
//				realK=(xy_avr-x_avr*y_avr)/(xx_avr-x_avr*x_avr);//正斜率
				// *ob = maxLine[0];//获取K
				*ob = 80 - maxLineH[0]-(1/(*oK))*(maxLine[0]);//获取K
			
			   //滤除无用斜率
			  if(((*oK) >= 20) || ((*oK) <= -20))
				{
						(*oK) = 0;
				}
			
        /*用完清除无用的最长有效段，以便下一次采集*/
			//   *maxLen = 0;
	    //   memset(maxLine,0,(NEEDHEIGHT)/(SKIPLINE));		
			  
				return GOTSLOPE;
		}
}
