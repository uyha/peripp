#pragma once

#include <span>
#include <system_error>
#include <tl/expected.hpp>
#include <type_traits>

namespace peripp {
namespace detail {
template <typename Protocol>
class I2CBase {
public:
  I2CBase(char const *path);

  static auto create(char const *path) noexcept -> tl::expected<Protocol, std::errc>;

  I2CBase(I2CBase const &)                     = delete;
  auto operator=(I2CBase const &) -> I2CBase & = delete;

  I2CBase(I2CBase &&other) noexcept;
  auto operator=(I2CBase &&other) noexcept -> I2CBase &;

  ~I2CBase() noexcept;

  auto set_10_bit() noexcept -> tl::expected<void, std::errc>;
  auto set_7_bit() noexcept -> tl::expected<void, std::errc>;

  auto functionality() const noexcept -> tl::expected<unsigned long, std::errc>;

protected:
  I2CBase(int m_handle) noexcept;

  int m_handle;
  bool is_10_bit = false;
};
} // namespace detail

class I2C final : public detail::I2CBase<I2C> {
public:
  auto send(std::uint16_t slave_address, std::span<std::uint8_t> buffer) noexcept -> tl::expected<void, std::errc>;

  template <typename... u8>
    requires(std::is_same_v<std::remove_cvref_t<u8>, std::uint8_t> && ...)
  auto send(std::uint16_t slave_address, u8... data) noexcept -> tl::expected<void, std::errc> {
    auto buffer = std::array<std::uint8_t, sizeof...(data)>{data...};

    return send(slave_address, buffer);
  }

  auto receive(std::uint16_t slave_address, std::span<std::uint8_t> buffer) noexcept -> tl::expected<void, std::errc>;
};

class SMBus final : public detail::I2CBase<SMBus> {};
} // namespace peripp
