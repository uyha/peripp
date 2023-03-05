#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <peripp/i2c.hpp>
#include <peripp/macros.hpp>
#include <sys/ioctl.h>
#include <unistd.h>

namespace peripp {
namespace detail {
template <typename Protocol>
I2CBase<Protocol>::I2CBase(char const *path)
    : m_handle{::open(path, O_RDWR)} {
  if (m_handle == -1) {
    throw std::system_error(errno, std::system_category());
  }
}

template <typename Protocol>
auto I2CBase<Protocol>::create(char const *path) noexcept -> tl::expected<Protocol, std::errc> {
  auto handle = ::open(path, O_RDWR);

  PERIPP_CHECK(handle)

  return Protocol{handle};
}

template <typename Protocol>
I2CBase<Protocol>::I2CBase(I2CBase &&other) noexcept
    : m_handle{other.m_handle} {
  other.m_handle = -1;
}

template <typename Protocol>
auto I2CBase<Protocol>::operator=(I2CBase &&other) noexcept -> I2CBase & {
  this->~I2CBase();

  m_handle       = other.m_handle;
  other.m_handle = -1;

  return *this;
}

template <typename Protocol>
I2CBase<Protocol>::~I2CBase() noexcept {
  if (m_handle == -1) {
    return;
  }

  ::close(m_handle);
}

template <typename Protocol>
I2CBase<Protocol>::I2CBase(int handle) noexcept
    : m_handle{handle} {}

template <typename Protocol>
auto I2CBase<Protocol>::set_10_bit() noexcept -> tl::expected<void, std::errc> {
  auto result = ::ioctl(m_handle, I2C_TENBIT, true);
  PERIPP_CHECK(result);

  is_10_bit = true;

  return {};
}

template <typename Protocol>
auto I2CBase<Protocol>::set_7_bit() noexcept -> tl::expected<void, std::errc> {
  auto result = ::ioctl(m_handle, I2C_TENBIT, false);
  PERIPP_CHECK(result);

  is_10_bit = false;

  return {};
}

template <typename Protocol>
auto I2CBase<Protocol>::functionality() const noexcept -> tl::expected<unsigned long, std::errc> {
  unsigned long funcs;
  auto result = ::ioctl(m_handle, I2C_FUNCS, &funcs);
  PERIPP_CHECK(result);
  return funcs;
}
} // namespace detail

template class detail::I2CBase<I2C>;

auto I2C::send(std::uint16_t slave_address, std::span<std::uint8_t> buffer) noexcept -> tl::expected<void, std::errc> {
  auto msg = ::i2c_msg{.addr  = slave_address,
                       .flags = static_cast<std::uint16_t>(is_10_bit ? I2C_M_TEN : 0),
                       .len   = static_cast<std::uint16_t>(std::size(buffer)),
                       .buf   = std::data(buffer)};

  auto data = ::i2c_rdwr_ioctl_data{.msgs = &msg, .nmsgs = 1};

  auto result = ::ioctl(m_handle, I2C_RDWR, &data);
  PERIPP_CHECK_RETURN(result);
}
} // namespace peripp
