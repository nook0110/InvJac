#pragma once

#include <boost/program_options.hpp>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>

#include "checker.hpp"
#include "map.hpp"
#include "map_factory.hpp"
#include "mysql.hpp"

class MapGeneratorStrategy
{
 public:
  virtual Map Generate() const = 0;

  virtual ~MapGeneratorStrategy() = default;
};

class UserInputMapStrategy : public MapGeneratorStrategy
{
 public:
  explicit UserInputMapStrategy(std::istream& input_stream,
                                std::ostream& output_stream)
      : input(input_stream), out(output_stream)
  {}

  Map Generate() const override
  {
    out << "Input your map:\n";
    return MapFactory::CreateMapFromInput(input, out);
  }

 private:
  std::istream& input;
  std::ostream& out;
};

class MapChecker
{
 public:
  using Settings = CheckerSettings;

  explicit MapChecker(const Settings& settings = {},
                      std::unique_ptr<MapGeneratorStrategy> strategy =
                          std::make_unique<UserInputMapStrategy>(std::cin,
                                                                 std::cout))
      : checker_(settings), strategy_(std::move(strategy))
  {}

  void SetStrategy(std::unique_ptr<MapGeneratorStrategy> strategy)
  {
    strategy_ = std::move(strategy);
  }

  void SetSettings(const Settings& settings) { checker_ = Checker(settings); }

  CheckResult PerformCheck()
  {
    auto map = GenerateMap();
    checker_.SetMap(&map);
    auto result = checker_.PerformCheck();
    return CheckResult(std::move(map), std::move(result));
  }

 private:
  Map GenerateMap() const { return strategy_->Generate(); }

  Checker checker_;
  std::unique_ptr<MapGeneratorStrategy> strategy_;
};

class InvJacApp
{
 public:
  InvJacApp(std::istream& input_stream = std::cin,
            std::ostream& output_stream = std::cout)
      : input_(input_stream), out_(output_stream)
  {}

  void Run()
  {
    out_ << "Running the InvJac application...\n";
    auto result = map_checker_.PerformCheck();
    out_ << result.ToStr() << "\n";
    if (use_database_)
    {
      database_->InsertCheckResult(result);
    }
  }

  bool ParseCommandLine(int argc, char* argv[])
  {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")(
        "input-file", "input file")("use-database",
                                    po::value<bool>()->default_value(false),
                                    "use MySQL database")(
        "db-host", po::value<std::string>()->default_value("localhost"),
        "database host")("db-user",
                         po::value<std::string>()->default_value("root"),
                         "database user")(
        "db-password", po::value<std::string>()->default_value("root"),
        "database password")(
        "db-name", po::value<std::string>()->default_value("checked_functions"),
        "database name")(
        "results-table",
        po::value<std::string>()->default_value("test_results"),
        "results table")(
        "passed-table",
        po::value<std::string>()->default_value("passed_results"),
        "passed table")(
        "failed-table",
        po::value<std::string>()->default_value("failed_results"),
        "failed table")(
        "error-table", po::value<std::string>()->default_value("error_results"),
        "error table");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
      out_ << desc << "\n";
      return true;
    }

    if (vm.count("input-file"))
    {
      input_file_.open(vm["input-file"].as<std::string>());
      if (!input_file_)
      {
        out_ << "Error opening input file.\n";
        return false;
      }
    }

    use_database_ = vm["use-database"].as<bool>();
    if (use_database_)
    {
      database_ = std::make_unique<FunctionDatabase>(
          vm["db-host"].as<std::string>(), vm["db-user"].as<std::string>(),
          vm["db-password"].as<std::string>(), vm["db-name"].as<std::string>(),
          vm["results-table"].as<std::string>(),
          vm["passed-table"].as<std::string>(),
          vm["failed-table"].as<std::string>(),
          vm["error-table"].as<std::string>());
    }
    return false;
  }

 private:
  std::istream& input_ = std::cin;
  std::ostream& out_ = std::cout;
  std::ifstream input_file_;

  bool use_database_ = false;
  std::unique_ptr<FunctionDatabase> database_;
  MapChecker map_checker_;
};
