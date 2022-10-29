#include "elfloader/elfloader.hh"
#include "test_header.hh"

#include <sstream>

TEST(elfloader, ctorFail) {
  std::istringstream ss{"Bad input stream"};
  EXPECT_THROW(sim::ELFLoader{ss}, std::runtime_error);
}

#include "test_footer.hh"
