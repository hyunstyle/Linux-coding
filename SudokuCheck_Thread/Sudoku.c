#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 

// sudoku check program //
//  semester 2017 - 1   //
//     made by YSH      // 


#define LEFTTOP 1
#define CENTERTOP 2
#define RIGHTTOP 3
#define LEFTCENTER 4
#define CENTER 5
#define RIGHTCENTER 6
#define LEFTBOTTOM 7
#define CENTERBOTTOM 8
#define RIGHTBOTTOM 9

typedef enum {false, true} bool;
char* fname = "input.txt";
FILE* inputFile;
char inputSudoku[9][9];

bool initializeSudoku();
bool checkSubMatrix(char* arr, int location);
void* horizonCheckThread(void* arg);
void* verticalCheckThread(void* arg);
void* subCheckThread(void* arg);


int main()
{
	pthread_t tid_1, tid_2, tid_3, tid_4, tid_5, tid_6, tid_7, tid_8, tid_9, tid_10, tid_11;
	int isCreated;
	void* r_1; 
	void* r_2; 
	void* r_3; 
	void* r_4; 
	void* r_5; 
	void* r_6; 
	void* r_7; 
	void* r_8; 
	void* r_9; 
	void* r_10; 
	void* r_11; 

	// initializing input sudoku. if 1 row has more than 9 munber, exit main function.
	bool isValid;
	isValid = initializeSudoku();
	if(!isValid)
	{
		return 0;
	}

	printf("----input.txt has 9*9 sudoku format----\n");

	isCreated = pthread_create(&tid_1, NULL, horizonCheckThread, NULL);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_2, NULL, verticalCheckThread, NULL);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}

	isCreated = pthread_create(&tid_3, NULL, subCheckThread, (void*)LEFTTOP);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_4, NULL, subCheckThread, (void*)CENTERTOP);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_5, NULL, subCheckThread, (void*)RIGHTTOP);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_6, NULL, subCheckThread, (void*)LEFTCENTER);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_7, NULL, subCheckThread, (void*)CENTER);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_8, NULL, subCheckThread, (void*)RIGHTCENTER);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_9, NULL, subCheckThread, (void*)LEFTBOTTOM);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_10, NULL, subCheckThread, (void*)CENTERBOTTOM);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}
	isCreated = pthread_create(&tid_11, NULL, subCheckThread, (void*)RIGHTBOTTOM);
	if(isCreated < 0)
	{
		perror("thread create error.\n");
		exit(0);
	}

	
	
	pthread_join(tid_1, &r_1);
	pthread_join(tid_2, &r_2);
	pthread_join(tid_3, &r_3);
	pthread_join(tid_4, &r_4);
	pthread_join(tid_5, &r_5);
	pthread_join(tid_6, &r_6);
	pthread_join(tid_7, &r_7);
	pthread_join(tid_8, &r_8);
	pthread_join(tid_9, &r_9);
	pthread_join(tid_10, &r_10);
	pthread_join(tid_11, &r_11);

	printf("All created threads are terminated.\n\n");

	
	if((int)r_1 == 1 && (int)r_2 == 1 && (int)r_3 == 1 && (int)r_4 == 1 
		&& (int)r_5 == 1 && (int)r_6 == 1 && (int)r_7 == 1 && (int)r_8 == 1 
		&& (int)r_9 == 1 && (int)r_10 == 1 && (int)r_11 == 1)
	{
		printf("Valid result ! \n");
	}
	else
	{
		printf("Invalid result ! \n");
	}

	return 0;
}

void* horizonCheckThread(void* arg)
{
	printf("thread %d is running...\n", 1);
	int valid = 1;
	int unvalid = -1;
	int i, j, k;
	for(i = 0; i < 9; i++)
	{
		for(j = 0; j < 9; j++)
		{
			for(k = j+1; k<9; k++)
			{
				if(inputSudoku[i][j] == inputSudoku[i][k])
				{
					printf("row: %d and col: %d, row: %d and col: %d has same value.\n"
							, i+1, j+1, i+1, k+1);
					pthread_exit((void*)unvalid);
				}
			} 
			//printf("row %d is valid.\n", j);
		}

	}

	pthread_exit((void*)valid);
}

void* verticalCheckThread(void* arg)
{
	printf("thread %d is running...\n", 2);
	int valid = 1;
	int unvalid = -1;
	int i, j, k;
	for(i = 0; i < 9; i++)
	{
		for(j = 0; j < 9; j++)
		{
			for(k = j+1; k<9; k++)
			{
				if(inputSudoku[j][i] == inputSudoku[k][i])
				{
					printf("row: %d and col: %d, row: %d and col: %d has same value.\n"
							, j+1, i+1, k+1, i+1);
					pthread_exit((void*)unvalid);
				}
			} 
			//printf("row %d is valid.\n", j);
		}

	}

	pthread_exit((void*)valid);
}

