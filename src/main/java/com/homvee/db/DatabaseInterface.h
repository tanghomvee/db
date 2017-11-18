// DatabaseLogic.h : DatabaseLogic DLL 的主头文件
//

#pragma once


#define     OTL_ORA10G_R2
#include    "otlv4.h"
#pragma comment(lib,"oci.lib")

const int EXCEPTION_STRING_MAX_LEN = 3500;

// 释放指针
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)				{ if(p) {delete (p);   (p)=NULL; }} 
#define SAFE_DELETE_ARRAY(p)		{ if(p) {delete[] (p);   (p)=NULL; } }
#endif


class   IConvertObserver
{
public:
	virtual bool convert(const char* data,unsigned length,char* strOut,unsigned& outSize)   =   0;
};

// CDatabaseInterface
// 有关此类实现的信息，请参阅 DatabaseLogic.cpp
//

class  CDatabaseInterface
{
public:
	CDatabaseInterface(IConvertObserver* observer);

	static  CDatabaseInterface* createDbInterface(IConvertObserver* observer= 0);

public:

	/*************************************************************************
	* 函数名: ConnectDatabase
	* 功 能: 连接数据库
	* 参 数: strDNS:数据库
	*        strUser:用户名称
	*        strPWD:口令
	* 返回值: true:成功,false:失败
	*************************************************************************/
	virtual bool    ConnectDatabase(const char* strDNS,const char* strUser,const char* strPWD);
	/*************************************************************************
	* 函数名: ConnectDatabase
	* 功 能: 连接SQLITE数据库
	* 参 数: strDNS:数据库
	* 返回值: true:成功,false:失败
	*************************************************************************/
	virtual bool    ConnectSQLITEDatabase(const char* strDNS);

	virtual bool    GetConnectState();
	/*************************************************************************
	* 函数名: CloseConDatabase
	* 功 能: 断开与数据库的连接
	* 参 数: 
	* 返回值: 
	*************************************************************************/
	virtual void    CloseConDatabase();

	/*************************************************************************
	* 函数名: BeginTrans
	* 功 能: 开始事务
	* 参 数: 
	* 返回值:
	*************************************************************************/
	virtual void   BeginTrans();

	/*************************************************************************
	* 函数名: CommitTrans
	* 功 能: 提交事务
	* 参 数: 
	* 返回值:
	*************************************************************************/
	virtual void   CommitTrans();
	/*************************************************************************
	* 函数名: RollbackTrans
	* 功 能: 回滚事务
	* 参 数: 
	* 返回值:
	*************************************************************************/
	virtual void   RollbackTrans();
	/*************************************************************************
	* 函数名: SqlExec
	* 功 能: 执行SQL
	* 参 数: strSQL：SQL语句
	* 返回值: < 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int    SqlExec(const char* strSQL);

	/**
	*   执行SQL,不需要事务
	*/
	virtual int     SqlDirectExec(const char* pSQLL);
	/*************************************************************************
	* 函数名: SqlInsert
	* 功 能: 插入数据
	* 参 数: strSQL：插入数据SQL
	* 返回值: < 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int    SqlInsert(const char*  strSQL);
	/*************************************************************************
	* 函数名: SqlUpdate
	* 功 能: 更新数据
	* 参 数: strSQL：更新数据SQL
	* 返回值: < 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int    SqlUpdate(const char*  strSQL,std::string& strMsg);


	/**
	*   更新二进制字段值
	*   UpdateBlob(
	*/
	virtual int    UpdateBlob( const char* pTable ,const char* szColum,const void* pData,int len,const char* szCondition);


	/*************************************************************************
	* 函数名: SqlDelete
	* 功 能: 删除数据
	* 参 数: strSQL：删除数据SQL
	* 返回值:< 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int    SqlDelete(const char*  strSQL);
	/*************************************************************************
	* 函数名: SqlQuery
	* 功 能: 查询数据
	* 参 数: strSQL：查询数据SQL
	*        ppXML：返回查询结果xml 
	* 返回值:< 0 :有错,>= 0:记录条数
	*************************************************************************/
	virtual int    SqlQuery(const char*  strSQL,char** ppXML);


	/*
	*	函数名：ReleaseXML
	*  功能：解析释放  SqlQuery的char** ppXML
	*/
	virtual void   ReleaseXML(const char* pXML);

	/**
	*   释放blob字段
	*/
	virtual void    ReleaseBlob(void* pBLOB);
	/*
	*	函数名：Release
	*  功能：释放DatabaseInterfaceImpl
	*/
	virtual void Release();

private:
	//抛出异常
	void ThrowException(otl_exception& p);

protected:
	otl_connect         m_db;
	IConvertObserver*   m_observer;
	char   m_chException[EXCEPTION_STRING_MAX_LEN];



};
