#pragma once

#include <stdio.h>
#include <WinSock.h>  //一定要包含这个，或者winsock2.h
#include "mysql.h"    //引入mysql头文件(一种方式是在vc目录里面设置，一种是文件夹拷到工程目录，然后这样包含)
#include <Windows.h>

class mySQL
{
public:
	mySQL();
	~mySQL();

	MYSQL mysql; //mysql连接
	MYSQL_FIELD *fd;  //字段列数组
	char field[32][32];  //存字段名二维数组
	MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
	MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列
	char query[150]; //查询语句

	bool mySQL::ConnectDatabase();     //函数声明
	void mySQL::FreeConnect();
	bool mySQL::QueryDatabase();  //查询2
	bool mySQL::InsertData();
	bool mySQL::ModifyData();
	bool mySQL::DeleteData();
	void mySQL::test();
};