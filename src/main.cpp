#include <gfx/application.h>
#include <gfx/MainLayer.h>

int main()
{
    //create the application
    MnesApplication app;
    app.AddLayer(Main());
    //run it
    app.Run();

    return 0;
}
