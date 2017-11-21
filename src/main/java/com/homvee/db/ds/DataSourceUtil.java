package com.homvee.db.ds;

import com.alibaba.druid.pool.DruidDataSourceFactory;
import com.alibaba.druid.util.StringUtils;
import com.homvee.db.enums.DBType;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.sql.DataSource;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;


//
/**
 * The Class DataSourceUtil.
 */
public class DataSourceUtil {
    protected static Logger LOGGER = LogManager.getLogger(DataSourceUtil.class);

    private static DataSource mySqlDataSource;
    private static DataSource oracleDataSource;
    private static Properties mySqlProp;
    private static Properties oracleProp;

    static {
        String mysqlCfg = DBType.MYSQL.getDbConfigFilePath();
        if(StringUtils.isEmpty(mysqlCfg)) {
            mysqlCfg =  DBType.MYSQL.name().toLowerCase()+".properties";
        }
        mySqlProp = getProps(mysqlCfg);

        String oracleCfg = DBType.ORACLE.getDbConfigFilePath();
        if(StringUtils.isEmpty(oracleCfg)) {
            oracleCfg =  DBType.ORACLE.name().toLowerCase()+".properties";
        }
        oracleProp = getProps(oracleCfg);
    }

    public static final DataSource getDataSource(DBType dbType) throws Exception {
        switch (dbType){
            case MYSQL:
                if (mySqlDataSource == null) {
                    if(mySqlProp == null){
                        throw new Exception("MySQL DataSource Properties is null");
                    }
                    synchronized (DBType.MYSQL) {
                        if (mySqlDataSource == null) {
                            mySqlDataSource = DruidDataSourceFactory.createDataSource(mySqlProp);
                        }
                    }
                }
                return  mySqlDataSource;
            case ORACLE:
                if (oracleDataSource == null) {
                    if(oracleProp == null){
                        throw new Exception("Oracle DataSource Properties is null");
                    }
                    synchronized (DBType.ORACLE) {
                        if (oracleDataSource == null) {
                            oracleDataSource = DruidDataSourceFactory.createDataSource(oracleProp);
                        }
                    }
                }
                return  oracleDataSource;
            default:
                throw new Exception(dbType + " not support");
        }

    }

    public static Properties getMySqlProp() {
        return mySqlProp;
    }

    public static Properties getOracleProp() {
        return oracleProp;
    }

    private static Properties  getProps(String filePath){
        InputStream in = DataSourceUtil.class.getClassLoader().getResourceAsStream(filePath);
        if (in == null){
            LOGGER.error("db config file[{}] not found" , DataSourceUtil.class.getClassLoader().getResource(filePath));
            return  null;
        }
        Properties properties = new Properties();
        try {
            properties.load(in);
        } catch (IOException e) {
            LOGGER.error("load db config file[{}] error" , DataSourceUtil.class.getClassLoader().getResource(filePath) ,e);
        }
        return properties;
    }
}

