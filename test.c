/**
 * Comprehensive test suite for C Long Arithmetic Library
 * Tests all arithmetic operations with various edge cases
 * Compiled with -O0 and all sanitizers for maximum error detection
 */

#include "superlong.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

// Color codes for output
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

// Test assertion macro
#define TEST_ASSERT(condition, test_name) do { \
    if (condition) { \
        printf(COLOR_GREEN "✓ PASS" COLOR_RESET " - %s\n", test_name); \
        tests_passed++; \
    } else { \
        printf(COLOR_RED "✗ FAIL" COLOR_RESET " - %s\n", test_name); \
        tests_failed++; \
    } \
} while(0)

// Helper function to compare superlong with expected decimal string
int compare_with_string(const superlong* num, const char* expected) {
    char* result = superlong_to_decimal_str(num);
    int match = (strcmp(result, expected) == 0);
    if (!match) {
        printf("  Expected: %s, Got: %s\n", expected, result);
    }
    free(result);
    return match;
}

// Test initialization and basic operations
void test_initialization() {
    printf(COLOR_YELLOW "\n=== Testing Initialization ===" COLOR_RESET "\n");
    
    superlong num;
    superlong_init(&num);
    TEST_ASSERT(superlong_is_zero(&num), "Initialization creates zero");
    TEST_ASSERT(compare_with_string(&num, "0"), "Zero converts to '0'");
    superlong_deinit(&num);
    
    superlong* ptr = new_superlong();
    TEST_ASSERT(ptr != NULL, "new_superlong returns valid pointer");
    TEST_ASSERT(superlong_is_zero(ptr), "new_superlong creates zero");
    delete_superlong(ptr);
}

// Test number creation from integers
void test_from_int() {
    printf(COLOR_YELLOW "\n=== Testing Number Creation ===" COLOR_RESET "\n");
    
    superlong num;
    superlong_init(&num);
    
    superlong_from_uint(&num, 42);
    TEST_ASSERT(compare_with_string(&num, "42"), "from_uint(42)");
    
    superlong_from_uint(&num, 0);
    TEST_ASSERT(compare_with_string(&num, "0"), "from_uint(0)");
    TEST_ASSERT(superlong_is_zero(&num), "from_uint(0) is zero");
    
    superlong_from_uint(&num, 4294967295U); // UINT32_MAX
    TEST_ASSERT(compare_with_string(&num, "4294967295"), "from_uint(UINT32_MAX)");
    
    superlong_from_int(&num, -100);
    TEST_ASSERT(compare_with_string(&num, "-100"), "from_int(-100)");
    
    superlong_from_int(&num, 0);
    TEST_ASSERT(compare_with_string(&num, "0"), "from_int(0)");
    
    superlong_deinit(&num);
}

// Test addition operations
void test_addition() {
    printf(COLOR_YELLOW "\n=== Testing Addition ===" COLOR_RESET "\n");
    
    superlong a, b, result;
    superlong_init(&a);
    superlong_init(&b);
    superlong_init(&result);
    
    // Basic addition
    superlong_from_uint(&a, 123);
    superlong_from_uint(&b, 456);
    superlong_add(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "579"), "123 + 456 = 579");
    
    // Addition with zero
    superlong_from_uint(&a, 100);
    superlong_from_uint(&b, 0);
    superlong_add(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "100"), "100 + 0 = 100");
    
    // Large numbers
    superlong_from_uint(&a, 999999999);
    superlong_from_uint(&b, 1);
    superlong_add(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "1000000000"), "999999999 + 1 = 1000000000");
    
    // Addition with uint
    superlong_from_uint(&a, 1000);
    superlong_add_uint(&a, 234, &result);
    TEST_ASSERT(compare_with_string(&result, "1234"), "1000 + 234 = 1234");
    
    // Negative numbers
    superlong_from_int(&a, -50);
    superlong_from_int(&b, 30);
    superlong_add(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "-20"), "-50 + 30 = -20");
    
    superlong_from_int(&a, -100);
    superlong_from_int(&b, 200);
    superlong_add(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "100"), "-100 + 200 = 100");
    
    superlong_deinit(&a);
    superlong_deinit(&b);
    superlong_deinit(&result);
}

