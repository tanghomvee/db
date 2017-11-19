package com.homvee.db.dao.impl;

import static org.junit.Assert.fail;

import java.util.ArrayList;
import java.util.List;

import org.junit.Test;

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
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		teacher.setId(1);
		teacher.setAge(15);
		teacher.setName("THWqs");
		mySqlBaseDaoImpl.update(teacher.build());
	}

	@Test
	public void testSaveMessage() {
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		teacher.setAge(30);
		teacher.setName("THW");
		teacher.setSalary(50);
		mySqlBaseDaoImpl.save(teacher.build());
	}

	@Test
	public void testCountMessage() {
		fail("Not yet implemented");
	}

	@Test
	public void testQueryStringListOfQClassOfQextendsMessage() {
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		teacher.setAge(30);
		teacher.setName("THW");
		GRPCMsg.Teacher.Builder teacher1 = GRPCMsg.Teacher.newBuilder();
		System.out.println(mySqlBaseDaoImpl.query(teacher.build(), teacher1));
	}

	@Test
	public void testGetTableName() {
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		System.out.println(mySqlBaseDaoImpl.getTableName(teacher.build()));
	}
	
	@Test
	public void testGetByPrimaryKey() {
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		teacher.setId(1);
		System.out.println(mySqlBaseDaoImpl.getByPrimaryKey(teacher.build()));
	}
	
	@Test
	public void testDelete() {
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		teacher.setId(2);
		System.out.println(mySqlBaseDaoImpl.delete(teacher.build()));
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
		String sql = "select count(*) from teacher where age = ?";
		List<Object> parm  = new ArrayList<Object>();
		parm.add(15);
		System.out.println(mySqlBaseDaoImpl.count(sql,parm));
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
		GRPCMsg.Teacher.Builder teacher = GRPCMsg.Teacher.newBuilder();
		String sql = "select * from teacher where age = ?";
		List<Object> parm  = new ArrayList<Object>();
		parm.add(15);
		System.out.println(mySqlBaseDaoImpl.query(sql,parm, teacher));
	}

}
