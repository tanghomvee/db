package com.homvee.db;

import java.util.Map;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.alibaba.druid.util.StringUtils;
import com.google.protobuf.DescriptorProtos;
import com.google.protobuf.Descriptors;
import com.google.protobuf.Message;
import com.homvee.db.annotations.TableName;
import com.homvee.db.dao.impl.BaseDaoImpl;

public class MessageUtil {
	
	protected static Logger LOGGER = LogManager.getLogger(BaseDaoImpl.class);
	
	public static Object getValueByColName(Message message,String colName) {
		 Map<Descriptors.FieldDescriptor, Object> filedMap = message.getAllFields();
		 for (Descriptors.FieldDescriptor descriptor : filedMap.keySet()){
	            Descriptors.FieldDescriptor.Type type = descriptor.getType();
	            if(DescriptorProtos.FieldDescriptorProto.Type.TYPE_MESSAGE.equals(type) ||
	                    descriptor.isRepeated() || descriptor.isMapField()){
	                LOGGER.info("data-Type={} not support update" , type);
	                continue;
	            }
	            String colNameTemp = descriptor.getName();
	            if (colName.equals(colNameTemp)){
	            	return filedMap.get(descriptor);
	            }
	            
	        }
		return null;
	}
	
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
}
