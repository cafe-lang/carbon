//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020-2021 Thakee Nathees
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

// !!! AUTO GENERATED DO NOT EDIT !!!

#ifndef NATIVE_BIND_GEN_H
#define NATIVE_BIND_GEN_H

namespace carbon {


template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<ptr<T>> : std::true_type {};

#define DECLARE_VAR_TYPE(m_var_type, m_T) 	VarTypeInfo m_var_type = _remove_and_get_type_info<m_T>();


template <typename T, typename std::enable_if<std::is_same<T, void>::value, bool>::type = true>
VarTypeInfo _get_type_info() { return var::_NULL; }

template <typename T, typename std::enable_if<std::is_integral<T>::value
			&& !std::is_const<T>::value && !std::is_reference<T>::value, bool>::type = true>
VarTypeInfo _get_type_info() {
	if (std::is_same<T, bool>::value) return var::BOOL;
	return var::INT;
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value
			&& !std::is_const<T>::value && !std::is_reference<T>::value, bool>::type = true>
VarTypeInfo _get_type_info() { return var::FLOAT; }

template <typename T, typename std::enable_if<std::is_same<T, String>::value, bool>::type = true>
VarTypeInfo _get_type_info() { return var::STRING; }

template <typename T, typename std::enable_if<std::is_same<T, const char*>::value, bool>::type = true>
VarTypeInfo _get_type_info() { return var::STRING; }

template <typename T, typename std::enable_if<std::is_same<T, Array>::value, bool>::type = true>
VarTypeInfo _get_type_info() { return var::ARRAY; }

template <typename T, typename std::enable_if<std::is_same<T, Map>::value, bool>::type = true>
VarTypeInfo _get_type_info() { return var::MAP; }

template <typename T, typename std::enable_if<std::is_same<T, var>::value, bool>::type = true>
VarTypeInfo _get_type_info() { return var::VAR; }

template <typename T, typename std::enable_if<is_shared_ptr<T>::value, bool>::type = true>
VarTypeInfo _get_type_info() { return { var::OBJECT, T::element_type::get_type_name_s() }; }

// -- for no overloaded method found compiler errors ------------
template <typename T, typename std::enable_if<std::is_reference<T>::value, bool>::type = true>
VarTypeInfo _get_type_info() { throw "INTERNAL BUG"; }
template <typename T, typename std::enable_if<std::is_const<T>::value, bool>::type = true>
VarTypeInfo _get_type_info() { throw "INTERNAL BUG"; }
template <typename T, typename std::enable_if<std::is_pointer<T>::value
	&& !std::is_same<T, const char*>::value, bool>::type = true>
VarTypeInfo _get_type_info() { throw "INTERNAL BUG"; }
// --------------------------------------------------------------

template <typename T>
VarTypeInfo _remove_and_get_type_info() {

	if (std::is_reference<T>::value) {
		return _remove_and_get_type_info<typename std::remove_reference<T>::type>();

	} else if (std::is_const<T>::value){
		return _remove_and_get_type_info<typename std::remove_const<T>::type>();

	} else if (std::is_pointer<T>::value){
		if (std::is_same<T, const char*>::value) {
			return _get_type_info<T>();
		} else {
			return _remove_and_get_type_info<typename std::remove_pointer<T>::type>();
		}

	} else {
		return _get_type_info<T>();
	}
	/* done return here : let compiler warn if missed anything */
}


//template <typename T, typename std::enable_if<std::is_same<T, Array>::value, bool>::type = true>
//VarTypeInfo _get_type_info() { return var::ARRAY; }


class BindData {
protected:
	const char* name;
	const char* class_name;

public:
	enum Type {
		METHOD,
		STATIC_FUNC,
		MEMBER_VAR,
		STATIC_VAR,
		STATIC_CONST,
		ENUM,
		ENUM_VALUE,
	};
	virtual Type get_type() const = 0;
	virtual const char* get_name() const { return name; }
	virtual const char* get_type_name() const { return class_name; }
	virtual int get_argc() const { DEBUG_BREAK(); THROW_ERROR(Error::BUG, "invalid call"); }
	virtual const ptr<MemberInfo> get_member_info() const = 0;
};

class MethodBind : public BindData {
protected:
	int argc = 0;
	ptr<MethodInfo> mi;

public:
	virtual BindData::Type get_type() const { return BindData::METHOD; }
	virtual int get_argc() const { return argc; }

	virtual var call(Object* self, stdvec<var*>& args) const = 0;
	const ptr<MethodInfo> get_method_info() const { return mi; }
	const ptr<MemberInfo> get_member_info() const override { return mi; }
};

class StaticFuncBind : public BindData {
protected:
	int argc;
	ptr<MethodInfo> mi;

public:
	virtual BindData::Type get_type()   const { return BindData::STATIC_FUNC; }
	virtual int get_argc()              const { return argc; }

	virtual var call(stdvec<var*>& args) const = 0;
	const ptr<MethodInfo> get_method_info() const { return mi; }
	const ptr<MemberInfo> get_member_info() const override { return mi; }
};

// ---------------- MEMBER BIND START --------------------------------------
class PropertyBind : public BindData {
protected:
	ptr<PropertyInfo> pi;
public:
	virtual BindData::Type get_type() const { return BindData::MEMBER_VAR; }
	virtual var& get(Object* self) const = 0;

	const ptr<PropertyInfo> get_prop_info() const { return pi; }
	const ptr<MemberInfo> get_member_info() const override { return pi; }
};

template<typename Class>
class _PropertyBind : public PropertyBind {
protected:
	typedef var Class::* member_ptr_t;
	member_ptr_t member_ptr;
public:
	_PropertyBind(const char* p_name, const char* p_class_name, member_ptr_t p_member_ptr, ptr<PropertyInfo> p_pi) {
		name = p_name;
		class_name = p_class_name;
		member_ptr = p_member_ptr;
		pi = p_pi;
		pi->_set_bind((void*)this);
	}

	virtual var& get(Object* self) const override {
		return ((Class*)(self))->*member_ptr;
	}
};

template<typename Class>
ptr<PropertyBind> _bind_member(const char* p_name, const char* p_class_name, var Class::* p_member_ptr, const var& p_value = var()) {
	var Class::* member_ptr = p_member_ptr;
	return newptr<_PropertyBind<Class>>(p_name, p_class_name, member_ptr, newptr<PropertyInfo>(p_name, var::VAR, p_value));
}
// ------------------------------------------------------------------------


// ---------------- STATIC MEMBER BIND START ------------------------------
class StaticPropertyBind : public BindData {
	var* member = nullptr;
	ptr<PropertyInfo> pi;
public:
	virtual BindData::Type get_type() const { return BindData::STATIC_VAR; }

	StaticPropertyBind(const char* p_name, const char* p_class_name, var* p_member, ptr<PropertyInfo> p_pi) {
		name = p_name;
		class_name = p_class_name;
		member = p_member;
		pi = p_pi;
		pi->_set_bind((void*)this);
	}
	virtual var& get() const { return *member; }
	const ptr<PropertyInfo> get_prop_info() const { return pi; }
	const ptr<MemberInfo> get_member_info() const override { return pi; }
};

inline ptr<StaticPropertyBind> _bind_static_member(const char* p_name, const char* p_class_name, var* p_member) {
	return newptr<StaticPropertyBind>(p_name, p_class_name, p_member, newptr<PropertyInfo>(p_name, var::VAR, *p_member));
}
// ------------------------------------------------------------------------

// ---------------- STATIC CONST BIND START ------------------------------
class ConstantBind : public BindData {
protected:
	ptr<PropertyInfo> pi;
public:
	virtual BindData::Type get_type() const { return BindData::STATIC_CONST; }
	virtual var get() const = 0;

	const ptr<PropertyInfo> get_prop_info() const { return pi; }
	const ptr<MemberInfo> get_member_info() const override { return pi; }
};

template<typename T>
class _ConstantBind : public ConstantBind {
	T* _const = nullptr;
public:
	_ConstantBind(const char* p_name, const char* p_class_name, T* p_const, ptr<PropertyInfo> p_pi) {
		name = p_name;
		class_name = p_class_name;
		_const = p_const;
		pi = p_pi;
		pi->_set_bind((void*)this);
	}

	virtual var get() const override {
		return *_const;
	}
};

template<typename T>
ptr<ConstantBind> _bind_static_const(const char* p_name, const char* p_class_name, T* p_const) {
	DECLARE_VAR_TYPE(datatype, T);
	return newptr<_ConstantBind<T>>(p_name, p_class_name, p_const, newptr<PropertyInfo>(p_name, datatype, *p_const));
}
// ------------------------------------------------------------------------

// ---------------- ENUM BIND START ------------------------------

class EnumBind : public BindData {
	ptr<EnumInfo> ei;
public:
	EnumBind(const char* p_name, const char* p_class_name, ptr<EnumInfo> p_ei) {
		name = p_name;
		class_name = p_class_name;
		ei = p_ei;
	}
	virtual BindData::Type get_type() const { return BindData::ENUM; }
	int64_t get(const String& p_value_name) const {
		const stdmap<String, int64_t>& values = ei->get_values();
		stdmap<String, int64_t>::const_iterator it = values.find(p_value_name);
		if (it != values.end()) return it->second;
		throw Error(Error::ATTRIBUTE_ERROR, String::format("value \"%s\" isn't exists on enum %s.", p_value_name.c_str(), name));
	}
	const ptr<EnumInfo> get() const { return ei; }
	const ptr<EnumInfo> get_enum_info() const { return ei; }
	const ptr<MemberInfo> get_member_info() const override { return ei; }
};
inline ptr<EnumBind> _bind_enum(const char* p_name, const char* p_class_name, const stdvec<std::pair<String, int64_t>>& p_values) {
	return newptr<EnumBind>(p_name, p_class_name, newptr<EnumInfo>(p_name, p_values));
}

class EnumValueBind : public BindData {
	ptr<EnumValueInfo> evi;
	int64_t value;
public:
	EnumValueBind(const char* p_name, const char* p_class_name, int64_t p_value, ptr<EnumValueInfo> p_evi) {
		name = p_name;
		class_name = p_class_name;
		value = p_value;
		evi = newptr<EnumValueInfo>(p_name, p_value);
	}
	virtual BindData::Type get_type() const { return BindData::ENUM_VALUE; }
	int64_t get() const { return value; }

	const ptr<EnumValueInfo> get_enum_value_info() const { return evi; }
	const ptr<MemberInfo> get_member_info() const override { return evi; }
};

// -----------------------------------------------------------------------

template<typename T, typename R>
using M0 = R(T::*)();

template<typename T, typename R>
using M0_c = R(T::*)() const;

template<typename T, typename R, typename a0>
using M1 = R(T::*)(a0);

template<typename T, typename R, typename a0>
using M1_c = R(T::*)(a0) const;

template<typename T, typename R, typename a0, typename a1>
using M2 = R(T::*)(a0, a1);

template<typename T, typename R, typename a0, typename a1>
using M2_c = R(T::*)(a0, a1) const;

template<typename T, typename R, typename a0, typename a1, typename a2>
using M3 = R(T::*)(a0, a1, a2);

template<typename T, typename R, typename a0, typename a1, typename a2>
using M3_c = R(T::*)(a0, a1, a2) const;

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
using M4 = R(T::*)(a0, a1, a2, a3);

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
using M4_c = R(T::*)(a0, a1, a2, a3) const;

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
using M5 = R(T::*)(a0, a1, a2, a3, a4);

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
using M5_c = R(T::*)(a0, a1, a2, a3, a4) const;

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
using M6 = R(T::*)(a0, a1, a2, a3, a4, a5);

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
using M6_c = R(T::*)(a0, a1, a2, a3, a4, a5) const;

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
using M7 = R(T::*)(a0, a1, a2, a3, a4, a5, a6);

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
using M7_c = R(T::*)(a0, a1, a2, a3, a4, a5, a6) const;

template<typename R>
using F0 = R(*)();

template<typename R, typename a0>
using F1 = R(*)(a0);

template<typename R, typename a0, typename a1>
using F2 = R(*)(a0, a1);

template<typename R, typename a0, typename a1, typename a2>
using F3 = R(*)(a0, a1, a2);

template<typename R, typename a0, typename a1, typename a2, typename a3>
using F4 = R(*)(a0, a1, a2, a3);

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
using F5 = R(*)(a0, a1, a2, a3, a4);

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
using F6 = R(*)(a0, a1, a2, a3, a4, a5);

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
using F7 = R(*)(a0, a1, a2, a3, a4, a5, a6);


template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_0(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)();
}
template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_0(Object* self, const M& method, stdvec<var*>& args) {
	(((T*)self)->*method)(); return var();
}

template<typename T, typename R>
class _MethodBind_M0 : public MethodBind {
	typedef M0<T, R> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M0(const char* p_name, const char* p_class_name, int p_argc, M0<T, R> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 0) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 0 - default_arg_count));
		} else if (args_given > 0) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 0 argument(s).", 0 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 0 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_0<T, _Tmethod, R>(self, method, args);
	}
};
template<typename T, typename R>
class _MethodBind_M0_c : public MethodBind {
	typedef M0_c<T, R> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M0_c(const char* p_name, const char* p_class_name, int p_argc, M0_c<T, R> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 0) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 0 - default_arg_count));
		} else if (args_given > 0) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 0 argument(s).", 0 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 0 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_0<T, _Tmethod, R>(self, method, args);
	}
};

