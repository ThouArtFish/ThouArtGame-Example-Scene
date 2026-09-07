#include <BaseStateClass.hpp>

int main() {
	TAGBaseState::initGame(TAGBaseState::GameInitializer(1280, 720, "My Awesome Game"));

	// Include your game states here!

	return TAGBaseState::runGame();
}