#pragma once

#ifdef _MSC_VER
#	pragma warning(disable: 4116) // disable type definition warning in msvc
#	define GC_REG_VAL(Register, VariableToStockVal) __asm mov VariableToStockVal, Register
#else
#	define GC_REG_VAL(Register, VariableToStockVal) asm volatile ("movl %%"#Register", %0" : "=r" (VariableToStockVal))
#endif

#define GC_PADDING_SIZE (sizeof(struct{ int A; char B; }) - sizeof(int))
#define GC_MAX_OBJ_INIT 6
