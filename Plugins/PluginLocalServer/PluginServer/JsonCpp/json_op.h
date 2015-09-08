#include "json.h"

///do the operations in @CODE, and return @RETVAL
#define do_return(CODE, RETVAL) { CODE; return RETVAL; }
///do the operations in @CODE, and the break
#define do_break(CODE) { CODE; break; }
///check boolean value of @EXP, if false, return @RETVAL
#define fail_return(EXP, RETVAL) { if(!(EXP)) return RETVAL; }
///check boolean value of @EXP, if false, break;
#define fail_break(EXP) { if(!(EXP)) break; }
///check boolean value of @EXP, if false, do the @CODE
#define fail_do(EXP, CODE) { if(!(EXP)) { CODE; } }

typedef Json::Value value_t;

#pragma warning( disable : 4505)

/**
* check property existence on the specified @obj.
* if failed, it will produce a warn-log.
*/
static bool warn_if_prop_exists(const value_t& obj, const std::string& prop_name)
{
	if(obj.isMember(prop_name))
		return false;
	return true;
}

static bool warn_if_prop_not_set(const value_t& obj, const std::string& prop_name)
{
	if(!obj.isMember(prop_name))
		return false;
	if(obj[prop_name].isNull())
		return false;
	return true;
}

/**
* add new property to the specified @obj.
* if this property has already existed, false will be returned, and produce a warn-log.
*/
template<typename T>
static bool add_prop(value_t& obj, const std::string& prop_name, const T& prop_val)
{
	fail_return(warn_if_prop_exists(obj, prop_name), false);
	obj[prop_name] = prop_val;
	return true;
}

static bool add_prop(value_t& obj, const std::string& prop_name, Json::Int64 prop_val)
{
	fail_return(warn_if_prop_exists(obj, prop_name), false);
	obj[prop_name] = (Json::Value::Int64)prop_val;
	return true;
}

static bool add_prop(value_t& obj, const std::string& prop_name, Json::UInt64 prop_val)
{
	fail_return(warn_if_prop_exists(obj, prop_name), false);
	obj[prop_name] = (Json::Value::UInt64)prop_val;
	return true;
}

/**
* write property value to the specified @obj.
* if this property has already existed, the old value will be overwritten to new value.
*/
template<typename T>
static bool write_prop(value_t& obj, const std::string& prop_name, const T& prop_val)
{
	obj[prop_name] = prop_val;
	return true;
}

static bool write_prop(value_t& obj, const std::string& prop_name, Json::Int64 prop_val)
{
	obj[prop_name] = (Json::Value::Int64)prop_val;
	return true;
}

static bool write_prop(value_t& obj, const std::string& prop_name, Json::UInt64 prop_val)
{
	obj[prop_name] = (Json::Value::UInt64)prop_val;
	return true;
}

/**
* get property value from the specified @obj.
* if this property is not existed or its value is NULL, false will be returned, and produce a warn-log.
*/

template<typename T>
static bool read_prop(const value_t& obj, const std::string& prop_name, T& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false);
	const value_t& val = obj[prop_name];
	try{ prop_val = val.asInt();
	}catch(std::exception&) { return false; }
	return true;
}

static bool read_prop(const value_t& obj, const std::string& prop_name, std::string& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false)
		try{ prop_val = obj[prop_name].asString();
	}catch(std::exception&) { return false; }
	return true;
}

static bool read_prop(const value_t& obj, const std::string& prop_name, Json::UInt& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false);
	try{ prop_val = obj[prop_name].asUInt();
	}catch(std::exception&) { return false; }
	return true;
}

static bool read_prop(const value_t& obj, const std::string& prop_name, Json::Int64& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false);
	try{ prop_val = obj[prop_name].asInt64();
	}catch(std::exception&) { return false; }
	return true;
}

static bool read_prop(const value_t& obj, const std::string& prop_name, Json::UInt64& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false);
	try{ prop_val = obj[prop_name].asUInt64();
	}catch(std::exception&) { return false; }
	return true;
}

static bool read_prop(const value_t& obj, const std::string& prop_name, float& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false);
	try{ prop_val = obj[prop_name].asFloat();
	}catch(std::exception&) { return false; }
	return true;
}

static bool read_prop(const value_t& obj, const std::string& prop_name, double& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false);
	try{ prop_val = obj[prop_name].asDouble();
	}catch(std::exception&) { return false; }
	return true;
}

static bool read_prop(const value_t& obj, const std::string& prop_name, bool& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false)
		try{ prop_val = obj[prop_name].asBool();
	}catch(std::exception&) { return false; }
	return true;
}

static bool read_prop(const value_t& obj, const std::string& prop_name, value_t& prop_val)
{
	fail_return(warn_if_prop_not_set(obj, prop_name), false);
	try{ prop_val = obj[prop_name];
	}catch(std::exception&) { return false; }
	return true;
}

