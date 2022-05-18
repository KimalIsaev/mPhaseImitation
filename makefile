.PHONY: clean exe

exe:
	clang -O3 -lm avl_tree.c distribution_functions.c read_double_pair_from_file.c main.c -o mPhaseImitation.exe

clean:
	rm mPhaseImitation.exe
