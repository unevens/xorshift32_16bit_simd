# xorshift32_16bit_simd
An implementation of the [16 bit xorshift32](https://b2d-f9r.blogspot.com/2010/08/16-bit-xorshift-rng-now-with-more.html) random number 
generator, that uses SIMD instructions to compute 4 independent random numbers at the same time, each from a different seed.

Used to generate white noise in https://github.com/unevens/avec
