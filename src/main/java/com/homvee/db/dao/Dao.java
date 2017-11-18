package com.homvee.db.dao;


import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
/**
 * Dao 基础类 主要实现一些基本的查询 修改 新增方法
 * @author chensr
 *
 */
public interface Dao {
	
	/**
     * 修改
     * @param sql
     * @param params sql参数
     * @return 修改的行数，-1 表示错误
     */
    int update(String sql , Object ... params);
    
    /**
     * 修改
     * @param sql
     * @param params sql参数
     * @return 修改的行数，-1 表示错误
     */
    int update(String sql , List<?> params);
    
    /**
     *  新增
     * @param sql
     * @param params sql参数
     * @return 新增的行数，-1 表示错误
     */
    int save(String sql , List<?> params);
    
    /**
     * 新增
     * @param sql
     * @param params sql参数
     * @return 新增的行数，-1 表示错误
     */
    int save(String sql , Object ... params);
    
    /**
     * 统计记录数
     * @param sql
     * @param params sql参数
     * @return 查询的行数，-1 表示错误
     */
    long count(String sql , List<?> params);
    
    /**
     * 统计记录数
     * @param sql
     * @param params sql参数
     * @return 查询的行数，-1 表示错误
     */
    long count(String sql , Object ... params);
    
    /**
     * @Document 查询数据库
     * @param sql
     * @param params
     * @return 查询结果
     */
    List<Map<String, Object>> query(String sql , List<?> params);

    /**
     * 通过message 修改对象
     * @param message
     * @return 更新成功数
     */
    int update(Message message);
    
    /**
     * 通过message 保存对象 
     * @param message
     * @return 保存成功数
     */
    int save(Message message);
    
    /**
     * 通过sql查询
     * @param sql 查询语句
     * @param params 查询参数
     * @param builder Message
     * @return 查询结果集
     */
    List<Message> query(String sql , List<?> params , Message.Builder builder);
    
    /**
     * 通过message查询
     * @param queryMsg
     * @param builder
     * @return 查询结果集
     */
    List< Message> query(Message queryMsg,  Message.Builder builder);
}
