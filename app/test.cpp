#include "add.h"

/* Call header in googletest */
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace test
{
  TEST(calc, addfunc)
  {    
    EXPECT_EQ(addint(3, 2), 5);
  }
}