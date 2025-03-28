#pragma once
#include <glog/logging.h>
#include <mysqlx/xdevapi.h>

#include <variant>

#include "checker.hpp"
#include "map.hpp"
#include "point.hpp"

/**
 * @brief Configuration for the database connection.
 */
struct DatabaseConfig
{
  std::string host = "localhost";               ///< Database host.
  std::string user = "root";                    ///< Database user.
  std::string password = "root";                ///< Database password.
  std::string database = "checked_functions";   ///< Database name.
  std::string results_table = "test_results";   ///< Results table name.
  std::string passed_table = "passed_results";  ///< Passed results table name.
  std::string failed_table = "failed_results";  ///< Failed results table name.
  std::string error_table = "error_results";    ///< Error results table name.
  std::string contraction_table =
      "contraction_results";  ///< Contraction results table name.
  std::string keller_table = "keller_results";  ///< Keller results table name.
};

/**
 * @brief Class to interact with the function database.
 */
class FunctionDatabase
{
 public:
  /**
   * @brief Enum representing the type of result.
   */
  enum class ResultType
  {
    Passed,
    Failed,
    Error,
    Contraction,
    Keller  ///< New Keller result type
  };

  /**
   * @brief Constructs a FunctionDatabase with the given configuration.
   * @param config Database configuration.
   */
  FunctionDatabase(const DatabaseConfig& config)
      : session_(
            mysqlx::Session(config.host, 33060, config.user, config.password)),
        schema_(session_.getSchema(config.database)),
        results_table_(schema_.getTable(config.results_table)),
        passed_table_(schema_.getTable(config.passed_table)),
        failed_table_(schema_.getTable(config.failed_table)),
        error_table_(schema_.getTable(config.error_table)),
        contraction_table_(schema_.getTable(config.contraction_table)),
        keller_table_(schema_.getTable(config.keller_table))
  {}

  /**
   * @brief Checks if a map exists in the database.
   * @param map Map to check.
   * @return True if the map exists, false otherwise.
   */
  bool MapExists(const Map& map)
  {
    auto result = results_table_.select("id")
                      .where("name = :map_name")
                      .bind("map_name", map.ToStr())
                      .execute();
    return result.count() > 0;
  }

  /**
   * @brief Inserts a check result into the database.
   * @param check_result Check result to insert.
   */
  void InsertCheckResult(const CheckResult& check_result)
  {
    const auto& map = check_result.GetMap();
    const auto& result = check_result.GetResult();

    ResultType result_type;
    if (std::holds_alternative<CheckResult::Passed>(result))
    {
      result_type = ResultType::Passed;
    }
    else if (std::holds_alternative<CheckResult::Failed>(result))
    {
      result_type = ResultType::Failed;
    }
    else if (std::holds_alternative<CheckResult::Error>(result))
    {
      result_type = ResultType::Error;
    }
    else if (std::holds_alternative<CheckResult::Contraction>(result))
    {
      result_type = ResultType::Contraction;
    }
    else if (std::holds_alternative<CheckResult::Keller>(result))
    {
      result_type = ResultType::Keller;
    }
    else
    {
      LOG(ERROR) << "Unhandled result variant in InsertCheckResult.";
      return;
    }

    auto insert_result = results_table_.insert("name", "result_type")
                             .values(map.ToStr(), ToString(result_type))
                             .execute();
    auto result_id = insert_result.getAutoIncrementValue();

    InsertResultDetails(result_id, result, result_type);
  }

 private:
  mysqlx::Session session_;          ///< MySQL session.
  mysqlx::Schema schema_;            ///< MySQL schema.
  mysqlx::Table results_table_;      ///< Results table.
  mysqlx::Table passed_table_;       ///< Passed results table.
  mysqlx::Table failed_table_;       ///< Failed results table.
  mysqlx::Table error_table_;        ///< Error results table.
  mysqlx::Table contraction_table_;  ///< Contraction results table.
  mysqlx::Table keller_table_;       ///< Keller results table.

  /**
   * @brief Converts a ResultType to a string.
   * @param result_type Result type to convert.
   * @return String representation of the result type.
   */
  std::string ToString(ResultType result_type)
  {
    switch (result_type)
    {
      case ResultType::Passed:
        return "Passed";
      case ResultType::Failed:
        return "Failed";
      case ResultType::Error:
        return "Error";
      case ResultType::Contraction:
        return "Contraction";
      case ResultType::Keller:
        return "Keller";
      default:
        return "Unknown";
    }
  }

  /**
   * @brief Inserts result details into the appropriate table.
   * @param result_id ID of the result.
   * @param result Result to insert.
   * @param result_type Type of the result.
   */
  void InsertResultDetails(std::int64_t result_id,
                           const CheckResult::Result& result,
                           ResultType result_type)
  {
    switch (result_type)
    {
      case ResultType::Passed:
      {
        const auto& passed = std::get<CheckResult::Passed>(result);
        passed_table_.insert("result_id", "amount_of_tests")
            .values(result_id, passed.check_amount)
            .execute();
        break;
      }
      case ResultType::Failed:
      {
        const auto& failed = std::get<CheckResult::Failed>(result);
        failed_table_.insert("result_id", "test", "result")
            .values(result_id, failed.point.ToStr(), ToStr(failed.value))
            .execute();
        break;
      }
      case ResultType::Error:
      {
        const auto& error = std::get<CheckResult::Error>(result);
        error_table_.insert("result_id", "error")
            .values(result_id, error.message)
            .execute();
        break;
      }
      case ResultType::Contraction:
      {
        const auto& contraction = std::get<CheckResult::Contraction>(result);
        contraction_table_.insert("result_id", "contraction_point")
            .values(result_id, contraction.point.ToStr())
            .execute();
        break;
      }
      case ResultType::Keller:
      {
        const auto& keller = std::get<CheckResult::Keller>(result);
        keller_table_.insert("result_id", "jacobian")
            .values(result_id, ToStr(keller.jacobian))
            .execute();
        break;
      }
      default:
        LOG(ERROR) << "Unhandled result type in InsertResultDetails.";
        break;
    }
  }
};