// DatabaseServcie.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DatabaseService.h"
#include <sstream>
#include <algorithm>
#include <WinCrypt.h>

#pragma comment(lib,"crypt32")

using namespace acl;

_declspec(dllexport) IDatabaseService* acl::ICreateDatabaseService()
{
	CDatabaseService* databaseService=new CDatabaseService();
	if(databaseService->ConnectDB())
	{
		return databaseService;
	}
	return NULL;
}


Query::Query(CDatabaseInterface* m_pInterface)
{
	m_pRes          =   0;
	this->m_pInterface    =   m_pInterface;
}

Query::~Query()
{
	if (m_pInterface && m_pRes)
	{
		m_pInterface->ReleaseXML(m_pRes);
	}
}

/*
*	执行SQL语句
*/
bool Query::exeSQL( const char* pSql )
{
	if (m_pInterface == 0  ||   !m_pInterface->GetConnectState())
	{
		return  false;
	}
	if (m_pRes != 0)
	{
		m_pInterface->ReleaseXML(m_pRes);
	}
	int retValue=m_pInterface->SqlQuery(pSql,&m_pRes);
	if(retValue<0)
	{
		return false;
	}
	try
	{
		/**
		* 直接把内存中的m_pRes解析出来
		*/
		parse<0>(m_pRes);
		return  true;
	}
	catch (...)
	{
		return  false;
	}
}

CDatabaseService::CDatabaseService()
{
	pInterface=NULL;
	GetDatabaseConfig(m_strDataBase,m_strUserName,m_strPassWord);
}

CDatabaseService::CDatabaseService(std::string m_strUserName,std::string m_strPassWord,std::string m_strDataBase)
{
	pInterface=NULL;
	this->m_strUserName=m_strUserName;
	this->m_strPassWord=m_strPassWord;
	this->m_strDataBase=m_strDataBase;
}

CDatabaseService::~CDatabaseService(void)
{
	if(pInterface)
	{
		pInterface->Release();
		delete pInterface;
		pInterface=NULL;
	}
}

//连接数据库
bool CDatabaseService::ConnectDB( )
{
	if (NULL==pInterface  ||   !pInterface->GetConnectState())
	{
		pInterface    =   CDatabaseInterface::createDbInterface();
		bool bRes       =   pInterface->ConnectDatabase(m_strDataBase.c_str(),m_strUserName.c_str(),m_strPassWord.c_str());
		if ( !bRes )
		{
			pInterface->Release();
			pInterface    =   0;
			SetMyLastError("连接数据库失败");
		}
		return bRes;
	}
	return true;

}

//根据Message获取表名
bool CDatabaseService::GetNameByMessage(const ::google::protobuf::Message& message, std::string& strName)
{
	strName="";
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	if(pRef)
	{
		const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
		if(descriptor)
		{
			strName=descriptor->full_name();//表名
			return true;
		}
	}
	return false;
}


//判断是否为主键
bool CDatabaseService::IsPrimaryKey(std::string tableName,std::string fieldName)
{
	std::set<std::string> primaryKeys;
	GetPrimaryKeys(tableName,primaryKeys);
    transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
	if(primaryKeys.count(fieldName))
	{
		return true;
	}
	return false;
}

//获取表的所有主键
bool CDatabaseService::GetPrimaryKeys(std::string tableName,std::set<std::string>& primaryKeys)
{
	std::string strSQL="select cu.column_name from user_cons_columns cu,user_constraints au where cu.constraint_name=au.constraint_name and au.constraint_type='P' and au.table_name='";
	strSQL+=tableName+"'";

	if(pInterface)
	{
		std::string strKey;
		Query	query(pInterface);
		query.exeSQL(strSQL.c_str());
		rapidxml::xml_node<>* root    =   query.first_node();
		rapidxml::xml_node<>* item    =   root->first_node();
		for ( ; item != 0 ; item = item->next_sibling())
		{
			strKey=GET_STRING(item,COLUMN_NAME);
			transform(strKey.begin(),strKey.end(),strKey.begin(),::toupper);
			primaryKeys.insert(strKey);
		}
		return true;
	}

	SetMyLastError("数据库访问接口指针为空");
	return false;
}

//获取对象的所有属性
void CDatabaseService::GetAllFields(const ::google::protobuf::Message& message,std::set<std::string>& allFields)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	int fieldCount=descriptor->field_count();//字段总数
	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		std::string fieldName=field->name();//字段名称
		transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
		allFields.insert(fieldName);
	}		
}

//获取所有的BLOB字段
void CDatabaseService::GetBlobFields(const ::google::protobuf::Message& message,std::set<std::string>& allFields)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	int fieldCount=descriptor->field_count();//字段总数
	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		std::string fieldName=field->name();//字段名称
		transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
		if(fieldName.find("BLOB")!=fieldName.npos)
		{
			allFields.insert(fieldName);
		}
	}		
}

//获取所有的非BLOB字段
void CDatabaseService::GetNoBlobFields(const ::google::protobuf::Message& message,std::set<std::string>& allFields)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	int fieldCount=descriptor->field_count();//字段总数
	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		std::string fieldName=field->name();//字段名称
		transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
		if(fieldName.find("BLOB")==fieldName.npos)
		{
			allFields.insert(fieldName);
		}
	}		
}

//获取对象属性值
void CDatabaseService::GetFieldValue(const ::google::protobuf::Message& message,const ::google::protobuf::FieldDescriptor* field,std::string& fieldValue,bool& isMessage)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	::google::protobuf::FieldDescriptor::Type fieldType=field->type();//字段类型
	switch(fieldType)
	{
	case ::google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE: //嵌套消息类型
		{      
			isMessage=true;
		}
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_STRING:
	case ::google::protobuf::FieldDescriptor::Type::TYPE_BYTES:
		{
			fieldValue="'"+pRef->GetString(message,field);
			fieldValue+="'";
		}	
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_INT32:
		fieldValue=intToString(pRef->GetInt32(message,field));
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_INT64:
		fieldValue=intToString(pRef->GetInt64(message,field));
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_UINT32:
		fieldValue=intToString(pRef->GetUInt32(message,field));
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_UINT64:
		fieldValue=intToString(pRef->GetUInt64(message,field));
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_FLOAT:
		fieldValue=floatToString(pRef->GetFloat(message,field));
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE:
		fieldValue=doubleToString(pRef->GetDouble(message,field));
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_BOOL:
		fieldValue=boolToString(pRef->GetBool(message,field));
		break;
	default:
		break;
	}

}

