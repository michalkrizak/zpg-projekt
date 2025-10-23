#include "Application.h"
#include <iostream>

int main() {
    try {
        Application app(2000, 1000, "ZPG");
        app.initialize();
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
