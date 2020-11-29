PHONY = clean

EXE = client
SRCS = $(EXE).c
OBJS=$(SRCS:.c=.o)
CC = gcc

$(EXE): $(OBJS)
		$(CC) $(OBJS) -o $(EXE)

.c.o:
		$(CC) -c $< -o $@

clean:
		rm $(OBJS) $(EXE)
