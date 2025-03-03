#pragma once

#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>

#include "checker.hpp"
#include "map.hpp"
#include "map_factory.hpp"
#include "mysql.hpp"

/**
 * @brief Strategy interface for generating maps.
 */
class MapGeneratorStrategy
{
 public:
  /**
   * @brief Generates a map.
   * @return Generated map.
   */
  virtual Map Generate() const = 0;

  virtual ~MapGeneratorStrategy() = default;
};

/**
 * @brief Strategy for generating maps from user input.
 */
class UserInputMapStrategy : public MapGeneratorStrategy
{
 public:
  /**
   * @brief Constructs a UserInputMapStrategy.
   * @param input_stream Input stream for user input.
   * @param output_stream Output stream for messages.
   */
  explicit UserInputMapStrategy(std::istream& input_stream,
                                std::ostream& output_stream)
      : input(input_stream), out(output_stream)
  {}

  /**
   * @brief Generates a map from user input.
   * @return Generated map.
   */
  Map Generate() const override
  {
    out << "Input your map:\n";
    return MapFactory::CreateMapFromInput(input, out);
  }

 private:
  std::istream& input;  ///< Input stream for user input.
  std::ostream& out;    ///< Output stream for messages.
};

/**
 * @brief Class to check maps using a specified strategy.
 */
class MapChecker
{
 public:
  using Settings = CheckerSettings;

  /**
   * @brief Constructs a MapChecker with the given settings and strategy.
   * @param settings Settings for the checker.
   * @param strategy Strategy for generating maps.
   */
  explicit MapChecker(const Settings& settings = {},
                      std::unique_ptr<MapGeneratorStrategy> strategy =
                          std::make_unique<UserInputMapStrategy>(std::cin,
                                                                 std::cout))
      : checker_(settings), strategy_(std::move(strategy))
  {}

  /**
   * @brief Sets the strategy for generating maps.
   * @param strategy New strategy for generating maps.
   */
  void SetStrategy(std::unique_ptr<MapGeneratorStrategy> strategy)
  {
    strategy_ = std::move(strategy);
  }

  /**
   * @brief Sets the settings for the checker.
   * @param settings New settings for the checker.
   */
  void SetSettings(const Settings& settings) { checker_ = Checker(settings); }

  /**
   * @brief Performs the check on the generated map.
   * @return Result of the check.
   */
  CheckResult PerformCheck()
  {
    auto map = GenerateMap();
    checker_.SetMap(&map);
    auto result = checker_.PerformCheck();
    return CheckResult(std::move(map), std::move(result));
  }

 private:
  /**
   * @brief Generates a map using the current strategy.
   * @return Generated map.
   */
  Map GenerateMap() const { return strategy_->Generate(); }

  Checker checker_;  ///< Checker instance.
  std::unique_ptr<MapGeneratorStrategy>
      strategy_;  ///< Strategy for generating maps.
};

/**
 * @brief Main application class for InvJac.
 */
class InvJacApp
{
 public:
  /**
   * @brief Constructs an InvJacApp with the given input and output streams.
   * @param input_stream Input stream for user input.
   * @param output_stream Output stream for messages.
   */
  InvJacApp(std::istream& input_stream = std::cin,
            std::ostream& output_stream = std::cout)
      : input_(input_stream), out_(output_stream)
  {}

  /**
   * @brief Runs the InvJac application.
   */
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

  /**
   * @brief Parses command line arguments.
   * @param argc Argument count.
   * @param argv Argument values.
   * @return True if help message is displayed, false otherwise.
   */
  bool ParseCommandLine(int argc, char* argv[])
  {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        // clang-format off
        (
            "help,h", 
            "produce help message"
        )
        (
            "input-file,i", 
            po::value<std::string>(), 
            "input file"
        )
        (
            "use-database,d",
            po::bool_switch(&use_database_)->default_value(true),
            "use MySQL database"
        )
        (
            "db-host,h", 
            po::value<std::string>()->default_value("localhost"),
            "database host"
        )
        (
            "db-user,u",
            po::value<std::string>()->default_value("root"),
            "database user"
        )
        (
            "db-password,p", 
            po::value<std::string>()->default_value("root"),
            "database password"
        )
        (
            "db-name", 
            po::value<std::string>()->default_value("checked_functions"),
            "database name"
        )
        (
            "results-table",
            po::value<std::string>()->default_value("test_results"),
            "results table"
        )
        (
            "passed-table",
            po::value<std::string>()->default_value("passed_results"),
            "passed table"
        )
        (
            "failed-table",
            po::value<std::string>()->default_value("failed_results"),
            "failed table"
        )
        (
            "error-table", 
            po::value<std::string>()->default_value("error_results"),
            "error table"
        );
    // clang-format on

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

    use_database_ =
        vm.count("use-database") ? vm["use-database"].as<bool>() : false;
    if (use_database_)
    {
      DatabaseConfig config;
      config.host = vm["db-host"].as<std::string>();
      config.user = vm["db-user"].as<std::string>();
      config.password = vm["db-password"].as<std::string>();
      config.database = vm["db-name"].as<std::string>();
      config.results_table = vm["results-table"].as<std::string>();
      config.passed_table = vm["passed-table"].as<std::string>();
      config.failed_table = vm["failed-table"].as<std::string>();
      config.error_table = vm["error-table"].as<std::string>();

      database_ = std::make_unique<FunctionDatabase>(config);
    }
    return false;
  }

 private:
  std::istream& input_ = std::cin;  ///< Input stream for user input.
  std::ostream& out_ = std::cout;   ///< Output stream for messages.
  std::ifstream input_file_;        ///< Input file stream.

  bool use_database_ = false;                   ///< Flag to use the database.
  std::unique_ptr<FunctionDatabase> database_;  ///< Database instance.
  MapChecker map_checker_;                      ///< Map checker instance.
};
