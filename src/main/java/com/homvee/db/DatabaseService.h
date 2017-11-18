#pragma once

#ifdef DLL_EXPORTS
#define DATABASESERVICE_API __declspec(dllexport)
#else
#define DATABASESERVICE_API __declspec(dllimport)
#endif

#include "IDatabaseService.h"
#include "DatabaseInterface.h"
#include "rapidxml.hpp"
#include <google/protobuf/message.h>
#include <string>
#include <vector>
#include <map>
#include <set>

/*
*	宏定义，取出相应的不同数据
*/
#define		GET_LONG(item,columName)	NULL!=item->first_attribute(#columName)?atol(item->first_attribute(#columName)->value()):0
#define     GET_INT(item,columName)     NULL!=item->first_attribute(#columName)?atoi(item->first_attribute(#columName)->value()):0
#define     GET_STRING(item,columName)  NULL!=item->first_attribute(#columName)?(item->first_attribute(#columName)->value()):""
#define		GET_DOUBLE(item,columName)  NULL!=item->first_attribute(#columName)?atof(item->first_attribute(#columName)->value()):0

namespace acl
{

class  CDatabaseService:public IDatabaseService
{
public:
	CDatabaseService(std::string g_strUserName,std::string g_strPassWord,std::string g_strDataBase);
	CDatabaseService();
	~CDatabaseService(void);
    
	//连接数据库
	bool ConnectDB( );
    
	//关闭数据库
	bool CloseDB();

	//释放对象列表
	bool FreeObjectList(std::vector<::google::protobuf::Message*>& resultList);

	::google::protobuf::Message* GetObjectByType(const std::string s_typeName);

	//新增对象
	bool IsObjectExist(::google::protobuf::Message& message,bool& isExist);
 
	//新增对象
	bool InsertObject(::google::protobuf::Message& message);

	//新增对象，指定属性
	bool InsertObject(::google::protobuf::Message& message,std::set<std::string> saveFields);
    
	//更新对象
	bool UpdateObject( ::google::protobuf::Message& message);

	//更新对象，指定属性
	bool UpdateObject( ::google::protobuf::Message& message,std::set<std::string> updateFields);
    
	//获取单个对象
	bool QueryObject(::google::protobuf::Message& message,bool upCascade=false,bool downCascade=false);
   
	//拷贝对象相同的属性
	void CopyObjectFields(const ::google::protobuf::Message& parent,::google::protobuf::Message& message);
    
	//递归设置对象属性
	bool SetObjectField(const ::google::protobuf::Message& parent,::google::protobuf::Message& message,std::string strField,std::string strValue);

	//获取单个对象，指定查询属性
	bool QueryObject(::google::protobuf::Message& message,bool upCascade,bool downCascade,std::set<std::string> queryFields);
    
	bool QueryObject(const ::google::protobuf::Message& parent,::google::protobuf::Message& message,bool upCascade,bool downCascade,std::set<std::string> queryFields);
    
	//查询二进制数据
	bool QueryBlob(::google::protobuf::Message& message,std::string queryField,char*& pData,unsigned long& blobSize);

	//查询二进制数据至文件
	bool QueryBlob(::google::protobuf::Message& message,std::string queryField,char* pPath);
    
	//更新二进制数据
	bool UpdateBlob(const ::google::protobuf::Message& message,std::string updateField,char* pData,unsigned long blobSize );

	//根据文件更新二进制数据
	bool UpdateBlob(const ::google::protobuf::Message& message,std::string updateField,char* pPath);
    
	//根据Message更新二进制数据
	bool UpdateMessageBlob( const ::google::protobuf::Message& parent,const ::google::protobuf::Message& message,std::set<std::string> updateFields );
    
	//查询单表
	bool QuerySingleObjectList( const ::google::protobuf::Message& message,std::vector<::google::protobuf::Message*>& resultList);

	//查询多个对象
	bool QueryObjectList( const ::google::protobuf::Message& message,std::string querySQL, std::vector<::google::protobuf::Message*>& resultList);
    
	//删除多个对象
	bool DeleteObjectList( std::vector<::google::protobuf::Message*> objectList);
    
	//删除表数据
	bool DeleteBySql( std::string deleteSql);
    
	//构建插入SQL语句
	bool BuildInsertSQL(const ::google::protobuf::Message& parent,const ::google::protobuf::Message& message,std::set<std::string> saveFields,std::string& strSql);

	//构建更新SQL语句
	bool BuildUpdateSQL(const ::google::protobuf::Message& parent,const ::google::protobuf::Message& message,std::set<std::string> udateFields,std::string& strSql,bool isBlob);

	//构建查询SQL语句
	bool BuildQuerySQL( const ::google::protobuf::Message& message,std::map<std::string,::google::protobuf::FieldDescriptor::Type>& fieldMap,std::set<std::string> queryFields,std::string& strSql);

	bool BuildQueryCountSQL(::google::protobuf::Message& message,std::string& strQuerySql);
    
	//构建删除SQL语句
	bool BuildDeleteSQL(std::vector<::google::protobuf::Message*> objectList,std::string& strSql);
	bool BuildDeleteSQL(::google::protobuf::Message& message,std::string& strDeleteSql);
    
	//构建对象属性和类型
	void BuildMessageField( const ::google::protobuf::Message& message,std::map<std::string,::google::protobuf::FieldDescriptor::Type>& fieldMap);
    
