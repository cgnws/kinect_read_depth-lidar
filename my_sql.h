#pragma once

#include <stdio.h>
#include <WinSock.h>  //һ��Ҫ�������������winsock2.h
#include "mysql.h"    //����mysqlͷ�ļ�(һ�ַ�ʽ����vcĿ¼�������ã�һ�����ļ��п�������Ŀ¼��Ȼ����������)
#include <Windows.h>

class mySQL
{
public:
	mySQL();
	~mySQL();

	MYSQL mysql; //mysql����
	MYSQL_FIELD *fd;  //�ֶ�������
	char field[32][32];  //���ֶ�����ά����
	MYSQL_RES *res; //����ṹ�������е�һ����ѯ�����
	MYSQL_ROW column; //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ����ʾ�����е���
	char query[150]; //��ѯ���

	bool mySQL::ConnectDatabase();     //��������
	void mySQL::FreeConnect();
	bool mySQL::QueryDatabase();  //��ѯ2
	bool mySQL::InsertData();
	bool mySQL::ModifyData();
	bool mySQL::DeleteData();
	void mySQL::test();
};