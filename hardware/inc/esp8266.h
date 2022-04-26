#ifndef _ESP8266_H_
#define _ESP8266_H_

#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־

void ESP8266_Init(void);
void ESP8266_Clear(void);
_Bool ESP8266_SendCmd(char *cmd, char *res);
void compare_rece_mess(char *nlp_mess[]);
void print_rece_mess(void);

#endif
