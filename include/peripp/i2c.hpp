#include <span>
#include <system_error>
#include <tl/expected.hpp>
#include <type_traits>
namespace peripp {
class I2C {
public:
  I2C(char const *path);

  static auto create(char const *path) noexcept -> tl::expected<I2C, std::errc>;

  I2C(I2C const &)                     = delete;
  auto operator=(I2C const &) -> I2C & = delete;

  I2C(I2C &&other) noexcept;
  auto operator=(I2C &&other) noexcept -> I2C &;

  ~I2C() noexcept;

  auto set_10_bit() noexcept -> tl::expected<void, std::errc>;
  auto set_7_bit() noexcept -> tl::expected<void, std::errc>;

  auto send(std::uint16_t slave_address, std::span<std::uint8_t> buffer) noexcept -> tl::expected<void, std::errc>;
  template <typename... u8>
    requires(std::is_same_v<std::remove_cvref_t<u8>, std::uint8_t> && ...)
  auto send(std::uint16_t slave_address, u8... data) noexcept -> tl::expected<void, std::errc> {
    auto buffer = std::array<std::uint8_t, sizeof...(data)>{data...};

    return send(slave_address, buffer);
  }

  auto functionality() const noexcept -> tl::expected<unsigned long, std::errc>;

private:
  I2C(int m_handle) noexcept;

  int m_handle;
  bool is_10_bit;
};
} // namespace peripp
