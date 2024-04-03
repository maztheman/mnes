#include <gfx/MainLayer.h>
#include <gfx/application.h>

int main()
{
  // create the application
  MnesApplication app;
  app.AddLayer(Main());
  // run it
  app.Run();

  return 0;
}
