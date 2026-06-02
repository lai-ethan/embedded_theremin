#define PI 3.14159265358979323846f

float pow_int (float x, int n);
float fast_cos(float x);
float sin(float x);

float pow_int(float x, int n) {
  float a = 1;
  if (n < 0) {
    x = 1 / x;
    n = -n;
  }

  while (n > 0) {
    if (n & 1) {
      a = a * x;
    }
    x = x * x;
    n = n / 2;
  }

  return a;
}

float fast_cos(float x) {
    int sign = 1;
    if (x < 0) {
        x = -x;
    }
    
    if (x > 2 * PI) {
        x -= (int)(x / 2 / PI) * 2 * PI;
    }
    
    if (x > PI) {
        x -= PI;
        sign *= -1;
    }
    
    if (x > PI / 2) {
        x = PI - x;
        sign *= -1;
    }
    
    if (x > PI / 4) {
        return sign * sin(PI / 2 - x);
    }
    
    float approx = 1 
                - pow_int(x, 2) / 2 
                + pow_int(x, 4) / 24 
                - pow_int(x, 6) / 720 
                + pow_int(x, 8) / 40320 
                - pow_int(x, 10) / 3628800 
                + pow_int(x, 12) / 479001600 
                - pow_int(x, 14) / 87178291200;

    return sign * approx;
}

float sin(float x) {
    int sign = 1;

    if (x < 0) {
        x = -x;
        sign *= -1;
    }
    
    if (x > 2 * PI) {
        x -= (int)(x / 2 / PI) * 2 * PI;
    }
    
    if (x > PI) {
        x -= PI;
        sign *= -1;
    }
    
    if (x > PI / 2) {
        x = PI - x;
    }
    
    if (x > PI / 4) {
        return sign * fast_cos(PI / 2 - x);
    }
    
    float approx = x 
                - pow_int(x, 3) / 6 
                + pow_int(x, 5) / 120 
                - pow_int(x, 7) / 5040 
                + pow_int(x, 9) / 362880 
                - pow_int(x, 11) / 39916800 
                + pow_int(x, 13) / 6227020800;
  
    return sign * approx;
}
