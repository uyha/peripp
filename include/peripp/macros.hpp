#pragma once

#define PERIPP_CHECK(var)                                                                                              \
  if ((var) == -1) {                                                                                                   \
    return tl::unexpected{static_cast<std::errc>(errno)};                                                              \
  }

#define PERIPP_CHECK_RETURN(var)                                                                                       \
  if ((var) == -1) {                                                                                                   \
    return tl::unexpected{static_cast<std::errc>(errno)};                                                              \
  }                                                                                                                    \
  return {};
