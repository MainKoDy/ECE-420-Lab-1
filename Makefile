CC = gcc
OBJECTS = lab1_IO.o matrix_mult_pth.o

# Compile all object files, create executable './main'.
run: $(OBJECTS)
	$(CC) -g -Wall -o main $(OBJECTS) -lpthread -lm

# Compile matrixgen, create executable './matrixgen'.
runmat: matrixgen.o
	$(CC) -o matrixgen matrixgen.o

# Create object file 'matrix_multi_pth.o'.
matrix_multi_pth.o: matrix_mult_pth.c
	$(CC) -c matrix_mult_pth.c

# Create object file 'lab1_IO.o'.
lab1_IO.o: lab1_IO.c
	$(CC) -c lab1_IO.c

# Create object file 'matrixgen.o'.
matrixgen.o: matrixgen.c
	$(CC) -c matrixgen.c

# Delete all object files and the executable.
clean:
	rm -f ./main $(OBJECTS)

# Delete all object files and executables pertaining to "matrixgen", including the data_input file
cleanmat:
	rm -f data_input data_output ./matrixgen matrixgen.o