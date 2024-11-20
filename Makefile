# Define the compiler
CC = gcc

# Define the compiler flags
CFLAGS = -I/opt/homebrew/opt/sdl2/include -I/opt/homebrew/opt/fftw/include
LDFLAGS = -L/opt/homebrew/opt/sdl2/lib -L/opt/homebrew/opt/fftw/lib
LIBS = -lSDL2 -lfftw3

# Define the output file
TARGET = AudioVisualiser

# Define the source files
SRC = main.c

# Define the object files
OBJ = $(SRC:.c=.o)

# Default rule
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS) $(LIBS)

# Compile the source files into object files
%.o: %.c
	$(CC) -c $(CFLAGS) $<

# Clean up the compiled files
clean:
	rm -f $(OBJ) $(TARGET)