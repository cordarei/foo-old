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

void swap(tree &left, tree &right) {
  std::string ts;
  std::vector<tree> tv;
  swap(left.label, ts);
  swap(left.children, tv);
  swap(left.label, right.label);
  swap(left.children, right.children);
  swap(right.label, ts);
  swap(right.children, tv);
}


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

    // std::cerr << "# " << buff << std::endl;

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
            //this assumes that nothing is left in `buff`
            if (t.label.empty() && t.children.size() == 1) {
              return t.children[0];
            } else {
              return t;
            }
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


/*
 * pretty-print a tree to the given ostream
 *
 * believe me, there are infinitely better ways to do this
 */
std::ostream& operator<<(std::ostream &out, const tree &t) {
  std::vector<std::pair<int,tree const *>> stack{std::make_pair(0, &t)};

  while(!stack.empty()) {
    auto p = stack.back();
    stack.pop_back();
    for (int i = p.first; i > 0; --i)
      out << ' ';
    if (!p.second) {
      out << ')' << std::endl;
    } else if (p.second->children.size() == 1) {
      if (p.second->children[0].children.size() == 0) {
        out << '('
	    << p.second->label
	    << ' '
	    << p.second->children[0].label
	    << ')'
	    << std::endl;
      } else {
        stack.emplace_back(p.first, nullptr);
        if (!p.second->label.empty()) {
          out << '('
	      << p.second->label
	      << std::endl;
          stack.emplace_back(p.first + 2, &p.second->children[0]);
        } else {
          stack.emplace_back(p.first, &p.second->children[0]);
        }
      }
    } else if (p.second->children.size() > 1) {
      stack.emplace_back(p.first, nullptr);
      out << '('
	  << p.second->label
	  << std::endl;
      for (auto it = p.second->children.rbegin(); it != p.second->children.rend(); ++it)
        stack.emplace_back(p.first + 2, &*it);
    } else {
      out << "wtf??" << std::endl;
    }
  }

  return out;
}

/*
 * simplify tree - remove function tags, indices, and empty elements
 *
 * This is destructive! It will modify the tree in place.
 */
void simplify_tree(tree &t) {
  t.label = t.label.substr(0, t.label.find('-'));
  for (auto &c : t.children) {
    if (c.label != "-NONE-" && c.children.size() > 0)
      simplify_tree(c);
  }
}


/*
 * binarize tree - return a new tree in which each node has at most two children
 */
tree binarize_tree(const tree &t) {
  switch(t.children.size()) {
  case 0:
    return tree(t.label, {});
  case 1:
    return tree(t.label, {binarize_tree(t.children[0])});
  case 2:
    return tree(t.label, {binarize_tree(t.children[0]),
                          binarize_tree(t.children[1])});
  default:
    break;
  }

  tree empty{"", {}};
  tree root{t.label, {empty, empty}};

  tree *parent = &root;
  std::vector<tree> children;
  children.reserve(t.children.size());
  for (auto const &c : t.children)
    children.push_back(binarize_tree(c));

  while (children.size() > 2) {
    swap(parent->children[0], children.front());
    children.erase(children.begin());

    std::string lbl;
    for (auto const &c : children) {
      lbl += c.label;
      lbl += "|";
    }
    lbl.erase(lbl.size() - 1);

    tree left{lbl, {empty, empty}};
    swap(parent->children[1], left);
    parent = &parent->children[1];
  }

  swap(parent->children, children);

  return root;
}

int main(int argc, char** argv) {
  int count = 0;
  try {
    while (true) {
      auto t = read_texpr(std::cin);
      ++count;
      std::cout << "Read tree:" << std::endl;
      std::cout << t << std::endl;
      simplify_tree(t);
      std::cout << "Simplified tree:" << std::endl;
      std::cout << t << std::endl;
      auto b = binarize_tree(t);
      std::cout << "Binarized tree:" << std::endl;
      std::cout << b << std::endl;
    }
  } catch(std::runtime_error e) {
    std::cout << e.what() << std::endl;
  }

  std::cout << count << " trees read." << std::endl;

  return 0;
}