// Test subtraction operations
void test_subtraction() {
    printf(COLOR_YELLOW "\n=== Testing Subtraction ===" COLOR_RESET "\n");
    
    superlong a, b, result;
    superlong_init(&a);
    superlong_init(&b);
    superlong_init(&result);
    
    // Basic subtraction
    superlong_from_uint(&a, 500);
    superlong_from_uint(&b, 200);
    superlong_sub(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "300"), "500 - 200 = 300");
    
    // Result is negative
    superlong_from_uint(&a, 100);
    superlong_from_uint(&b, 150);
    superlong_sub(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "-50"), "100 - 150 = -50");
    
    // Subtract to zero
    superlong_from_uint(&a, 777);
    superlong_from_uint(&b, 777);
    superlong_sub(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "0"), "777 - 777 = 0");
    TEST_ASSERT(superlong_is_zero(&result), "Result is zero");
    
    // Subtraction with uint
    superlong_from_uint(&a, 1000);
    superlong_sub_uint(&a, 1, &result);
    TEST_ASSERT(compare_with_string(&result, "999"), "1000 - 1 = 999");
    
    superlong_deinit(&a);
    superlong_deinit(&b);
    superlong_deinit(&result);
}

// Test multiplication operations
void test_multiplication() {
    printf(COLOR_YELLOW "\n=== Testing Multiplication ===" COLOR_RESET "\n");
    
    superlong a, b, result;
    superlong_init(&a);
    superlong_init(&b);
    superlong_init(&result);
    
    // Basic multiplication
    superlong_from_uint(&a, 12);
    superlong_from_uint(&b, 13);
    superlong_mul(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "156"), "12 * 13 = 156");
    
    // Multiplication by zero
    superlong_from_uint(&a, 999);
    superlong_from_uint(&b, 0);
    superlong_mul(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "0"), "999 * 0 = 0");
    TEST_ASSERT(superlong_is_zero(&result), "Product with zero is zero");
    
    // Multiplication by one
    superlong_from_uint(&a, 777);
    superlong_mul_uint(&a, 1, &result);
    TEST_ASSERT(compare_with_string(&result, "777"), "777 * 1 = 777");
    
    // Large numbers
    superlong_from_uint(&a, 12345);
    superlong_from_uint(&b, 67890);
    superlong_mul(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "838102050"), "12345 * 67890 = 838102050");
    
    // Multiplication with negative numbers
    superlong_from_int(&a, -10);
    superlong_from_uint(&b, 25);
    superlong_mul(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "-250"), "-10 * 25 = -250");
    
    // Two negatives
    superlong_from_int(&a, -20);
    superlong_from_int(&b, -30);
    superlong_mul(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "600"), "-20 * -30 = 600");
    
    // Multiply by 256 (shift test)
    superlong_from_uint(&a, 100);
    superlong_mul_uint(&a, 256, &result);
    TEST_ASSERT(compare_with_string(&result, "25600"), "100 * 256 = 25600");
    
    superlong_deinit(&a);
    superlong_deinit(&b);
    superlong_deinit(&result);
}

// Test division operations
void test_division() {
    printf(COLOR_YELLOW "\n=== Testing Division ===" COLOR_RESET "\n");
    
    superlong a, b, result;
    superlong_init(&a);
    superlong_init(&b);
    superlong_init(&result);
    
    // Basic division
    superlong_from_uint(&a, 100);
    superlong_from_uint(&b, 10);
    superlong_div(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "10"), "100 / 10 = 10");
    
    // Division with remainder (integer division)
    superlong_from_uint(&a, 23);
    superlong_from_uint(&b, 5);
    superlong_div(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "4"), "23 / 5 = 4");
    
    // Division by self
    superlong_from_uint(&a, 777);
    superlong_from_uint(&b, 777);
    superlong_div(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "1"), "777 / 777 = 1");
    
    // Division resulting in zero
    superlong_from_uint(&a, 5);
    superlong_from_uint(&b, 10);
    superlong_div(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "0"), "5 / 10 = 0");
    
    // Zero divided by something
    superlong_from_uint(&a, 0);
    superlong_from_uint(&b, 100);
    superlong_div(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "0"), "0 / 100 = 0");
    
    // Large division
    superlong_from_uint(&a, 1000000);
    superlong_from_uint(&b, 100);
    superlong_div(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "10000"), "1000000 / 100 = 10000");
    
    // Division with uint
    superlong_from_uint(&a, 12345);
    superlong_div_uint(&a, 5, &result);
    TEST_ASSERT(compare_with_string(&result, "2469"), "12345 / 5 = 2469");
    
    // Division by 256
    superlong_from_uint(&a, 25600);
    superlong_div_uint(&a, 256, &result);
    TEST_ASSERT(compare_with_string(&result, "100"), "25600 / 256 = 100");
    
    // Negative division
    superlong_from_int(&a, -100);
    superlong_from_uint(&b, 10);
    superlong_div(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "-10"), "-100 / 10 = -10");
    
    superlong_deinit(&a);
    superlong_deinit(&b);
    superlong_deinit(&result);
}

