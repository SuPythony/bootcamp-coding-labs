#include "spellcheck.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>

template <typename Iterator, typename UnaryPred>
std::vector<Iterator> find_all(Iterator begin, Iterator end, UnaryPred pred);

Corpus tokenize(std::string& source) {
  auto spaces = find_all(source.begin(), source.end(), [](char c) { return std::isspace(c); });
  Corpus tokens;
  std::transform(spaces.begin(), spaces.end()-1, spaces.begin()+1, std::inserter(tokens, tokens.begin()), [&source](auto it1, auto it2) {
    return Token{source, it1, it2};
  });
  std::erase_if(tokens, [](Token token) { return token.content.empty(); });
  return tokens;
}

std::set<Misspelling> spellcheck(const Corpus& source, const Dictionary& dictionary) {
  auto view = source 
      | std::views::filter([&dictionary](Token token) { return !dictionary.contains(token.content); })
      | std::views::transform([&dictionary](Token token) {
        auto view = dictionary | std::views::filter([&token](std::string word) {
          return levenshtein(token.content, word) == 1;
        });
        std::set<std::string> sugg(view.begin(), view.end());
        return Misspelling{token, sugg};
      }) | std::views::filter([](Misspelling miss) { return !miss.suggestions.empty(); });
  return std::set<Misspelling>(view.begin(), view.end());
};

/* Helper methods */

#include "utils.cpp"