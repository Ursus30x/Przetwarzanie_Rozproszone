#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <pqueue.h>

char *filename = "queue.dat";

int itemId = 0;

typedef struct item item;
struct item {
	int id;
};


item *
produce() {
	int time = rand() % 5 + 1;
	item *it = (item *)malloc(sizeof(item));
	
	sleep(time);
	it->id = itemId;
	itemId += 1;
	return it;
}

void
consume(item *it) {
	int time = rand() % 5 + 1;
	sleep(time);
	free(it);
}


void
producer(pid_t childPid) {
}

void
consumer() {
}

int
main(int argc, char **argv) {
	pid_t pid;
	pqueue *qu = NULL;
	/* watch -n 1 ps -l --forest */
	
	/* create empty queue */
	qserialize(qu, sizeof(item), filename);
	

	return 0;
}

