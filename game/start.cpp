#include "include/MainStateClass.hpp"
#include <BaseStateClass.hpp>

int main() {
	TAGBaseState::initGame(TAGBaseState::GameInitializer(1280, 720, "My Awesome Game"));
	
	TAGBaseState::addState<MainState>("MAIN");

	return TAGBaseState::runGame();
}
