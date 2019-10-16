#include "EngineErrors.h"
#include <iostream>
#include <cstdlib>
#include <SDL\SDL.h>
namespace gameEngine {
	void fatalError(std::string errorString){
		std::cout << errorString << std::endl;
		std::cout << "Enter any key to quit...";
		int tmp;
		std::cin >> tmp;
		exit(1);
	}
}
