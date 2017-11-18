package com.homvee.db.msgs;

import com.google.protobuf.*;
import com.homvee.db.annotations.TableName;

import java.io.IOException;
import java.io.OutputStream;
import java.util.List;
import java.util.Map;

@TableName(name = "cccc")
public class MyMsg implements Message {
    public void writeTo(CodedOutputStream output) throws IOException {

    }

    public int getSerializedSize() {
        return 0;
    }

    public Parser<? extends Message> getParserForType() {
        return null;
    }

    @Override
    public ByteString toByteString() {
        return null;
    }

    public byte[] toByteArray() {
        return new byte[0];
    }

    public void writeTo(OutputStream output) throws IOException {

    }

    public void writeDelimitedTo(OutputStream output) throws IOException {

    }

    public Builder newBuilderForType() {
        return null;
    }

    public Builder toBuilder() {
        return null;
    }

    public Message getDefaultInstanceForType() {
        return null;
    }

    public boolean isInitialized() {
        return false;
    }

    public List<String> findInitializationErrors() {
        return null;
    }

    public String getInitializationErrorString() {
        return null;
    }

    public Descriptors.Descriptor getDescriptorForType() {
        return null;
    }

    public Map<Descriptors.FieldDescriptor, Object> getAllFields() {
        return null;
    }

    public boolean hasOneof(Descriptors.OneofDescriptor oneof) {
        return false;
    }

    public Descriptors.FieldDescriptor getOneofFieldDescriptor(Descriptors.OneofDescriptor oneof) {
        return null;
    }

    public boolean hasField(Descriptors.FieldDescriptor field) {
        return false;
    }

    public Object getField(Descriptors.FieldDescriptor field) {
        return null;
    }

    public int getRepeatedFieldCount(Descriptors.FieldDescriptor field) {
        return 0;
    }

    public Object getRepeatedField(Descriptors.FieldDescriptor field, int index) {
        return null;
    }

    public UnknownFieldSet getUnknownFields() {
        return null;
    }
}
