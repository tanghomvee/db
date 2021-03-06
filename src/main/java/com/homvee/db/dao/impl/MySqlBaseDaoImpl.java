package com.homvee.db.dao.impl;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.sql.DataSource;

import com.alibaba.druid.util.StringUtils;
import com.google.common.collect.Lists;
import com.google.protobuf.DescriptorProtos;
import com.google.protobuf.Descriptors;
import com.google.protobuf.Message;
import com.homvee.db.MessageUtil;
import com.homvee.db.ds.DataSourceUtil;
import com.homvee.db.enums.DBType;

/**
 * mysql dao 实现
 * @author chensr
 *
 */
public  class MySqlBaseDaoImpl extends BaseDaoImpl {


    @Override
   public DataSource getDataSource() throws Exception {
        return DataSourceUtil.getDataSource(DBType.MYSQL);
    }

    @Override
    public String getPrimaryKeyName(String tableName) {
        StringBuffer sql = new StringBuffer("SELECT a.TABLE_SCHEMA, a.TABLE_NAME,a.COLUMN_NAME,a.CONSTRAINT_NAME, a.REFERENCED_TABLE_NAME,a.REFERENCED_COLUMN_NAME FROM " +
                "information_schema.KEY_COLUMN_USAGE a " +
                "WHERE a.TABLE_NAME =?  AND a.TABLE_SCHEMA NOT IN ('sys' ,'mysql' , 'information_schema') AND CONSTRAINT_NAME='PRIMARY'");
        List<Map<String , Object>> data = super.query(sql.toString() , Lists.newArrayList(tableName));
        return (String) data.get(0).get("COLUMN_NAME");
    }
    
    /**
     * 根据主键查询数据
     * @param message
     * @return
     */
    public Message getByPrimaryKey(Message message) {
        String tableName = getTableName(message);
        String keyName = getPrimaryKeyName(tableName);
        StringBuffer sql = new StringBuffer("select * from " + tableName + " where "+keyName+" = ?");
        List<Object> params = new ArrayList<Object>();
        params.add(MessageUtil.getValueByColName(message, keyName));
        List<Message> retData = query(sql.toString(), params, message.toBuilder());
        if(null != retData && retData.size() != 0) {
        	return retData.get(0);
        }
		return null;
    }
    
    
    /**
     * 根据主键查询数据
     * @param message
     * @return
     */
    public int delete(Message message) {
        String tableName = getTableName(message);
        StringBuffer sql = new StringBuffer("DELETE from " + tableName +" where 1=1 ");
        List<Object> params = Lists.newArrayList();
        Map<Descriptors.FieldDescriptor, Object> filedMap = message.getAllFields();
        for (Descriptors.FieldDescriptor descriptor : filedMap.keySet()){
            Descriptors.FieldDescriptor.Type type = descriptor.getType();
            if(DescriptorProtos.FieldDescriptorProto.Type.TYPE_MESSAGE.equals(type) || descriptor.isRepeated() || descriptor.isMapField()){
                    continue;
            }
            Object val = filedMap.get(descriptor);
            if (val == null){
                continue;
            }
            String colName = descriptor.getName();
            sql.append(" AND ").append(colName + "= ?");
            params.add(val);
        }
		return  super.delete(sql.toString() , params);
    }
    

    /**
     * 更新数据
     * @param message 保存对象
     */
    public int update(Message message) {
        if (!checkMsg(message)){
            return -1;
        }

        Map<Descriptors.FieldDescriptor, Object> filedMap = message.getAllFields();
        String tableName = getTableName(message);
        StringBuffer sql = new StringBuffer("update " + tableName + " set ");
        String keyName = getPrimaryKeyName(tableName);
        Object keyVal = null;
        List<Object> params = Lists.newArrayList();
        for (Descriptors.FieldDescriptor descriptor : filedMap.keySet()){
            Descriptors.FieldDescriptor.Type type = descriptor.getType();
            if(DescriptorProtos.FieldDescriptorProto.Type.TYPE_MESSAGE.equals(type) ||
                    descriptor.isRepeated() || descriptor.isMapField()){
                LOGGER.info("data-Type={} not support update" , type);
                continue;
            }
            Object colVal = filedMap.get(descriptor);
            String colName = descriptor.getName();
            if (keyName.equals(colName)){
                keyVal = colVal;
                continue;
            }
            sql.append(colName + " = ? ,");
            params.add(colVal);
        }
        if(StringUtils.isEmpty(keyName) || keyVal == null){
            LOGGER.info("primary Key-Name or Key-Value 不存在");
            return -1;
        }
        sql = sql.deleteCharAt(sql.lastIndexOf(","));
        sql.append(" WHERE ").append(keyName).append(" = ?");
        params.add(keyVal);

        return super.update(sql.toString() , params);
    }

