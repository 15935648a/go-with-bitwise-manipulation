CC      = gcc
CFLAGS  = -O2 -Wall -Wextra
LDFLAGS = -lm

TARGET  = go_engine
SRCS    = bitboard.c mcts.c gtp.c main.c
OBJS    = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

train: $(TARGET)
	./$(TARGET) --train 10000

gtp: $(TARGET)
	./$(TARGET) --gtp

.PHONY: clean train gtp