//构建插入SQL语句
bool CDatabaseService::BuildInsertSQL(const ::google::protobuf::Message& parent,const ::google::protobuf::Message& message,std::set<std::string> saveFields,std::string& strSql )
{
	if(saveFields.size()==0)
	{
		SetMyLastError("插入字段为空");
		return false;
	}
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::string strTableName=descriptor->name();//表名
    
	strSql="";//完整查询SQL语句
	std::string parentSql="";//父表插入SQL语句
	std::string subSql="";//子表插入SQL语句
	std::string strSqlHead="INSERT INTO ";
	strSqlHead+=strTableName;
	strSqlHead+="(";
	std::string strSqlTail=" VALUES ( ";
	int fieldCount=descriptor->field_count();//字段总数
	int currentIndex=0;//当前非消息字段索引
	int insertCount=0;//实际插入的字段数
	std::string tempSql;//临时SQL

	std::string strParentName="";//调用表名
	GetNameByMessage(parent,strParentName);

	std::set<std::string>::iterator itr=saveFields.begin();
	for(;itr!=saveFields.end();itr++)
	{   
		std::string fieldName=*itr;
		transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::tolower);//字段统一转为小写
		const ::google::protobuf::FieldDescriptor* field=descriptor->FindFieldByName(fieldName);
		if(field)
		{
			std::string fieldValue="";
			bool isMessage=false;
			if(field->is_repeated())//重复字段，即子表
			{	
				int fieldSize=pRef->FieldSize(message,field);
				if(fieldSize>0)
				{
					std::string strSubName="";
					GetNameByMessage(pRef->GetRepeatedMessage(message,field,0),strSubName);
					if(strSubName==strParentName)//避免死循环
					{
						continue;
					}
				}
				for(int j=0;j<fieldSize;j++)
				{
					std::set<std::string> subFields;
					GetNoBlobFields(pRef->GetRepeatedMessage(message,field,j),subFields);//注意BLOB字段
					if(BuildInsertSQL(message,pRef->GetRepeatedMessage(message,field,j),subFields,tempSql))//递归调用
					{
						subSql+=tempSql;
					}
					
				}
				isMessage=true;
				continue;
			}

			GetFieldValue(message,field,fieldValue,isMessage);
			if(isMessage)//主表
			{
				std::string strSubName="";
				GetNameByMessage(pRef->GetMessage(message,field),strSubName);
				if(strSubName==strParentName)//避免死循环
				{
					continue;
				}

				std::set<std::string> inFields;
				GetNoBlobFields(pRef->GetMessage(message,field),inFields);
				if(BuildInsertSQL(message,pRef->GetMessage(message,field),inFields,tempSql))//递归调用
				{
					parentSql+=tempSql;
				}
				
			}
			else
			{
				if(!pRef->HasField(message,field))
				{
					continue;
				}
				currentIndex++;
				if(currentIndex>1)
				{
					strSqlHead+=",";
					strSqlTail+=",";
				}
				strSqlHead+=*itr;
				strSqlTail+=fieldValue;	
				insertCount++;
			}

		}
	}
   
	strSqlHead+=")";
	strSqlTail+="); ";
	strSql+=parentSql;
	strSql+=strSqlHead+strSqlTail;
	strSql+=subSql;

	if(insertCount==0)
	{
		SetMyLastError("插入字段为空");
		return false;
	}
	return true;

}

//构建更新SQL语句(子表采取先删除后插入策略)
bool CDatabaseService::BuildUpdateSQL( const ::google::protobuf::Message& parent, const ::google::protobuf::Message& message,std::set<std::string> updateFields,std::string& strSql,bool isBlob )
{
	if(updateFields.size()==0)
	{
		SetMyLastError("更新字段为空");
		return false;
	}
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::string strTableName=descriptor->name();//表名
    
	strSql="";//完整查询SQL语句
	std::string parentSql="";//父表更新SQL语句
	std::string subSql="";//子表更新SQL语句
	std::string strSqlHead="UPDATE ";
	strSqlHead+=strTableName;
	strSqlHead+=" SET ";
	std::string strSqlTail=" WHERE ";
	int primaryCount=0;//主键个数
	int fieldCount=descriptor->field_count();//字段总数
	int currentIndex=0;//当前非消息字段索引
	int setCount=0;//实际更新字段数

	std::string strParentName="";//调用表名
	std::set<std::string> primaryKeys;
	GetNameByMessage(parent,strParentName);
    GetPrimaryKeys(strTableName,primaryKeys);
	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		std::string fieldName=field->name();//字段名称
		std::string fieldValue="";
		bool isMessage=false;//消息标识
		bool isPrimary=false;//主键标识
		std::string tempSql;//临时SQL
		if(field->is_repeated())//重复字段，即子表
		{
			std::string subName;
			::google::protobuf::Message* tempMessage=message.New();
			::google::protobuf::Message* subMessage=pRef->AddMessage(tempMessage,field);;
			if(subMessage)
			{
				GetNameByMessage(*subMessage,subName);
				if(subName==strParentName)//避免死循环
				{
					continue;
				}
				//构建删除SQL语句
				CopyObjectFields(message,*subMessage);
				BuildDeleteSQL(*subMessage,tempSql);
				subSql+=tempSql;
			}
			delete tempMessage;
			
			int fieldSize=pRef->FieldSize(message,field);
			//构建插入SQL语句
			for(int j=0;j<fieldSize;j++)
			{
				std::set<std::string> subFields;
				if(isBlob)
				{
                    GetBlobFields(pRef->GetRepeatedMessage(message,field,j),subFields);
				}
				else
				{
                    GetNoBlobFields(pRef->GetRepeatedMessage(message,field,j),subFields);
				}
				if(BuildInsertSQL(message,pRef->GetRepeatedMessage(message,field,j),subFields,tempSql))//构造插入语句
				{
					subSql+=tempSql;
				}	
			}
			isMessage=true;
			continue;
		}
		GetFieldValue(message,field,fieldValue,isMessage);
		if(isMessage)
		{
			std::string strSubName="";
			GetNameByMessage(pRef->GetMessage(message,field),strSubName);
			if(strSubName==strParentName)//避免死循环
			{
				continue;
			}
			std::set<std::string> inFields;
			if(isBlob)
			{
				GetBlobFields(pRef->GetMessage(message,field),inFields);
			}
			else
			{
				GetNoBlobFields(pRef->GetMessage(message,field),inFields);
			}
			if(BuildUpdateSQL(message,pRef->GetMessage(message,field),inFields,tempSql,isBlob))//递归调用
			{
				parentSql+=tempSql;
			}
		}
        transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
		if(primaryKeys.count(fieldName))//主键
		{
			if(primaryCount>0)
			{
				strSqlTail+=" AND ";
			}
			strSqlTail+=fieldName+"=";
			strSqlTail+=fieldValue;
			primaryCount++;
		}
		else if(!isMessage)
		{
			if(!updateFields.count(fieldName))//不属于更新字段
			{
				continue;
			}
			if(!pRef->HasField(message,field))
			{
				continue;
			}
			currentIndex++;
			if(currentIndex>1)
			{
				strSqlHead+=",";
			}
			strSqlHead+=fieldName+"=";
			strSqlHead+=fieldValue;
			setCount++;
		}
	}	
    strSql+=parentSql;
	if(primaryCount!=0&&setCount!=0)
	{
		strSql+=strSqlHead+strSqlTail+";";
	}
	strSql+=subSql;
	return true;
}
//构建查询SQL语句,取fieldValue为非空的字段为匹配字段
bool CDatabaseService::BuildQuerySQL( const ::google::protobuf::Message& message,std::map<std::string,::google::protobuf::FieldDescriptor::Type>& fieldMap,std::set<std::string> queryFields,std::string& strSql )
{
	if(queryFields.size()==0)
	{
		SetMyLastError("查询字段为空");
		return false;
	}
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::string strTableName=descriptor->name();//表名

	strSql="";//完整查询SQL语句
	std::string strSqlHead="SELECT ";
	std::string strSqlTail=" FROM ";
	strSqlTail+=strTableName;
	strSqlTail+=" WHERE ";
	int primaryCount=0;//主键个数
	int fieldCount=descriptor->field_count();
	int currentIndex=0;

	BuildMessageField(message,fieldMap);

	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		std::string fieldName=field->name();//字段名称
		transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);

		std::string fieldValue="";
		bool isMessage=false;//消息标识
		GetFieldValue(message,field,fieldValue,isMessage);

		if(!field->is_repeated()&&!isMessage&&pRef->HasField(message,field)&&fieldName.find("BLOB")==fieldName.npos)
		{
			if(primaryCount>0)
			{
				strSqlTail+=" AND ";
			}
			strSqlTail+=fieldName+"=";
			strSqlTail+=fieldValue;
			primaryCount++;
		}
        
		if(!queryFields.count(fieldName))//不属于查询字段
		{
			continue;
		}
		if(!isMessage)
		{
			currentIndex++;
			if(currentIndex>1)
			{
				strSqlHead+=",";
			}
			//判断是否为BLOB字段
			if(fieldName.find("BLOB")!=fieldName.npos)
			{
				std::string strRealField=fieldName.substr(5,fieldName.length()-5);
				strRealField+=" as ";
				strRealField+=fieldName;
				strSqlHead+=strRealField;
			}
			else
			{
                strSqlHead+=fieldName;
			}	
		}	
	}	

	strSql+=strSqlHead+strSqlTail;

	if(primaryCount==0)
	{
		SetMyLastError("条件字段为空");
		return false;
	}

	return true;

}

