#include <glog/logging.h>
#include <mysqlx/xdevapi.h>

#include <cstddef>
#include <variant>

#include "checker.hpp"
#include "map.hpp"
#include "point.hpp"

struct DatabaseConfig
{
  std::string host = "localhost";
  std::string user = "root";
  std::string password = "root";
  std::string database = "checked_functions";
  std::string results_table = "test_results";
  std::string passed_table = "passed_results";
  std::string failed_table = "failed_results";
  std::string error_table = "error_results";
  std::string contraction_table = "contraction_results";
};

class FunctionDatabase
{
 public:
  enum class ResultType
  {
    Passed,
    Failed,
    Error,
    Contraction
  };

  FunctionDatabase(const DatabaseConfig& config)
      : session_(
            mysqlx::Session(config.host, 33060, config.user, config.password)),
        schema_(session_.getSchema(config.database)),
        results_table_(schema_.getTable(config.results_table)),
        passed_table_(schema_.getTable(config.passed_table)),
        failed_table_(schema_.getTable(config.failed_table)),
        error_table_(schema_.getTable(config.error_table)),
        contraction_table_(schema_.getTable(config.contraction_table))
  {}

  bool MapExists(const Map& map)
  {
    auto result = results_table_.select("id")
                      .where("name = :map_name")
                      .bind("map_name", map.ToStr())
                      .execute();
    return result.count() > 0;
  }

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
  mysqlx::Session session_;
  mysqlx::Schema schema_;
  mysqlx::Table results_table_;
  mysqlx::Table passed_table_;
  mysqlx::Table failed_table_;
  mysqlx::Table error_table_;
  mysqlx::Table contraction_table_;

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
      default:
        return "Unknown";
    }
  }

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
        contraction_table_.insert("result_id", "point")
            .values(result_id, contraction.point.ToStr())
            .execute();
        break;
      }
      default:
        LOG(ERROR) << "Unhandled result type in InsertResultDetails.";
        break;
    }
  }
};