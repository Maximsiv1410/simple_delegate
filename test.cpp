#include <iostream>
#include "delegate.hpp"

	/*
	*	simple utility class 
	*	to check move semantics 
	*	by logging 
	*/

class Entity {
public:
	Entity() {
		std::cout << " === constructing entity === \n";
	}

	Entity(const Entity& other) {
		std::cout << "=== copying entity ===\n";
	}

	Entity(Entity&& other) {
		std::cout << "=== moving entity ===\n";
	}
};

class Victim {
public:
	int internal{};
	int foo(int temp, int& via_ref, Entity&& entity) {
		via_ref = 100;
		Entity local_entity = std::move(entity);
		return temp + internal + via_ref;
	}

	void bar() {
		std::cout << "called 'bar'\n";
	}
};

int main() {
	int via_ref = 0;
	Entity entity;

	Victim *x = new Victim();
	x->internal = 10;

	delegate<int> first(x, 
						&Victim::foo,
						1,
						std::ref(via_ref),
						std::move(entity));

	// trying to call nullptr object
	Victim* nptr = nullptr;
	delegate<void> second(nptr, &Victim::bar);
	

	try {
		auto res = first.call();
		std::cout << "via_ref contains: '" << via_ref << "'\n";
		std::cout << "result of call is: '" << res  << "'\n";

		second.call();
	}
	catch (std::exception& ex) {
		std::cout << "Exception: " << ex.what() << "\n";
	}

	delete x;
	return 0;
}