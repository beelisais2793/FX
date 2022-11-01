#!/bin/sh
cd build/bin || echo "Error: Please run this script from the project's root directory as ./scripts/valgrind-linux.sh"

echo "Started valgrind..."
valgrind --num-callers=50 \
	--leak-resolution=high \
	--leak-check=full \
	--track-origins=yes \
	--time-stamp=yes \
	--suppressions=../../scripts/valgrind-suppressions.supp \
	./test_dpsfg 2>&1 | tee ../../dpsfg_tests_grind.out
cd .. && cd ..