template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_1(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)(*args[0]);
}
template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_1(Object* self, const M& method, stdvec<var*>& args) {
	(((T*)self)->*method)(*args[0]); return var();
}

template<typename T, typename R, typename a0>
class _MethodBind_M1 : public MethodBind {
	typedef M1<T, R, a0> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M1(const char* p_name, const char* p_class_name, int p_argc, M1<T, R, a0> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 1) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 1 - default_arg_count));
		} else if (args_given > 1) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 1 argument(s).", 1 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 1 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_1<T, _Tmethod, R>(self, method, args);
	}
};
template<typename T, typename R, typename a0>
class _MethodBind_M1_c : public MethodBind {
	typedef M1_c<T, R, a0> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M1_c(const char* p_name, const char* p_class_name, int p_argc, M1_c<T, R, a0> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 1) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 1 - default_arg_count));
		} else if (args_given > 1) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 1 argument(s).", 1 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 1 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_1<T, _Tmethod, R>(self, method, args);
	}
};

template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_2(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)(*args[0], *args[1]);
}
template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_2(Object* self, const M& method, stdvec<var*>& args) {
	(((T*)self)->*method)(*args[0], *args[1]); return var();
}

template<typename T, typename R, typename a0, typename a1>
class _MethodBind_M2 : public MethodBind {
	typedef M2<T, R, a0, a1> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M2(const char* p_name, const char* p_class_name, int p_argc, M2<T, R, a0, a1> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 2) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 2 - default_arg_count));
		} else if (args_given > 2) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 2 argument(s).", 2 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 2 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_2<T, _Tmethod, R>(self, method, args);
	}
};
template<typename T, typename R, typename a0, typename a1>
class _MethodBind_M2_c : public MethodBind {
	typedef M2_c<T, R, a0, a1> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M2_c(const char* p_name, const char* p_class_name, int p_argc, M2_c<T, R, a0, a1> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 2) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 2 - default_arg_count));
		} else if (args_given > 2) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 2 argument(s).", 2 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 2 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_2<T, _Tmethod, R>(self, method, args);
	}
};

