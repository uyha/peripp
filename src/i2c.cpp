#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <peripp/i2c.hpp>
#include <peripp/macros.hpp>
#include <sys/ioctl.h>
#include <unistd.h>

namespace peripp {
I2C::I2C(char const *path)
    : m_handle{::open(path, O_RDWR)} {
  if (m_handle == -1) {
    throw std::system_error(errno, std::system_category());
  }
}

auto I2C::create(char const *path) noexcept -> tl::expected<I2C, std::errc> {
  auto handle = ::open(path, O_RDWR);

  PERIPP_CHECK(handle)

  return I2C{handle};
}

I2C::I2C(I2C &&other) noexcept
    : m_handle{other.m_handle} {
  other.m_handle = -1;
}
auto I2C::operator=(I2C &&other) noexcept -> I2C & {
  this->~I2C();

  m_handle       = other.m_handle;
  other.m_handle = -1;

  return *this;
}

I2C::~I2C() noexcept {
  if (m_handle == -1) {
    return;
  }

  ::close(m_handle);
}

I2C::I2C(int handle) noexcept
    : m_handle{handle} {}

auto I2C::set_10_bit() noexcept -> tl::expected<void, std::errc> {
  auto result = ::ioctl(m_handle, I2C_TENBIT, true);
  PERIPP_CHECK(result);

  is_10_bit = true;

  return {};
}
auto I2C::set_7_bit() noexcept -> tl::expected<void, std::errc> {
  auto result = ::ioctl(m_handle, I2C_TENBIT, false);
  PERIPP_CHECK(result);

  is_10_bit = false;

  return {};
}

auto I2C::send(std::uint16_t slave_address, std::span<std::uint8_t> buffer) noexcept -> tl::expected<void, std::errc> {
  auto msg = ::i2c_msg{.addr  = slave_address,
                       .flags = 0,
                       .len   = static_cast<std::uint16_t>(std::size(buffer)),
                       .buf   = std::data(buffer)};

  auto data = ::i2c_rdwr_ioctl_data{.msgs = &msg, .nmsgs = 1};

  auto result = ::ioctl(m_handle, I2C_RDWR, &data);
  PERIPP_CHECK_RETURN(result);
}

auto I2C::functionality() const noexcept -> tl::expected<unsigned long, std::errc> {
  unsigned long funcs;
  auto result = ::ioctl(m_handle, I2C_FUNCS, &funcs);
  PERIPP_CHECK(result);
  return funcs;
}
} // namespace peripp
