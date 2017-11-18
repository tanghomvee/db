package impl;

import com.google.protobuf.Message;
import com.homvee.db.dao.impl.MySqlBaseDaoImpl;

import java.util.List;

public class Test {
    public static void main(String[] args) throws Exception {
        MySqlBaseDaoImpl mySqlBaseDao = new MySqlBaseDaoImpl();

        SubScriptReqProto.SubScriptReq.Builder subScriptReq = SubScriptReqProto.SubScriptReq.newBuilder();
        subScriptReq.setAddress("sdssddsds");
        subScriptReq.setSubReqID(1);
        subScriptReq.setProductName("productName");
        subScriptReq.setUserName("username");
//        mySqlBaseDao.save(subScriptReq.build());

       List<Message> messages = mySqlBaseDao.query("select * from SubScriptReq" , null , SubScriptReqProto.SubScriptReq.newBuilder());

        System.out.println(messages.size());
    }
}
