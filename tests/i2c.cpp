#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <filesystem>
#include <peripp/i2c.hpp>

namespace fs = std::filesystem;

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

TEST_CASE("I2C creation with a real bus", "[.real][i2c][create]") {
  auto bus_path = std::getenv("PERIPP_I2C_PATH");
  if (bus_path == nullptr) {
    SKIP("PERIPP_I2C_PATH is not defined, skipping tests");
  }
  if (not fs::exists(bus_path)) {
    FAIL("PERIPP_I2C_PATH is defined but does not exist");
  }

  SECTION("Construct with valid path should succeed") {
    REQUIRE_NOTHROW(peripp::I2C{bus_path});
  }
}

TEST_CASE("Getting functionality of a I2C bus", "[.real][i2c][function]") {}
