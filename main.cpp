#include "include/Server.hpp"


int main(int ac, char **av) {
    try {
        Server server(ac, av);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << RED << e.what() << RESET <<std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
