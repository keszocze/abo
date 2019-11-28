#include <catch2/catch.hpp>

#include <from_papers.hpp>
#include <simple.hpp>

#include <dump_dot.hpp>

#include <sstream>
#include <iostream>

TEST_CASE("Display BDD in a fancy way") {


  Cudd mgr(2);
  std::vector<std::string> names{"and"};

  BDD and_bdd = abo::example_bdds::and_bdd(mgr, 2);


  std::stringstream s1, s2, s3;


  abo::util::dump_dot_readable(mgr, std::vector<BDD>{and_bdd},s1, names);
  abo::util::dump_dot_readable(mgr, std::vector<BDD>{and_bdd},s2);
  abo::util::dump_dot_readable(mgr, and_bdd,s3);


  CHECK(s2.str() == s3.str());
}