//构建对象属性和类型
void CDatabaseService::BuildMessageField( const ::google::protobuf::Message& message,std::map<std::string,::google::protobuf::FieldDescriptor::Type>& fieldMap)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::string strTableName=descriptor->name();//表名

	for(int i=0;i<descriptor->field_count();i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		std::string fieldName=field->name();//字段名称
		bool isMessage=false;//消息标识
		::google::protobuf::FieldDescriptor::Type fieldType=field->type();//字段类型
		if(field->is_repeated())//重复字段，即子表
		{
			fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::MAX_TYPE;
		}
		switch(fieldType)
		{
		case ::google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE: //嵌套消息类型(父表)
			{      
				isMessage=true;
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE;
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_STRING:
		case ::google::protobuf::FieldDescriptor::Type::TYPE_BYTES:
			{
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_STRING;
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_INT32:
			{
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_INT32;
			}			
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_INT64:
			{
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_INT32;
			}			
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_UINT32:
			{
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_INT32;
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_UINT64:
			{
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_INT32;
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_FLOAT:			
			{			
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_FLOAT;
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE:			
			{				
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE;
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_BOOL:			
			{			
				fieldMap[fieldName]=::google::protobuf::FieldDescriptor::Type::TYPE_BOOL;
			}

			break;
		default:
			break;
		}			
	}	
}

//构建删除SQL语句
bool CDatabaseService::BuildDeleteSQL(std::vector<::google::protobuf::Message*> objectList,std::string& strDeleteSql)
{
	int count=objectList.size();
	strDeleteSql="BEGIN ";
	for(int i=0;i<count;i++)
	{
        ::google::protobuf::Message* message=objectList[i];
		const ::google::protobuf::Message::Reflection* pRef=message->GetReflection();
		const ::google::protobuf::Descriptor* descriptor=message->GetDescriptor(); 
		std::string strTableName=descriptor->name();//表名

		std::string strSql="DELETE FROM ";
		strSql+=strTableName;
		strSql+=" WHERE ";
		int primaryCount=0;//主键个数
		int fieldCount=descriptor->field_count();
		int currentIndex=0;

		for(int i=0;i<fieldCount;i++)
		{
			const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
			if(!pRef->HasField(*message,field))
			{
				continue;
			}
			std::string fieldName=field->name();//字段名称

			std::string fieldValue="";
			bool isMessage=false;//消息标识
			GetFieldValue(*message,field,fieldValue,isMessage);
			transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
			if(primaryCount>0)
			{
				strSql+=" AND ";
			}
			strSql+=fieldName+"=";
			strSql+=fieldValue;
			primaryCount++;
		}	
		strDeleteSql+=strSql+";";

		if(primaryCount==0)
		{
			SetMyLastError("条件字段为空");
			return false;
		}
	}
	strDeleteSql+=" END;";
	return true;
}

//构建删除SQL语句
bool CDatabaseService::BuildDeleteSQL(::google::protobuf::Message& message,std::string& strDeleteSql)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	if(!pRef)
	{
		return false;
	}
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	if(!descriptor)
	{
		return false;
	}
	std::string strTableName=descriptor->name();//表名

	std::string strSql="DELETE FROM ";
	strSql+=strTableName;
	strSql+=" WHERE ";
	int primaryCount=0;//主键个数
	int fieldCount=descriptor->field_count();
	int currentIndex=0;

	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		
		std::string fieldName=field->name();//字段名称

		std::string fieldValue="";
		bool isMessage=false;//消息标识

		GetFieldValue(message,field,fieldValue,isMessage);
		transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);

		if(isMessage||field->is_repeated()||!pRef->HasField(message,field))
		{
			continue;
		}

		if(primaryCount>0)
		{
			strSql+=" AND ";
		}
		strSql+=fieldName+"=";
		strSql+=fieldValue;
		primaryCount++;	
	}	
	strDeleteSql+=strSql+";";

	if(primaryCount==0)
	{
		SetMyLastError("条件字段为空");
		return false;
	}
	return true;
}

//构建个数查询SQL语句
bool CDatabaseService::BuildQueryCountSQL(::google::protobuf::Message& message,std::string& strQuerySql)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	if(!pRef)
	{
		return false;
	}
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	if(!descriptor)
	{
		return false;
	}
	std::string strTableName=descriptor->name();//表名

	std::string strSql="SELECT COUNT(*) AS CXSL FROM ";
	strSql+=strTableName;
	strSql+=" WHERE ";
	int primaryCount=0;//主键个数
	int fieldCount=descriptor->field_count();
	int currentIndex=0;

	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		
		std::string fieldName=field->name();//字段名称

		std::string fieldValue="";
		bool isMessage=false;//消息标识

		GetFieldValue(message,field,fieldValue,isMessage);
		transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
		if(isMessage||field->is_repeated()||!pRef->HasField(message,field))
		{
			continue;
		}
		if(primaryCount>0)
		{
			strSql+=" AND ";
		}
		strSql+=fieldName+"=";
		strSql+=fieldValue;
		primaryCount++;	
	}	
	strQuerySql+=strSql+"";

	if(primaryCount==0)
	{
		SetMyLastError("条件字段为空");
		return false;
	}
	return true;
}

//插入对象
bool CDatabaseService::InsertObject( ::google::protobuf::Message& message )
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::set<std::string> saveFields;
	std::set<std::string> updateFields;
	GetNoBlobFields(message,saveFields);//获取非BLOB字段
	if(InsertObject(message,saveFields))
	{
		GetBlobFields(message,updateFields);//获取非BLOB字段
		return UpdateMessageBlob(message,message,updateFields);
	}
    
	return false;
}

//根据指定的字段插入对象
bool CDatabaseService::InsertObject( ::google::protobuf::Message& message,std::set<std::string> saveFields )
{
	std::string strSQL="BEGIN ";
	std::string tempSql;
	if(!BuildInsertSQL(message,message,saveFields,tempSql))
	{
		return false;
	}
	strSQL+=tempSql;
	strSQL+=" END;";
	if(pInterface)
	{
		std::string strMessage;
		int retValue=pInterface->SqlUpdate(strSQL.c_str(),strMessage);
		if(retValue>=0)
		{
			pInterface->CommitTrans();//提交事务
			return true;
		}
		else
		{
			SetMyLastError(strMessage);
			pInterface->RollbackTrans();//回滚事务
		}
	}
	return false;

}

