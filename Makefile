CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c2x -g

SRCS=main.c hash_table.c
OBJS=$(SRCS:.c=.o)
TARGET=shakespeare

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full -v ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
