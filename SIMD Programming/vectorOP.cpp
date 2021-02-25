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
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  
  //
  __pp_vec_float x, result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_vec_float one = _pp_vset_float(1.f);
  __pp_vec_float upper_pp = _pp_vset_float(9.999999f);
  __pp_vec_int y, countExp;
  __pp_vec_int zero_int = _pp_vset_int(0);
  __pp_vec_int one_int = _pp_vset_int(1);
  __pp_mask maskAllOne, maskIsZero, maskIsNotZero, maskIsUpper, maskCountIsPositive;
  int mask_width, count_one; // decide calculate units
  float one_f = 1.f;
  // float upper[4] = {9.999999f}; 
  float upper = 9.999999f;
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    // printf("i=%d\n", i);
    mask_width = i + VECTOR_WIDTH <= N ? VECTOR_WIDTH : N - i;
    // All ones
    maskAllOne = _pp_init_ones(mask_width);

    // All zeros
    maskIsZero = _pp_init_ones(0);
    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAllOne); // x = values[i];
    // Load vector of exponents from contiguous memory addresses
    _pp_vload_int(y, exponents + i, maskAllOne); // y = exponents[i];
    
    _pp_veq_int(maskIsZero, y, zero_int, maskAllOne); // if (y == 0) {
    // Execute instruction ("if" clause)
    _pp_vset_float(result, one_f, maskIsZero); //   output[i] = 1.f; } // Use vset not vload
    // Inverse maskIsNegative to generate "else" mask
    maskIsNotZero = _pp_mask_not(maskIsZero); // } else {
    _pp_vsub_int(countExp, y, one_int, maskIsNotZero); //   countExp = y-1;
    
    _pp_vload_float(result, values + i, maskIsNotZero); //   result = val
    maskCountIsPositive = _pp_init_ones(0);
    count_one = 1;
    while(count_one > 0)
		{
		  _pp_vgt_int(maskCountIsPositive, countExp, zero_int, maskIsNotZero); // if (count > 0) { 
      _pp_vmult_float(result, result, x, maskCountIsPositive); //  X*X
      _pp_vsub_int(countExp, countExp, one_int, maskCountIsPositive); //   count = count-1;}      
      count_one = _pp_cntbits(maskCountIsPositive);
		}
    
    maskIsUpper = _pp_init_ones(0);
    _pp_vgt_float(maskIsUpper, result, upper_pp, maskIsNotZero); // if (result>9.9999f){
    _pp_vset_float(result, upper, maskIsUpper); //   result = 9.9999f
    // printf("exp=%d %d %d %d\n", y.value[0], y.value[1], y.value[2], y.value[3]);
    // printf("maskIsUpper=%d %d %d %d\n", maskIsUpper.value[0], maskIsUpper.value[1], maskIsUpper.value[2], maskIsUpper.value[3]);
    // printf("result=%f %f %f %f\n", result.value[0], result.value[1], result.value[2], result.value[3]);
    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAllOne); //  output[i] = result; }
    
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

  __pp_vec_float x, result, result_interleave;
  __pp_mask maskAllOne, maskIsZero, maskIsNotZero, maskIsUpper, maskCountIsPositive;
  float output = 0.f;
  
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    // All ones
    maskAllOne = _pp_init_ones();

    // All zeros
    maskIsZero = _pp_init_ones(0);
    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAllOne); // x = values[i];
    _pp_hadd_float(result, x);
    _pp_interleave_float(result_interleave, result);
    _pp_hadd_float(result, result_interleave);
    output += result.value[1];
    
  }

  return output;
}