//更新对象
bool CDatabaseService::UpdateObject( ::google::protobuf::Message& message )
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::set<std::string> noBlobFields;
	std::set<std::string> blobFields;
	GetNoBlobFields(message,noBlobFields);//获取非BLOB字段
	if(UpdateObject(message,noBlobFields))
	{
		GetBlobFields(message,blobFields);//获取BLOB字段
		return UpdateMessageBlob(message,message,blobFields);
	}
	return false;
}

//根据指定的对象更新对象
bool CDatabaseService::UpdateObject( ::google::protobuf::Message& message,std::set<std::string> updateFields )
{
	std::string strSQL="BEGIN ";
	std::string tempSql="";
	if(!BuildUpdateSQL(message,message,updateFields,tempSql,false))
	{
		return false;
	}
	strSQL+=tempSql;
	strSQL+=" END;";
	if(pInterface)
	{
		std::string strMessage;
		int retValue=pInterface->SqlUpdate(strSQL.c_str(),strMessage);
		if(retValue>=0)
		{
			pInterface->CommitTrans();//提交事务
			return true;
		}
		else
		{
			pInterface->RollbackTrans();//回滚事务
			SetMyLastError(strMessage);
		}
	}
	return false;

}

//拷贝对象相同的属性
void CDatabaseService::CopyObjectFields(const ::google::protobuf::Message& parent,::google::protobuf::Message& message)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	if(!pRef)
	{
		return;
	}
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	if(!descriptor)
	{
		return;
	}
	std::string strTableName=descriptor->full_name();//表名

	const ::google::protobuf::Message::Reflection* pParentRef=parent.GetReflection();
	if(!pParentRef)
	{
		return;
	}
	const ::google::protobuf::Descriptor* parentDescriptor=parent.GetDescriptor(); 
	if(!parentDescriptor)
	{
		return;
	}
	std::string strParentName=parentDescriptor->full_name();//调用表名

	if(strTableName==strParentName)
	{
		return;
	}

	//此处要添加例外！
	std::string srcFieldName;
	std::string refFieldName;
	GetExceptionTable(strParentName,strTableName,srcFieldName,refFieldName);
	transform(srcFieldName.begin(),srcFieldName.end(),srcFieldName.begin(),::tolower);
	transform(refFieldName.begin(),refFieldName.end(),refFieldName.begin(),::tolower);

	int fieldCount=parentDescriptor->field_count();
	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* parentField=parentDescriptor->field(i);
		if(!parentField)
		{
			continue;
		}
		std::string fieldName=parentField->name();//字段名称
		std::string fieldValue="";//字段值
		double dValue;
		float fValue;
		int iValue;
		bool isMessage=false;//消息标识
		GetFieldValue(parent,parentField,fieldValue,isMessage);
		if('\''==fieldValue[0])
		{
			fieldValue=fieldValue.substr(1,fieldValue.length()-2);//删除两端的单引号
		}
		if(isMessage||parentField->is_repeated())
		{
			continue;
		}

		//此处要添加例外！
		if(srcFieldName==fieldName)//找到例外
		{
			fieldName=refFieldName;
		}
		const ::google::protobuf::FieldDescriptor* field=descriptor->FindFieldByName(fieldName);
		if(!field)
		{
			continue;
		}
		::google::protobuf::FieldDescriptor::Type fieldType=field->type();//字段类型
		switch(fieldType)
		{
		case ::google::protobuf::FieldDescriptor::Type::TYPE_STRING:
		case ::google::protobuf::FieldDescriptor::Type::TYPE_BYTES:
			{                      
				pRef->SetString(&message,field,fieldValue);
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_FLOAT:
			{						
				fValue=(float)atof(fieldValue.c_str());
				pRef->SetFloat(&message,field,fValue);
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE:
			{						
				dValue=atof(fieldValue.c_str());
				pRef->SetDouble(&message,field,dValue);
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_INT32:
			{
				iValue=atoi(fieldValue.c_str());
				pRef->SetInt32(&message,field,iValue);
			}
			break;
		case ::google::protobuf::FieldDescriptor::Type::TYPE_INT64:
			{
				iValue=atoi(fieldValue.c_str());
				pRef->SetInt64(&message,field,iValue);
			}
			break;
		default:
			break;
		}
	}
}

//递归设置对象的属性
bool CDatabaseService::SetObjectField(const ::google::protobuf::Message& parent,::google::protobuf::Message& message,std::string strField,std::string strValue)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	if(!pRef)
	{
		return false;
	}
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	if(!descriptor)
	{
		return false;
	}

	std::string strTableName=descriptor->full_name();
	std::string strName="";//调用表名
	GetNameByMessage(parent,strName);

	//此处要添加例外！
	std::string srcFieldName;
	std::string refFieldName;
	
	GetExceptionTable(strName,strTableName,srcFieldName,refFieldName);
	transform(srcFieldName.begin(),srcFieldName.end(),srcFieldName.begin(),::tolower);
	transform(refFieldName.begin(),refFieldName.end(),refFieldName.begin(),::tolower);
	transform(strField.begin(),strField.end(),strField.begin(),::tolower);//字段统一转为小写
	std::string strFieldName=strField;

	//此处要添加例外！
	if(srcFieldName==strField)//找到例外
	{
		strFieldName=refFieldName;
	}
	const ::google::protobuf::FieldDescriptor* field=descriptor->FindFieldByName(strFieldName);
	if(!field)
	{
		return false;
	}
	::google::protobuf::FieldDescriptor::Type fieldType=field->type();//属性类型
	switch(fieldType)
	{
	case ::google::protobuf::FieldDescriptor::Type::TYPE_STRING:
	case ::google::protobuf::FieldDescriptor::Type::TYPE_BYTES:
		{                      
			pRef->SetString(&message,field,strValue);
		}
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE:
		{						
			double dValue=atof(strValue.c_str());
			pRef->SetDouble(&message,field,dValue);
		}
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_FLOAT:
		{						
			float fValue=(float)atof(strValue.c_str());
			pRef->SetFloat(&message,field,fValue);
		}
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_INT32:
	case ::google::protobuf::FieldDescriptor::Type::TYPE_INT64:
		{
			int iValue=atoi(strValue.c_str());
			pRef->SetInt32(&message,field,iValue);
		}
		break;
	case ::google::protobuf::FieldDescriptor::Type::TYPE_BOOL:
		{						
			int iValue=atoi(strValue.c_str());
			pRef->SetBool(&message,field,iValue);
		}
		break;
	default:
		break;
	}
    
	//递归设置主表和子表
	int fieldCount=descriptor->field_count();
	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		if(!field)
		{
			continue;
		}
		::google::protobuf::FieldDescriptor::Type fieldType=field->type();//属性类型
		if(field->is_repeated())//重复字段
		{
			int fieldSize=pRef->FieldSize(message,field);;
			for(int i=0;i<fieldSize;i++)
			{
				::google::protobuf::Message* subMessage=pRef->MutableRepeatedMessage(&message,field,i);
				if(subMessage)
				{
					std::string subName;
					GetNameByMessage(*subMessage,subName);
					if(subName==strName)//避免死循环
					{
						continue;
					}
					SetObjectField(message,*subMessage,strField,strValue);
				}
			}
		}
		else if(fieldType==::google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE)
		{
			::google::protobuf::Message* parentMessage=pRef->MutableMessage(&message,field);
			if(parentMessage)
			{
				std::string parentName;
				GetNameByMessage(*parentMessage,parentName);
				if(parentName==strName)//避免死循环
				{
					break;
				}
				SetObjectField(message,*parentMessage,strField,strValue);
			}
		}
	}
	return true;
}