void* subCheckThread(void* arg)
{
	char subMatrix[9];
	int valid = 1;
	int unvalid = -1;
	int i, j, k;
	switch((int)arg)
	{
		case LEFTTOP:
			printf("thread %d is running...\n", 3);
			k = 0;
			for(i = 0; i < 3; i++)
			{
				for(j = 0; j < 3; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, LEFTTOP))
			{
				pthread_exit((void*)unvalid);
			}
			
			pthread_exit((void*)valid);
			break;
		case CENTERTOP:
			printf("thread %d is running...\n", 4);
			k = 0;
			for(i = 0; i < 3; i++)
			{
				for(j = 3; j < 6; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, CENTERTOP))
			{
				pthread_exit((void*)unvalid);
			}

			pthread_exit((void*)valid);
			break;
		case RIGHTTOP:
			printf("thread %d is running...\n", 5);
			k = 0;
			for(i = 0; i < 3; i++)
			{
				for(j = 6; j < 9; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, RIGHTTOP))
			{
				pthread_exit((void*)unvalid);
			}

			pthread_exit((void*)valid);
			break;
		case LEFTCENTER:
			printf("thread %d is running...\n", 6);
			k = 0;
			for(i = 3; i < 6; i++)
			{
				for(j = 0; j < 3; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, LEFTCENTER))
			{
				pthread_exit((void*)unvalid);
			}

			pthread_exit((void*)valid);
			break;
		case CENTER:
			printf("thread %d is running...\n", 7);
			k = 0;
			for(i = 3; i < 6; i++)
			{
				for(j = 3; j < 6; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, CENTER))
			{
				pthread_exit((void*)unvalid);
			}

			pthread_exit((void*)valid);
			break;
		case RIGHTCENTER:
			printf("thread %d is running...\n", 8);
			k = 0;
			for(i = 3; i < 6; i++)
			{
				for(j = 6; j < 9; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, RIGHTCENTER))
			{
				pthread_exit((void*)unvalid);
			}

			pthread_exit((void*)valid);
			break;
		case LEFTBOTTOM:
			printf("thread %d is running...\n", 9);
			k = 0;
			for(i = 6; i < 9; i++)
			{
				for(j = 0; j < 3; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, LEFTBOTTOM))
			{
				pthread_exit((void*)unvalid);
			}

			pthread_exit((void*)valid);
			break;
		case CENTERBOTTOM:
			printf("thread %d is running...\n", 10);
			k = 0;
			for(i = 6; i < 9; i++)
			{
				for(j = 3; j < 6; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, CENTERBOTTOM))
			{
				pthread_exit((void*)unvalid);
			}

			pthread_exit((void*)valid);
			break;
		case RIGHTBOTTOM:
			printf("thread %d is running...\n", 11);
			k = 0;
			for(i = 6; i < 9; i++)
			{
				for(j = 6; j < 9; j++)
				{
					subMatrix[k] = inputSudoku[i][j];
					k++;
				}
			}
			if(!checkSubMatrix(subMatrix, RIGHTBOTTOM))
			{
				pthread_exit((void*)unvalid);
			}

			pthread_exit((void*)valid);
			break;


	}

	
}

bool checkSubMatrix(char* arr, int location)
{
	int i, j;
	for(i = 0; i<9; i++)
	{
		for(j = i; j<8; j++)
		{
			if(arr[i] == arr[j+1])
			{
				printf("%d submatrix has unvalid sudoku format.\n", location);
				return false;
			} 
		}
	}

	return true;
}

bool initializeSudoku()
{
	if((inputFile=fopen(fname, "r"))==NULL)
	{
		printf("file open error");
		return false;
	}
	
	printf("file is opened\n");
	
	int p;
	int i = 0, j = 0;
	while((p = fgetc(inputFile)) != EOF)
	{
		if(p == ' ')
		{
			continue;
		}
		else if(p == '\n')
		{
			if(i != 9)
			{
				printf("Invalid result! \n");
				fclose(inputFile);
				return false;
			}
			i = 0;
			j++;
		}
		else
		{
			if(p != '1' && p!= '2' && p!= '3' && p!= '4' && p!= '5' && p!= '6'
				 && p!= '7' && p!= '8' && p!= '9')
			{
				printf("Invalid result! \n");
				fclose(inputFile);
				return false;
			}
			if(i > 8)
			{
				printf("Invalid result! \n");
				fclose(inputFile);
				return false;
			}
			inputSudoku[j][i] = p;
			i++;
		}
	}

	printf("------input sudoku------\n\n");

	for(i = 0; i<9; i++)
	{
		for(j = 0; j<9; j++)
		{
			printf("%c ", inputSudoku[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	fclose(inputFile);
	return true;
}


