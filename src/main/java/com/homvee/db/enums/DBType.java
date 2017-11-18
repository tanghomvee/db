package com.homvee.db.enums;

public enum DBType {
    MYSQL("mysql.properties"),ORACLE("oracle.properties");
    private String dbConfigFilePath;

    DBType(String dbConfigFilePath) {
        this.dbConfigFilePath = dbConfigFilePath;
    }

    public String getDbConfigFilePath() {
        return dbConfigFilePath;
    }
}