//查询对象
bool CDatabaseService::QueryObject( ::google::protobuf::Message& message ,bool upCascade,bool downCascade)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	if(!pRef)
	{
		return false;
	}
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	if(!descriptor)
	{
		return false;
	}
	std::set<std::string> queryFields;
	GetAllFields(message,queryFields);
	return QueryObject(message,upCascade,downCascade,queryFields);
}

//根据指定的查询字段查询对象
bool CDatabaseService::QueryObject(::google::protobuf::Message& message,bool upCascade,bool downCascade,std::set<std::string> queryFields)
{
	return QueryObject(message,message,upCascade,downCascade,queryFields);
}

//根据指定的查询字段查询对象
bool CDatabaseService::QueryObject(const ::google::protobuf::Message& parent,::google::protobuf::Message& message,bool upCascade,bool downCascade,std::set<std::string> queryFields)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	if(!pRef)
	{
		return false;
	}
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	if(!descriptor)
	{
		return false;
	}
	std::map<std::string,::google::protobuf::FieldDescriptor::Type> fieldMap;
	std::string strTableName=descriptor->name();//表名

	const ::google::protobuf::Message::Reflection* pParentRef=parent.GetReflection();
	if(!pParentRef)
	{
		return false;
	}
	const ::google::protobuf::Descriptor* parentDescriptor=parent.GetDescriptor(); 
	if(!parentDescriptor)
	{
		return false;
	}
	std::string strParentName=parentDescriptor->name();//调用表名
    
	//拷贝调用表属性
	CopyObjectFields(parent,message);

	std::string strSQL;
	if(!BuildQuerySQL(message,fieldMap,queryFields,strSQL))
	{
		return false;
	}
	//printf(strSQL.c_str());
	if(pInterface)
	{
		Query	query(pInterface);
		query.exeSQL(strSQL.c_str()); // 查询SQL语句不能加分号
		rapidxml::xml_node<>* root    =   query.first_node();
		rapidxml::xml_node<>* item    =   root->first_node();
		for ( ; item != 0 ; item = item->next_sibling())
		{
			std::string fieldName;
			std::string sValue="";
			int iValue=0;
			double dValue=0;
			float fValue=0;
			::google::protobuf::FieldDescriptor::Type fieldType;
			std::string subSql="";//子表查询语句
			std::map<std::string,::google::protobuf::FieldDescriptor::Type>::iterator mIter = fieldMap.begin();
			for(; mIter != fieldMap.end(); ++mIter)//遍历表字段
			{
				fieldName=mIter->first;
				fieldType=mIter->second;
				transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::tolower);
				const ::google::protobuf::FieldDescriptor* field=descriptor->FindFieldByName(fieldName);
				transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
				
                //判断是否为BLOB字段
				if(field&&fieldName.find("BLOB")!=fieldName.npos)
				{
					size_t  data    =   0;
					char* pData; //blob数据
					unsigned long blobSize; //blob大小 
					if(NULL!=item->first_attribute(fieldName.c_str()))
					{
						const char* blob    =   item->first_attribute(fieldName.c_str())->value();
						if (blob && strcmp(blob,"0 0"))
						{
							sscanf(blob,"%u %u",&blobSize,&data);
							pData				=   new char[blobSize];
							if(NULL==pData)
							{
								continue;
							}
							memcpy(pData,(char*)data,blobSize);
							pInterface->ReleaseBlob((char*)data);
                            char* tempBuf;
							EncodeBase64(pData,blobSize,tempBuf);
							SAFE_DELETE_ARRAY(pData);
							std::string strBuf(tempBuf);  
							pRef->SetString(&message,field,strBuf);
							SAFE_DELETE(tempBuf);
						} 
						else
						{
							blobSize=0;
							pData=NULL;
						}
					}                
					continue;
				}

				if(field&&downCascade&&field->is_repeated())//重复字段，即级联查询子表
				{
					std::string subName;
					::google::protobuf::Message* subMessage=pRef->AddMessage(&message,field);;
					if(subMessage)
					{
						GetNameByMessage(*subMessage,subName);
						if(subName==strParentName)//避免死循环
						{
							continue;
						}
						
						std::set<std::string> subFiels;
						std::map<std::string,::google::protobuf::FieldDescriptor::Type> tempFieldMap;
						std::vector<::google::protobuf::Message*> resultList;
						GetAllFields(*subMessage,subFiels);
						//拷贝调用表属性
						CopyObjectFields(message,*subMessage);
						//构建查询语句查询子表
						if(!BuildQuerySQL(*subMessage,tempFieldMap,subFiels,subSql))
						{
							return false;
						}
						QueryObjectList(*subMessage,subSql,resultList);
						int retSize=resultList.size();
						if(retSize==0)
						{
							pRef->RemoveLast(&message,field);
							continue;
						}
						for(unsigned int i=0;i<resultList.size();i++)
						{
							::google::protobuf::Message* newMessage=NULL;
							if(i==0)
							{
								newMessage=subMessage;
							}
							else
							{
								newMessage=pRef->AddMessage(&message,field);

							}
							if(newMessage)
							{
								newMessage->CopyFrom(*resultList[i]);
								std::set<std::string> newFields;
								GetAllFields(*newMessage,newFields);
								QueryObject(message,*newMessage,upCascade,downCascade,newFields);//递归查询字表
							}

						}
						FreeObjectList(resultList);
					}										                  					
					continue;
				}

				switch(fieldType)
				{
				case ::google::protobuf::FieldDescriptor::Type::TYPE_STRING:
					{                      
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							sValue=item->first_attribute(fieldName.c_str())->value();
						}
						pRef->SetString(&message,field,sValue);
					}
					break;
				case ::google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE:
					{						
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							dValue=atof(item->first_attribute(fieldName.c_str())->value());
						}
						pRef->SetDouble(&message,field,dValue);
					}
					break;
				case ::google::protobuf::FieldDescriptor::Type::TYPE_FLOAT:
					{						
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							fValue=(float)atof(item->first_attribute(fieldName.c_str())->value());
						}
						pRef->SetFloat(&message,field,fValue);
					}
					break;
				case ::google::protobuf::FieldDescriptor::Type::TYPE_INT32:
					{
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							iValue=atoi(item->first_attribute(fieldName.c_str())->value());
						}
						pRef->SetInt32(&message,field,iValue);
					}
					break;
				case ::google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE://父表
					{
						if(upCascade)//级联查询
						{
							::google::protobuf::Message* subMessage=pRef->MutableMessage(&message,field);
							std::string subName;
							GetNameByMessage(*subMessage,subName);
							if(subName!=strParentName)//避免死循环
							{
								std::set<std::string> subFields;
								GetAllFields(*subMessage,subFields);
								//拷贝调用表属性
								CopyObjectFields(message,*subMessage);
								QueryObject(message,*subMessage,upCascade,downCascade,subFields);//递归查询主表
							}	
						}
					}			
					break;
				default:
					break;
				}
			}
			break; //取一个对象
		}	
	}
	return true;
}

