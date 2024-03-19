CSTANDARD := -ansi

_FLAGS   :=-Wall -Wextra -pedantic -Wparentheses -Wformat=2 \
           -Wshadow -Wwrite-strings -Wredundant-decls -Wmissing-include-dirs \
					 -pedantic-errors -Wuninitialized -Wmissing-declarations \
					 -Wdouble-promotion -Wfloat-equal -Wmain -Wundef
_CFLAGS  :=$(CSTANDARD) -Wstrict-prototypes -Wnested-externs -Wold-style-definition \
          -Wbad-function-cast -Wno-unknown-pragmas -Wno-unused-function
# GCC warnings that Clang doesn't provide:
ifeq ($(CC),gcc)
	_CFLAGS+=-Wjump-misses-init -Wlogical-op
endif

all:
	gcc *.c $(_FLAGS) $(_CFLAGS)

debug:
	gcc *.c -g3 $(_FLAGS) $(_CFLAGS)
