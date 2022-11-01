#pragma once

#include <cmath>
#include "StdObject.h"
#include "Json/json.h"

namespace Std 
{
using namespace Json;

struct JSONDouble;
struct JSONInt;
struct JSONUInt;
struct JSONInt64;
struct JSONUInt64;
struct JSONString;
struct JSONBool;
struct JSONArray;

// eventually they will proxied by type.
// figure out how to convert it into lua value without function.
struct JSONValue : public StdObject
{
    Value v;
    
    JSONValue(double val) { v = Value(val); }
    JSONValue(bool val) { v = Value(val); }
    JSONValue(const char* val) { v = Value(val); }
    JSONValue(const JSONValue& val) { v = val.v; }
    JSONValue() {}
    JSONValue(Value & val) { v = val; }

    //JSONValue& operator= (JSONValue & other) { v = other.v; return *this; }

    void copy(Value & other) { v.copy(other); }
    ValueType type() { return v.type(); }
    
    JSONValue& operator = (const JSONValue& val) { v = val.v; return *this; }

    Value& operator[](const char * key);
    JSONValue __getitem(const char * key) ;

    JSONDouble d();
    JSONInt    i32();
    JSONUInt   ui32();
    JSONInt64  i64();
    JSONUInt64 ui64();
    JSONBool   b();
    JSONString str();
    JSONArray  array();

    void __setitem(const char * key, double val)
    {
        v[key] = val;
    }
    void __setitem(const char * key, const char* val)
    {
        v[key] = val;
    }    
    void __setitem(const char * key, bool val)
    {
        v[key] = val;
    }

    JSONValue getarrayitem(int index)
    {
        return JSONValue(v[index]);
    }
    
    void setarrayitem(int index, double val)
    {
        v[index] = val;
    }
    void setarrayitem(int index, const char* val)
    {
        v[index] = val;
    }
    void setarrayitem(int index, bool val)
    {
        v[index] = val;
    }

    void append(JSONValue & val)
    {
        v.append(val.v);
    }
    bool insert(size_t index, JSONValue & val)
    {
        return v.insert(index,val.v);
    }
    void remove(const char * key)
    {
        v.removeMember(key);
    }
    bool remove_index(size_t index, JSONValue & removed)
    {
        return v.removeIndex(index,&removed.v);
    }
    bool isMember(const char * key)
    {
        return v.isMember(key);
    }
    bool empty() const { return v.empty(); }
    void clear() { v.clear(); }
    void resize(size_t newsize) { v.resize(newsize); }
    size_t size() const { return v.size(); }

    bool isNull() const { return v.isNull(); }
    bool isBool() const { return v.isBool(); }
    bool isInt() const { return v.isInt(); }
    bool isInt64() const { return v.isInt64(); }
    bool isUInt() const { return v.isUInt(); }
    bool isUInt64() const { return v.isUInt64(); }
    bool isIntegral() const { return v.isIntegral(); }
    bool isDouble() const { return v.isDouble(); }    
    bool isNumeric() const { return v.isNumeric(); }
    bool isString() const { return v.isString(); }
    bool isArray() const { return v.isArray(); }
    bool isObject() const { return v.isObject(); }

    std::string asString() const { return v.asString(); }        
    int32_t asInt() const { return v.asInt(); }
    uint32_t asUInt() const { return v.asUInt(); }            
    float asFloat() const { return v.asFloat(); }
    double asDouble() const { return v.asDouble(); }
    bool asBool() const { return v.asBool(); }    
    
};

struct JSONDouble : public JSONValue
{
    JSONDouble(const Value & value) { v = value; }
    JSONDouble(const JSONValue & u) { v = u.v; }
    JSONDouble(const JSONDouble & u) { v = u.v; }
    JSONDouble(double val) { v = Value(val); }

    JSONDouble operator = (const JSONDouble & val) { v = val.v; return *this; }

    double value() const { return v.asDouble(); }    
    
    operator double() { return value(); }
    
    JSONDouble operator + (const JSONDouble & b) { return JSONDouble(v.asDouble() + b.v.asDouble()); }
    JSONDouble operator - (const JSONDouble & b) { return JSONDouble(v.asDouble() - b.v.asDouble()); }
    JSONDouble operator * (const JSONDouble & b) { return JSONDouble(v.asDouble() * b.v.asDouble()); }
    JSONDouble operator / (const JSONDouble & b) { return JSONDouble(v.asDouble() / b.v.asDouble()); }
    JSONDouble operator ^ (const JSONDouble & b) { return JSONDouble(std::pow(v.asDouble(),b.v.asDouble())); }
    JSONDouble operator % (const JSONDouble & b) { return JSONDouble(std::fmod(v.asDouble(),b.asDouble())); }


