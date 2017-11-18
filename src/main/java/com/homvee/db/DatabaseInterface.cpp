// DatabaseLogic.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include <algorithm>
#include <string>
#include "DatabaseInterface.h"


// CDatabaseInterface 构造

CDatabaseInterface::CDatabaseInterface(IConvertObserver* observer)
{
	m_observer  =   observer;
}


//抛出异常
void CDatabaseInterface::ThrowException(otl_exception& p)
{

	static char chException[3501] =  {0};
	memset(chException, 0, EXCEPTION_STRING_MAX_LEN);
	sprintf
		(
		chException,
		"@@@@@Database Exception!!@@@@@@ Code = %d ; Msg = %s ; Infor = %s ;Stat = %s ;Text = %s \r\n",
		p.code,
		p.msg,
		p.var_info,
		p.sqlstate,
		p.stm_text
		);
	throw chException;
}

/*************************************************************************
* 函数名: ConnectDatabase
* 功 能: 连接数据库
* 参 数: strDNS:数据库
*        strUser:用户名称
*        strPWD:口令
* 返回值: true:成功,false:失败
*************************************************************************/
bool CDatabaseInterface::ConnectDatabase(const char* strDNS,const char* strUser,const char* strPWD)
{
	if (strDNS == 0 || strUser == 0 ||  strPWD == 0)
	{
		return false;
	}

	char   szBuf[1024] =   {0};
	sprintf(szBuf,"%s/%s@%s",strUser,strPWD,strDNS);
	try
	{
		m_db.rlogon(szBuf);
		m_db.auto_commit_on();
	}
	catch (otl_exception& p)
	{
		ThrowException(p);
		return false;
	}
	return  true;
}
/*************************************************************************
* 函数名: ConnectSQLITEDatabase
* 功 能: 连接SQLITE数据库
* 参 数: strDNS:数据库
* 返回值: true:成功,false:失败
*************************************************************************/
bool CDatabaseInterface::ConnectSQLITEDatabase(const char* strDNS)
{
	if (strDNS == 0)
	{
		return false;
	}

	char   szBuf[1024] =   {0};
	sprintf(szBuf,"%s%s","DSN=",strDNS);
	try
	{
		m_db.rlogon(szBuf);
	}
	catch (otl_exception& p)
	{
		  ThrowException(p);
	}
	return  true;
}
/*************************************************************************
* 函数名: CloseConDatabase
* 功 能: 断开与数据库的连接
* 参 数: 
* 返回值: 
*************************************************************************/
void CDatabaseInterface::CloseConDatabase()
{
	try
	{
		m_db.logoff();
	}
	catch(otl_exception& p)
	{
		ThrowException(p);
	}
}

/*************************************************************************
* 函数名: BeginTrans
* 功 能: 开始事务
* 参 数: 
* 返回值:
*************************************************************************/
void  CDatabaseInterface::BeginTrans()
{
	m_db.auto_commit_off();
}

/*************************************************************************
* 函数名: CommitTrans
* 功 能: 提交事务
* 参 数: 
* 返回值:
*************************************************************************/
void CDatabaseInterface::CommitTrans()
{
	try
	{
		m_db.commit();
	}
	catch(otl_exception& p)
	{
		ThrowException(p);
	}
}
/*************************************************************************
* 函数名: RollbackTrans
* 功 能: 回滚事务
* 参 数: 
* 返回值:
*************************************************************************/
void CDatabaseInterface::RollbackTrans()
{
	try
	{
		m_db.rollback();
	}
	catch(otl_exception& p)
	{
		 ThrowException(p);
	}
}
/*************************************************************************
* 函数名: SqlExec
* 功 能: 执行SQL
* 参 数: strSQL：SQL语句
* 返回值: < 0 :有错,>= 0:影响记录条数
*************************************************************************/
int CDatabaseInterface::SqlExec(const char* strSQL)
{
	int ret = -1;
	try
	{
		ret = otl_cursor::direct_exec(m_db,strSQL,otl_exception::enabled);
		ret = ret<0?0:ret;
	}
	catch(otl_exception& p)
	{
		ThrowException(p);
	}
	return   ret;
}