template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_3(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)(*args[0], *args[1], *args[2]);
}
template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_3(Object* self, const M& method, stdvec<var*>& args) {
	(((T*)self)->*method)(*args[0], *args[1], *args[2]); return var();
}

template<typename T, typename R, typename a0, typename a1, typename a2>
class _MethodBind_M3 : public MethodBind {
	typedef M3<T, R, a0, a1, a2> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M3(const char* p_name, const char* p_class_name, int p_argc, M3<T, R, a0, a1, a2> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 3) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 3 - default_arg_count));
		} else if (args_given > 3) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 3 argument(s).", 3 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 3 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_3<T, _Tmethod, R>(self, method, args);
	}
};
template<typename T, typename R, typename a0, typename a1, typename a2>
class _MethodBind_M3_c : public MethodBind {
	typedef M3_c<T, R, a0, a1, a2> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M3_c(const char* p_name, const char* p_class_name, int p_argc, M3_c<T, R, a0, a1, a2> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 3) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 3 - default_arg_count));
		} else if (args_given > 3) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 3 argument(s).", 3 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 3 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_3<T, _Tmethod, R>(self, method, args);
	}
};

template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_4(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)(*args[0], *args[1], *args[2], *args[3]);
}
template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_4(Object* self, const M& method, stdvec<var*>& args) {
	(((T*)self)->*method)(*args[0], *args[1], *args[2], *args[3]); return var();
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
class _MethodBind_M4 : public MethodBind {
	typedef M4<T, R, a0, a1, a2, a3> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M4(const char* p_name, const char* p_class_name, int p_argc, M4<T, R, a0, a1, a2, a3> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 4) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 4 - default_arg_count));
		} else if (args_given > 4) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 4 argument(s).", 4 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 4 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_4<T, _Tmethod, R>(self, method, args);
	}
};
template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
class _MethodBind_M4_c : public MethodBind {
	typedef M4_c<T, R, a0, a1, a2, a3> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M4_c(const char* p_name, const char* p_class_name, int p_argc, M4_c<T, R, a0, a1, a2, a3> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 4) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 4 - default_arg_count));
		} else if (args_given > 4) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 4 argument(s).", 4 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 4 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_4<T, _Tmethod, R>(self, method, args);
	}
};

