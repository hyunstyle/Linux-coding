#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // 12123215 Yu Sang Hyeon //


long long* resultArray;
void* fiboThread(void* arg);


int main()
{
	int input;
	void* status;
	printf("Input: ");
	scanf("%d", &input);

	pthread_t tid;
	int isCreated;

	isCreated = pthread_create(&tid, NULL, fiboThread, (void*)input);
	if(isCreated < 0)
	{
		perror("thread create error : ");
		exit(0);
	}

	pthread_join(tid, &status);
	printf("-------------------------------\nthread is terminated. \n");

	if ( (int)status <= 0)
	{
		printf("input format error.\n");
		return;
	}
	else if ( (int)status == 1)
	{
		printf("Output: 0\n");
		return;
	}
	else
	{
		int i;

		printf("Output: ");
		for(i = 0; i<input; i++)
		{
			printf("%lld ", resultArray[i]);
		}
		printf("\n");

		free(resultArray);
		return;
	}
	
	
	
}

void* fiboThread(void* arg)
{
	printf("-------------------------------\nthread is running...\n");
	int input = (int)arg;

	if(input <= 0)
	{
		printf("%d is unvalid input format. input must bigger than 0\n", input); 
		pthread_exit((void*)arg);
		
	}
	else if (input == 1)
	{
		pthread_exit((void*)arg); 
	}
	else
	{
		resultArray = (long long*)malloc(sizeof(long long) * (input));
		resultArray[0] = 0;
		resultArray[1] = 1;
	
		int i;
		for(i = 2; i<input; i++)
		{
			resultArray[i] = resultArray[i-1] + resultArray[i-2];
		}

		printf("thread finishes execution.\n");
		pthread_exit((void*)arg);
		
	}
}