int     CDatabaseInterface::SqlDirectExec(const char* pSQLL)
{
	int ret = -1;
	try
	{
		ret = m_db.direct_exec(pSQLL,otl_exception::enabled);
	}
	catch(otl_exception& p)
	{
		ThrowException(p);
	}
	return   ret;
}

/*************************************************************************
* 函数名: SqlInsert
* 功 能: 插入数据
* 参 数: strSQL：插入数据SQL
* 返回值: < 0 :有错,>= 0:影响记录条数
*************************************************************************/
int CDatabaseInterface::SqlInsert(const char*  strSQL)
{
	int ret = -1;
	try
	{
		ret = otl_cursor::direct_exec(m_db,strSQL,otl_exception::enabled);
	}
	catch(...)
	{
		//ThrowException(p);
	}
	return   ret;
}
/*************************************************************************
* 函数名: SqlUpdate
* 功 能: 更新数据
* 参 数: strSQL：更新数据SQL
* 返回值: < 0 :有错,>= 0:影响记录条数
*************************************************************************/
int CDatabaseInterface::SqlUpdate(const char*  strSQL,std::string& strMsg)
{
	int ret = -1;
	try
	{
		ret = otl_cursor::direct_exec(m_db,strSQL,otl_exception::enabled);
	}
	catch(otl_exception& p)
	{
		char* strErrorMsg=(char*)p.msg;
		strMsg=strErrorMsg;
	}

	return   ret;
}

/**
*   更新二进制字段值
*   UpdateBlob(
*/
int    CDatabaseInterface::UpdateBlob
( 
 const char* pTable ,
 const char* szColum,
 const void* pData,
 int len,
 const char* szCondition
 )
{
	char    szBuf[1024] = {0};
	sprintf(szBuf,"update %s  set %s = :%s<raw_long> where %s",pTable,szColum,szColum,szCondition);

	try
	{
		otl_stream  otlStream(1,szBuf, m_db);//大文件提示内存不足（30MB以上）
		otlStream.set_commit(0);
		otl_long_string data(len);
		memcpy(data.v,pData,len);
		data.set_len(len);

		otlStream<<data;
		otlStream.close();
		return otlStream.get_rpc();
	}
	catch (otl_exception& p)
	{
		ThrowException(p);
	}
	return  -1;
}

/*************************************************************************
* 函数名: SqlDelete
* 功 能: 删除数据
* 参 数: strSQL：删除数据SQL
* 返回值:< 0 :有错,>= 0:影响记录条数
*************************************************************************/
int CDatabaseInterface::SqlDelete(const char*  strSQL)
{
	int ret = -1;
	try
	{
		ret = otl_cursor::direct_exec(m_db,strSQL,otl_exception::enabled);
	}
	catch (...)
	{
		//ThrowException(p);
	}
	return ret;
}


void  toString(std::string& strResult)
{
	std::string::iterator itr = strResult.begin() ;
	for (
		;itr != strResult.end() ; 
		)
	{
		std::string     strData;;
		if (* itr == '\"')
		{
			size_t  nPos    =   itr - strResult.begin();
			strData = "&quot;";
			strResult.replace(itr,itr + 1,strData.c_str(),strData.size());
			itr =   strResult.begin() + (nPos) + strData.size();
		}
		else if (* itr == '\'')
		{
			size_t  nPos    =   itr - strResult.begin();
			strData =   "&apos;";
			strResult.replace(itr,itr + 1,strData.c_str(),strData.size());
			itr =   strResult.begin() + (nPos) + strData.size();
		}
		else if(* itr == '>')
		{
			size_t  nPos    =   itr - strResult.begin();
			strData =   "&gt;";
			strResult.replace(itr,itr + 1,strData.c_str(),strData.size());

			itr =   strResult.begin() + (nPos) + strData.size();
		}
		else if(* itr == '<')
		{
			size_t  nPos    =   itr - strResult.begin();
			strData =   "&lt;";
			strResult.replace(itr,itr + 1,strData.c_str(),strData.size());
			itr =   strResult.begin() + (nPos) + strData.size();
		}
		else if(* itr == '&')
		{
			size_t  nPos    =   itr - strResult.begin();
			strData =   "&amp;";
			strResult.replace(itr,itr + 1,strData.c_str(),strData.size());
			itr =   strResult.begin() + (nPos) + strData.size();
		}
		else
		{
			++ itr;
		}
	}
}

