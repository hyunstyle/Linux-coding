#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 10
typedef int buffer_item;
typedef enum {false, true} bool;

buffer_item buffer[BUFFER_SIZE];

buffer_item item;

buffer_item count;
buffer_item in;
buffer_item out;

sem_t mutex;
sem_t empty; // initialized to buffer_size
sem_t full; // initialized to 0

sem_t isValidProduce;
sem_t isValidConsume;

pthread_mutex_t sync;
pthread_mutex_t itemSync;

bool grantProduce;
bool grantConsume;
int tempRemove;

void* producer(void *param);
void* consumer(void *param);
void* produceMonitor(void *param);
void* consumeMonitor(void *param);

int insert_item(buffer_item item);
int remove_item(buffer_item *passedItem);


int main(int argc, char *argv[])
{
	if(argc != 4)
	{
		if(argc < 4)
		{
			printf("you must compile like a format of ./mutex argv[1], argv[2], argv[3]\n");
			exit(0);
		}
		else
		{
			printf("too many command line argument! after argv[3], is not allowed.\n");
			exit(0);
		}
	}
	pthread_t* tid_producer;
	tid_producer = (pthread_t*)malloc(sizeof(pthread_t)*(atoi(argv[2])));

	pthread_t* tid_consumer;
	tid_consumer = (pthread_t*)malloc(sizeof(pthread_t)*(atoi(argv[3])));
	
	pthread_t pro_monitor, con_monitor;
	int isCreated;

	count = 0;
	in = 0;
	out = 0;
	grantProduce = false;
	grantConsume = false;
	tempRemove = 0;
	
	pthread_mutex_init(&sync, NULL);
	pthread_mutex_init(&itemSync, NULL);
	
	if (sem_init(&mutex, 0, 1) == -1)
	{
		perror("semaphore initialization error.");
		exit(0);
	}
	if (sem_init(&full, 0, 0) == -1)
	{
		perror("semaphore initialization error.");
		exit(0);
	}

	if (sem_init(&empty, 0, BUFFER_SIZE) == -1)
	{
		perror("semaphore initialization error.");
		exit(0);
	}

	if (sem_init(&isValidProduce, 0, 0) == -1)
	{
		perror("semaphore initialization error.");
		exit(0);
	}

	if (sem_init(&isValidConsume, 0, 0) == -1)
	{
		perror("semaphore initialization error.");
		exit(0);
	}

	int i;
	for(i = 0; i<atoi(argv[2]); i++)
	{ 
		
		isCreated = pthread_create(&tid_producer[i], NULL, producer, 0);
		if(isCreated < 0)
		{
			perror("thread create error.\n");
			exit(0);
		}
	}

	for(i = 0; i<atoi(argv[3]); i++)
	{
		isCreated = pthread_create(&tid_consumer[i], NULL, consumer, 0);
		if(isCreated < 0)
		{
			perror("thread create error.\n");
			exit(0);
		}	

	}
	isCreated = pthread_create(&pro_monitor, NULL, produceMonitor, 0);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}

	isCreated = pthread_create(&con_monitor, NULL, consumeMonitor, 0);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	
	printf("main sleeps %d sec and terminated!!\n", atoi(argv[1]));
	sleep(atoi(argv[1]));

	
	free(tid_producer);
	free(tid_consumer);
	
	printf("End.\n");
	return 0;
}

void* producer(void *param)
{
	srand(time(NULL));

	while(true)
	{
		//printf("thread producer is running...\n");
		sleep(1);
		pthread_mutex_lock(&itemSync);
		item = rand()%100 + 1;
		sem_wait(&isValidProduce);
		if(grantProduce)
		{
			if(insert_item(item)) // fail
			{
				printf("produce error.\n");
			}
			else // success 
			{
				printf("Producer produced %d.\n", item);
				printf("current count : %d\n\n", count);
			}
		}
		else
		{
			printf("produce is rejected! item is : %d\n", item);
		}
		pthread_mutex_unlock(&itemSync);
	}
}

void* consumer(void *param)
{
	
	buffer_item nextConsumeItem; 

	while(true)
	{
		while(in == out)
		{
			printf("no item\n");
			sleep(1);
		}
		
		if(tempRemove != buffer[out])
			tempRemove = buffer[out];

		sleep(3);

		sem_wait(&isValidConsume);
		if(grantConsume)
		{
			if(remove_item(&nextConsumeItem))
			{
				printf("consume error.\n");
			}
			else
			{
				printf("Consumer cousumed %d\n", nextConsumeItem);
				printf("current count : %d\n\n", count);
			}
		}
		
	}
}

int insert_item(buffer_item item)
{
	int tempCnt;
	while(count == BUFFER_SIZE)
	{
		printf("buffer is full!\n");
		sleep(1);
	}
	//printf("item : %d\n", item);
	sem_wait(&empty);
	sem_wait(&mutex);
	tempCnt = count;
	buffer[in] = item;	
	in = (in + 1) % BUFFER_SIZE;
	count++;
	if(count != tempCnt)
	{
		sem_post(&mutex);
		sem_post(&full);
		return 0;
	}
	else
	{	
		sem_post(&mutex);
		sem_post(&full);
		return -1;
	}
	

	
}

int remove_item(buffer_item *passedItem)
{
	
	sem_wait(&full);
	sem_wait(&mutex);
	
	*passedItem = buffer[out];
	buffer[out] = 0;
	out = (out + 1) % BUFFER_SIZE;
	count--;
	grantConsume = false;
	tempRemove = 0;
	
	if(*passedItem != 0)
	{
		sem_post(&mutex);
		sem_post(&empty);
		return 0;
	}
	else
	{	sem_post(&mutex);
		sem_post(&empty);
		return -1;
	}
	

	
}

void* produceMonitor(void *param)
{
	
	int tempItem;
	while(true)
	{
		//printf("thread proMonitor is running...\n");
		tempItem = item;		
		sleep(1);
		if(tempItem != item)
		{
			
			if(item <= 50)
			{
				grantProduce = true;
				sem_post(&isValidProduce);
			}
			else
			{
				grantProduce = false;
				sem_post(&isValidProduce);
			}
			
		}
	}
}

void* consumeMonitor(void *param)
{
	
	while(true)
	{	
		sleep(1);

		if(tempRemove != 0)
		{
			pthread_mutex_lock(&sync);

			if(tempRemove <= 25)
			{	
				grantConsume = true;
				sem_post(&isValidConsume);
			}
			else
			{
				printf("monitor. before divide : %d\n", tempRemove);
				grantConsume = false;
				tempRemove = tempRemove / 2;
				buffer[out] = tempRemove;
				printf("monitor. after divide : %d\n", tempRemove);
				sem_post(&isValidConsume);
			}

			pthread_mutex_unlock(&sync);
		}
		
	}
}