template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_5(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)(*args[0], *args[1], *args[2], *args[3], *args[4]);
}
template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_5(Object* self, const M& method, stdvec<var*>& args) {
	(((T*)self)->*method)(*args[0], *args[1], *args[2], *args[3], *args[4]); return var();
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
class _MethodBind_M5 : public MethodBind {
	typedef M5<T, R, a0, a1, a2, a3, a4> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M5(const char* p_name, const char* p_class_name, int p_argc, M5<T, R, a0, a1, a2, a3, a4> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 5) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 5 - default_arg_count));
		} else if (args_given > 5) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 5 argument(s).", 5 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 5 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_5<T, _Tmethod, R>(self, method, args);
	}
};
template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
class _MethodBind_M5_c : public MethodBind {
	typedef M5_c<T, R, a0, a1, a2, a3, a4> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M5_c(const char* p_name, const char* p_class_name, int p_argc, M5_c<T, R, a0, a1, a2, a3, a4> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 5) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 5 - default_arg_count));
		} else if (args_given > 5) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 5 argument(s).", 5 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 5 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_5<T, _Tmethod, R>(self, method, args);
	}
};

template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_6(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]);
}
template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_6(Object* self, const M& method, stdvec<var*>& args) {
	(((T*)self)->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]); return var();
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
class _MethodBind_M6 : public MethodBind {
	typedef M6<T, R, a0, a1, a2, a3, a4, a5> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M6(const char* p_name, const char* p_class_name, int p_argc, M6<T, R, a0, a1, a2, a3, a4, a5> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 6) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 6 - default_arg_count));
		} else if (args_given > 6) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 6 argument(s).", 6 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 6 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_6<T, _Tmethod, R>(self, method, args);
	}
};
template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
class _MethodBind_M6_c : public MethodBind {
	typedef M6_c<T, R, a0, a1, a2, a3, a4, a5> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M6_c(const char* p_name, const char* p_class_name, int p_argc, M6_c<T, R, a0, a1, a2, a3, a4, a5> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 6) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 6 - default_arg_count));
		} else if (args_given > 6) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 6 argument(s).", 6 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 6 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_6<T, _Tmethod, R>(self, method, args);
	}
};