    JSONDouble operator + (const double b) { return JSONDouble(v.asDouble() + b); }
    JSONDouble operator - (const double b) { return JSONDouble(v.asDouble() - b); }
    JSONDouble operator * (const double b) { return JSONDouble(v.asDouble() * b); }
    JSONDouble operator / (const double b) { return JSONDouble(v.asDouble() / b); }
    JSONDouble operator ^ (const double b) { return JSONDouble(std::pow(v.asDouble(),b)); }
    JSONDouble operator % (const double b) { return JSONDouble(std::fmod(v.asDouble(),b)); }

};
struct JSONInt : public JSONValue
{
    JSONInt(const Value & value) { v = value; }
    JSONInt(const JSONValue & u) { v = u.v; }
    JSONInt(const JSONInt & u) { v = u.v; }
    JSONInt(int val) { v = Value(val); }

    JSONInt operator = (int val) { v = val; return *this; }

    int value() const { return v.asInt(); }    

    JSONInt operator + (const JSONInt & b) { return JSONInt(v.asInt() + b.v.asInt()); }
    JSONInt operator - (const JSONInt & b) { return JSONInt(v.asInt() - b.v.asInt()); }
    JSONInt operator * (const JSONInt & b) { return JSONInt(v.asInt() * b.v.asInt()); }
    JSONInt operator / (const JSONInt & b) { return JSONInt(v.asInt() / b.v.asInt()); }
    JSONInt operator ^ (const JSONInt & b) { return JSONInt(std::pow(v.asInt(),b.v.asInt())); }
    JSONInt operator % (const JSONInt & b) { return JSONInt(v.asInt() % b.v.asInt()); }

    JSONInt operator + (const int  b) { return JSONInt(v.asInt() + b); }
    JSONInt operator - (const int  b) { return JSONInt(v.asInt() - b); }
    JSONInt operator * (const int  b) { return JSONInt(v.asInt() * b); }
    JSONInt operator / (const int  b) { return JSONInt(v.asInt() / b); }
    JSONInt operator ^ (const int  b) { return JSONInt(std::pow(v.asInt(),b)); }
    JSONInt operator % (const int  b) { return JSONInt(v.asInt() % b); }

};
struct JSONUInt : public JSONValue
{
    JSONUInt(const Value & value) { v = value; }
    JSONUInt(const JSONValue & u) { v = u.v; }
    JSONUInt(const JSONUInt & u) { v = u.v; }
    JSONUInt(unsigned int val) { v = Value(val); }

    JSONUInt operator = (unsigned int val) { v = val; return *this; }

    unsigned int value() const { return v.asUInt(); }    

    JSONUInt operator + (const JSONInt & b) { return JSONUInt(v.asUInt() + b.v.asUInt()); }
    JSONUInt operator - (const JSONInt & b) { return JSONUInt(v.asUInt() - b.v.asUInt()); }
    JSONUInt operator * (const JSONInt & b) { return JSONUInt(v.asUInt() * b.v.asUInt()); }
    JSONUInt operator / (const JSONInt & b) { return JSONUInt(v.asUInt() / b.v.asUInt()); }
    JSONUInt operator ^ (const JSONInt & b) { return JSONUInt(std::pow(v.asUInt(),b.v.asUInt())); }
    JSONUInt operator % (const JSONInt & b) { return JSONUInt(v.asUInt() % b.v.asUInt()); }

    JSONUInt operator + (const unsigned int  b) { return JSONUInt(v.asUInt() + b); }
    JSONUInt operator - (const unsigned int  b) { return JSONUInt(v.asUInt() - b); }
    JSONUInt operator * (const unsigned int  b) { return JSONUInt(v.asUInt() * b); }
    JSONUInt operator / (const unsigned int  b) { return JSONUInt(v.asUInt() / b); }
    JSONUInt operator ^ (const unsigned int  b) { return JSONUInt(std::pow(v.asUInt(),b)); }
    JSONUInt operator % (const unsigned int  b) { return JSONUInt(v.asUInt() % b); }

};
struct JSONInt64 : public JSONValue
{
    JSONInt64(const Value & value) { v = value; }
    JSONInt64(const JSONValue & u) { v = u.v; }
    JSONInt64(const JSONInt64 & u) { v = u.v; }
    JSONInt64(int64_t val) { v = Value(val); }

