#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>


typedef double probability_t;

class weighted_symbol_t : std::pair<std::string, probability_t> {
public:
  weighted_symbol_t(std::string symbol, probability_t prob)
    : std::pair<std::string, probability_t>(symbol, prob) {}

  std::string const& symbol() const { return first; }
  probability_t prob() const { return second; }
};

class binary_rule_t {
  std::string _lhs;
  std::pair<std::string, std::string> _rhs;
  probability_t _prob;

public:
  binary_rule_t(std::string lhs, std::string rhs1, std::string rhs2, probability_t prob)
    : _lhs(lhs),
      _rhs(make_pair(rhs1, rhs2)),
      _prob(prob)
  {}

  std::string const& lhs() const { return _lhs; }
  std::pair<std::string, std::string> const& rhs() const { return _rhs; }
  probability_t prob() const { return _prob; }
};

class unary_rule_t {
  std::string _lhs;
  std::string _rhs;
  probability_t _prob;

public:
  unary_rule_t(std::string lhs, std::string rhs, probability_t prob)
    : _lhs(lhs),
      _rhs(rhs),
      _prob(prob)
  {}

  std::string const& lhs() const { return _lhs; }
  std::string const& rhs() const { return _rhs; }
  probability_t prob() const { return _prob; }
};

class chart_t {
public:
  typedef std::vector<weighted_symbol_t> cell_type;
private:
  std::vector<std::vector<cell_type> > _chart;

  cell_type::iterator find_symbol(cell_type &cell, std::string const &symbol) {
    return std::find_if(cell.begin(), cell.end(), [&](weighted_symbol_t s) { return s.symbol() == symbol; });
  }
public:
  explicit chart_t(size_t n) {
    _chart.resize(n);
    for (size_t i = 0; i < n; ++i)
      _chart[i].resize(n - i);
  }

  cell_type& cell(size_t i, size_t j) {
    assert(j != 0);
    return _chart[i][j - 1];
  }
  weighted_symbol_t& symbol(size_t i, size_t j, std::string symbol) {
    return *find_symbol(cell(i, j), symbol);
  }

  void update(size_t i, size_t j, weighted_symbol_t const &s) {
    auto &c = cell(i,j);
    auto it = find_symbol(c, s.symbol());
    if (it == c.end()) {
      c.push_back(s);
    } else if (it->prob() < s.prob()) {
      *it = s;
    }
  }

  bool check_symbol(size_t i, size_t j, std::string const &symbol) {
    return find_symbol(cell(i,j), symbol) != cell(i,j).end();
  }
};


template <typename Container, typename Element>
bool contains(Container const &c, Element e) {
  return (std::find(c.begin(), c.end(), e) != c.end());
}

template <typename Container, typename Predicate>
bool any_of(Container const &c, Predicate p) {
  return std::any_of(c.begin(), c.end(), p);
}

std::vector<weighted_symbol_t> reachable_unary_symbols(weighted_symbol_t symbol, std::vector<unary_rule_t> const &unary_rules) {
  std::vector<weighted_symbol_t> reachable;
  std::vector<weighted_symbol_t> queue{symbol};

  // std::cerr << "reachable_unary_symbols(<" << symbol.symbol() << "," << symbol.prob() << ">)" << std::endl;

  while (!queue.empty()) {
    for (auto const &rule : unary_rules) {
      auto match = [&](weighted_symbol_t s){return s.symbol() == rule.lhs();};
      if (rule.rhs() == queue.front().symbol()
          && !any_of(reachable, match)
          && !any_of(queue, match)) {
        weighted_symbol_t s(rule.lhs(), rule.prob() * queue.front().prob());
        reachable.push_back(s);
        queue.push_back(s);
      }
    }
    queue.erase(queue.begin());
  }

  // std::cerr << "reachable: ";
  // for (auto const &s : reachable) {
  //   std::cerr << "<" << s.symbol() << "," << s.prob() << "> ";
  // }
  // std::cerr << std::endl;

  return reachable;
}


int main(int argc, char** argv) {
  std::vector<binary_rule_t> binary_rules;
  std::vector<unary_rule_t> unary_rules;

  if (argc != 2) {
    std::cerr << "Usage: parse <grammar>" << std::endl;
    exit(-1);
  }

  std::cerr << "reading rules..." << std::endl;
  std::ifstream rule_file(argv[1]);
  std::string line;
  while (std::getline(rule_file, line)) {
    auto tabpos = line.find('	');
    auto prob = stof(line.substr(tabpos + 1));

    auto pos = line.find(' ');
    std::string lhs = line.substr(0, pos);

    pos = line.find(' ', pos + 1) + 1;
    auto pos2 = line.find(' ', pos);

    if (pos2 != std::string::npos) {
      //binary rule
      std::string rhs1 = line.substr(pos, pos2 - pos);
      std::string rhs2 = line.substr(pos2 + 1, tabpos - pos2 - 1);
      binary_rules.emplace_back(lhs, rhs1, rhs2, prob);
      // std::cerr << "binary rule:<" << lhs << ">=<" << rhs1 << "><" << rhs2 << ">\n";
    } else {
      //unary rule
      std::string rhs = line.substr(pos, tabpos - pos);
      unary_rules.emplace_back(lhs, rhs, prob);
      // std::cerr << "unary rule:<" << lhs << ">=<" << rhs << ">\n";
    }
  }
  std::cerr << "finished reading rules." << std::endl;

  /* format: <word> <pos_tag> */
  std::vector<std::pair<std::string, std::string> > tokens;
  while (std::getline(std::cin, line)) {
    auto pos = line.find(' ');
    tokens.emplace_back(line.substr(0, pos), line.substr(pos + 1));
  }

  std::cerr << "finished reading " << tokens.size() << " tokens." << std::endl;

  chart_t chart(tokens.size());

  std::cerr << "created chart" << std::endl;

  for (size_t i = 0; i < tokens.size(); ++i) {
    chart.cell(i, 1).emplace_back(tokens[i].second, 1.0);
    auto reachable = reachable_unary_symbols(chart.cell(i, 1).back(), unary_rules);
    for (auto const &s : reachable) {
      chart.update(i, 1, s);
    }
  }

  std::cerr << "initialized chart" << std::endl;

  for (size_t j = 2; j <= tokens.size(); ++j) {
    for (size_t i = 0; i <= (tokens.size() - j); ++i) {
      for (size_t k = 1; k < j; ++k) {
        for (auto const &rule : binary_rules) {
          //check rule
          if (chart.check_symbol(i, k, rule.rhs().first)
              && chart.check_symbol(i + k, j - k, rule.rhs().second)) {

            probability_t p = rule.prob()
              * chart.symbol(i, k, rule.rhs().first).prob()
              * chart.symbol(i + k, j - k, rule.rhs().second).prob();

            weighted_symbol_t symbol(rule.lhs(), p);

            chart.update(i, j, symbol);

            auto reachable = reachable_unary_symbols(symbol, unary_rules);
            for (auto const &s : reachable) {
              chart.update(i, j, s);
            }
          }
        }
      }
    }
  }

  std::cerr << "finished filling chart" << std::endl;

  for (size_t j = 1; j <= tokens.size(); ++j) {
    for (size_t i = 0; i <= (tokens.size() - j); ++i) {
      for (auto const &s : chart.cell(i, j)) {
        std::cout << "(" << i << "," << i + j - 1 << ")	" << s.symbol() << "	" << s.prob() << std::endl;
      }
    }
  }

  return 0;
}