template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_7(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]);
}
template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_method_7(Object* self, const M& method, stdvec<var*>& args) {
	(((T*)self)->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]); return var();
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
class _MethodBind_M7 : public MethodBind {
	typedef M7<T, R, a0, a1, a2, a3, a4, a5, a6> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M7(const char* p_name, const char* p_class_name, int p_argc, M7<T, R, a0, a1, a2, a3, a4, a5, a6> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 7) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 7 - default_arg_count));
		} else if (args_given > 7) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 7 argument(s).", 7 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 7 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_7<T, _Tmethod, R>(self, method, args);
	}
};
template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
class _MethodBind_M7_c : public MethodBind {
	typedef M7_c<T, R, a0, a1, a2, a3, a4, a5, a6> _Tmethod;
	_Tmethod method;
public:
	_MethodBind_M7_c(const char* p_name, const char* p_class_name, int p_argc, M7_c<T, R, a0, a1, a2, a3, a4, a5, a6> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 7) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 7 - default_arg_count));
		} else if (args_given > 7) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 7 argument(s).", 7 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 7 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_method_7<T, _Tmethod, R>(self, method, args);
	}
};
template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_0(const F& static_func, stdvec<var*>& args) {
	return static_func();
}
template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_0(const F& static_func, stdvec<var*>& args) {
	static_func(); return var();
}

template<typename R>
class _StaticFuncBind_F0 : public StaticFuncBind {
	typedef F0<R> _Tfunc;
	_Tfunc static_func;
public:
	_StaticFuncBind_F0(const char* p_name, const char* p_class_name, int p_argc, F0<R> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 0) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 0 - default_arg_count));
		} else if (args_given > 0) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 0 argument(s).", 0 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 0 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_static_func_0<_Tfunc, R>(static_func, args);
	}

};

template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_1(const F& static_func, stdvec<var*>& args) {
	return static_func(*args[0]);
}
template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_1(const F& static_func, stdvec<var*>& args) {
	static_func(*args[0]); return var();
}

template<typename R, typename a0>
class _StaticFuncBind_F1 : public StaticFuncBind {
	typedef F1<R, a0> _Tfunc;
	_Tfunc static_func;
public:
	_StaticFuncBind_F1(const char* p_name, const char* p_class_name, int p_argc, F1<R, a0> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 1) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 1 - default_arg_count));
		} else if (args_given > 1) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 1 argument(s).", 1 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 1 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_static_func_1<_Tfunc, R>(static_func, args);
	}

};

template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_2(const F& static_func, stdvec<var*>& args) {
	return static_func(*args[0], *args[1]);
}
template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_2(const F& static_func, stdvec<var*>& args) {
	static_func(*args[0], *args[1]); return var();
}

template<typename R, typename a0, typename a1>
class _StaticFuncBind_F2 : public StaticFuncBind {
	typedef F2<R, a0, a1> _Tfunc;
	_Tfunc static_func;
public:
	_StaticFuncBind_F2(const char* p_name, const char* p_class_name, int p_argc, F2<R, a0, a1> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 2) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 2 - default_arg_count));
		} else if (args_given > 2) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 2 argument(s).", 2 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 2 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_static_func_2<_Tfunc, R>(static_func, args);
	}

};

template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_3(const F& static_func, stdvec<var*>& args) {
	return static_func(*args[0], *args[1], *args[2]);
}
template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_3(const F& static_func, stdvec<var*>& args) {
	static_func(*args[0], *args[1], *args[2]); return var();
}

