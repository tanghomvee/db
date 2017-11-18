package impl;

import java.util.List;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.google.protobuf.Message;
import com.homvee.db.dao.impl.MySqlBaseDaoImpl;

public class Test {
	private static Logger logger = LogManager.getLogger(Test.class);
    public static void main(String[] args) throws Exception {
    	logger.info("请注意 测试开始了=============================");
        MySqlBaseDaoImpl mySqlBaseDao = new MySqlBaseDaoImpl();

        SubScriptReqProto.SubScriptReq.Builder subScriptReq = SubScriptReqProto.SubScriptReq.newBuilder();
        subScriptReq.setAddress("sdssddsds");
        subScriptReq.setSubReqID(1);
        subScriptReq.setProductName("productName");
        subScriptReq.setUserName("username");
//        mySqlBaseDao.save(subScriptReq.build());

        mySqlBaseDao.save(subScriptReq.build());
       List<Message> messages = mySqlBaseDao.query("select * from SubScriptReq" , null , SubScriptReqProto.SubScriptReq.newBuilder());
       for(Message message :messages) {
    	   System.out.println(message.getAllFields());
    	   System.out.println(message.getDefaultInstanceForType());
       }

        System.out.println(messages.size());
    }
}
