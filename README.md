# C Long Arithmetic Library

A high-performance arbitrary-precision arithmetic library written in C, implementing operations on integers of unlimited size.

## Features

- **Arithmetic**: Addition, subtraction, multiplication, and division operations
- **Signed Numbers**: Full support for both positive and negative integers
- **Optimized Algorithms**: Karatsuba multiplication for improved performance on large numbers
- **Factorial Computation**: Built-in factorial function for large numbers
- **String Conversion**: Convert arbitrary-precision integers to decimal string representation

## Building

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
├── test.c                  # Tester
├── Makefile                # Build system
└── README.md              
```
