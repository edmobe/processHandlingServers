PHONY = clean

EXE = client
SRCS = $(EXE).c
OBJS=$(SRCS:.c=.o)
CC = gcc
FLAGS= -fopenmp

$(EXE): $(OBJS)
		$(CC) $(OBJS) -o $(EXE) $(FLAGS)

.c.o:
		$(CC) -c $< -o $@ $(FLAGS)

clean:
		rm $(OBJS) $(EXE)
