#pragma once

#include <array>
#include <memory>

#define ROWS_COUNT 16
#define COLS_COUNT 30

typedef int Type;

extern const Type RowsCount;
extern const Type ColsCount;


extern const Type FieldSize;


typedef std::shared_ptr<std::array<std::array<Type, COLS_COUNT>, ROWS_COUNT>> PtrFields;
typedef std::pair<Type, Type> FieldCoords;
