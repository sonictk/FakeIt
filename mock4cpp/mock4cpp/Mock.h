#ifndef Mock_h__
#define Mock_h__

#include <type_traits>
#include "../mockutils/DynamicProxy.h"
#include "ClousesImpl.h"
#include "mockito_clouses.h"
#include "MockRepository.h"

using namespace mock4cpp;
using namespace mock4cpp::clouses;

// template <typename R>
// typename std::enable_if<!std::is_void<R>::value, mock4cpp::mockito::clouses::FirstFunctionWhenClouse<R>&>::type
// When(const R& r) {
// 	throw 1;
// }
// 
// template <typename R>
// typename std::enable_if<std::is_void<R>::value, mock4cpp::mockito::clouses::FirstProcedureWhenClouse<R>&>::type
// When(R) {
// 	throw 1;
// }

template <typename C>
struct Mock : private MockBase
{	

	static_assert(std::is_polymorphic<C>::value, "Can only mock a polymorphic type");

	Mock() : MockBase{}, instance{}{
	}
	
	~Mock(){
	}
	
	C& get()
	{
		return instance.get();
	}

	C& operator()()
	{
		return instance.get();
	}

	template <typename R, typename... arglist, class = typename std::enable_if<!std::is_void<R>::value>::type>
	StubFunctionClouse<R, arglist...> When(R(C::*vMethod)(arglist...) const){
		auto methodWithoutConstVolatile = reinterpret_cast<R(C::*)(arglist...)>(vMethod);
		return When(methodWithoutConstVolatile);
	}

	template < typename R, typename... arglist, class = typename std::enable_if<!std::is_void<R>::value>::type>
	StubFunctionClouse<R, arglist...> When(R(C::*vMethod)(arglist...) volatile){
		auto methodWithoutConstVolatile = reinterpret_cast<R(C::*)(arglist...)>(vMethod);
		return When(methodWithoutConstVolatile);
	}

	template <typename R, typename... arglist, class = typename std::enable_if<!std::is_void<R>::value>::type>
	StubFunctionClouse<R, arglist...> When(R(C::*vMethod)(arglist...) const volatile){
		auto methodWithoutConstVolatile = reinterpret_cast<R(C::*)(arglist...)>(vMethod);
		return When(methodWithoutConstVolatile);
	}

	template <typename R, typename... arglist, class = typename std::enable_if<!std::is_void<R>::value>::type>
	StubFunctionClouse<typename R, arglist...> When(R(C::*vMethod)(arglist...)) {
		return StubImpl(vMethod);
	}

	//		

	template <typename R, typename... arglist, class = typename std::enable_if<std::is_void<R>::value>::type>
	StubProcedureClouse<R, arglist...> When(R(C::*vMethod)(arglist...) const){
		auto methodWithoutConstVolatile = reinterpret_cast<R(C::*)(arglist...)>(vMethod);
		return When(methodWithoutConstVolatile);
	}

	template <typename R, typename... arglist, class = typename std::enable_if<std::is_void<R>::value>::type>
	StubProcedureClouse<R, arglist...> When(R(C::*vMethod)(arglist...) volatile){
		auto methodWithoutConstVolatile = reinterpret_cast<R(C::*)(arglist...)>(vMethod);
		return When(methodWithoutConstVolatile);
	}

	template <typename R, typename... arglist, class = typename std::enable_if<std::is_void<R>::value>::type>
	StubProcedureClouse<R, arglist...> When(R(C::*vMethod)(arglist...) const volatile){
		auto methodWithoutConstVolatile = reinterpret_cast<R(C::*)(arglist...)>(vMethod);
		return When(methodWithoutConstVolatile);
	}

	template <typename R, typename... arglist, class = typename std::enable_if<std::is_void<R>::value>::type>
	StubProcedureClouse<R, arglist...> When(R(C::*vMethod)(arglist...)){
		auto methodWithoutConstVolatile = reinterpret_cast<R(C::*)(arglist...)>(vMethod);
		return StubImpl(methodWithoutConstVolatile);
	}

	template <class DM, typename... arglist
		, class = typename std::enable_if<std::is_member_object_pointer<DM>::value>::type
	>
	void Stub(DM member, const arglist&... ctorargs)
	{
		stubDataMember(member, ctorargs...);
	}

	template <typename H, typename... M
		, class = typename std::enable_if<std::is_member_function_pointer<H>::value>::type
	>
	void Stub(H head, M... tail) 
	{
		//auto headWithoutConstVolatile = reinterpret_cast<std::remove_cv<H>::type>(head);
		When(head);
		Stub(tail...);
	}

	template <
		typename MEMBER
		, class = typename std::enable_if<std::is_member_function_pointer<MEMBER>::value>::type
	>
	auto operator [](MEMBER member) -> decltype(When(member)) {
		return When(member);
	}

private:

	DynamicProxy<C> instance;

	template <typename R, typename... arglist>
	MethodMock<R, arglist...>* stubMethodIfNotStubbed(R(C::*vMethod)(arglist...)){
		if (!instance.isStubbed(vMethod)){
			auto methodMock = new MethodMock<R, arglist...>(*this);
			instance.stubMethod(vMethod, methodMock);
		}
		MethodMock<R, arglist...> * methodMock = instance.getMethodMock<MethodMock<R, arglist...> *>(vMethod);
		return methodMock;
	}

	template <typename R, typename... arglist, class = typename std::enable_if<!std::is_void<R>::value>::type>
	StubFunctionClouse<R, arglist...> StubImpl(R(C::*vMethod)(arglist...)){
		auto methodMock = stubMethodIfNotStubbed(vMethod);
		return StubFunctionClouse<R, arglist...>(*methodMock);
	}
	
	template <typename R, typename... arglist, class = typename std::enable_if<std::is_void<R>::value>::type>
	StubProcedureClouse<R, arglist...> StubImpl(R(C::*vMethod)(arglist...)){
		auto methodMock = stubMethodIfNotStubbed(vMethod);
		return StubProcedureClouse<R, arglist...>(*methodMock);
	}

	void Stub(){}

	template <class MEMBER_TYPE, typename... arglist>
	void stubDataMember(MEMBER_TYPE C::*member, const arglist&... ctorargs)
	{
		instance.stubDataMember(member, ctorargs...);
	}

};

// namespace mockito {
// 	
// 	template <typename C, typename R, typename... arglist>
// 	struct MethodStubbingMock : public MethodInvocationHandler<R, arglist...>
// 	{
// 		MethodStubbingMock(Mock<C>& mock) :mock{ mock }
// 		{}
// 
// 		virtual ~MethodStubbingMock() override {}
// 
// 		typename std::enable_if <std::is_void<R>::value, R> ::type
// 		handleMethodInvocation(const arglist&... args) override {
//  			R(C::*vMethod)(arglist...);
//  			StubProcedureClouse<R, arglist...>& clouse = mock.When(vMethod);
// 			// add clouse to list ...
// 			return DefaultValue::value<R>();
// 		}
// 
// 		typename std::enable_if <!std::is_void<R>::value, R> ::type
// 		handleMethodInvocation(const arglist&... args) override {
// 			R(C::*vMethod)(arglist...);
// 			StubFunctionClouse<R, arglist...>& clouse = mock.When(vMethod);
// 			// add clouse to list ...
// 			return DefaultValue::value<R>();
// 		}
// 
// 	private:
// 		Mock<C>& mock;
// 	};
// 
// }
#endif // Mock_h__