#include "my_sql.h"

mySQL::mySQL()
{

}
mySQL::~mySQL()
{

}

//连接数据库
bool mySQL::ConnectDatabase()
{
	//初始化mysql
	mysql_init(&mysql);  //连接mysql，数据库

	//返回false则连接失败，返回true则连接成功
	if (!(mysql_real_connect(&mysql, "localhost", "root", "930417", "test", 3306, NULL, 0))) //中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去
	{
		printf("Error connecting to database:%s\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Connected...\n");
		return true;
	}
}
//释放资源
void mySQL::FreeConnect()
{
	//释放资源
	mysql_free_result(res);
	mysql_close(&mysql);
}
/***************************数据库操作***********************************/
//其实所有的数据库操作都是先写个sql语句，然后用mysql_query(&mysql,query)来完成，包括创建数据库或表，增删改查
//查询数据

bool mySQL::QueryDatabase()
{
	mysql_query(&mysql, "set names gbk");
	//返回0 查询成功，返回1查询失败
	if (mysql_query(&mysql, "select * from user"))        //执行SQL语句
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	res = mysql_store_result(&mysql);
	//打印数据行数
	printf("number of dataline returned: %d\n", mysql_affected_rows(&mysql));
	for (int i = 0; fd = mysql_fetch_field(res); i++)  //获取字段名
		strcpy_s(field[i], fd->name);
	int j = mysql_num_fields(res);  // 获取列数
	for (int i = 0; i<j; i++)  //打印字段
		printf("%10s\t", field[i]);
	printf("\n");
	while (column = mysql_fetch_row(res))
	{
		for (int i = 0; i<j; i++)
			printf("%10s\t", column[i]);
		printf("\n");
	}
	return true;
}
//插入数据
bool mySQL::InsertData()
{
	sprintf_s(query, "insert into user values (NULL, 'Lilei', 'wyt2588zs','lilei23@sina.cn');");  //可以想办法实现手动在控制台手动输入指令
	if (mysql_query(&mysql, query))        //执行SQL语句
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}
//修改数据
bool mySQL::ModifyData()
{
	sprintf_s(query, "update user set email='lilei325@163.com' where name='Lilei'");
	if (mysql_query(&mysql, query))        //执行SQL语句
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}
//删除数据
bool mySQL::DeleteData()
{
	/*sprintf_s(query, "delete from user where id=6");*/
	char query[100];
	printf("please input the sql:\n");
	gets_s(query);  //这里手动输入sql语句
	if (mysql_query(&mysql, query))        //执行SQL语句
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}

void mySQL::test()
{
	ConnectDatabase();//连接数据库
	QueryDatabase();//查询数据
	InsertData();//插入数据
	QueryDatabase();
	ModifyData();//修改数据
	QueryDatabase();
	DeleteData();//删除数据
	QueryDatabase();
	FreeConnect();//释放资源
}