	//获取表所有主键
	bool GetPrimaryKeys(std::string tableName,std::set<std::string>& primaryKeys);

	//判断表主键
	bool IsPrimaryKey(std::string tableName,std::string fieldName);
    
	//根据Message获取表名
	bool GetNameByMessage(const ::google::protobuf::Message& message, std::string& strName);
    
	//获取对象所有属性
	void GetAllFields(const ::google::protobuf::Message& message,std::set<std::string>& allFields);
    
	//获取对象所有BLOB属性
	void GetBlobFields(const ::google::protobuf::Message& message,std::set<std::string>& allFields);
    
	//获取对象所有非BLOB属性
	void GetNoBlobFields(const ::google::protobuf::Message& message,std::set<std::string>& allFields);
    
	//获取对象属性值
	void GetFieldValue(const ::google::protobuf::Message& message,const ::google::protobuf::FieldDescriptor* field,std::string& fieldValue,bool& isMessage);
    
	//整数转字符串
	std::string intToString(int value);
    
	//单精度浮点数转字符串
	std::string floatToString(float value);
   
	//双精度浮点数转字符串
	std::string doubleToString(double value);
   
	//BOOL类型转字符串
	std::string boolToString(bool value);

	//Base64编码
	 bool EncodeBase64( const char* sourceBuffer,const int bufLen,char*& base64String );

	//Base64解码
	 bool DecodeBase64( const char* base64String,int& bufLen,char*& sourceBuffer );
     
	 //获取最近一次错误信息
	 std::string GetMyLastError();
     
	 //设置最近一次错误信息
	 void SetMyLastError(std::string strErrorMsg);

	 void GetConfigFilePath(std::string strPrefix,std::string strFileName,std::string& strFilePath);
	 void GetDatabaseConfig( std::string& strService,std::string& strUser,std::string& strPassword );
	 bool GetExceptionTable(std::string strTable,std::string strRefTable,std::string& strField,std::string& strRefField);

	 /*************************************************************************
	 * 函数名: SqlExec
	 * 功 能: 执行SQL
	 * 参 数: strSQL：SQL语句
	 * 返回值: < 0 :有错,>= 0:影响记录条数
	 *************************************************************************/
	 int  SqlExec(const char* strSQL);

	 /*************************************************************************
	 * 函数名: SqlExec
	 * 功 能: 执行SQL，不需要事务
	 * 参 数: strSQL：SQL语句
	 * 返回值: < 0 :有错,>= 0:影响记录条数
	 *************************************************************************/
	 int  SqlDirectExec(const char* pSQLL);

	 /*************************************************************************
	 * 函数名: SqlInsert
	 * 功 能: 插入数据
	 * 参 数: strSQL：插入数据SQL
	 * 返回值: < 0 :有错,>= 0:影响记录条数
	 *************************************************************************/
	 int  SqlInsert(const char*  strSQL);

	 /*************************************************************************
	 * 函数名: SqlUpdate
	 * 功 能: 更新数据
	 * 参 数: strSQL：更新数据SQL
	 * 参 数: strMsg：错误信息
	 * 返回值: < 0 :有错,>= 0:影响记录条数
	 *************************************************************************/
	 int  SqlUpdate(const char*  strSQL,std::string& strMsg);

	 /*************************************************************************
	 * 函数名: 更新二进制字段值
	 * 功 能: 更新数据
	 * 参 数: 
	 * 返回值: < 0 :有错,>= 0:影响记录条数
	 *************************************************************************/
	 int  UpdateBlob( const char* pTable ,const char* szColum,const void* pData,int len,const char* szCondition);

	 /*************************************************************************
	 * 函数名: SqlDelete
	 * 功 能: 删除数据
	 * 参 数: strSQL：删除数据SQL
	 * 返回值:< 0 :有错,>= 0:影响记录条数
	 *************************************************************************/

	 int    SqlDelete(const char*  strSQL);
	 /*************************************************************************
	 * 函数名: SqlQuery
	 * 功 能: 查询数据
	 * 参 数: strSQL：查询数据SQL
	 * 参 数: ppXML：返回查询结果xml 
	 * 返回值:< 0 :有错,>= 0:记录条数
	 *************************************************************************/
	 int    SqlQuery(const char*  strSQL,char** ppXML);
     
	 //提交事务
	 void   CommitTrans();

	 /*
	 *	函数名：ReleaseXML
	 *  功能：解析释放  SqlQuery的char** ppXML
	 */
	 void   ReleaseXML(const char* pXML);

	 //获取下一个编号
	 bool GetNextNum( std::string strTableName,std::string strColumnName,int& iNum);
private:
	CDatabaseInterface* pInterface;//数据库操作接口
	std::string m_strUserName;//用户名
	std::string m_strPassWord;//密码
	std::string m_strDataBase;//数据库
	std::string strErrorMsg;//错误信息
};

class   Query :public rapidxml::xml_document<>
{
public:

	/*
	* 用于存放查询出来的结果   
	*/
	char*  m_pRes;
	/*
	* 用于连接数据库   
	*/
	CDatabaseInterface* m_pInterface;

	Query(CDatabaseInterface* m_pInterface);
	~Query();

	/*******************************************************************************************/
	/*函数名称：exeSQL
	/*函数功能：执行传入的SQL语句
	/*参数：	const char* sql,SQL语句 
	/*返回值：  成功返回TRUE，失败返回FALSE                                                      
	/*******************************************************************************************/
	bool exeSQL(const char* sql);
};

}
