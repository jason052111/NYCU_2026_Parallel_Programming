#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float clamp = _pp_vset_float(9.999999f);

  __pp_vec_int exponent;
  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int one = _pp_vset_int(1);
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    int active = N - i;

    if (active > VECTOR_WIDTH)
    {
      active = VECTOR_WIDTH;
    }

    __pp_mask maskAll = _pp_init_ones(active);

    _pp_vload_float(x, values + i, maskAll);
    _pp_vload_int(exponent, exponents + i, maskAll);
    _pp_vset_float(result, 1.f, maskAll);

    __pp_mask maskExpPositive = _pp_init_ones(0);
    _pp_vgt_int(maskExpPositive, exponent, zero, maskAll);

    while (_pp_cntbits(maskExpPositive) > 0)
    {
      _pp_vmult_float(result, result, x, maskExpPositive);
      _pp_vsub_int(exponent, exponent, one, maskExpPositive);

      _pp_vgt_int(maskExpPositive, exponent, zero, maskAll);
    }

    __pp_mask maskShouldClamp = _pp_init_ones(0);
    _pp_vgt_float(maskShouldClamp, result, clamp, maskAll);
    _pp_vset_float(result, 9.999999f, maskShouldClamp);
    _pp_vstore_float(output + i, result, maskAll);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //
  __pp_vec_float x;
  __pp_vec_float sum = _pp_vset_float(0.f);
  __pp_vec_float temp;
  __pp_mask maskAll = _pp_init_ones();

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    _pp_vload_float(x, values + i, maskAll);
    _pp_vadd_float(sum, sum, x, maskAll);
  }

  for (int width = VECTOR_WIDTH; width > 1; width /= 2)
  {
    _pp_hadd_float(temp, sum);
    _pp_interleave_float(sum, temp);
  }

  return sum.value[0];
}