    JSONInt64 operator = (int64_t val) { v = val; return *this; }

    Int64 value() const { return v.asInt64(); }    

    JSONInt64 operator + (const JSONInt64 & b) { return JSONInt64(v.asInt() + b.v.asInt64()); }
    JSONInt64 operator - (const JSONInt64 & b) { return JSONInt64(v.asInt() - b.v.asInt64()); }
    JSONInt64 operator * (const JSONInt64 & b) { return JSONInt64(v.asInt() * b.v.asInt64()); }
    JSONInt64 operator / (const JSONInt64 & b) { return JSONInt64(v.asInt() / b.v.asInt64()); }
    JSONInt64 operator ^ (const JSONInt64 & b) { return JSONInt64(std::pow(v.asInt64(),b.v.asInt64())); }
    JSONInt64 operator % (const JSONInt64 & b) { return JSONInt64(v.asInt() % b.v.asInt64()); }


    JSONInt64 operator + (const int64_t  b) { return JSONInt64(v.asInt64() + b); }
    JSONInt64 operator - (const int64_t  b) { return JSONInt64(v.asInt64() - b); }
    JSONInt64 operator * (const int64_t  b) { return JSONInt64(v.asInt64() * b); }
    JSONInt64 operator / (const int64_t  b) { return JSONInt64(v.asInt64() / b); }
    JSONInt64 operator ^ (const int64_t  b) { return JSONInt64(std::pow(v.asInt64(),b)); }
    JSONInt64 operator % (const int64_t  b) { return JSONInt64(v.asInt64() % b); }
};
struct JSONUInt64 : public JSONValue
{
    JSONUInt64(const Value & value) { v = value; }
    JSONUInt64(const JSONValue & u) { v = u.v; }
    JSONUInt64(const JSONUInt64 & u) { v = u.v; }
    JSONUInt64(uint64_t val) { v = Value(val); }

    JSONUInt64 operator = (uint64_t val) { v = val; return *this; }

    unsigned int value() const { return v.asUInt(); }    

    JSONUInt64 operator + (const JSONUInt64 & b) { return JSONUInt64(v.asUInt64() + b.v.asUInt64()); }
    JSONUInt64 operator - (const JSONUInt64 & b) { return JSONUInt64(v.asUInt64() - b.v.asUInt64()); }
    JSONUInt64 operator * (const JSONUInt64 & b) { return JSONUInt64(v.asUInt64() * b.v.asUInt64()); }
    JSONUInt64 operator / (const JSONUInt64 & b) { return JSONUInt64(v.asUInt64() / b.v.asUInt64()); }
    JSONUInt64 operator ^ (const JSONUInt64 & b) { return JSONUInt64(std::pow(v.asUInt64(),b.v.asUInt64())); }
    JSONUInt64 operator % (const JSONUInt64 & b) { return JSONUInt64(v.asUInt64() % b.v.asUInt64()); }

    JSONUInt64 operator + (const uint64_t b) { return JSONUInt64(v.asUInt64() + b); }
    JSONUInt64 operator - (const uint64_t b) { return JSONUInt64(v.asUInt64() - b); }
    JSONUInt64 operator * (const uint64_t b) { return JSONUInt64(v.asUInt64() * b); }
    JSONUInt64 operator / (const uint64_t b) { return JSONUInt64(v.asUInt64() / b); }
    JSONUInt64 operator ^ (const uint64_t b) { return JSONUInt64(std::pow(v.asUInt64(),b)); }
    JSONUInt64 operator % (const uint64_t b) { return JSONUInt64(v.asUInt64() % b); }
    
};
struct JSONString : public JSONValue
{
    JSONString(const Value & value) { v = value; }
    JSONString(const JSONValue & u) { v = u.v; }
    JSONString(const JSONString & s) { v = s.v;}
    JSONString(const char* val) { v = Value(val); }
    JSONString(const std::string & s) { v = Value(s.c_str()); }
    JSONString& operator = (const char* val) { v = val; return *this;  }

    std::string value() const { return v.asString()  ; }        
    
    JSONString operator + (const JSONString & b) { return JSONString(v.asString() + b.v.asString()); }
    JSONString operator + (const char * b) { return JSONString(v.asString() + b); }
    JSONString operator + (const std::string& b) { return JSONString(v.asString() + b); }
};
struct JSONBool : public JSONValue
{
    JSONBool(const JSONValue & u) { v = u.v; }
    JSONBool(const Value & val) { v = val; }
    JSONBool(const JSONBool & u) { v = u.v; }
    JSONBool(bool val) { v = Value(val); }

