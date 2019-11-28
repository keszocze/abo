#include <catch2/catch.hpp>

#include <from_papers.hpp>
#include <simple.hpp>

#include <dump_dot.hpp>

#include <iostream>

TEST_CASE("Display BDD in a fancy way") {
  std::cout << "FOOO\n";

  Cudd mgr(2);
  std::vector<std::string> names{"and"};

  BDD and_bdd = abo::example_bdds::and_bdd(mgr, 2);

  abo::util::dump_dot_readable(mgr, std::vector<BDD>{and_bdd},std::cout, names);
  abo::util::dump_dot_readable(mgr, std::vector<BDD>{and_bdd},std::cout);
  abo::util::dump_dot_readable(mgr, and_bdd,std::cout);
}