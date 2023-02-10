/*
 * Copyright (c) 2023, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

// If using GCC, temporary workaround for older libcudacxx defining _LIBCPP_VERSION
// undefine it before including spdlog, due to fmtlib checking if it is defined
// TODO: remove once libcudacxx is on Github and RAPIDS depends on it
#ifdef __GNUG__
#undef _LIBCPP_VERSION
#endif
#include <spdlog/spdlog.h>

namespace cudf {

/**
 * @brief Returns the global logger.
 *
 * This is a global instance of a spdlog logger. It can be used to configure logging behavior in
 * libcudf.
 *
 * Examples:
 * @code{.cpp}
 * // Turn off logging at runtime
 * cudf::logger().set_level(spdlog::level::off);
 * // Add a stdout sink to the logger
 * cudf::logger().sinks().push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
 * // Replace the default sink
 * cudf::logger().sinks() ={std::make_shared<spdlog::sinks::stderr_sink_mt>()};
 * @endcode
 *
 * Note: Changes to the sinks are not thread safe and should only be done during global
 * initialization.
 *
 * @return spdlog::logger& The logger.
 */
spdlog::logger& logger();

}  // namespace cudf
