/*
 * Copyright (c) 2019-2023, NVIDIA CORPORATION.
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

#include "avro_common.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace cudf {
namespace io {
namespace avro {

/**
 * @brief AVRO schema entry
 */
struct schema_entry {
  explicit schema_entry(type_kind_e kind_, int32_t parent_idx_ = -1, int32_t num_children_ = 0)
    : parent_idx(parent_idx_), num_children(num_children_), kind(kind_)
  {
  }
  int32_t parent_idx   = -1;  // index of parent entry in schema array, negative if no parent
  int32_t num_children = 0;
  type_kind_e kind     = type_not_set;
  logicaltype_kind_e logical_kind = logicaltype_not_set;
  std::string name                = "";
  std::vector<std::string> symbols;
};

/**
 * @brief AVRO output column
 */
struct column_desc {
  int32_t schema_data_idx  = -1;  // schema index of data column
  int32_t schema_null_idx  = -1;  // schema index of corresponding null object
  int32_t parent_union_idx = -1;  // index of this column in parent union (-1 if not a union member)
  std::string name         = "";
};

/**
 * @brief AVRO file metadata struct
 *
 * `metadata_size` is the size in bytes of the avro file header.
 *
 * `total_data_size` is the size of all data minus `metadata_size`.
 *
 * `num_rows` is the number of rows that will be processed.  If the user has not
 * requested the number of rows to be limited (i.e. via the `num_rows` param to
 * `read_avro()`), this number will represent all rows in the file *after* the
 * `skip_rows` parameter has been taken into consideration (assuming a request
 * has been made to also skip rows).
 *
 * `total_num_rows` is the total number of rows present in the file, across all
 * blocks.  This may be more than `num_rows` if the user has requested a limit
 * on the number of rows to return, or if `skip_rows` is active.
 *
 * `skip_rows` is the number of rows to skip.
 *
 * `block_list` is a list of all blocks in the file.
 */
struct file_metadata {
  std::map<std::string, std::string> user_data;
  std::string codec        = "";
  uint64_t sync_marker[2]  = {0, 0};
  size_t metadata_size     = 0;
  size_t total_data_size   = 0;
  size_type num_rows       = 0;
  size_type skip_rows      = 0;
  size_type total_num_rows = 0;
  uint32_t max_block_size  = 0;
  std::vector<schema_entry> schema;
  std::vector<block_desc_s> block_list;
  std::vector<column_desc> columns;
};

/**
 * @brief Extract AVRO schema from JSON string
 */
class schema_parser {
 protected:
  enum { MAX_SCHEMA_DEPTH = 32 };

 public:
  schema_parser() {}
  bool parse(std::vector<schema_entry>& schema, const std::string& str);

 protected:
  [[nodiscard]] bool more_data() const { return (m_cur < m_end); }
  std::string get_str();

 protected:
  const char* m_base;
  const char* m_cur;
  const char* m_end;
};

/**
 * @brief AVRO file container parsing class
 */
class container {
 public:
  container(uint8_t const* base, size_t len) noexcept : m_base{base}, m_cur{base}, m_end{base + len}
  {
  }

  [[nodiscard]] auto bytecount() const { return m_cur - m_base; }

  template <typename T>
  T get_raw()
  {
    if (m_cur + sizeof(T) > m_end) return T{};
    T val;
    memcpy(&val, m_cur, sizeof(T));
    m_cur += sizeof(T);
    return val;
  }

  template <typename T>
  T get_encoded();

 public:
  bool parse(file_metadata* md, size_t max_num_rows = 0x7fff'ffff, size_t first_row = 0);

 protected:
  const uint8_t* m_base;
  const uint8_t* m_cur;
  const uint8_t* m_end;
};

}  // namespace avro
}  // namespace io
}  // namespace cudf