template<typename R, typename a0, typename a1, typename a2>
class _StaticFuncBind_F3 : public StaticFuncBind {
	typedef F3<R, a0, a1, a2> _Tfunc;
	_Tfunc static_func;
public:
	_StaticFuncBind_F3(const char* p_name, const char* p_class_name, int p_argc, F3<R, a0, a1, a2> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 3) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 3 - default_arg_count));
		} else if (args_given > 3) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 3 argument(s).", 3 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 3 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_static_func_3<_Tfunc, R>(static_func, args);
	}

};

template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_4(const F& static_func, stdvec<var*>& args) {
	return static_func(*args[0], *args[1], *args[2], *args[3]);
}
template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_4(const F& static_func, stdvec<var*>& args) {
	static_func(*args[0], *args[1], *args[2], *args[3]); return var();
}

template<typename R, typename a0, typename a1, typename a2, typename a3>
class _StaticFuncBind_F4 : public StaticFuncBind {
	typedef F4<R, a0, a1, a2, a3> _Tfunc;
	_Tfunc static_func;
public:
	_StaticFuncBind_F4(const char* p_name, const char* p_class_name, int p_argc, F4<R, a0, a1, a2, a3> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 4) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 4 - default_arg_count));
		} else if (args_given > 4) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 4 argument(s).", 4 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 4 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_static_func_4<_Tfunc, R>(static_func, args);
	}

};

template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_5(const F& static_func, stdvec<var*>& args) {
	return static_func(*args[0], *args[1], *args[2], *args[3], *args[4]);
}
template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_5(const F& static_func, stdvec<var*>& args) {
	static_func(*args[0], *args[1], *args[2], *args[3], *args[4]); return var();
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
class _StaticFuncBind_F5 : public StaticFuncBind {
	typedef F5<R, a0, a1, a2, a3, a4> _Tfunc;
	_Tfunc static_func;
public:
	_StaticFuncBind_F5(const char* p_name, const char* p_class_name, int p_argc, F5<R, a0, a1, a2, a3, a4> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 5) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 5 - default_arg_count));
		} else if (args_given > 5) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 5 argument(s).", 5 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 5 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_static_func_5<_Tfunc, R>(static_func, args);
	}

};

template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_6(const F& static_func, stdvec<var*>& args) {
	return static_func(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]);
}
template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_6(const F& static_func, stdvec<var*>& args) {
	static_func(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]); return var();
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
class _StaticFuncBind_F6 : public StaticFuncBind {
	typedef F6<R, a0, a1, a2, a3, a4, a5> _Tfunc;
	_Tfunc static_func;
public:
	_StaticFuncBind_F6(const char* p_name, const char* p_class_name, int p_argc, F6<R, a0, a1, a2, a3, a4, a5> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 6) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 6 - default_arg_count));
		} else if (args_given > 6) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 6 argument(s).", 6 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 6 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_static_func_6<_Tfunc, R>(static_func, args);
	}

};

template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_7(const F& static_func, stdvec<var*>& args) {
	return static_func(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]);
}
template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_7(const F& static_func, stdvec<var*>& args) {
	static_func(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]); return var();
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
class _StaticFuncBind_F7 : public StaticFuncBind {
	typedef F7<R, a0, a1, a2, a3, a4, a5, a6> _Tfunc;
	_Tfunc static_func;
public:
	_StaticFuncBind_F7(const char* p_name, const char* p_class_name, int p_argc, F7<R, a0, a1, a2, a3, a4, a5, a6> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 7) { /* Args not enough. */
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 7 - default_arg_count));
		} else if (args_given > 7) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_ERROR(Error::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_ERROR(Error::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 7 argument(s).", 7 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 7 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		return _internal_call_static_func_7<_Tfunc, R>(static_func, args);
	}

};

template<typename T, typename R>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M0<T, R> m,
		 stdvec<var> default_args = {}) {
		
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>( ), make_stdvec<VarTypeInfo>(), ret, false, default_args, 0 );
	return newptr<_MethodBind_M0<T, R>>(method_name, p_class_name, 0, m, mi);
}

template<typename T, typename R>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M0_c<T, R> m,
		 stdvec<var> default_args = {}) {
		
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>( ), make_stdvec<VarTypeInfo>(), ret, false, default_args, 0 );
	return newptr<_MethodBind_M0_c<T, R>>(method_name, p_class_name, 0, m, mi);
}

template<typename T, typename R, typename a0>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M1<T, R, a0> m,
		const String& name0, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0 ), make_stdvec<VarTypeInfo>(vt0), ret, false, default_args, 1 );
	return newptr<_MethodBind_M1<T, R, a0>>(method_name, p_class_name, 1, m, mi);
}

