# C Long Arithmetic Library

A high-performance arbitrary-precision arithmetic library written in C, implementing operations on integers of unlimited size.

## Features

- **Arbitrary Precision**: Handle integers of any size, limited only by available memory
- **Complete Arithmetic**: Addition, subtraction, multiplication, and division operations
- **Signed Numbers**: Full support for both positive and negative integers
- **Optimized Algorithms**: Karatsuba multiplication for improved performance on large numbers
- **Factorial Computation**: Built-in factorial function for large numbers
- **String Conversion**: Convert arbitrary-precision integers to decimal string representation

## Architecture

The library uses a custom dynamic array implementation to store numbers in base-256 format:
- Each digit is stored as a `uint8_t` (0-255)
- Numbers are stored in little-endian byte order
- Separate sign field for positive/negative tracking
- Automatic memory management with safe allocation wrappers

## API Reference

### Initialization and Cleanup

```c
void superlong_init(superlong* num);
void superlong_deinit(superlong* num);
superlong* new_superlong();
void delete_superlong(superlong* num);
```

### Number Creation

```c
void superlong_from_int(superlong* num, int16_t n);
void superlong_from_uint(superlong* num, uint32_t n);
```

### Arithmetic Operations

#### Addition
```c
void superlong_add(const superlong* a, const superlong* b, superlong* res);
void superlong_add_uint(const superlong* a, uint32_t b, superlong* res);
```

#### Subtraction
```c
void superlong_sub(const superlong* a, const superlong* b, superlong* res);
void superlong_sub_uint(const superlong* a, uint32_t b, superlong* res);
```

#### Multiplication
```c
void superlong_mul(const superlong* a, const superlong* b, superlong* res);
void superlong_mul_uint(const superlong* a, uint32_t b, superlong* res);
```

#### Division
```c
void superlong_div(const superlong* a, const superlong* b, superlong* res);
void superlong_div_uint(const superlong* a, uint32_t b, superlong* res);
```

### Utility Functions

```c
void superlong_copy(const superlong* src, superlong* dest);
void superlong_negate(superlong* num);
int superlong_is_zero(const superlong* num);
char* superlong_to_decimal_str(const superlong* num);
void superlong_factorial(uint32_t n, superlong* res);
```

## Usage Example

```c
#include "superlong.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Initialize numbers
    superlong a, b, result;
    superlong_init(&a);
    superlong_init(&b);
    superlong_init(&result);
    
    // Set values
    superlong_from_uint(&a, 12345);
    superlong_from_uint(&b, 67890);
    
    // Perform addition
    superlong_add(&a, &b, &result);
    
    // Convert to string and print
    char* str = superlong_to_decimal_str(&result);
    printf("Result: %s\n", str);
    free(str);
    
    // Cleanup
    superlong_deinit(&a);
    superlong_deinit(&b);
    superlong_deinit(&result);
    
    return 0;
}
```

### Factorial Example

```c
superlong fact;
superlong_init(&fact);

// Calculate 100!
superlong_factorial(100, &fact);

char* str = superlong_to_decimal_str(&fact);
printf("100! = %s\n", str);
free(str);

superlong_deinit(&fact);
```

## Building

### Requirements
- GCC or Clang compiler
- GNU Make

### Compilation

```bash
# Build the library and tests
make

# Run tests with all sanitizers
make test

# Clean build artifacts
make clean
```

### Build Options

The Makefile includes comprehensive sanitizer support:
- **AddressSanitizer**: Detects memory errors (buffer overflows, use-after-free, etc.)
- **UndefinedBehaviorSanitizer**: Catches undefined behavior
- **LeakSanitizer**: Finds memory leaks

All tests are compiled with `-O0` (no optimization) for better debugging and sanitizer accuracy.

## Testing

The test suite (`test.c`) includes:
- Basic arithmetic operations
- Edge cases (zero, negative numbers)
- Large number computations
- Factorial calculations
- String conversion tests
- Memory leak verification

Run tests:
```bash
make test
```

## Implementation Details

### Memory Layout
Numbers are stored as dynamic arrays of bytes (base-256):
- `digits`: Array of `uint8_t` values
- `sign`: Integer (-1 for negative, 0 for zero, 1 for positive)
- `len`: Current number of digits
- `cap`: Allocated capacity

### Algorithms

- **Karatsuba Multiplication**: O(n^1.585) complexity for large numbers (≥16 bytes)
- **School Multiplication**: Used for smaller numbers
- **Binary Search Division**: Efficient digit-by-digit division

### Safety Features

- Safe memory allocation wrappers (`nc_malloc`, `nc_realloc`)
- Automatic normalization (removing leading zeros)
- Division by zero protection

## Project Structure

```
C-Long-Arithmetic/
├── src/
│   ├── superlong.h         # Main API header
│   ├── superlong.c         # Implementation
│   ├── generate-arr.h      # Dynamic array macros
│   ├── safe-alloc.h        # Safe allocation headers
│   └── safe-alloc.c        # Safe allocation implementation
├── test.c                  # Test suite
├── Makefile                # Build system
└── README.md               # This file
```

## License

This project is available for educational and commercial use.

## Contributing

Contributions are welcome! Please ensure:
- Code follows the existing style
- All tests pass with sanitizers enabled
- New features include appropriate tests

## Author

Developed as part of System Programming coursework.

## Acknowledgments

- Uses Karatsuba algorithm for efficient multiplication
- Inspired by GMP (GNU Multiple Precision Arithmetic Library)