//释放对象列表
bool CDatabaseService::FreeObjectList(std::vector<::google::protobuf::Message*>& resultList)
{
	std::vector<::google::protobuf::Message*>::iterator it=resultList.begin();
	while (it!=resultList.end())
	{
		::google::protobuf::Message* message=*it;
		SAFE_DELETE(message);
		message=NULL;
		it++;
	}
	resultList.clear();
	return true;
}

//查询单个对象列表
bool CDatabaseService::QuerySingleObjectList( const ::google::protobuf::Message& message,std::vector<::google::protobuf::Message*>& resultList)
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::set<std::string> queryFields;
	GetAllFields(message,queryFields);
	std::map<std::string,::google::protobuf::FieldDescriptor::Type> fieldMap;
	std::string strSQL;
	if(BuildQuerySQL(message,fieldMap,queryFields,strSQL))
	{
		return QueryObjectList(message,strSQL,resultList);
	}
	return false;
}

//根据自定义查询语句查询对象列表
bool CDatabaseService::QueryObjectList( const ::google::protobuf::Message& message,std::string querySQL, std::vector<::google::protobuf::Message*>& resultList)
{
	FreeObjectList(resultList);
	std::map<std::string,::google::protobuf::FieldDescriptor::Type> fieldMap;
	BuildMessageField(message,fieldMap);
	if(pInterface)
	{
		Query	query(pInterface);
		query.exeSQL(querySQL.c_str()); // 查询SQL语句不能加分号
		rapidxml::xml_node<>* root    =   query.first_node();
		rapidxml::xml_node<>* item    =   root->first_node();
		for ( ; item != 0 ; item = item->next_sibling())
		{
			::google::protobuf::Message* newMessage=message.New();//复制新对象实例
			const ::google::protobuf::Message::Reflection* pRef=newMessage->GetReflection();
			const ::google::protobuf::Descriptor* descriptor=newMessage->GetDescriptor(); 
			std::string fieldName;
			std::string sValue="";
			int iValue=0;
			double dValue=0;
			::google::protobuf::FieldDescriptor::Type fieldType;
			std::map<std::string,::google::protobuf::FieldDescriptor::Type>::iterator mIter = fieldMap.begin();
			for(; mIter != fieldMap.end(); ++mIter)
			{
				fieldName=mIter->first;
				fieldType=mIter->second;
				transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::tolower);
				const ::google::protobuf::FieldDescriptor* field=descriptor->FindFieldByName(fieldName);
				transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
				if(fieldName.find("BLOB")!=fieldName.npos)
				{
					size_t  data    =   0;
					char* pData; //blob数据
					unsigned long blobSize; //blob大小 
					if(NULL!=item->first_attribute(fieldName.c_str()))
					{
						const char* blob    =   item->first_attribute(fieldName.c_str())->value();
						if (blob && strcmp(blob,"0 0"))
						{
							sscanf(blob,"%u %u",&blobSize,&data);
							pData				=   new char[blobSize];
							if(NULL==pData)
							{
								continue;
							}
							memcpy(pData,(char*)data,blobSize);
							pInterface->ReleaseBlob((char*)data);
							char* tempBuf;
							EncodeBase64(pData,blobSize,tempBuf);
							SAFE_DELETE_ARRAY(pData);
							std::string strBuf(tempBuf);  
							pRef->SetString(newMessage,field,strBuf);
							SAFE_DELETE(tempBuf);
						} 
						else
						{
							blobSize               =   0;
							pData				=	NULL;
						}
					}

					continue;
				}
				switch(fieldType)
				{
				case ::google::protobuf::FieldDescriptor::Type::TYPE_STRING:
					{                      
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							sValue=item->first_attribute(fieldName.c_str())->value();
						}
						pRef->SetString(newMessage,field,sValue);
					}
					break;
				case ::google::protobuf::FieldDescriptor::Type::TYPE_FLOAT:
					{						
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							dValue=atof(item->first_attribute(fieldName.c_str())->value());
						}
						pRef->SetFloat(newMessage,field,(float)dValue);
					}
					break;
				case ::google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE:
					{						
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							dValue=atof(item->first_attribute(fieldName.c_str())->value());
						}
						pRef->SetDouble(newMessage,field,dValue);
					}
					break;
				case ::google::protobuf::FieldDescriptor::Type::TYPE_INT32:
					{
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							iValue=atoi(item->first_attribute(fieldName.c_str())->value());
						}
						pRef->SetInt32(newMessage,field,iValue);
					}
					break;
				case ::google::protobuf::FieldDescriptor::Type::TYPE_BOOL:
					{
						if(NULL!=item->first_attribute(fieldName.c_str()))
						{
							iValue=atoi(item->first_attribute(fieldName.c_str())->value());
						}
						pRef->SetBool(newMessage,field,iValue);
					}
					break;
				default:
					break;
				}

			}

			resultList.push_back(newMessage);
		}	
	}

	return true;
}

//查询Blob字段
bool CDatabaseService::QueryBlob( ::google::protobuf::Message& message,std::string queryField,char*& pData,unsigned long& blobSize )
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::map<std::string,::google::protobuf::FieldDescriptor::Type> fieldMap;
	std::string strTableName=descriptor->name();//表名
	std::set<std::string> queryFields;
	queryFields.insert(queryField);
	std::string strSQL="";
	if(!BuildQuerySQL(message,fieldMap,queryFields,strSQL))
	{
		return false;
	}
	if(pInterface)
	{
		Query queryBlob(pInterface);
		queryBlob.exeSQL(strSQL.c_str());
		rapidxml::xml_node<>* pRoot    =   queryBlob.first_node();
		rapidxml::xml_node<>* pItem    =   pRoot->first_node();
		for ( ; pItem != 0 ; pItem = pItem->next_sibling())
		{
			size_t  data    =   0;
			if(NULL!=pItem->first_attribute(queryField.c_str()))
			{
				const char* blob    =   pItem->first_attribute(queryField.c_str())->value();
				const ::google::protobuf::FieldDescriptor* field=descriptor->FindFieldByName(queryField);
				if (blob && strcmp(blob,"0 0"))
				{
					sscanf(blob,"%u %u",&blobSize,&data);
					pData				=   new char[blobSize];
					if(NULL==pData)
					{
						continue;
					}
					memcpy(pData,(char*)data,blobSize);
					pInterface->ReleaseBlob((char*)data);					
				} 
				else
				{
					blobSize               =   0;
					pData				=	NULL;
					return false;
				}
			}
		}
		return true;
	}
	SetMyLastError("数据库访问接口指针为空");
	return false;  
}

//查询Blob字段至文件
bool CDatabaseService::QueryBlob( ::google::protobuf::Message& message,std::string queryField,char* pPath)
{
	char* pData=NULL;
	unsigned long blobSize=0;
	if(!QueryBlob(message,queryField,pData,blobSize))
	{
		return false;
	}
	if(NULL!=pPath)
	{
		FILE*	pOutFile	=	fopen(pPath,"wb");
		if (!pOutFile)
		{
			SAFE_DELETE_ARRAY(pData);
			pData=NULL;
			SetMyLastError("文件打开失败");
			return	false;	
		}
		else
		{
			fwrite(pData,blobSize,1,pOutFile);
			fclose(pOutFile);
			SAFE_DELETE_ARRAY(pData);
			pData=NULL;
			return true;
		}
	}
	SetMyLastError("文件路径为空");
	return false;
}

