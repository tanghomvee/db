package com.homvee.db.dao.impl;

import com.alibaba.druid.util.StringUtils;
import com.google.common.collect.Lists;
import com.google.protobuf.Descriptors;
import com.google.protobuf.Message;
import com.homvee.db.annotations.TableName;
import com.homvee.db.dao.Dao;

import javax.sql.DataSource;
import java.sql.*;
import java.util.*;

public abstract class BaseDaoImpl implements Dao {

    protected  abstract DataSource getDataSource() throws Exception;
    protected  abstract String getPrimaryKeyName(String tableName) ;
    protected static  String getTableName(Message message){
        if(message == null){
            //TODO 日志
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


    public int update(String sql , Object ... params){
        return  this.update(sql , Arrays.asList(params));
    }
    public int update(String sql , List<?> params){
        return  this.saveOrUpdate(sql , params);
    }
    public int save(String sql , List<?> params){
        return  this.saveOrUpdate(sql , params);
    }
    public int save(String sql , Object ... params){
        return  this.save(sql , Arrays.asList(params));
    }
    public int count(String sql , List<?> params){
        List<Map<String , Object>> data = this.query(sql , params);
        if(data == null || data.size() < 1){
            return  0;
        }
        Map<String , Object> objectMap = data.get(0);
        List<String> keys = new ArrayList<String>(objectMap.keySet());
        return  (Integer) objectMap.get(keys.get(0));
    }
    public int count(String sql , Object ... params){
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
            //TODO
            e.printStackTrace();
        }finally {
           close(connection  ,statement , null);
        }

        return  -1;
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
            //TODO
            e.printStackTrace();
        }finally {
           close(connection  ,statement , resultSet);
        }

        return  null;
    }




    public void close(Connection connection , Statement  statement , ResultSet resultSet){
        if(resultSet != null){
            try {
                resultSet.close();
            } catch (SQLException e) {
                //TODO 日志
                e.printStackTrace();
            }
        }
        if(statement != null){
            try {
                statement.close();
            } catch (SQLException e) {
                //TODO 日志
                e.printStackTrace();
            }
        }
        if(connection != null){
            try {

                connection.close();
            } catch (SQLException e) {
                //TODO 日志
                e.printStackTrace();
            }
        }
    }

    protected void setParameters(PreparedStatement statement , List<?> params) throws SQLException {
        if(params == null || params.size() < 1){
            return;
        }
        for (int i = 0 , index = 1; i < params.size(); i++) {
            statement.setObject(index++, params.get(i));
        }
    }
    protected boolean checkMsg(Message msg){
        if (msg == null){
            //TODO 日志
            return false;
        }
        Map<Descriptors.FieldDescriptor, Object> filedMap = msg.getAllFields();
        if(filedMap == null || filedMap.size() < 1){
            //TODO 日志
            return false;
        }

        return true;
    }
}
