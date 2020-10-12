#pragma once

#include <tuple>
#include <utility>
#include <memory>

template<typename ReturnType>
struct impl_base {
	virtual ReturnType call() = 0;

	virtual ~impl_base() {}
};

template<typename ReturnType, typename Owner, typename Method, typename... Args>
struct impl : public impl_base<ReturnType> {
	Owner owner;
	Method method;
	std::tuple<Args...> args;

	template<typename... Params>
	impl(Owner o, Method m, Params&&... a)
		: owner(o),
		method(m),
		args(std::forward<Args>(a)...) 
	{
		// TODO: check if Params == Args
		//static_assert(std::is_same_v<std::decay_t<C>, std::decay_t<Args>>..., "Wrong function arguments\n");
	}

	template<typename... Params, std::size_t... Indices>
	ReturnType invoke_method(std::tuple<Params...>& tup, std::index_sequence<Indices...>) {
		return (owner->*method)(std::forward<Params>(std::get<Indices>(tup))...);
	}

	template<typename... Params>
	ReturnType invoke_method(std::tuple<Params...>& tup) {
		return invoke_method(tup, std::make_index_sequence<sizeof...(Params)>{});
	}

	ReturnType call() override {
		if (nullptr == owner) {
			throw std::runtime_error("Caller was nullptr");
		}
		return invoke_method(args);
	}
};


template<typename T>
struct delegate {
private:
	std::unique_ptr<impl_base<T>> base;

public:
	template<typename Owner, typename Method, typename... Args>
	delegate(Owner&& owner, Method&& method, Args... args) : 
		base{ std::make_unique<impl<T, Owner, Method, std::decay_t<Args>...>>
			(owner, method, std::forward<Args>(args)...) }
	{			
		static_assert(std::is_pointer_v<std::remove_reference_t<Owner>>, "Caller should be pointer to object");
		static_assert(std::is_member_function_pointer_v<Method>, "Caller should be pointer to object");
	}

	delegate(const delegate<T> & other) = default;
	delegate<T> & operator=(const delegate<T> & other) = default;

	delegate(delegate<T> && other) = default;
	delegate<T> & operator=(delegate<T> && other) = default;


	// TODO: put arguments through 'call'
	T call() {
		return base->call();
	}
};
