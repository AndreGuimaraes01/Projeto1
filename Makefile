CC = gcc
# Adicionada a flag -O2 para melhor performance no processamento de pixels
CFLAGS = -std=c99 -Wall -Wextra -I/ucrt64/include/SDL3 -O2
# O -lm é essencial para a função sqrt() do histograma/desvio padrão
LIBS = -L/ucrt64/lib -lSDL3 -lSDL3_image -lSDL3_ttf -lm

SRC = src/main.c src/image.c
OBJ = $(SRC:.c=.o)
TARGET = programa

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET).exe $(TARGET)
