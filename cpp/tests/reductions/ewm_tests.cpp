/*
 * Copyright (c) 2021, NVIDIA CORPORATION.
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

#include "scan_tests.hpp"

#include <cudf_test/column_utilities.hpp>
#include <cudf_test/column_wrapper.hpp>
#include <cudf_test/iterator_utilities.hpp>

#include <cudf/detail/aggregation/aggregation.hpp>
#include <cudf/reduction.hpp>

using aggregation = cudf::aggregation;
using cudf::null_policy;
using cudf::scan_type;
using namespace cudf::test::iterators;

template <typename T>
struct TypedEwmScanTest : BaseScanTest<T> {
  inline void test_ungrouped_ewma_scan(cudf::column_view const& input,
                                       cudf::column_view const& expect_vals,
                                       std::unique_ptr<aggregation> const& agg,
                                       null_policy null_handling)
  {
    std::cout << "hello world!" << std::endl;
    auto col_out = cudf::scan(input, agg, scan_type::INCLUSIVE, null_handling);
    std::cout << "scan done!" << std::endl;
    CUDF_TEST_EXPECT_COLUMNS_EQUIVALENT(expect_vals, col_out->view());
  }
};

TYPED_TEST_SUITE(TypedEwmScanTest, cudf::test::FloatingPointTypes);

TYPED_TEST(TypedEwmScanTest, Ewm)
{
  auto const v = [] {
    return make_vector<TypeParam>({1.0,1.0,1.0});
  }();
  auto col = this->make_column(v);

  auto const expected_ewma_vals =
    cudf::test::fixed_width_column_wrapper<TypeParam>{{1.0,1.0,1.0}};
  this->test_ungrouped_ewma_scan(
    *col, expected_ewma_vals, cudf::make_ewma_aggregation(0.5, true), null_policy::INCLUDE);
}
