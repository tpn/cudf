#!/bin/bash
# Copyright (c) 2022-2023, NVIDIA CORPORATION.

# Common setup steps shared by Python test jobs
source "$(dirname "$0")/test_python_common.sh"

rapids-mamba-retry install \
  --channel "${CPP_CHANNEL}" \
  --channel "${PYTHON_CHANNEL}" \
  dask-cudf cudf_kafka custreamz

rapids-logger "Check GPU usage"
nvidia-smi

set +e

rapids-logger "pytest dask_cudf"
pushd python/dask_cudf
pytest \
  --cache-clear \
  --junitxml="${RAPIDS_TESTS_DIR}/junit-dask-cudf.xml" \
  --numprocesses=8 \
  --dist=loadscope \
  --cov-config=.coveragerc \
  --cov=dask_cudf \
  --cov-report=xml:"${RAPIDS_COVERAGE_DIR}/dask-cudf-coverage.xml" \
  --cov-report=term \
  dask_cudf
exitcode=$?

if (( ${exitcode} != 0 )); then
    SUITEERROR=${exitcode}
    echo "FAILED: 1 or more tests in dask-cudf"
fi
popd

rapids-logger "pytest custreamz"
pushd python/custreamz
pytest \
  --cache-clear \
  --junitxml="${RAPIDS_TESTS_DIR}/junit-custreamz.xml" \
  --numprocesses=8 \
  --dist=loadscope \
  --cov-config=.coveragerc \
  --cov=custreamz \
  --cov-report=xml:"${RAPIDS_COVERAGE_DIR}/custreamz-coverage.xml" \
  --cov-report=term \
  custreamz
exitcode=$?

if (( ${exitcode} != 0 )); then
    SUITEERROR=${exitcode}
    echo "FAILED: 1 or more tests in custreamz"
fi
popd

exit ${SUITEERROR}
