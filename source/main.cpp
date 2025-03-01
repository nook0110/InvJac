#include <cstdlib>

#include "application.hpp"

void TestMap(const Map& map, FunctionDatabase& db);
int main(int argc, char* argv[])
{
  google::SetStderrLogging(google::LogSeverity::INFO);
  google::InitGoogleLogging(argv[0]);
  cln::cl_inhibit_floating_point_underflow = true;

  InvJacApp app;
  if (app.ParseCommandLine(argc, argv))
  {
    return EXIT_SUCCESS;
  }
  app.Run();
  return EXIT_SUCCESS;
}