    /**
     * 保存数据
     * @param message 保存对象
     */
    public int save(Message message) {
        if (!checkMsg(message)){
            LOGGER.info("message is error");
            return -1;
        }
        Map<Descriptors.FieldDescriptor, Object> filedMap = message.getAllFields();
        String tableName = getTableName(message);
        StringBuffer sql = new StringBuffer("insert into " + tableName );
        StringBuffer clos = new StringBuffer("(");
        StringBuffer cloVals = new StringBuffer(" values (");
        List<Object> params = Lists.newArrayList();
        for (Descriptors.FieldDescriptor descriptor : filedMap.keySet()){
            Descriptors.FieldDescriptor.Type type = descriptor.getType();
            if(DescriptorProtos.FieldDescriptorProto.Type.TYPE_MESSAGE.equals(type) ||
                   descriptor.isRepeated() || descriptor.isMapField()){
                LOGGER.info("data-Type={} not support save" , type);
                continue;
            }
            Object colVal = filedMap.get(descriptor);
            String colName = descriptor.getName();
            clos.append(colName).append(",");
            cloVals.append("?,");
            params.add(colVal);
        }
        clos = clos.deleteCharAt(clos.lastIndexOf(","));
        clos.append(")");
        cloVals = cloVals.deleteCharAt(cloVals.lastIndexOf(","));
        cloVals.append(")");
        sql.append(clos).append(cloVals);
        System.out.println(sql);
        return super.save(sql.toString() , params);
    }



    /**
     * 通过sql查询
     * @param sql 查询语句
     * @param params 查询参数
     * @param builder Message
     */
    public List< Message> query(String sql, List<?> params, Message.Builder builder) {
        List<Map<String,Object>> data = query(sql , params);
        if(data == null || data.size() < 1){
            return null;
        }
        List<Message> retData = Lists.newArrayList();

        String tableName = getTableName(builder.buildPartial());
        String keyName = getPrimaryKeyName(tableName);

        for (Map<String , Object> tmpMap : data){
            List<Descriptors.FieldDescriptor> fieldDescriptors =  builder.getDescriptorForType().getFields();
            for (Descriptors.FieldDescriptor descriptor : fieldDescriptors){
                Descriptors.FieldDescriptor.Type type = descriptor.getType();
                String colName = descriptor.getName();
                Object val = tmpMap.get(colName);
                if(DescriptorProtos.FieldDescriptorProto.Type.TYPE_MESSAGE.equals(type) || descriptor.isRepeated() || descriptor.isMapField()){
                    //TODO 查询关联的对象
                    if (DescriptorProtos.FieldDescriptorProto.Type.TYPE_MESSAGE.equals(type)){

                    } else if (descriptor.isRepeated()){
                        val = new ArrayList<Message>();

                    }else {
                        continue;
                    }
                }
                builder.setField(descriptor , val);

            }
            retData.add(builder.build());
        }
        return retData;
    }

    /**
     * 通过message查询
     * @param queryMsg
     * @param builder
     */
    public List< Message> query(Message queryMsg,  Message.Builder builder){
        List<Message> retData = Lists.newArrayList();
        if (!checkMsg(queryMsg)){
            LOGGER.info("query message is error");
            return retData;
        }
        String tableName = getTableName(builder.buildPartial());
        List<Object> params = Lists.newArrayList();
        StringBuffer sql = new StringBuffer("select * from ");
        sql.append(tableName).append(" where 1=1 ");
        Map<Descriptors.FieldDescriptor, Object> filedMap = queryMsg.getAllFields();
        for (Descriptors.FieldDescriptor descriptor : filedMap.keySet()){
            Descriptors.FieldDescriptor.Type type = descriptor.getType();
            if(DescriptorProtos.FieldDescriptorProto.Type.TYPE_MESSAGE.equals(type) || descriptor.isRepeated() || descriptor.isMapField()){
                    continue;
            }
            Object val = filedMap.get(descriptor);
            if (val == null){
                continue;
            }
            String colName = descriptor.getName();

            sql.append(" AND ").append(colName + "= ?");
            params.add(val);
        }

        return query(sql.toString() , params ,builder);
    }
}
