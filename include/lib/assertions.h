#pragma once

#ifdef DEBUG

#include <Arduino.h>

#define BENCHMARK(fn)                                                                              \
  do {                                                                                             \
    Serial.print("--- Beginning Benchmark for " #fn "\n");                                         \
    unsigned long bench_start = micros();                                                          \
    fn;                                                                                            \
    unsigned long bench_stop = micros();                                                           \
    Serial.print("--- Ended Benchmark for " #fn ", took ");                                        \
    Serial.print(bench_start <= bench_stop ? bench_stop - bench_start                              \
                                           : ULONG_MAX - bench_start + bench_stop);                \
    Serial.print("us.\n");                                                                         \
  } while (0)

#define VERIFY(expression)                                                                         \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      Serial.print("ASSERTION FAILED: " #expression);                                              \
      Serial.print(" in " __FILE__ ":");                                                           \
      Serial.print(__LINE__);                                                                      \
      Serial.flush();                                                                              \
      exit(EXIT_FAILURE);                                                                          \
    }                                                                                              \
  } while (0)

#else

#define BENCHMARK(fn) fn;

#define VERIFY(expression)                                                                         \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      exit(EXIT_FAILURE);                                                                          \
    }                                                                                              \
  } while (0)

#endif

#define VERIFY_NOT_REACHED() VERIFY(false)