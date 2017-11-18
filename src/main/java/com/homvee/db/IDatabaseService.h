#pragma once

#include <string>
#include <google/protobuf/message.h>

//数据库服务，能够以约定的方式完成ORM和相关操作
namespace acl{

/*
	定义了数据操作的相关接
*/
class IDatabaseService
{
public:

	/**
	/*@brief 释放对象指针列表
	/*@version V1.00 
	/*@param[in] resultList protobuf对象指针列表
	/*@return true 释放成功 false 释放失败
	*/	
	virtual bool FreeObjectList(std::vector<::google::protobuf::Message*>& resultList)=0;

	/**
	/*@brief 插入对象至数据库表，支持级联插入，对象名称和属性与数据库表与字段一一对应
	/*@version V1.00 
	/*@param[in] message protobuf对象
	/*@return true 插入成功 false 插入失败
	*/	
	virtual bool InsertObject(::google::protobuf::Message& message)=0;

	/**
	/*@brief 根据指定的属性插入对象至数据库表
	/*@version V1.00 
	/*@param[in] message protobuf对象
	/*@param[in] insertFields 指定插入的属性
	/*@return true 插入成功 false 插入失败
	*/	
	virtual bool InsertObject(::google::protobuf::Message& message,std::set<std::string> insertFields)=0;

	/**
	/*@brief 更新对象至数据库表，支持级联更新，对象名称和属性与数据库表与字段一一对应
	/*@version V1.00 
	/*@param[in] message protobuf对象
	/*@return true 更新成功 false 更新失败
	*/	
	virtual bool UpdateObject( ::google::protobuf::Message& message)=0;
    
	/**
	/*@brief 根据指定的属性更新对象至数据库表
	/*@version V1.00 
	/*@param[in] message protobuf对象
	/*@param[in] updateFields 指定更新的属性
	/*@return true 更新成功 false 更新失败
	*/	
	virtual bool UpdateObject( ::google::protobuf::Message& message,std::set<std::string> updateFields)=0;

	/**
	/*@brief 递归设置对象属性
	/*@version V1.00 
	/*@param[in] parent 上级对象
	/*@param[in/out] message 本级对象
	/*@param[in] strField 设置的属性
	/*@param[in] strValue 设置的属性值
	/*@return true 设置成功 false 设置失败
	*/	
	virtual bool SetObjectField(const ::google::protobuf::Message& parent,::google::protobuf::Message& message,std::string strField,std::string strValue)=0;

	/**
	/*@brief 根据主键查询数据库表数据至对象
	/*@version V1.00 
	/*@param[in/out] message protobuf对象，需要设置查询主键
	/*@param[in] upCascade 是否向上级联查询
	/*@param[in] downCascade 是否向下级联查询
	/*@return true 查询成功 false 查询失败
	*/	
	virtual bool QueryObject(::google::protobuf::Message& message,bool upCascade=false,bool downCascade=false)=0;

	virtual bool IsObjectExist( ::google::protobuf::Message& message,bool& isExist )=0;

	//获取下一个编号
	virtual bool GetNextNum( std::string strTableName,std::string strColumnName,int& iNum)=0;
    
	/**
	/*@brief 根据主键和指定的属性查询数据库表数据至对象
	/*@version V1.00 
	/*@param[in/out] message protobuf对象，需要设置查询主键
	/*@param[in] cascade 是否级联查询
	/*@param[in] queryFields 指定查询的属性
	/*@return true 查询成功 false 查询失败
	*/	
	virtual bool QueryObject(::google::protobuf::Message& message,bool upCascade,bool downCascade,std::set<std::string> queryFields)=0;

	/**
	/*@brief 根据主键和指定的属性查询数据库表Blob字段至字符指针
	/*@version V1.00 
	/*@param[in] message protobuf对象，需要设置查询主键
	/*@param[in] queryField 查询属性，对应数据库的Blob字段
	/*@param[out] pData 查询返回的Blob字符指针
	/*@param[out] blobSize 查询返回的Blob字符长度
	/*@return true 查询成功 false 查询失败
	*/	
	virtual bool QueryBlob(::google::protobuf::Message& message,std::string queryField,char*& pData,unsigned long& blobSize)=0;

	/**
	/*@brief 根据主键和指定的属性查询数据库表Blob字段至文件
	/*@version V1.00 
	/*@param[in] message protobuf对象，需要设置查询主键
	/*@param[in] queryField 查询属性，对应数据库的Blob字段
	/*@param[in] pPath 文件绝对路径，Blob数据以二进制方式写入文件
	/*@return true 查询成功 false 查询失败
	*/	
	virtual bool QueryBlob(::google::protobuf::Message& message,std::string queryField,char* pPath)=0;

	/**
	/*@brief 根据主键和指定的属性更新数据库表Blob字段
	/*@version V1.00 
	/*@param[in] message protobuf对象，需要设置更新主键
	/*@param[in] updateField 更新属性，对应数据库的Blob字段
	/*@param[out] pData Blob字符指针
	/*@param[out] blobSize Blob字符长度
	/*@return true 更新成功 false 更新失败
	*/	
	virtual bool UpdateBlob(const ::google::protobuf::Message& message,std::string updateField,char* pData,unsigned long blobSize )=0;

	/************************************************************************/
	/*@brief 根据主键和指定的属性将磁盘文件更新数据库表Blob字段
	/*@version V1.00 
	/*@param[in] message protobuf对象，需要设置更新主键
	/*@param[in] updateField 更新属性，对应数据库的Blob字段
	/*@param[out] pPath 文件绝对路径
	/*@return true 更新成功 false 更新失败
	/************************************************************************/	
	virtual bool UpdateBlob(const ::google::protobuf::Message& message,std::string updateField,char* pPath)=0;

