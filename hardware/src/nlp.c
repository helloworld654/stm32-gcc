#include "led_c8.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "usart.h"
#include "esp8266.h"

#define PRINT_NLP_INFO    0
/*    used for same string calcu    */
#define EQUAL   1   //EQUAL表示c[i][j]是由c[i-1][j-1]+1来的=此时两个序列有相同的字符
#define UP      2   //UP表示c[i][j]是由c[i-1][j]来的========此时两个序列没有相同的字符
#define LEFT    3   //LEFT表示c[i][j]是由[ci][j-1]来的======此时两个序列没有相同的字符
#define SAME_STR_NUM  50

char same_str[SAME_STR_NUM] = {0};
int same_index = 0;

/*    info used send to esp    */
unsigned char nlp_key[] = "kEy3701a0635063d428";
unsigned char wifi_ssid[] = "helloworld123456";
unsigned char wifi_password[] = "HelloWorld123456";

uint8_t setup_nlp(char *nlp_data[3])
{
	uart1_init(115200);
	Usart2_Init(115200);
	reset_esp_process();
	ESP8266_Clear();
    ESP8266_SendCmd("AT","OK");

/*    AT command test    */
	if(!ESP8266_SendCmd("AT","OK")){
		UsartPrintf(USART_DEBUG, "send AT success\r\n");
	}
	else{
		UsartPrintf(USART_DEBUG, "send AT fail\r\n");
        return 1;
	}
/*    send nlp key string    */
	if(!ESP8266_SendCmd("nlp_key","OK")){
		UsartPrintf(USART_DEBUG, "ready to send nlp_key\r\n");
	}
	else{
		UsartPrintf(USART_DEBUG, "send nlp_key not ready\r\n");
        return 1;
	}
	if(!ESP8266_SendCmd(*nlp_data,"OK")){
		UsartPrintf(USART_DEBUG, "send nlp_key content success\r\n");
	}
	else{
		UsartPrintf(USART_DEBUG, "send nlp_key content fail\r\n");
        return 1;
	}
/*    send wifi ssid string    */
	if(!ESP8266_SendCmd("wifi_ssid","OK")){
		UsartPrintf(USART_DEBUG, "ready to send wifi_ssid\r\n");
	}
	else{
		UsartPrintf(USART_DEBUG, "send wifi_ssid not ready\r\n");
        return 1;
	}
	if(!ESP8266_SendCmd(*(nlp_data+1),"OK")){
		UsartPrintf(USART_DEBUG, "send wifi_ssid content success\r\n");
	}
	else{
		UsartPrintf(USART_DEBUG, "send wifi_ssid content fail\r\n");
        return 1;
	}
/*    send wifi password string    */
	if(!ESP8266_SendCmd("wifi_password","OK")){
		UsartPrintf(USART_DEBUG, "ready to send wifi_password\r\n");
	}
	else{
		UsartPrintf(USART_DEBUG, "send wifi_password not ready\r\n");
        return 1;
	}
	if(!ESP8266_SendCmd(*(nlp_data+2),"OK")){
		UsartPrintf(USART_DEBUG, "send wifi_password content success\r\n");
	}
	else{
		UsartPrintf(USART_DEBUG, "send wifi_password content fail\r\n");
        return 1;
	}

    return 0;
}

/***函数一、判断LCS长度***/
int Lcs_len(char *str1, char *str2, int **char1, int **char2)
{
    int m = strlen(str1);
    int n = strlen(str2);  //求出两个数组的边界长度
    int i, j;

    for (i = 0; i <=  m; i++)
    {
        char1[i][0] = 0;
    }
 
    for (j = 0; j <=  n; j++)  //初始化边界条件
    {
        char1[0][j] = 0;
    }
 
    for ( i = 1; i <= m; i++)
    {
        for ( j = 1; j <= n; j++)
        {
            if( str1[i-1] == str2[j-1] )
    // 这里使用i-1以及j-1是由于数组的下标从0开始
    //另一种实现方式是逆序实现，对于路径的确定更方便
            {
                char1[i][j] = char1[i-1][j-1] + 1;
                char2[i][j] = EQUAL;
            }
            else if (char1[i-1][j] >= char1[i][j-1])//在j循环时若字符串不等
                {                                // 则只用判断char中的元素
                char1[i][j] = char1[i-1][j];
                char2[i][j] = UP;
                }
            else
                {
                char1[i][j] = char1[i][j-1];
                char2[i][j] = LEFT;
                }
        }
 
 
    }
    return char1[m][n];  //递归的最终位存储的数字就是LCS长度
}
 
/***函数二、输出LCS***/
void Print_Lcs( char *str, int **b, int i, int j)
{
    if( i == 0 || j == 0)
        return;    //递归至边界则扫描完毕
    if( b[i][j] == EQUAL)
    {                     //对于相等的元素，其路径为左上方对角移动
        Print_Lcs(str, b, i - 1, j - 1);
#if defined(PRINT_NLP_INFO) && PRINT_NLP_INFO
        // printf("%c ", str[i-1]);  //相等的话，原字符序列向前递归一位并打印出字符
        printf("%02x ", str[i-1]);  //相等的话，原字符序列向前递归一位并打印出字符
#endif
		same_str[same_index++] = str[i-1];
    }
    else if ( b[i][j] == UP )  //不相等时判断方向：向上则数组向上位移
        Print_Lcs(str, b, i - 1, j);
    else
        Print_Lcs(str, b, i , j - 1); //否则数组下标向左位移一位
 
}
 
/***函数三、整合LCS函数***/
uint8_t Find_Lcs( char *str1, char *str2)
{
    int i,j,length,result_same;
    int len1 = strlen(str1),
        len2 = strlen(str2);
    //申请二维数组
    int **c = (int **)malloc(sizeof(int*) * (len1 + 1));
    int **b = (int **)malloc(sizeof(int*) * (len1 + 1));
	same_index = 0;
    for( i = 0; i<= len1; i++ )  //这个等号之前没加，导致内存泄漏
    {
        c[i] = (int *)malloc(sizeof(int) * (len2 + 1));
        b[i] = (int *)malloc(sizeof(int) * (len2 + 1));
    }
 
    //将c[len1][len2]和b[len1][len2]初始化为0
    for ( i = 0; i<= len1; i++)
        for( j = 0; j <= len2; j++)
        {
            c[i][j] = 0;
            b[i][j] = 0;
        }
 
    //计算LCS的长度
    length = Lcs_len(str1, str2, c, b);
#if defined(PRINT_NLP_INFO) && PRINT_NLP_INFO
    printf("The number of the Longest-Common-Subsequence is %d\n", length);
    //利用数组b输出最长子序列
    printf("The Longest-Common-Subsequence is: ");
#endif
    Print_Lcs(str1, b, len1, len2);
#if defined(PRINT_NLP_INFO) && PRINT_NLP_INFO
	same_str[same_index] = '\0';
	printf("\r\nthe result is:%s\r\n",same_str);
#endif
    //动态内存释放
    for ( i = 0; i <= len1; i++)
    {
        free(c[i]);
        free(b[i]);
    }
    free(c);
    free(b);
	result_same = (int)((float)same_index/strlen(str1)*100);
#if defined(PRINT_NLP_INFO) && PRINT_NLP_INFO
	printf("same_index:%d,strlen(str1):%d,result_same:%d\r\n",same_index,strlen(str1),result_same);
#endif
	return result_same;
}