//更新Blob字段
bool CDatabaseService::UpdateBlob(const ::google::protobuf::Message& message,std::string updateField,char* pData,unsigned long blobSize )
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 
	std::map<std::string,::google::protobuf::FieldDescriptor::Type> fieldMap;
	std::string strTableName=descriptor->name();//表名
	std::set<std::string> primaryKeys;
	GetPrimaryKeys(strTableName,primaryKeys);

	int fieldCount=descriptor->field_count();//字段总数
	int primaryCount=0;
	std::string strSql="";//更新SQL语句

	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		std::string fieldName=field->name();//字段名称
		std::string fieldValue="";
		bool isPrimary=false;//主键标识
		bool isMessage=false;//消息标识
		GetFieldValue(message,field,fieldValue,isMessage);
        transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
		if(primaryKeys.count(fieldName))//主键
		{
			if(primaryCount>0)
			{
				strSql+=" AND ";
			}
			strSql+=fieldName+"=";
			strSql+=fieldValue;
			primaryCount++;
		}
	}

	if(pInterface&&primaryCount>0)
	{
		std::string strRealField=updateField.substr(5,updateField.length()-5);
		if(pInterface->UpdateBlob(strTableName.c_str(),strRealField.c_str(),pData,blobSize,strSql.c_str())<0)
		{
			SAFE_DELETE_ARRAY(pData);
			pData=NULL;
			SetMyLastError("更新媒体数据失败");
			return false;
		}
		SAFE_DELETE_ARRAY(pData);
		pData=NULL;
		return true;
	}
	SetMyLastError("数据库访问接口指针为空");
	return false;
}

//根据文件更新Blob字段
bool CDatabaseService::UpdateBlob( const ::google::protobuf::Message& message,std::string updateField,char* pPath )
{
	FILE*	pFile	=	fopen(pPath,"rb");
	if (!pFile)
	{
		return	false;	
	}
	fseek(pFile,0,SEEK_END);
	unsigned long  size    =   ftell(pFile);
	fseek(pFile,0,SEEK_SET);
	char*   pBuffer  =   new char[size + 1];
	memset(pBuffer,0,size + 1);
	fread(pBuffer,1,size,pFile);
	fclose(pFile);

	return UpdateBlob(message,updateField,pBuffer,size);
}

//根据Message更新Blob字段(需要递归更新)
bool CDatabaseService::UpdateMessageBlob( const ::google::protobuf::Message& parent, const ::google::protobuf::Message& message,std::set<std::string> updateFields )
{
	const ::google::protobuf::Message::Reflection* pRef=message.GetReflection();
	const ::google::protobuf::Descriptor* descriptor=message.GetDescriptor(); 

	std::set<std::string> allFields;
	GetAllFields(message,allFields);//获取所有字段
	int fieldCount=allFields.size();

	std::string strParentName="";
	GetNameByMessage(parent,strParentName);

	for(int i=0;i<fieldCount;i++)
	{
		const ::google::protobuf::FieldDescriptor* field=descriptor->field(i);
		std::string fieldName=field->name();//字段名称
		std::string fieldValue="";
		bool isMessage=false;//消息标识
		if(field->is_repeated())//重复字段，即子表
		{
			int fieldSize=pRef->FieldSize(message,field);
			if(fieldSize>0)
			{
				std::string strSubName="";
				GetNameByMessage(pRef->GetRepeatedMessage(message,field,0),strSubName);
				if(strSubName==strParentName)//避免死循环
				{
					continue;
				}

				for(int j=0;j<fieldSize;j++)
				{
					const ::google::protobuf::Message& subMessage=pRef->GetRepeatedMessage(message,field,j);
					std::set<std::string> subBlobFields;
					GetBlobFields(subMessage,subBlobFields);//获取BLOB字段
					UpdateMessageBlob(message,subMessage,subBlobFields);
				}
			}
			continue;
		}
		GetFieldValue(message,field,fieldValue,isMessage);
		if(isMessage)
		{
			std::string strSubName="";
			GetNameByMessage(pRef->GetMessage(message,field),strSubName);
			if(strSubName==strParentName)//避免死循环
			{
				continue;
			}

			const ::google::protobuf::Message& parentMessage=pRef->GetMessage(message,field);
			std::set<std::string> parentBlobFields;
			GetBlobFields(parentMessage,parentBlobFields);//获取BLOB字段
			UpdateMessageBlob(message,parentMessage,parentBlobFields);
		}
		else
		{
			transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::toupper);
			if(!updateFields.count(fieldName))//非BLOB字段忽略
			{
				continue;
			}
			int bufLen=0;
			char* pData="";
			transform(fieldName.begin(),fieldName.end(),fieldName.begin(),::tolower);
			const ::google::protobuf::FieldDescriptor* field=descriptor->FindFieldByName(fieldName);
			if(!field)
			{
				continue;
			}
			GetFieldValue(message,field,fieldValue,isMessage);
			if(fieldValue==""||fieldValue=="''")
			{
				continue;
			}
			DecodeBase64(fieldValue.c_str(),bufLen,pData);

			if(!pData)
			{
				SetMyLastError("媒体数据BASE64编码失败");
				return false;
			}
			if(!UpdateBlob(message,fieldName,pData,bufLen))
			{
				return false;
			}
		}
	}	
	return true;
}

//整型转字符串
std::string CDatabaseService::intToString(int value)
{
	std::stringstream ss;
	ss<<value;
	return ss.str();
}

//单精度浮点型转字符串
std::string CDatabaseService::floatToString(float value)
{
	std::stringstream ss;
	ss<<value;
	return ss.str();
}

//双精度浮点型转字符串
std::string CDatabaseService::doubleToString(double value)
{
	std::stringstream ss;
	ss<<value;
	return ss.str();
}

//BOOL类型转字符串
std::string CDatabaseService::boolToString(bool value)
{
	std::stringstream ss;
	ss<<value;
	return ss.str();
}

//根据SQL语句删除数据库表数据
bool CDatabaseService::DeleteBySql( std::string deleteSql )
{
	if(pInterface)
	{
		int retValue=pInterface->SqlDelete(deleteSql.c_str());
		return retValue<0?false:true;
		
	}
	return false;

}

//删除对象列表
bool CDatabaseService::DeleteObjectList( std::vector<::google::protobuf::Message*> objectList )
{
   int count=objectList.size();
   if(count>0)
   {
	   std::string strDeleteSql;
	   if(!BuildDeleteSQL(objectList,strDeleteSql))
	   {
		   return false;
	   }
	   return DeleteBySql(strDeleteSql);
   }
   return true;
}

//Base64编码
bool CDatabaseService::EncodeBase64( const char* sourceBuffer,const int bufLen,char*& base64String )
{
	
	DWORD dwLen;
	const BYTE* pbBinary=(const BYTE*)sourceBuffer;
	CryptBinaryToStringA(pbBinary,bufLen,CRYPT_STRING_BASE64,NULL,&dwLen);
	base64String=new char[dwLen];
	CryptBinaryToStringA(pbBinary,bufLen,CRYPT_STRING_BASE64,base64String,&dwLen);

	return true; 

}

