#include <catch2/catch_test_macros.hpp>
#include <charconv>
#include <cstring>
#include <filesystem>
#include <linux/i2c.h>
#include <peripp/i2c.hpp>

namespace fs = std::filesystem;

consteval auto operator""_u8(unsigned long long int value) -> std::uint8_t {
  if (value > std::numeric_limits<std::uint8_t>::max()) {
    throw std::overflow_error{"value is too big to be converted to std::uint8_t"};
  }
  return static_cast<std::uint8_t>(value);
}

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

TEST_CASE("I2C creation with a i2c-stub bus", "[.virtual][i2c][create]") {
  auto bus_path = std::getenv("PERIPP_I2C_PATH");
  if (bus_path == nullptr) {
    SKIP("PERIPP_I2C_PATH is not defined, skipping tests");
  }
  if (not fs::exists(bus_path)) {
    FAIL("PERIPP_I2C_PATH is defined but does not exist");
  }

  SECTION("Constructor with valid path should succeed") {
    REQUIRE_NOTHROW(peripp::I2C{bus_path});
  }
  SECTION("`create` static member function with valid path should succeed") {
    auto bus = peripp::I2C::create(bus_path);
    REQUIRE(bus);
  }
}

TEST_CASE("Getting functionality of a i2c-stub bus", "[.virtual][i2c][function]") {
  auto bus_path = std::getenv("PERIPP_I2C_PATH");
  if (bus_path == nullptr) {
    SKIP("PERIPP_I2C_PATH is not defined, skipping tests");
  }
  if (not fs::exists(bus_path)) {
    FAIL("PERIPP_I2C_PATH is defined but does not exist");
  }
  auto bus = peripp::I2C::create(bus_path);
  REQUIRE(bus);

  auto funcs = bus->functionality();
  REQUIRE(funcs);
  REQUIRE(*funcs
          == (I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_READ_BYTE | I2C_FUNC_SMBUS_WRITE_BYTE
              | I2C_FUNC_SMBUS_READ_BYTE_DATA | I2C_FUNC_SMBUS_WRITE_BYTE_DATA | I2C_FUNC_SMBUS_READ_WORD_DATA
              | I2C_FUNC_SMBUS_WRITE_WORD_DATA | I2C_FUNC_SMBUS_READ_I2C_BLOCK | I2C_FUNC_SMBUS_WRITE_I2C_BLOCK));
}

// i2c-stub does not support I2C operations
TEST_CASE("Sending to or receiving from a i2c-stub bus shall fail", "[.virtual][i2c][read][write]") {
  auto bus_path          = std::getenv("PERIPP_I2C_PATH");
  auto slave_address_env = std::getenv("PERIPP_I2C_ADDRESS");
  auto slave_address     = std::uint16_t{};

  if (bus_path == nullptr) {
    SKIP("PERIPP_I2C_PATH is not defined, skipping tests");
  }
  if (not fs::exists(bus_path)) {
    FAIL("PERIPP_I2C_PATH is defined but does not exist");
  }
  if (slave_address_env == nullptr) {
    SKIP("PERIPP_I2C_ADDRESS is not defined, skipping tests");
  }
  auto const result =
      std::from_chars(slave_address_env + 2, slave_address_env + std::strlen(slave_address_env), slave_address, 16);

  if (result.ec != std::errc{}) {
    FAIL("PERIPP_I2C_ADDRESS is defined but is not a valid hex number");
  }
  auto bus = peripp::I2C::create(bus_path);
  REQUIRE(bus);

  auto send_result = bus->send(slave_address, 0x5A_u8);
  REQUIRE_FALSE(send_result);
  REQUIRE(send_result.error() == std::errc::not_supported);
}