template<typename T, typename R, typename a0>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M1_c<T, R, a0> m,
		const String& name0, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0 ), make_stdvec<VarTypeInfo>(vt0), ret, false, default_args, 1 );
	return newptr<_MethodBind_M1_c<T, R, a0>>(method_name, p_class_name, 1, m, mi);
}

template<typename T, typename R, typename a0, typename a1>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M2<T, R, a0, a1> m,
		const String& name0, const String& name1, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1 ), make_stdvec<VarTypeInfo>(vt0, vt1), ret, false, default_args, 2 );
	return newptr<_MethodBind_M2<T, R, a0, a1>>(method_name, p_class_name, 2, m, mi);
}

template<typename T, typename R, typename a0, typename a1>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M2_c<T, R, a0, a1> m,
		const String& name0, const String& name1, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1 ), make_stdvec<VarTypeInfo>(vt0, vt1), ret, false, default_args, 2 );
	return newptr<_MethodBind_M2_c<T, R, a0, a1>>(method_name, p_class_name, 2, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M3<T, R, a0, a1, a2> m,
		const String& name0, const String& name1, const String& name2, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2), ret, false, default_args, 3 );
	return newptr<_MethodBind_M3<T, R, a0, a1, a2>>(method_name, p_class_name, 3, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M3_c<T, R, a0, a1, a2> m,
		const String& name0, const String& name1, const String& name2, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2), ret, false, default_args, 3 );
	return newptr<_MethodBind_M3_c<T, R, a0, a1, a2>>(method_name, p_class_name, 3, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M4<T, R, a0, a1, a2, a3> m,
		const String& name0, const String& name1, const String& name2, const String& name3, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3), ret, false, default_args, 4 );
	return newptr<_MethodBind_M4<T, R, a0, a1, a2, a3>>(method_name, p_class_name, 4, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M4_c<T, R, a0, a1, a2, a3> m,
		const String& name0, const String& name1, const String& name2, const String& name3, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3), ret, false, default_args, 4 );
	return newptr<_MethodBind_M4_c<T, R, a0, a1, a2, a3>>(method_name, p_class_name, 4, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M5<T, R, a0, a1, a2, a3, a4> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4), ret, false, default_args, 5 );
	return newptr<_MethodBind_M5<T, R, a0, a1, a2, a3, a4>>(method_name, p_class_name, 5, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M5_c<T, R, a0, a1, a2, a3, a4> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4), ret, false, default_args, 5 );
	return newptr<_MethodBind_M5_c<T, R, a0, a1, a2, a3, a4>>(method_name, p_class_name, 5, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M6<T, R, a0, a1, a2, a3, a4, a5> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5), ret, false, default_args, 6 );
	return newptr<_MethodBind_M6<T, R, a0, a1, a2, a3, a4, a5>>(method_name, p_class_name, 6, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M6_c<T, R, a0, a1, a2, a3, a4, a5> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5), ret, false, default_args, 6 );
	return newptr<_MethodBind_M6_c<T, R, a0, a1, a2, a3, a4, a5>>(method_name, p_class_name, 6, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M7<T, R, a0, a1, a2, a3, a4, a5, a6> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, const String& name6, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5); DECLARE_VAR_TYPE(vt6, a6);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5, name6 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5, vt6), ret, false, default_args, 7 );
	return newptr<_MethodBind_M7<T, R, a0, a1, a2, a3, a4, a5, a6>>(method_name, p_class_name, 7, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M7_c<T, R, a0, a1, a2, a3, a4, a5, a6> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, const String& name6, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5); DECLARE_VAR_TYPE(vt6, a6);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5, name6 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5, vt6), ret, false, default_args, 7 );
	return newptr<_MethodBind_M7_c<T, R, a0, a1, a2, a3, a4, a5, a6>>(method_name, p_class_name, 7, m, mi);
}

template<typename R>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F0<R> f,
		 stdvec<var> default_args = {}) {
		
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>( ), make_stdvec<VarTypeInfo>(), ret, true, default_args, 0 );
	return newptr<_StaticFuncBind_F0<R>>(func_name, p_class_name, 0, f, mi);
}

template<typename R, typename a0>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F1<R, a0> f,
		const String& name0, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0 ), make_stdvec<VarTypeInfo>(vt0), ret, true, default_args, 1 );
	return newptr<_StaticFuncBind_F1<R, a0>>(func_name, p_class_name, 1, f, mi);
}

template<typename R, typename a0, typename a1>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F2<R, a0, a1> f,
		const String& name0, const String& name1, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1 ), make_stdvec<VarTypeInfo>(vt0, vt1), ret, true, default_args, 2 );
	return newptr<_StaticFuncBind_F2<R, a0, a1>>(func_name, p_class_name, 2, f, mi);
}

