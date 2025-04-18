//===-- Single-precision cos function -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/math/cosf.h"
#include "sincosf_utils.h"
#include "src/__support/FPUtil/BasicOperations.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/except_value_utils.h"
#include "src/__support/FPUtil/multiply_add.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"            // LIBC_UNLIKELY
#include "src/__support/macros/properties/cpu_features.h" // LIBC_TARGET_CPU_HAS_FMA

namespace LIBC_NAMESPACE_DECL {

#ifndef LIBC_MATH_HAS_SKIP_ACCURATE_PASS
// Exceptional cases for cosf.
static constexpr size_t N_EXCEPTS = 6;

static constexpr fputil::ExceptValues<float, N_EXCEPTS> COSF_EXCEPTS{{
    // (inputs, RZ output, RU offset, RD offset, RN offset)
    // x = 0x1.64a032p43, cos(x) = 0x1.9d4ba4p-1 (RZ)
    {0x55325019, 0x3f4ea5d2, 1, 0, 0},
    // x = 0x1.4555p51, cos(x) = 0x1.115d7cp-1 (RZ)
    {0x5922aa80, 0x3f08aebe, 1, 0, 1},
    // x = 0x1.48a858p54, cos(x) = 0x1.f48148p-2 (RZ)
    {0x5aa4542c, 0x3efa40a4, 1, 0, 0},
    // x = 0x1.3170fp63, cos(x) = 0x1.fe2976p-1 (RZ)
    {0x5f18b878, 0x3f7f14bb, 1, 0, 0},
    // x = 0x1.2b9622p67, cos(x) = 0x1.f0285cp-1 (RZ)
    {0x6115cb11, 0x3f78142e, 1, 0, 1},
    // x = 0x1.ddebdep120, cos(x) = 0x1.114438p-1 (RZ)
    {0x7beef5ef, 0x3f08a21c, 1, 0, 0},
}};
#endif // !LIBC_MATH_HAS_SKIP_ACCURATE_PASS

LLVM_LIBC_FUNCTION(float, cosf, (float x)) {
  using FPBits = typename fputil::FPBits<float>;

  FPBits xbits(x);
  xbits.set_sign(Sign::POS);

  uint32_t x_abs = xbits.uintval();
  double xd = static_cast<double>(xbits.get_val());

  // Range reduction:
  // For |x| > pi/16, we perform range reduction as follows:
  // Find k and y such that:
  //   x = (k + y) * pi/32
  //   k is an integer
  //   |y| < 0.5
  // For small range (|x| < 2^45 when FMA instructions are available, 2^22
  // otherwise), this is done by performing:
  //   k = round(x * 32/pi)
  //   y = x * 32/pi - k
  // For large range, we will omit all the higher parts of 16/pi such that the
  // least significant bits of their full products with x are larger than 63,
  // since cos((k + y + 64*i) * pi/32) = cos(x + i * 2pi) = cos(x).
  //
  // When FMA instructions are not available, we store the digits of 32/pi in
  // chunks of 28-bit precision.  This will make sure that the products:
  //   x * THIRTYTWO_OVER_PI_28[i] are all exact.
  // When FMA instructions are available, we simply store the digits of 32/pi in
  // chunks of doubles (53-bit of precision).
  // So when multiplying by the largest values of single precision, the
  // resulting output should be correct up to 2^(-208 + 128) ~ 2^-80.  By the
  // worst-case analysis of range reduction, |y| >= 2^-38, so this should give
  // us more than 40 bits of accuracy. For the worst-case estimation of range
  // reduction, see for instances:
  //   Elementary Functions by J-M. Muller, Chapter 11,
  //   Handbook of Floating-Point Arithmetic by J-M. Muller et. al.,
  //   Chapter 10.2.
  //
  // Once k and y are computed, we then deduce the answer by the cosine of sum
  // formula:
  //   cos(x) = cos((k + y)*pi/32)
  //          = cos(y*pi/32) * cos(k*pi/32) - sin(y*pi/32) * sin(k*pi/32)
  // The values of sin(k*pi/32) and cos(k*pi/32) for k = 0..63 are precomputed
  // and stored using a vector of 32 doubles. Sin(y*pi/32) and cos(y*pi/32) are
  // computed using degree-7 and degree-6 minimax polynomials generated by
  // Sollya respectively.

  // |x| < 0x1.0p-12f
  if (LIBC_UNLIKELY(x_abs < 0x3980'0000U)) {
    // When |x| < 2^-12, the relative error of the approximation cos(x) ~ 1
    // is:
    //   |cos(x) - 1| < |x^2 / 2| = 2^-25 < epsilon(1)/2.
    // So the correctly rounded values of cos(x) are:
    //   = 1 - eps(x) if rounding mode = FE_TOWARDZERO or FE_DOWWARD,
    //   = 1 otherwise.
    // To simplify the rounding decision and make it more efficient and to
    // prevent compiler to perform constant folding, we use
    //   fma(x, -2^-25, 1) instead.
    // Note: to use the formula 1 - 2^-25*x to decide the correct rounding, we
    // do need fma(x, -2^-25, 1) to prevent underflow caused by -2^-25*x when
    // |x| < 2^-125. For targets without FMA instructions, we simply use
    // double for intermediate results as it is more efficient than using an
    // emulated version of FMA.
#if defined(LIBC_TARGET_CPU_HAS_FMA_FLOAT)
    return fputil::multiply_add(xbits.get_val(), -0x1.0p-25f, 1.0f);
#else
    return static_cast<float>(fputil::multiply_add(xd, -0x1.0p-25, 1.0));
#endif // LIBC_TARGET_CPU_HAS_FMA_FLOAT
  }

#ifndef LIBC_MATH_HAS_SKIP_ACCURATE_PASS
  if (auto r = COSF_EXCEPTS.lookup(x_abs); LIBC_UNLIKELY(r.has_value()))
    return r.value();
#endif // !LIBC_MATH_HAS_SKIP_ACCURATE_PASS

  // x is inf or nan.
  if (LIBC_UNLIKELY(x_abs >= 0x7f80'0000U)) {
    if (xbits.is_signaling_nan()) {
      fputil::raise_except_if_required(FE_INVALID);
      return FPBits::quiet_nan().get_val();
    }

    if (x_abs == 0x7f80'0000U) {
      fputil::set_errno_if_required(EDOM);
      fputil::raise_except_if_required(FE_INVALID);
    }
    return x + FPBits::quiet_nan().get_val();
  }

  // Combine the results with the sine of sum formula:
  //   cos(x) = cos((k + y)*pi/32)
  //          = cos(y*pi/32) * cos(k*pi/32) - sin(y*pi/32) * sin(k*pi/32)
  //          = cosm1_y * cos_k + sin_y * sin_k
  //          = (cosm1_y * cos_k + cos_k) + sin_y * sin_k
  double sin_k, cos_k, sin_y, cosm1_y;

  sincosf_eval(xd, x_abs, sin_k, cos_k, sin_y, cosm1_y);

  return static_cast<float>(fputil::multiply_add(
      sin_y, -sin_k, fputil::multiply_add(cosm1_y, cos_k, cos_k)));
}

} // namespace LIBC_NAMESPACE_DECL