/**
* get property value from the specified @obj.
* unlike read_prop(), these functions don't have a return value.
*/

template<typename T>
static void read_prop_try(const value_t& obj, const std::string& prop_name, T& prop_val)
{
	const value_t& val = obj[prop_name];
	try{ prop_val = val.asInt();
	}catch(std::exception&) { }
}

static void read_prop_try(const value_t& obj, const std::string& prop_name, std::string& prop_val)
{
	try{ prop_val = obj[prop_name].asString();
	}catch(std::exception&) { }
}

static void read_prop_try(const value_t& obj, const std::string& prop_name, Json::UInt& prop_val)
{
	try{ prop_val = obj[prop_name].asUInt();
	}catch(std::exception&) { }
}

static void read_prop_try(const value_t& obj, const std::string& prop_name, Json::Int64& prop_val)
{
	try{ prop_val = obj[prop_name].asInt64();
	}catch(std::exception&) { }
}

static void read_prop_try(const value_t& obj, const std::string& prop_name, Json::UInt64& prop_val)
{
	try{ prop_val = obj[prop_name].asUInt64();
	}catch(std::exception&) { }
}

static void read_prop_try(const value_t& obj, const std::string& prop_name, float& prop_val)
{
	try{ prop_val = obj[prop_name].asFloat();
	}catch(std::exception&) { }
}

static void read_prop_try(const value_t& obj, const std::string& prop_name, double& prop_val)
{
	try{ prop_val = obj[prop_name].asDouble();
	}catch(std::exception&) { }
}

static void read_prop_try(const value_t& obj, const std::string& prop_name, bool& prop_val)
{
	try{ prop_val = obj[prop_name].asBool();
	}catch(std::exception&) { }
}

static void read_prop_try(const value_t& obj, const std::string& prop_name, value_t& prop_val)
{
	try{ prop_val = obj[prop_name];
	}catch(std::exception&) { }
}

/**
* message property operator
* a helper class via which we can easily operate message value's properties
*/
class JSON_API json_op
{
public:
	inline json_op():_good(true), _val(NULL) { }
	inline json_op(const value_t& v):_good(true), _val(const_cast<value_t*>(&v)) { }
	inline void val(const value_t& v) { _val = const_cast<value_t*>(&v); }
	inline value_t* val() { return _val; }
	inline void reset() { _good = true; }
	inline bool good() { return _good; }
	inline operator bool() { return good(); }

	template<typename T>
	json_op& add(const std::string& prop_name, const T& prop_val)
	{
		if(_good)
			_good &= add_prop(*_val, prop_name, prop_val);
		return *this;
	}

	template<typename T>
	json_op& read(const std::string& prop_name, T& prop_val)
	{
		if(_good)
			_good &= read_prop(*_val, prop_name, prop_val);
		return *this;
	}

	template<typename T>
	json_op& write(const std::string& prop_name, const T& prop_val)
	{
		if(_good)
			_good &= write_prop(*_val, prop_name, prop_val);
		return *this;
	}

	template<typename T>
	json_op& read_try(const std::string& prop_name, T& prop_val)
	{
		if(_good)
			read_prop_try(*_val, prop_name, prop_val);
		return *this;
	}

private:
	bool _good;
	value_t* _val;
};

class JSON_API json_i
{
private:
	Json::FastWriter m_fastWrt;
	Json::Value m_val;
	json_op m_op;

public:
	inline json_i() { m_op.val(m_val); }
	inline void reset() { m_op.reset(); }
	inline bool good() { return m_op.good(); }
	inline operator bool() { return good(); }
	inline std::string toString() { return m_fastWrt.write(m_val); }

	template<typename T>
	json_i& add(const std::string& prop_name, const T& prop_val)
	{
		m_op.add(prop_name, prop_val);
		return *this;
	}

	template<typename T>
	json_i& write(const std::string& prop_name, const T& prop_val)
	{
		m_op.write(prop_name, prop_val);
		return *this;
	}
};

class JSON_API json_o
{
private:
	Json::Reader m_reader;
	Json::Value m_val;
	json_op m_op;

public:
	inline json_o() {}
	inline void reset() { m_op.reset(); }
	inline bool good() { return m_op.good(); }
	inline operator bool() { return good(); }

	inline bool parse(const char* str_doc, int len)
	{
		if (m_reader.parse(str_doc, str_doc + len, m_val))
		{
			m_op.val(m_val);
			return true;
		}
		return false;
	}

	inline std::string toStyledString() const
	{
		return m_val.toStyledString();
	}

	template<typename T>
	json_o& read(const std::string& prop_name, T& prop_val)
	{
		m_op.read(prop_name, prop_val);
		return *this;
	}

	template<typename T>
	json_o& read_try(const std::string& prop_name, T& prop_val)
	{
		m_op.read_try(prop_name, prop_val);
		return *this;
	}
};

