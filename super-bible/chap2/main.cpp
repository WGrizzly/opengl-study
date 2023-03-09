#include "sb7.h"
#include <iostream>

using namespace std;

class my_app : public sb7::application
{
public:
    void render(double curr_time)
    {
        static const GLfloat color[] = {1.0, 0.8, 0.2, 1.0};
        glClearBufferfv(GL_COLOR, 0, color);
    }
};
DECLARE_MAIN(my_app);