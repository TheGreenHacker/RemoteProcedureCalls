CC = gcc
CFLAGS = -Wall -Wextra

default: server client

server: ll_t.o person_t.o serialize_int.o ll_pair_t.o
	$(CC) $(CFLAGS) ../serialize.c ll_t/ll_t.c person_t/person_t.c serialize_int/serialize_int.c ll_pair_t/ll_pair_t.c server.c -o server

client: ll_t.o person_t.o serialize_int.o
	$(CC) $(CFLAGS) ../serialize.c ll_t/ll_t.c person_t/person_t.c serialize_int/serialize_int.c ll_pair_t/ll_pair_t.c client.c -o client

ll_t.o:
	$(CC) $(CFLAGS) -c ll_t/ll_t.c -o ll_t.o

person_t.o:
	$(CC) $(CFLAGS) -c person_t/person_t.c -o person_t.o

serialize_int.o:
	$(CC) $(CFLAGS) -c serialize_int/serialize_int.c -o serialize_int.o

ll_pair_t.o:
	$(CC) $(CFLAGS) -c ll_pair_t/ll_pair_t.c -o ll_pair_t.o

clean:
	rm -f ll_t.o person_t.o serialize_int.o ll_pair_t.o client server