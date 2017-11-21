package com.homvee.db.dao.impl;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.sql.DataSource;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.alibaba.druid.util.StringUtils;
import com.google.common.collect.Lists;
import com.google.protobuf.Descriptors;
import com.google.protobuf.Message;
import com.homvee.db.annotations.TableName;
import com.homvee.db.dao.Dao;

/**
 * Dao 基础类 主要实现一些基本的查询 修改 新增方法
 * @author chensr
 *
 */
public abstract class BaseDaoImpl implements Dao {
	protected static Logger LOGGER = null;

    public BaseDaoImpl() {
        LOGGER = LogManager.getLogger(BaseDaoImpl.class);
    }

    /**
     * 获取数据源
     * @return DataSource 数据源
     * @throws Exception
     */
    protected  abstract DataSource getDataSource() throws Exception;
    
    /**
     * 通过表面获取主键
     * @param tableName
     * @return
     */
    protected  abstract String getPrimaryKeyName(String tableName) ;
    
    /**
     * 获取表名
     * @param message
     * @return 表名
     */
    public static  String getTableName(Message message){
        if(message == null){
            LOGGER.info("message is null!");
            return  null;
        }
        String name = null;
        Class clazz =  message.getClass();
        if(clazz.isAnnotationPresent(TableName.class)){
            TableName tableName = (TableName) clazz.getAnnotation(TableName.class);
            name = tableName.name();
        }
        if(StringUtils.isEmpty(name)){
            name = clazz.getSimpleName();
        }
        return name;
    }


    /**
     * 修改
     * @param sql
     * @param params sql参数
     * @return 修改的行数，-1 表示错误
     */
    public int update(String sql , Object ... params){
        return  this.update(sql , Arrays.asList(params));
    }
    
    /**
     * 修改
     * @param sql
     * @param params sql参数
     * @return 修改的行数，-1 表示错误
     */
    public int update(String sql , List<?> params){
        return  this.saveOrUpdate(sql , params);
    }
    
    /**
     *  新增
     * @param sql
     * @param params sql参数
     * @return 新增的行数，-1 表示错误
     */
    public int save(String sql , List<?> params){
        return  this.saveOrUpdate(sql , params);
    }
    
    /**
     * 新增
     * @param sql
     * @param params sql参数
     * @return 新增的行数，-1 表示错误
     */
    public int save(String sql , Object ... params){
        return  this.save(sql , Arrays.asList(params));
    }
    
    /**
     * 统计记录数
     * @param sql
     * @param params sql参数
     * @return 查询的行数，-1 表示错误
     */
    public long count(String sql , List<?> params){
        List<Map<String , Object>> data = this.query(sql , params);
        if(data == null || data.size() < 1){
            return  0;
        }
        Map<String , Object> objectMap = data.get(0);
        List<String> keys = new ArrayList<String>(objectMap.keySet());
        return  (Long) objectMap.get(keys.get(0));
    }
    
    /**
     * 统计记录数
     * @param sql
     * @param params sql参数
     * @return 查询的行数，-1 表示错误
     */
    public long count(String sql , Object ... params){
        return  this.count(sql , Arrays.asList(params));
    }

    /**
     * @Document 执行数据库的修改操作
     * @param sql
     * @param params sql参数
     * @return 修改的行数，-1 表示错误
     */
    public int saveOrUpdate(String sql , List<?> params){
        Connection connection = null;
        PreparedStatement statement = null;
        try {
            DataSource dataSource = getDataSource();
            connection =  dataSource.getConnection();
            statement = connection.prepareStatement(sql);
            setParameters(statement , params);
            return  statement.executeUpdate();
        } catch (Exception e) {
            LOGGER.error("sql={},params={} 执行异常", sql , params, e);
        }finally {
           close(connection  ,statement , null);
        }

        return  -1;
    }
    
    /**
     * 删除数据
     * @param sql
     * @param params
     * @return 修改的行数，-1 表示错误
     */
    public int delete(String sql , List<?> params) {
    	 return this.save(sql ,params);
    }

    /**
     * @Document 查询数据库
     * @param sql
     * @param params
     * @return 查询结果
     */
    public List<Map<String, Object>> query(String sql , List<?> params){
        Connection connection = null;
        PreparedStatement statement = null;
        ResultSet resultSet = null;
        try {
            DataSource dataSource = getDataSource();
            connection =  dataSource.getConnection();
            statement = connection.prepareStatement(sql);
            setParameters(statement , params);
            resultSet = statement.executeQuery();
            ResultSetMetaData metaData = resultSet.getMetaData();
            int cols = metaData.getColumnCount();
            List<Map<String,Object>> retData = Lists.newArrayList();
            while (resultSet.next()) {
                Map<String, Object> map = new HashMap<String, Object>();
                for (int i = 0; i < cols; i++) {
                    String colName = metaData.getColumnName(i + 1);
                    Object colVal = resultSet.getObject(colName);
                    map.put(colName, colVal);
                }
                retData.add(map);
            }
            return retData;

        } catch (Exception e) {
            LOGGER.error("sql={},params={} 执行异常", sql , params, e);
        }finally {
           close(connection  ,statement , resultSet);
        }

        return  null;
    }



    /**
     * 关闭数据连接
     * @param connection
     * @param statement
     * @param resultSet
     */
    public void close(Connection connection , Statement  statement , ResultSet resultSet){
        if(resultSet != null){
            try {
                resultSet.close();
            } catch (SQLException e) {
                LOGGER.error("执行resultSet.close异常", e);
            }
        }
        if(statement != null){
            try {
                statement.close();
            } catch (SQLException e) {
                LOGGER.error("执行statement.close异常", e);
            }
        }
        if(connection != null){
            try {

                connection.close();
            } catch (SQLException e) {
                LOGGER.error("执行connection.close异常", e);
            }
        }
    }

    /**
     * 设置参数
     * @param statement 
     * @param params params
     * @throws SQLException
     */
    protected void setParameters(PreparedStatement statement , List<?> params) throws SQLException {
        if(params == null || params.size() < 1){
            return;
        }
        for (int i = 0 , index = 1; i < params.size(); i++) {
            statement.setObject(index++, params.get(i));
        }
    }
    
    /**
     * 检查message 对象合法性
     * @param msg 
     * @return true 合法  false 不合法
     */
    protected boolean checkMsg(Message msg){
        if (msg == null){
            LOGGER.info("message is null");
            return false;
        }
        Map<Descriptors.FieldDescriptor, Object> filedMap = msg.getAllFields();
        if(filedMap == null || filedMap.size() < 1){
            LOGGER.info("filedMap is null");
            return false;
        }

        return true;
    }
}