	/************************************************************************/
	/*@brief 根据SQL语句查询
	/*@version V1.00 
	/*@param[in] message protobuf模板对象
	/*@param[in] querySQL 查询SQL语句
	/*@param[out] resultList 查询结果
	/*@return true 查询成功 false 查询失败
	/************************************************************************/	
	virtual bool QueryObjectList( const ::google::protobuf::Message& message,std::string querySQL, std::vector<::google::protobuf::Message*>& resultList)=0;

	/************************************************************************/
	/*@brief 根据对象查询列表（单表查询）
	/*@version V1.00 
	/*@param[in] message protobuf对象，需设置查询的字段值
	/*@param[out] resultList 查询结果
	/*@return true 查询成功 false 查询失败
	/************************************************************************/	
	virtual bool QuerySingleObjectList( const ::google::protobuf::Message& message,std::vector<::google::protobuf::Message*>& resultList)=0;

	/************************************************************************/
	/*@brief 根据对象删除数据库表数据
	/*@version V1.00 
	/*@param[in] objectList protobuf对象列表，需要设置更新主键
	/*@return true 删除成功 false 删除失败
	/************************************************************************/	
	virtual bool DeleteObjectList( std::vector<::google::protobuf::Message*> objectList)=0;

	/************************************************************************/
	/*@brief 根据SQL语句删除数据库表数据
	/*@version V1.00 
	/*@param[in] deleteSql 删除SQL语句
	/*@return true 删除成功 false 删除失败
	/************************************************************************/	
	virtual bool DeleteBySql( std::string deleteSql)=0;

	/************************************************************************/
	/*@brief Base64编码
	/*@version V1.00 
	/*@param[in] sourceBuffer 待编码字符
	/*@param[in] bufLen 待编码字符长度
	/*@param[out] base64String base64编码字符串
	/*@return true 编码成功 false 编码失败
	/************************************************************************/	
	virtual bool EncodeBase64( const char* sourceBuffer,const int bufLen,char*& base64String )=0;

	/**
	/*@brief Base64解码
	/*@version V1.00 
	/*@param[in] base64String 待解码字符
	/*@param[out] bufLen 解码字符长度
	/*@param[out] sourceBuffer 解码字符串
	/*@return true 解码成功 false 解码失败
	*/	
	virtual bool DecodeBase64( const char* base64String,int& bufLen,char*& sourceBuffer )=0;

	/**
	/*@brief 获取最近一次错误信息
	/*@version V1.00 
	/*@return 错误信息
	*/	
	virtual std::string GetMyLastError()=0;

	/**
	/*@brief 设置最近一次错误信息
	/*@version V1.00 
	/*@param[in] strErrorMsg 错误信息
	*/	
	virtual void SetMyLastError(std::string strErrorMsg)=0;
    
	/**
	/*@brief 关闭数据库连接
	/*@version V1.00 
	/*@return true 关闭成功 关闭失败
	*/	
	virtual bool CloseDB()=0;

	/*以下是原始接口*/
	/*************************************************************************
	* 函数名: SqlExec
	* 功 能: 执行SQL
	* 参 数: strSQL：SQL语句
	* 返回值: < 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int  SqlExec(const char* strSQL)=0;

	/*************************************************************************
	* 函数名: SqlExec
	* 功 能: 执行SQL，不需要事务
	* 参 数: strSQL：SQL语句
	* 返回值: < 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int  SqlDirectExec(const char* pSQLL)=0;

	/*************************************************************************
	* 函数名: SqlInsert
	* 功 能: 插入数据
	* 参 数: strSQL：插入数据SQL
	* 返回值: < 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int  SqlInsert(const char*  strSQL)=0;

	/*************************************************************************
	* 函数名: SqlUpdate
	* 功 能: 更新数据
	* 参 数: strSQL：更新数据SQL
	* 参 数: strMsg：错误信息
	* 返回值: < 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int  SqlUpdate(const char*  strSQL,std::string& strMsg)=0;

	/*************************************************************************
	* 函数名: 更新二进制字段值
	* 功 能: 更新数据
	* 参 数: 
	* 返回值: < 0 :有错,>= 0:影响记录条数
	*************************************************************************/
	virtual int  UpdateBlob( const char* pTable ,const char* szColum,const void* pData,int len,const char* szCondition)=0;

	/*************************************************************************
	* 函数名: SqlDelete
	* 功 能: 删除数据
	* 参 数: strSQL：删除数据SQL
	* 返回值:< 0 :有错,>= 0:影响记录条数
	*************************************************************************/

	virtual int    SqlDelete(const char*  strSQL)=0;
	/*************************************************************************
	* 函数名: SqlQuery
	* 功 能: 查询数据
	* 参 数: strSQL：查询数据SQL
	* 参 数: ppXML：返回查询结果xml 
	* 返回值:< 0 :有错,>= 0:记录条数
	*************************************************************************/
	virtual int    SqlQuery(const char*  strSQL,char** ppXML)=0;

	 virtual void   ReleaseXML(const char* pXML)=0;

	 /*************************************************************************
	 * 函数名: CommitTrans
	 * 功 能: 提交事务
	 * 参 数: 
	 * 返回值:
	 *************************************************************************/
	 virtual void   CommitTrans()=0;

};
 
/************************************************************************/
/*@brief 创建数据库服务接口
/*@version V1.00 
/*@return 数据库服务接口指针
/************************************************************************/	
_declspec(dllexport) IDatabaseService* ICreateDatabaseService( );

}
