#ifndef __USMART_STR_H
#define __USMART_STR_H	 	   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2011/6/13
//版本：V2.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2011-2021
//All rights reserved
//********************************************************************************
//升级说明
//V1.4
//增加了对参数为string类型的函数的支持.适用范围大大提高.
//优化了内存占用,静态内存占用为79个字节@10个参数.动态适应数字及字符串长度
//V2.0 
//1,修改了list指令,打印函数的完整表达式.
//2,增加了id指令,打印每个函数的入口地址.
//3,修改了参数匹配,支持函数参数的调用(输入入口地址).
//4,增加了函数名长度宏定义.	
//V2.1 20110707		 
//1,增加dec,hex两个指令,用于设置参数显示进制,及执行进制转换.
//注:当dec,hex不带参数的时候,即设定显示参数进制.当后跟参数的时候,即执行进制转换.
//如:"dec 0XFF" 则会将0XFF转为255,由串口返回.
//如:"hex 100" 	则会将100转为0X64,由串口返回
//2,新增usmart_get_cmdname函数,用于获取指令名字.
//V2.2 20110726	
//1,修正了void类型参数的参数统计错误.
//2,修改数据显示格式默认为16进制.
//V2.3 20110815
//1,去掉了函数名后必须跟"("的限制.
//2,修正了字符串参数中不能有"("的bug.
//3,修改了函数默认显示参数格式的修改方式. 
//V2.4 20110905
//1,修改了usmart_get_cmdname函数,增加最大参数长度限制.避免了输入错误参数时的死机现象.
//2,增加USMART_ENTIM2_SCAN宏定义,用于配置是否使用TIM2定时执行scan函数.
/////////////////////////////////////////////////////////////////////////////////////
//typedef unsigned long  u32;
//typedef unsigned short u16;
//typedef unsigned char  u8;
#include "stm32f10x.h"	 
						  
u8 usmart_get_parmpos(u8 num);						//得到某个参数在参数列里面的起始位置
u8 usmart_strcmp(u8*str1,u8 *str2);					//对比两个字符串是否相等
u32 usmart_pow(u8 m,u8 n);							//M^N次方
u8 usmart_str2num(u8*str,u32 *res);					//字符串转为数字
u8 usmart_get_cmdname(u8*str,u8*cmdname,u8 *nlen,u8 maxlen);//从str中得到指令名,并返回指令长度
u8 usmart_get_fname(u8*str,u8*fname,u8 *pnum);		//从str中得到函数名
u8 usmart_get_aparm(u8 *str,u8 *fparm,u8 *ptype); 	//从str中得到一个函数参数
u8 usmart_get_fparam(u8*str,u8 *parn);  			//得到str中所有的函数参数.
#endif