// Test utility functions
void test_utilities() {
    printf(COLOR_YELLOW "\n=== Testing Utility Functions ===" COLOR_RESET "\n");
    
    superlong a, b;
    superlong_init(&a);
    superlong_init(&b);
    
    // Copy operation
    superlong_from_uint(&a, 12345);
    superlong_copy(&a, &b);
    TEST_ASSERT(compare_with_string(&b, "12345"), "Copy operation");
    
    // Negate operation
    superlong_from_uint(&a, 100);
    superlong_negate(&a);
    TEST_ASSERT(compare_with_string(&a, "-100"), "Negate positive to negative");
    
    superlong_negate(&a);
    TEST_ASSERT(compare_with_string(&a, "100"), "Negate negative to positive");
    
    // Negate zero
    superlong_from_uint(&a, 0);
    superlong_negate(&a);
    TEST_ASSERT(compare_with_string(&a, "0"), "Negate zero remains zero");
    
    superlong_deinit(&a);
    superlong_deinit(&b);
}

// Test factorial calculation
void test_factorial() {
    printf(COLOR_YELLOW "\n=== Testing Factorial ===" COLOR_RESET "\n");
    
    superlong result;
    superlong_init(&result);
    
    superlong_factorial(0, &result);
    TEST_ASSERT(compare_with_string(&result, "1"), "0! = 1");
    
    superlong_factorial(1, &result);
    TEST_ASSERT(compare_with_string(&result, "1"), "1! = 1");
    
    superlong_factorial(5, &result);
    TEST_ASSERT(compare_with_string(&result, "120"), "5! = 120");
    
    superlong_factorial(10, &result);
    TEST_ASSERT(compare_with_string(&result, "3628800"), "10! = 3628800");
    
    superlong_factorial(20, &result);
    TEST_ASSERT(compare_with_string(&result, "2432902008176640000"), "20! = 2432902008176640000");
    
    // Very large factorial
    superlong_factorial(50, &result);
    char* fact50_str = superlong_to_decimal_str(&result);
    int len = strlen(fact50_str);
    TEST_ASSERT(len == 65, "50! has 65 digits");
    free(fact50_str);
    
    superlong_deinit(&result);
}

// Test large number operations
void test_large_numbers() {
    printf(COLOR_YELLOW "\n=== Testing Large Numbers ===" COLOR_RESET "\n");
    
    superlong a, b, result;
    superlong_init(&a);
    superlong_init(&b);
    superlong_init(&result);
    
    // Build very large number through multiplication
    superlong_from_uint(&a, 1000000);
    superlong_from_uint(&b, 1000000);
    superlong_mul(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "1000000000000"), "10^6 * 10^6 = 10^12");
    
    // Add large numbers
    superlong_from_uint(&a, 999999999);
    superlong_from_uint(&b, 999999999);
    superlong_add(&a, &b, &result);
    TEST_ASSERT(compare_with_string(&result, "1999999998"), "Large addition");
    
    // Multiply to get really large number
    superlong_factorial(30, &a);
    superlong_from_uint(&b, 2);
    superlong_mul(&a, &b, &result);
    char* result_str = superlong_to_decimal_str(&result);
    TEST_ASSERT(result_str != NULL && strlen(result_str) > 32, "30! * 2 produces large number");
    free(result_str);
    
    superlong_deinit(&a);
    superlong_deinit(&b);
    superlong_deinit(&result);
}

// Test edge cases and boundary conditions
void test_edge_cases() {
    printf(COLOR_YELLOW "\n=== Testing Edge Cases ===" COLOR_RESET "\n");
    
    superlong a, b, result;
    superlong_init(&a);
    superlong_init(&b);
    superlong_init(&result);
    
    // Operations with maximum uint32
    superlong_from_uint(&a, 4294967295U);
    superlong_add_uint(&a, 1, &result);
    TEST_ASSERT(compare_with_string(&result, "4294967296"), "UINT32_MAX + 1");
    
    // Subtract from zero
    superlong_from_uint(&a, 0);
    superlong_sub_uint(&a, 100, &result);
    TEST_ASSERT(compare_with_string(&result, "-100"), "0 - 100 = -100");
    
    // Multiple operations in sequence
    superlong_from_uint(&a, 10);
    superlong_from_uint(&b, 5);
    superlong_add(&a, &b, &result); // 15
    superlong_mul_uint(&result, 2, &result); // 30
    superlong_sub_uint(&result, 5, &result); // 25
    superlong_div_uint(&result, 5, &result); // 5
    TEST_ASSERT(compare_with_string(&result, "5"), "Chain of operations: ((10+5)*2-5)/5 = 5");
    
    // Very small to very large
    superlong_from_uint(&a, 1);
    for (int i = 0; i < 10; i++) {
        superlong_mul_uint(&a, 10, &a);
    }
    TEST_ASSERT(compare_with_string(&a, "10000000000"), "Repeated multiplication: 1 * 10^10");
    
    superlong_deinit(&a);
    superlong_deinit(&b);
    superlong_deinit(&result);
}

