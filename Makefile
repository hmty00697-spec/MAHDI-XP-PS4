TARGET = MAHDI-XP-PS4
OBJS = main.o

CC = clang
CFLAGS = -O2 -Wall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f $(OBJS) $(TARGET)
