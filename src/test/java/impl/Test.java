package impl;

import com.homvee.db.dao.impl.MySqlBaseDaoImpl;

public class Test {
    public static void main(String[] args) throws Exception {
        MySqlBaseDaoImpl mySqlBaseDao = new MySqlBaseDaoImpl();
        mySqlBaseDao.getDataSource();
        System.out.println(mySqlBaseDao.getPrimaryKeyName("SubScriptReqProto"));

        SubScriptReqProto.SubScriptReq.Builder subScriptReq = SubScriptReqProto.SubScriptReq.newBuilder();
        subScriptReq.setAddress("sdssddsds");

    }
}
