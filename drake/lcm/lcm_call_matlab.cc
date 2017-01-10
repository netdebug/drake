#include "drake/lcm/lcm_call_matlab.h"

#include <cstring>
#include <limits>
#include <string>

#include "lcm/lcm-cpp.hpp"

#include "drake/common/drake_assert.h"
#include "drake/common/never_destroyed.h"
#include "drake/lcmt_call_matlab.hpp"
#include "drake/lcmt_matlab_array.hpp"

namespace drake {
namespace lcm {

static int g_uid = 0;

LcmMatlabRemoteVariable::LcmMatlabRemoteVariable()
    : uid_(g_uid++)
// TODO(russt): replace this with a random int64_t, e.g.
// http://stackoverflow.com/questions/7114043/random-number-generation-in-c11-how-to-generate-how-do-they-work
// TODO(russt): david-german-tri recommended a more robust (but more complex)
// solution was to use e.g. [IP address + process id + time].  We decided this
// was sufficient for now.
{}

void ToLcmMatlabArray(const LcmMatlabRemoteVariable& var,
                      drake::lcmt_matlab_array* matlab_array) {
  matlab_array->type = drake::lcmt_matlab_array::REMOTE_VARIABLE_REFERENCE;
  matlab_array->rows = 1;
  matlab_array->cols = 1;
  matlab_array->num_bytes = sizeof(int64_t);
  matlab_array->data.resize(matlab_array->num_bytes);
  int64_t uid = var.uid();
  memcpy(matlab_array->data.data(), &uid, matlab_array->num_bytes);
}

void ToLcmMatlabArray(double var, drake::lcmt_matlab_array* matlab_array) {
  matlab_array->type = drake::lcmt_matlab_array::DOUBLE;
  matlab_array->rows = 1;
  matlab_array->cols = 1;
  matlab_array->num_bytes = sizeof(double);
  matlab_array->data.resize(matlab_array->num_bytes);
  memcpy(matlab_array->data.data(), &var, matlab_array->num_bytes);
}

void ToLcmMatlabArray(const Eigen::Ref<const Eigen::MatrixXd>& mat,
                      drake::lcmt_matlab_array* matlab_array) {
  matlab_array->type = drake::lcmt_matlab_array::DOUBLE;
  matlab_array->rows = mat.rows();
  matlab_array->cols = mat.cols();
  matlab_array->num_bytes = sizeof(double) * mat.rows() * mat.cols();
  matlab_array->data.resize(matlab_array->num_bytes);
  memcpy(matlab_array->data.data(), mat.data(), matlab_array->num_bytes);
}

void ToLcmMatlabArray(
    const Eigen::Ref <
        const Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>>& mat,
    drake::lcmt_matlab_array* matlab_array) {
  matlab_array->type = drake::lcmt_matlab_array::LOGICAL;
  matlab_array->rows = mat.rows();
  matlab_array->cols = mat.cols();
  matlab_array->num_bytes = sizeof(bool) * mat.rows() * mat.cols();
  matlab_array->data.resize(matlab_array->num_bytes);
  memcpy(matlab_array->data.data(), mat.data(), matlab_array->num_bytes);
}

void ToLcmMatlabArray(const std::string& str,
                      drake::lcmt_matlab_array* matlab_array) {
  matlab_array->type = drake::lcmt_matlab_array::CHAR;
  matlab_array->rows = 1;
  matlab_array->cols = str.length();
  matlab_array->num_bytes = sizeof(char) * str.length();
  matlab_array->data.resize(matlab_array->num_bytes);
  memcpy(matlab_array->data.data(), str.data(), matlab_array->num_bytes);
}

void internal::PublishLcmCallMatlab(const drake::lcmt_call_matlab& msg) {
  static never_destroyed<::lcm::LCM> lcm;

  if (!lcm.access().good()) return;

  lcm.access().publish("LCM_CALL_MATLAB", &msg);
}

}  // namespace lcm
}  // namespace drake
