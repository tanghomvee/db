package com.homvee.db.dao;


import com.google.protobuf.Message;

import java.util.List;
import java.util.Map;

public interface Dao {
    int update(String sql , Object ... params);
    int update(String sql , List<?> params);
    int save(String sql , List<?> params);
    int save(String sql , Object ... params);
    int count(String sql , List<?> params);
    int count(String sql , Object ... params);
    List<Map<String, Object>> query(String sql , List<?> params);

    int update(Message message);
    int save(Message message);
    List<Message> query(String sql , List<?> params , Message retClazz);
}