    JSONBool& operator = (bool val) { v = val; return *this;  }

    bool value() const { return v.asBool(); }        
};
struct JSONArray : public JSONValue
{
    JSONArray(const JSONValue & u) { v = u.v; }
    JSONArray(Value & val) { v = val; }

    Value& operator[](const char * key)
    {
        return v[key];
    }

    JSONValue __getitem(const char * key) 
    { 
        return JSONValue(v[key]);        
    }        
    void __setitem(const int index, double val)
    {
        v[index] = val;
    }
    void __setitem(const int index, const char* val)
    {
        v[index] = val;
    }    
    void __setitem(const int index, bool val)
    {
        v[index] = val;
    }
   
    void append(JSONValue & val)
    {
        v.append(val.v);
    }
    bool insert(size_t index, JSONValue & val)
    {
        return v.insert(index,val.v);
    }
    void remove(const char * key)
    {
        v.removeMember(key);
    }


    bool empty() const { return v.empty(); }
    void clear() { v.clear(); }
    void resize(size_t newsize) { v.resize(newsize); }
    size_t size() const { return v.size(); }

};

struct JSON
{
    Reader r;
    JSONValue root;
    

    JSON() = default;    
    ~JSON() = default;

    bool parse(const char * document, bool collectComments=true)
    {
        return r.parse(document,root.v,collectComments);
    }

    //const char* GetReadErrorMessages() { return r.getFormattedErrorMessages(); }

    
    Value& operator[](const char *key) { return root[key]; }
    
    JSONValue __getitem(const char * key);

    
    void __setitem(const char * key, const JSONValue & x)
    {
        root[key] = x.v;
    }

    JSONValue& get_root()      
    {
        return root; 
    }

    std::string to_string()
    {
        FastWriter w;
        return w.write(root.v);
    }
};

Value& JSONValue::operator[](const char * key)
{
    return v[key];
}

JSONValue JSONValue::__getitem(const char * key) 
{ 
        JSONValue r(v[key]);
        if(r.isBool()) return JSONBool(v[key]);
        if(r.isDouble()) return JSONDouble(v[key]);
        if(r.isInt()) return JSONInt(v[key]);
        if(r.isUInt()) return JSONUInt(v[key]);
        if(r.isInt64()) return JSONInt64(v[key]);
        if(r.isUInt64()) return JSONUInt64(v[key]);
        if(r.isString()) return JSONString(v[key]);
        if(r.isArray()) return JSONArray(v[key]);
        return r;
}

JSONValue JSON::__getitem(const char * key)
{            
    JSONValue r =  root[key];
    if(r.isBool()) return JSONBool(r);
    if(r.isDouble()) return JSONDouble(r);
    if(r.isInt()) return JSONInt(r);
    if(r.isUInt()) return JSONUInt(r);
    if(r.isInt64()) return JSONInt64(r);
    if(r.isUInt64()) return JSONUInt64(r);
    if(r.isString()) return JSONString(r);
    if(r.isArray()) return JSONArray(r);
    return r;
}

JSONDouble cast_double(JSONValue & v) { return JSONDouble(v); }
JSONInt cast_int(JSONValue & v) { return JSONInt(v); }
JSONUInt cast_uint(JSONValue & v) { return JSONUInt(v); }
JSONInt64 cast_int64(JSONValue & v) { return JSONInt64(v); }
JSONUInt64 cast_uint64(JSONValue & v) { return JSONUInt64(v); }
JSONString cast_string(JSONValue & v) { return JSONString(v); }
JSONBool cast_bool(JSONValue & v) { return JSONBool(v); }
JSONArray cast_array(JSONValue &v) { return JSONArray(v); }
JSONDouble JSONValue::d()       { return cast_double(*this); }
JSONInt    JSONValue::i32()     { return cast_int(*this); }
JSONUInt   JSONValue::ui32()    { return cast_uint(*this); }
JSONInt64  JSONValue::i64()     { return cast_int64(*this); }
JSONUInt64 JSONValue::ui64()    { return cast_uint64(*this); }
JSONBool   JSONValue::b()       { return cast_bool(*this); }
JSONString JSONValue::str()     { return cast_string(*this); }
JSONArray  JSONValue::array()   { return cast_array(*this); }

}