#include "Playground.h"
#include <cstdio>

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    MyApp app(argc, argv);

#if 0
    INFO("Attach RenderDoc...");
    getchar();
#endif

    app.Run();
}
