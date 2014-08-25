#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>


struct tree {
  std::string label;
  std::vector<tree> children;
  tree(std::string l, std::vector<tree> c) : label(l), children(c) {}
};


/*
 * Read a tree-expression from the input stream.
 *
 * texpr := "(" label label ")" | "(" label texpr * ")"
 * label := [!-'*-~]+
 *
 * Essentially, a label can contain any character other than
 * whitespace or parentheses.
 */

tree read_texpr(std::istream &in) {
  std::string buff;
  std::vector<tree> stack;
  enum class tok { lparen, rparen, label, none };
  tok last_token = tok::none;

  while (in >> buff) {
    auto cur = buff.cbegin();
    auto end = buff.cend();

    std::cerr << "# " << buff << std::endl;

    while (cur != end) {
      auto next_paren = std::find_if(cur, end, [](char c) { return c == '(' or c == ')'; });
      if (cur < next_paren) {
	if (stack.empty()) {
	  throw std::runtime_error("text outside tree");
	}

	if (last_token == tok::lparen) {
	  stack.back().label = std::string(cur, next_paren);
	} else if (last_token == tok::label) {
	  stack.back().children.emplace_back(std::string(cur, next_paren), std::vector<tree>{});
	} else {
	  throw std::runtime_error("text after close paren");
	}
	last_token = tok::label;
      } else {
	if ('(' == *next_paren) {
	  stack.emplace_back(std::string{}, std::vector<tree>{});
	  last_token = tok::lparen;
	} else {
	  last_token = tok::rparen;
	  tree t = stack.back();
	  stack.pop_back();

	  if (stack.empty()) {
	    return t; //this assumes that nothing is left in `buff`
	  } else {
	    stack.back().children.push_back(t);
	  }
	}
	++next_paren;
      }

      cur = next_paren;
    }
  }

  throw std::runtime_error("mismatched paren: reached end of input");
}


int main(int argc, char** argv)
{
  int count = 0;
  try {
    while (true) {
      auto t = read_texpr(std::cin);
      ++count;
    }
  } catch(std::runtime_error e) {
    std::cout << e.what() << std::endl;
  }

  std::cout << count << " trees read." << std::endl;

  return 0;
}
