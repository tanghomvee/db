package com.homvee.db.dao.impl;

import static org.junit.Assert.fail;

import org.junit.Test;

import com.google.protobuf.Message;
import com.homvee.grpc.service.msgs.GRPCMsg;

public class MySqlBaseDaoImplTest {
	MySqlBaseDaoImpl mySqlBaseDaoImpl  = new MySqlBaseDaoImpl();
	public static void main(String[] args) {
		MySqlBaseDaoImpl mySqlBaseDaoImpl  = new MySqlBaseDaoImpl();
		System.out.println(mySqlBaseDaoImpl.getPrimaryKeyName("dbuser"));
	}

	@Test
	public void testGetDataSource() {
		try {
			mySqlBaseDaoImpl.getDataSource();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Test
	public void testGetPrimaryKeyName() {
		System.out.println(mySqlBaseDaoImpl.getPrimaryKeyName("dbuser"));
	}

	@Test
	public void testUpdateMessage() {
		Message message = null;
		mySqlBaseDaoImpl.update(message);
	}

	@Test
	public void testSaveMessage() {
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		teacher.setAge(30);
		teacher.setName("THW");
		teacher.setSalary(10);
		mySqlBaseDaoImpl.save(teacher.build());
	}

	@Test
	public void testCountMessage() {
		fail("Not yet implemented");
	}

	@Test
	public void testQueryStringListOfQClassOfQextendsMessage() {
		fail("Not yet implemented");
	}

	@Test
	public void testGetTableName() {
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		System.out.println(mySqlBaseDaoImpl.getTableName(teacher.build()));
	}

	@Test
	public void testUpdateStringObjectArray() {
		fail("Not yet implemented");
	}

	@Test
	public void testUpdateStringListOfQ() {
		fail("Not yet implemented");
	}

	@Test
	public void testSaveStringListOfQ() {
		fail("Not yet implemented");
	}

	@Test
	public void testSaveStringObjectArray() {
		fail("Not yet implemented");
	}

	@Test
	public void testCountStringListOfQ() {
		fail("Not yet implemented");
	}

	@Test
	public void testCountStringObjectArray() {
		fail("Not yet implemented");
	}

	@Test
	public void testSaveOrUpdate() {
		fail("Not yet implemented");
	}

	@Test
	public void testQueryStringListOfQ() {
		fail("Not yet implemented");
	}

	@Test
	public void testClose() {
		fail("Not yet implemented");
	}

	@Test
	public void testSetParameters() {
		fail("Not yet implemented");
	}

}
