#include <cstdlib>

#include "application.hpp"

int main(int argc, char* argv[])
{
  google::SetStderrLogging(google::LogSeverity::INFO);
  google::InitGoogleLogging(argv[0]);
  cln::cl_inhibit_floating_point_underflow = true;
  fLI::FLAGS_v = 100;
  InvJacApp app(std::cin, std::cout);

  if (app.ParseCommandLine(argc, argv))
  {
    return EXIT_SUCCESS;
  }
  try
  {
    app.Run();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