//Base64解码
bool CDatabaseService::DecodeBase64( const char* base64String,int& bufLen,char*& sourceBuffer )
{
	DWORD cbBinary;
	DWORD dwSkip;
	DWORD dwFlags;
	std::string strBase64(base64String);  
	if('\''==strBase64[0])
	{
        strBase64=strBase64.substr(1,strBase64.length()-1);//删除首端的单引号
	}
	if('\''==strBase64[strBase64.length()-1])
	{
		strBase64=strBase64.substr(0,strBase64.length()-1);//删除末端的单引号
	}
	
	int uSize=strBase64.length();
	CryptStringToBinaryA(strBase64.data(), uSize, CRYPT_STRING_BASE64, NULL, &cbBinary, &dwSkip, &dwFlags);
	BYTE* outBuffer = new BYTE[cbBinary];
	CryptStringToBinaryA(strBase64.data(), uSize, CRYPT_STRING_BASE64, outBuffer, &cbBinary, &dwSkip, &dwFlags);
	sourceBuffer=(char*) outBuffer;
	bufLen=cbBinary;
	if (NULL!=sourceBuffer)
	{
		return true;
	}
	return false;
}

std::string CDatabaseService::GetMyLastError()
{
	return strErrorMsg;

}

//设置错误信息
void CDatabaseService::SetMyLastError( std::string strErrorMsg )
{
    this->strErrorMsg=strErrorMsg;
}

bool acl::CDatabaseService::CloseDB()
{
	if (pInterface&&pInterface->GetConnectState())
	{
		pInterface->Release();
		pInterface    =   NULL;
		return true;
	}
	return false;  
}

//根据名称获取对象
::google::protobuf::Message* CDatabaseService::GetObjectByType(const std::string s_typeName)
{
	if(""==s_typeName)
	{
		SetMyLastError("对象类型为空");
		return NULL;
	}
	const ::google::protobuf::Descriptor* descriptor=::google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(s_typeName);
	if(NULL!=descriptor)
	{
		const ::google::protobuf::Message* protoType=::google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		::google::protobuf::Message* object=protoType->New();
		if(NULL!=object)
		{
			SetMyLastError("创建对象实例失败");
			return object;
		}
	}
	return NULL;
}

//获取配置文件路径
void CDatabaseService::GetConfigFilePath(std::string strPrefix,std::string strFileName,std::string& strFilePath)
{
	char WorkPath[256]	=	{0};
	GetModuleFileName(NULL,WorkPath,256);
	std::string strFileFullPath	=	WorkPath;
	strFileFullPath	=	strFileFullPath.substr(0,strFileFullPath.find_last_of('\\'));
	strFileFullPath+="\\";
	strPrefix+="\\";
	strFilePath=strFileFullPath+strPrefix+strFileName;
}

//获取数据库配置
void CDatabaseService::GetDatabaseConfig( std::string& strService,std::string& strUser,std::string& strPassword )
{
	std::string fileName;
	GetConfigFilePath("ConfigFiles","Config.ini",fileName);
	char buf[64];

	GetPrivateProfileString("数据库配置", "服务名", "", buf, 256, fileName.c_str());
	strService = std::string(buf);
	GetPrivateProfileString("数据库配置", "用户名", "", buf, 256, fileName.c_str());
	strUser = std::string(buf);
	GetPrivateProfileString("数据库配置", "密码", "", buf, 256, fileName.c_str());
	strPassword = std::string(buf);
}

//获取例外表（主键与参考键不一致)
bool CDatabaseService::GetExceptionTable(std::string strTable,std::string strRefTable,std::string& strField,std::string& strRefField)
{
	std::string strSectionName=strTable+"-";
	strSectionName+=strRefTable;
	std::string fileName;
	GetConfigFilePath("ConfigFiles","Config.ini",fileName);
	char buf[64];

	GetPrivateProfileString(strSectionName.c_str(), "例外字段", "", buf, 256, fileName.c_str());
	strField = std::string(buf);
	GetPrivateProfileString(strSectionName.c_str(), "参考字段", "", buf, 256, fileName.c_str());
	strRefField = std::string(buf);

	if(strField==""||strRefField=="")
	{
		return false;
	}
	return true;
}

//执行SQL语句
int acl::CDatabaseService::SqlExec( const char* strSQL )
{
	if(pInterface)
	{
		return pInterface->SqlExec(strSQL);
	}
	return -1;
}

//直接执行SQL语句
int acl::CDatabaseService::SqlDirectExec( const char* pSQLL )
{
	if(pInterface)
	{
		return pInterface->SqlDirectExec(pSQLL);
	}
	return -1;

}

//插入SQL语句
int acl::CDatabaseService::SqlInsert( const char* strSQL )
{
	if(pInterface)
	{
		return pInterface->SqlInsert(strSQL);
	}
	return -1;

}

//更新SQL语句
int acl::CDatabaseService::SqlUpdate( const char* strSQL,std::string& strMsg )
{
	if(pInterface)
	{
		return pInterface->SqlUpdate(strSQL,strMsg);
	}
	return -1;

}

//更新BLOB字段
int acl::CDatabaseService::UpdateBlob( const char* pTable ,const char* szColum,const void* pData,int len,const char* szCondition )
{
	if(pInterface)
	{
		return pInterface->UpdateBlob(pTable,szColum,pData,len,szCondition);
	}
	return -1;

}

//删除SQL语句
int acl::CDatabaseService::SqlDelete( const char* strSQL )
{
	if(pInterface)
	{
		return pInterface->SqlDelete(strSQL);
	}
	return -1;

}

//查询SQL语句
int acl::CDatabaseService::SqlQuery( const char* strSQL,char** ppXML )
{
	if(pInterface)
	{
		return pInterface->SqlQuery(strSQL,ppXML);
	}
	return -1;
}

//提交事务
void acl::CDatabaseService::CommitTrans()
{
	if(pInterface)
	{
		pInterface->CommitTrans();
	}
}

//释放资源
void acl::CDatabaseService::ReleaseXML( const char* pXML )
{
	if(pInterface)
	{
		pInterface->ReleaseXML(pXML);
	}
}

//查看对象是否存在
bool acl::CDatabaseService::IsObjectExist( ::google::protobuf::Message& message,bool& isExist )
{
	std::string strQuerySql;
	if(!BuildQueryCountSQL(message,strQuerySql))
	{
		return false;
	}

	if(pInterface)
	{
		Query	query(pInterface);
		query.exeSQL(strQuerySql.c_str()); // 查询SQL语句不能加分号
		rapidxml::xml_node<>* root    =   query.first_node();
		rapidxml::xml_node<>* item    =   root->first_node();
		for ( ; item != 0 ; item = item->next_sibling())
		{
			int count=GET_INT(item,CXSL);
			if(count>0)
			{
				isExist=true;
			}
			else
			{
				isExist=false;
			}
			return true;
		}
	}
	isExist=false;
	return false;
}

//获取下一个编号
bool acl::CDatabaseService::GetNextNum( std::string strTableName,std::string strColumnName,int& iNum)
{
	char strQuerySql[64];
	sprintf_s(strQuerySql,64,"SELECT MAX(TO_NUMBER(%s)) as MAX_NUM FROM %s",strColumnName.c_str(),strTableName.c_str());
	if(pInterface)
	{
		Query	query(pInterface);
		query.exeSQL(strQuerySql); // 查询SQL语句不能加分号
		rapidxml::xml_node<>* root    =   query.first_node();
		rapidxml::xml_node<>* item    =   root->first_node();
		for ( ; item != 0 ; item = item->next_sibling())
		{
			iNum=GET_INT(item,MAX_NUM);
			return true;
		}
		iNum=0;
	}
	return false;
}
