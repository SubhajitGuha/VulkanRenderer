#include <iostream>
#include "application.hpp"
#include "memory"

int main(void)
{
    std::shared_ptr<Application> app = std::make_shared<Application>();
    app->Run();
    
    return 0;
}

