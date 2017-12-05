#pragma once
#include"stdlib.h"
#include"../include/CompilerDef.h"

/*变量类型*/
typedef enum {
	SC_GLOBAL = 0,          //全局变量
	SC_LOCAL = 1			//局部变量	
}e_VarType;

/*语法状态*/
typedef enum {
	SC_STATE_NULL = 0,		//空状态，没有语法缩进
	SC_STATE_SP = 1,		//空格	
	SC_STATE_LF_HT = 2,     //换行并缩进，每一个声明、函数定义、语句结束都要置为此状态
	SC_STATE_DELAY = 3		//延迟到取出下一单词后确定输出格式

}e_SynState;