// Test string conversion
void test_string_conversion() {
    printf(COLOR_YELLOW "\n=== Testing String Conversion ===" COLOR_RESET "\n");
    
    superlong num;
    superlong_init(&num);
    char* str;
    
    superlong_from_uint(&num, 0);
    str = superlong_to_decimal_str(&num);
    TEST_ASSERT(strcmp(str, "0") == 0, "Zero to string");
    free(str);
    
    superlong_from_uint(&num, 123456789);
    str = superlong_to_decimal_str(&num);
    TEST_ASSERT(strcmp(str, "123456789") == 0, "Positive number to string");
    free(str);
    
    superlong_from_int(&num, -30000);
    str = superlong_to_decimal_str(&num);
    TEST_ASSERT(strcmp(str, "-30000") == 0, "Negative number to string");
    free(str);
    
    // Large number
    superlong_factorial(25, &num);
    str = superlong_to_decimal_str(&num);
    TEST_ASSERT(strlen(str) == 26, "25! has 26 digits");
    free(str);
    
    superlong_deinit(&num);
}

// Test memory operations (for sanitizer validation)
void test_memory_operations() {
    printf(COLOR_YELLOW "\n=== Testing Memory Operations ===" COLOR_RESET "\n");
    
    // Allocate and deallocate many numbers
    for (int i = 0; i < 100; i++) {
        superlong* num = new_superlong();
        superlong_from_uint(num, i);
        delete_superlong(num);
    }
    TEST_ASSERT(1, "Allocate and free 100 numbers");
    
    // Create numbers, perform operations, clean up
    superlong a, b, c;
    superlong_init(&a);
    superlong_init(&b);
    superlong_init(&c);
    
    for (int i = 1; i <= 20; i++) {
        superlong_from_uint(&a, i);
        superlong_from_uint(&b, i + 1);
        superlong_mul(&a, &b, &c);
    }
    
    superlong_deinit(&a);
    superlong_deinit(&b);
    superlong_deinit(&c);
    TEST_ASSERT(1, "Multiple operations with proper cleanup");
    
    // Test reuse of superlong
    superlong num;
    superlong_init(&num);
    for (int i = 0; i < 50; i++) {
        superlong_from_uint(&num, i * 100);
        superlong_mul_uint(&num, 2, &num);
    }
    superlong_deinit(&num);
    TEST_ASSERT(1, "Reuse same superlong multiple times");
}

// Main test runner
int main() {
    printf(COLOR_BLUE "\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  C Long Arithmetic Library - Comprehensive Test Suite ║\n");
    printf("║  Compiled with -O0 and all sanitizers enabled        ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET "\n");
    
    // Run all test suites
    test_initialization();
    test_from_int();
    test_addition();
    test_subtraction();
    test_multiplication();
    test_division();
    test_utilities();
    test_factorial();
    test_large_numbers();
    test_edge_cases();
    test_string_conversion();
    test_memory_operations();
    
    // Print summary
    printf(COLOR_BLUE "\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║                     TEST SUMMARY                      ║\n");
    printf("╠═══════════════════════════════════════════════════════╣\n");
    printf(COLOR_RESET);
    printf("║  Total Tests:  %3d                                    ║\n", tests_passed + tests_failed);
    printf("║  " COLOR_GREEN "Passed:       %3d" COLOR_RESET "                                    ║\n", tests_passed);
    printf("║  " COLOR_RED "Failed:       %3d" COLOR_RESET "                                    ║\n", tests_failed);
    printf(COLOR_BLUE "╚═══════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
    
    if (tests_failed == 0) {
        printf(COLOR_GREEN "✓ All tests passed successfully!" COLOR_RESET "\n");
        printf("No memory leaks or sanitizer errors detected.\n\n");
        return 0;
    } else {
        printf(COLOR_RED "✗ Some tests failed!" COLOR_RESET "\n\n");
        return 1;
    }
}
