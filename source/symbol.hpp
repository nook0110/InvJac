#pragma once

#include <ginac/lst.h>
#include <ginac/registrar.h>
#include <ginac/symbol.h>

#include <format>
#include <string>
#include <vector>

using Symbol = GiNaC::symbol;

class Symbols
{
 public:
  static Symbol GetSymbol(size_t i)
  {
    FitInto(i + 1);
    return symbols_[i];
  }

  static GiNaC::lst GetSymbolsList() { return GetSymbolsList(symbols_.size()); }

  static GiNaC::lst GetSymbolsList(size_t amount)
  {
    FitInto(amount);
    GiNaC::lst symbol_list;
    for (size_t i = 0; i < amount; ++i)
    {
      symbol_list.append(GetSymbol(i));
    }
    return symbol_list;
  }

  static void FitInto(size_t i)
  {
    while (i > symbols_.size())
    {
      symbols_.emplace_back(std::format("x_{}", symbols_.size()));
    }
  }

 private:
  static std::vector<Symbol> symbols_;
};
