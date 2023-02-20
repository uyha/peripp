#include <catch2/catch_test_macros.hpp>
#include <peripp/i2c.hpp>

TEST_CASE("I2C creation", "[i2c][create]") {
  SECTION("Constructor with invalid path should throw an exception") {
    REQUIRE_THROWS_AS((peripp::I2C{"/some/random/path"}), std::system_error);
  }

  SECTION("`create` static member function invoked with invalid should return a failure") {
    auto result = peripp::I2C::create("/some/random/path");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == std::errc::no_such_file_or_directory);
  }
}
