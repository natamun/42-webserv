#include <exception>
#include <iostream>
#include <vector>

#include "ConfigParser.hpp"

int main()
{
    try
    {
        ConfigParser parser("tests/.conf");
        std::vector<Server> servers = parser.parse();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}