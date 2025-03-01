#include <glog/logging.h>
#include <mysqlx/xdevapi.h>

#include <cstddef>
#include <variant>

#include "checker.hpp"
#include "map.hpp"
#include "point.hpp"

class FunctionDatabase
{
 public:
  FunctionDatabase(const std::string& host = "localhost",
                   const std::string& user = "root",
                   const std::string& password = "root",
                   const std::string& database = "checked_functions",
                   const std::string& results_table = "test_results",
                   const std::string& passed_table = "passed_results",
                   const std::string& failed_table = "failed_results",
                   const std::string& error_table = "error_results")
      : session_(mysqlx::Session(host, 33060, user, password)),
        schema_(InitScheme(database)),

        results_table_(InitResultsTable(results_table)),
        passed_table_(InitPassedTable(passed_table)),
        failed_table_(InitFailedTable(failed_table)),
        error_table_(InitErrorTable(error_table))
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
    auto map = check_result.GetMap();
    const auto& result = check_result.GetResult();

    std::string result_type;
    if (std::holds_alternative<CheckResult::Passed>(result))
    {
      result_type = "Passed";
    }
    else if (std::holds_alternative<CheckResult::Failed>(result))
    {
      result_type = "Failed";
    }
    else if (std::holds_alternative<CheckResult::Error>(result))
    {
      result_type = "Error";
    }

    auto insert_result = results_table_.insert("name", "result_type")
                             .values(map.ToStr(), result_type)
                             .execute();
    auto result_id = insert_result.getAutoIncrementValue();

    if (result_type == "Passed")
    {
      const auto& passed = std::get<CheckResult::Passed>(result);
      passed_table_.insert("result_id", "amount_of_tests")
          .values(result_id, passed.check_amount)
          .execute();
    }
    else if (result_type == "Failed")
    {
      const auto& failed = std::get<CheckResult::Failed>(result);
      failed_table_.insert("result_id", "test", "result")
          .values(result_id, failed.point.ToStr(), ToStr(failed.value))
          .execute();
    }
    else if (result_type == "Error")
    {
      const auto& error = std::get<CheckResult::Error>(result);
      error_table_.insert("result_id", "error")
          .values(result_id, error.message)
          .execute();
    }
  }

 private:
  mysqlx::Table InitResultsTable(const std::string& results_table_name)
  {
    return schema_.getTable(results_table_name);
  }

  mysqlx::Table InitPassedTable(const std::string& passed_table_name)
  {
    return schema_.getTable(passed_table_name);
  }

  mysqlx::Table InitFailedTable(const std::string& failed_table_name)
  {
    return schema_.getTable(failed_table_name);
  }

  mysqlx::Table InitErrorTable(const std::string& error_table_name)
  {
    return schema_.getTable(error_table_name);
  }

  mysqlx::Schema InitScheme(const std::string& database)
  {
    return session_.getSchema(database);
  }

  mysqlx::Session session_;
  mysqlx::Schema schema_;
  mysqlx::Table results_table_;
  mysqlx::Table passed_table_;
  mysqlx::Table failed_table_;
  mysqlx::Table error_table_;
};