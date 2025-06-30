#pragma once

#include <cstdint>

typedef char char8;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef size_t usize;

typedef float float32;
typedef double float64;

typedef bool bool8;

constexpr uint64 uint64_max = 0xFFFFFFFFFFFFFFFF;
constexpr uint32 uint32_max = 0xFFFFFFFF;
constexpr uint16 uint16_max = 0xFFFF;
constexpr uint8 uint8_max = 0xFF;

#include <type_traits>

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;