template<typename R, typename a0, typename a1, typename a2>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F3<R, a0, a1, a2> f,
		const String& name0, const String& name1, const String& name2, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2), ret, true, default_args, 3 );
	return newptr<_StaticFuncBind_F3<R, a0, a1, a2>>(func_name, p_class_name, 3, f, mi);
}

template<typename R, typename a0, typename a1, typename a2, typename a3>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F4<R, a0, a1, a2, a3> f,
		const String& name0, const String& name1, const String& name2, const String& name3, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2, name3 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3), ret, true, default_args, 4 );
	return newptr<_StaticFuncBind_F4<R, a0, a1, a2, a3>>(func_name, p_class_name, 4, f, mi);
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F5<R, a0, a1, a2, a3, a4> f,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2, name3, name4 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4), ret, true, default_args, 5 );
	return newptr<_StaticFuncBind_F5<R, a0, a1, a2, a3, a4>>(func_name, p_class_name, 5, f, mi);
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F6<R, a0, a1, a2, a3, a4, a5> f,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5), ret, true, default_args, 6 );
	return newptr<_StaticFuncBind_F6<R, a0, a1, a2, a3, a4, a5>>(func_name, p_class_name, 6, f, mi);
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F7<R, a0, a1, a2, a3, a4, a5, a6> f,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, const String& name6, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5); DECLARE_VAR_TYPE(vt6, a6);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5, name6 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5, vt6), ret, true, default_args, 7 );
	return newptr<_StaticFuncBind_F7<R, a0, a1, a2, a3, a4, a5, a6>>(func_name, p_class_name, 7, f, mi);
}


template<typename T, typename R>
using MVA = R(T::*)(stdvec<var*>&);

template<typename R>
using FVA = R(*)(stdvec<var*>&);


template <typename T, class M, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_va(Object* self, const M& method, stdvec<var*>& args) {
	return (((T*)self)->*method)(args);
}

template <typename T, class M, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_va(Object* self, const M& method, stdvec<var*>& args) {{
	(((T*)self)->*method)(args); return var();
}}

template<typename T, typename R>
class _MethodBind_MVA : public MethodBind {
	typedef MVA<T, R> _Tmethod_va;
	_Tmethod_va method;
public:
	_MethodBind_MVA(const char* p_name, const char* p_class_name, MVA<T, R> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = -1;
		method = p_method;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(Object* self, stdvec<var*>& args) const override {
		return _internal_call_static_func_va<T, _Tmethod_va, R>(self, method, args);
	}

	const ptr<MethodInfo> get_method_info() const { return mi; }
};

// -----------------------------------------------------------

template <class F, typename _TRet, typename std::enable_if<!std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_va(const F& static_func, stdvec<var*>& args) {
	return static_func(args);
}

template <class F, typename _TRet, typename std::enable_if<std::is_same<_TRet, void>::value, bool>::type = true>
inline var _internal_call_static_func_va(const F& static_func, stdvec<var*>& args) {{
	static_func(args); return var();
}}


template<typename R>
class _StaticFuncBind_FVA : public StaticFuncBind {
	typedef FVA<R> _Tfunc_va;
	_Tfunc_va static_func;
public:
	_StaticFuncBind_FVA(const char* p_name, const char* p_class_name, FVA<R> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = -1;
		static_func = p_func;
		mi = p_mi;
		mi->_set_bind((void*)this);
	}
	virtual var call(stdvec<var*>& args) const override {
		return _internal_call_static_func_va<_Tfunc_va, R>(static_func, args);
	}

	const ptr<MethodInfo> get_method_info() const { return mi; }
};

template<typename T, typename R>
ptr<MethodBind> _bind_va_method(const char* method_name, const char* p_class_name, MVA<T, R> m) {
	DECLARE_VAR_TYPE(ret, R);
	ptr<MethodInfo> mi = newptr<MethodInfo>( method_name, make_stdvec<String>(), make_stdvec<VarTypeInfo>(), ret, false, make_stdvec<var>(), -1);
	return newptr<_MethodBind_MVA<T, R>>(method_name, p_class_name, m, mi);
}

template<typename R>
ptr<StaticFuncBind> _bind_va_static_func(const char* func_name, const char* p_class_name, FVA<R> f) {
	DECLARE_VAR_TYPE(ret, R);
	ptr<MethodInfo> mi = newptr<MethodInfo>( func_name, make_stdvec<String>(), make_stdvec<VarTypeInfo>(), ret, true, make_stdvec<var>(), -1);
	return newptr<_StaticFuncBind_FVA<R>>(func_name, p_class_name, f, mi);
}
} // namespace

#endif // NATIVE_BIND_GEN_H
