#include <catch2/catch_test_macros.hpp>
#include <peripp/i2c.hpp>

TEST_CASE("I2C creation", "[i2c][create]") {
  SECTION("Constructor with invalid path should throw an exception") {
    REQUIRE_THROWS_AS((peripp::I2C{"/some/random/path"}), std::system_error);
  }
}