/*************************************************************************
* 函数名: SqlQuery
* 功 能: 查询数据(默认将所有查询结果字段名称转换为大写)
* 参 数: strSQL：查询数据SQL
*        strData：返回查询结果xml 
* 返回值:< 0 :有错,>= 0:记录条数
*************************************************************************/
int CDatabaseInterface::SqlQuery(const char*  strSQL,char** ppRet)
{
	otl_stream*      sqlStream = 0;
	try
	{
		sqlStream = new otl_stream(1, strSQL, m_db);              //第一个参数什么含义，到底写几
	}
	catch(...)
	{
		delete sqlStream;
		sqlStream = NULL;
		return  -1;
	}


	static  std::string     strResult;
	strResult.reserve(1024 * 1024 * 16);
	strResult.clear();
	strResult   +=  "<root>\n";

	int                 nRecCnt =   0;

	otl_column_desc*    colDes  =   0;
	int                 colNum  =   0;

	try
	{
		colDes  =  sqlStream->describe_select(colNum);
	}
	catch (...)
	{
		delete sqlStream;
		sqlStream = NULL;
		return -1;
	}

	while(!sqlStream->eof())
	{
		++nRecCnt;
		/**
		*   临时缓冲区 
		*   保存一行数据
		*/
		char    szBuf[1024 * 16]    =   {"<item "};
		int     iLength             =   strlen(szBuf);
		for ( int i = 0 ;i < colNum ; ++ i )
		{
			char            szCol[1024] =   {0};
			double          dData       =   0;
			float           fData       =   0;
			int             iData       =   0;
			long int        lData       =   0;
			short           sData       =   0;
			unsigned short  usData      =   0;
			unsigned int    uiData      =   0;

			switch(colDes[i].otl_var_dbtype)
			{
			case otl_var_char:
				{
					char        szOut[2048];
					unsigned    outLength   =   0;

					*sqlStream>>szCol;
					if (m_observer)
					{
						m_observer->convert(szCol,strlen(szCol),szOut,outLength);
						std::string strData = szOut;
						toString(strData) ;
						std::string strName =   std::string(colDes[i].name);
						transform(strName.begin(),strName.end(),strName.begin(),::toupper);
						sprintf(szBuf + iLength," %s =\"%s\"",strName.c_str(),strData.c_str());
						iLength +=   strlen(szBuf + iLength);
					}
					else
					{
						std::string strData = szCol;
						toString(strData) ;
						std::string strName =   std::string(colDes[i].name);
						transform(strName.begin(),strName.end(),strName.begin(),::toupper);
						sprintf(szBuf + iLength," %s =\"%s\"",strName.c_str(),strData.c_str());
						iLength +=   strlen(szBuf + iLength);
					}

				}
				break;
			case otl_var_double:
				{
					*sqlStream>>dData;
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					if (colDes[i].scale ==  0)  //判断是否为整数
					{
						sprintf(szBuf + iLength," %s =\"%d\"",strName.c_str(),(int)dData);

					}
					else
					{
						sprintf(szBuf + iLength," %s =\"%f\"",strName.c_str(),(float)dData);

					}
					iLength +=   strlen(szBuf + iLength);
				}
				break;
			case otl_var_float:
				{
					*sqlStream>>fData;
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					sprintf(szBuf + iLength,"%s =\"%f\"",strName.c_str(),fData);
					iLength +=   strlen(szBuf + iLength);
				}
				break;
			case otl_var_int:
				{
					*sqlStream>>iData;
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					sprintf(szBuf + iLength," %s =\"%d\"",strName.c_str(),iData);
					iLength +=   strlen(szBuf + iLength);
				}
				break;
			case otl_var_unsigned_int:
				{
					*sqlStream>>uiData;
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					sprintf(szBuf + iLength," %s =\"%d\"",strName.c_str(),uiData);
					iLength +=   strlen(szBuf + iLength);
				}
				break;
			case otl_var_short:
				{
					*sqlStream>>sData;
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					sprintf(szBuf + iLength," %s =\"%d\"",strName.c_str(),(int)sData);
					iLength +=   strlen(szBuf + iLength);
				}
				break;
			case otl_var_long_int:
				{
					*sqlStream>>lData;
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					sprintf(szBuf + iLength," %s =\"%d\"",strName.c_str(),(int)lData);
					iLength +=   strlen(szBuf + iLength);
				}
				break;
			case otl_var_timestamp:
				{
					otl_datetime    dateTime;
					*sqlStream>>dateTime;
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					char  szTime[128];
					if (sqlStream->is_null())
					{
						memset(szTime,0,128);
					}
					else
					{
						sprintf(szTime,"%d/%d/%d %d:%d:%d",
							dateTime.year,
							dateTime.month,
							dateTime.day,
							dateTime.hour,
							dateTime.minute,
							dateTime.second);
					}
					sprintf(szBuf + iLength," %s =\"%s\"",strName.c_str(),szTime);
					iLength +=   strlen(szBuf + iLength);
				}
				break;
			case otl_var_varchar_long:
				{
				}
				break;
			case otl_var_raw_long:
				{
					otl_long_string strBuff(1);      //此处要修改
					*sqlStream>>strBuff;
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					int nLength = strBuff.len();
					char *pData = NULL;
					pData = new char[nLength];
					if(NULL != pData)
					{
						memcpy(pData, strBuff.v, nLength);

						char    szTemp[128] = {0};
						sprintf(szTemp,"%d %d", nLength, size_t(pData));
						sprintf(szBuf + iLength," %s =\"%s\"",strName.c_str(),szTemp);
						iLength +=   strlen(szBuf + iLength);
					}
					else
					{
						return -1;
					}

				}
				break;
			case otl_var_clob:
			case otl_var_blob:
				{ 
					otl_lob_stream   lob;
					*sqlStream>>lob;
					char*   pData   =   0;

					int     nBlob   =   0;
					while(!lob.eof())
					{ 
						otl_long_string buffer(1024*1024*16);
						lob>>   buffer;
						int     nLength =   buffer.len();
						if (nBlob == 0)
						{
							pData   =   new char[nLength];
							memcpy(pData,buffer.v,nLength);
						}
						else
						{
							char*pNew   =   new char[nBlob + nLength];
							memcpy(pNew,pData,nBlob);
							memcpy(pNew + nBlob,buffer.v,nLength);
							delete  []pData;
							pData   =   pNew;
						}
						nBlob   +=  nLength;
					}
					char    szTemp[128];
					sprintf(szTemp,"%d %d",nBlob,size_t(pData));
					std::string strName =   std::string(colDes[i].name);
					transform(strName.begin(),strName.end(),strName.begin(),::toupper);
					sprintf(szBuf + iLength," %s =\"%s\"",strName.c_str(),szTemp);
					iLength +=   strlen(szBuf + iLength);
				}
				break;
			case otl_var_refcur:
				{
				}
				break;
			case otl_var_long_string:
				{
				}
				break;

			case otl_var_db2time:
				{
				}
				break;
			case otl_var_db2date:
				{
				}
				break;
			case otl_var_tz_timestamp:
				{
				}
				break;
			case otl_var_ltz_timestamp:
				{
				}
				break;
			case otl_var_bigint:
				{
				}
				break;
			}
		}
		strcpy(szBuf + iLength,"/>");
		strResult   +=  szBuf;
	}


	strResult   +=  "</root>";

	size_t      nSize   =   strResult.size();
	char*       pRet    =   new char[nSize + 1];
	pRet[nSize] =   0;
	memcpy(pRet,strResult.c_str(),nSize);
	*ppRet  =  pRet;

	if(0 != sqlStream)
	{
		delete sqlStream;
		sqlStream = NULL;
	}
	return  nRecCnt;


}

void   CDatabaseInterface::ReleaseXML(const char* pXML)
{
	if(NULL != pXML)
	{
		delete  []pXML;
		pXML = NULL;
	}
}

/**
*   释放blob字段
*/
void    CDatabaseInterface::ReleaseBlob(void* pBLOB)
{
	if(NULL != pBLOB)
	{
		delete  []pBLOB;
		pBLOB = NULL;
	}

}

void   CDatabaseInterface::Release()
{
	delete  this;
}

bool CDatabaseInterface::GetConnectState()
{
	if (0   ==  m_db.connected)
	{
		return false;
	}
	else
	{
		return  true;
	}

}

CDatabaseInterface* CDatabaseInterface::createDbInterface( IConvertObserver* observer )
{
    return  new CDatabaseInterface(observer);
}
