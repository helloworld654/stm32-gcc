#ifndef __GREY_H__
#define __GREY_H__

#include "sys.h"

#define MODIFYVAL  3 //ÿ�ε����Ĵ�С
#define MAXGREY  0xE0//���Ҷ�ֵ
#define MINGREY  0x0F  //��С�Ҷ�ֵ

/*����ɨ����ɵ�ͼ���С  120��80��*/
#define NEEDHEIGHT  80     //��λ80
#define NEEDWITH    120  //��Ϊ120

#define MIDHORIHEIGHT (((NEEDHEIGHT)/(SKIPLINE)/2) -1)//�м�ˮƽ�߶�λ�ã�
#define MIDVERIWIDETH  (NEEDWITH/2)//�м䴹ֱλ�õ�LOC�����ڱ��ڼ���ƫ��

/*��������Ĳ�ֵ*/
#define DOWNJUMP   254 //�Ӱ�ɫ����ɫ������
#define UPJUMP     -254//�Ӻ�ɫ����ɫ������

/*�������ұ߽���ʱ��ѡ������Сͼ�м������ļ������*/
#define SKIPLINE 4

#define DEVIATION 3 //�߽���������֮���ֵ�Ĳ�ֵ���������Χ

#define MIDHORLOC (((NEEDWITH) / 2) - 1)//�м�ˮƽλ���е�  59

void cameraRefresh(void);
void camera_refresh(void);
void changMidGrey(void);
void printToUart(void);
void getLineEdge(u8 *leftBlackLoc,u8 *rightBlackLoc,u16 startLine,u16 endLine,u16 skipLine);
void cameraOperation(void);
int getUsefulLine(void);
void getOneSideUsefulLine(u8 *needBlackLoc,u8 countZero,u8 *maxUBlackLine,u8 *maxUBlackLineLen,u8 *maxUBlackHeight);
int getLineLocCompare2MidLine(int *realVerticalLoc);
void memsetBothBlackLoc(void);
int getCmdByDeviLoc(void);
int getLineWidth(u8 *lMaxULine,u8 *rMaxULine,u8 lMaxULen,u8 rMaxULen,u8 *lMaxLineHei,u8 *rMaxLineHei,u8 *lineWidth);

#endif
