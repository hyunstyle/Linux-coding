#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define THE_NUMBER_OF_FRAME 128
#define THE_NUMBER_OF_TLB 16


// made by Yu Sang Hyeon, VIRTUAL MEMORY MANAGER.
// 2017 - 06

typedef enum {false, true} bool;

FILE* txtLogicalAddress;
FILE* BACKING_STORE;
FILE* txtPhysicalAddress;
FILE* txtFrameTable;
char readBuffer[20];
char writeBuffer[100];
unsigned char backingStoreBuffer[PAGE_SIZE];
int logicalAddress;
int logicalAddressTable[PAGE_SIZE];
int frameNumber;
int pageNumber;
int pageOffset;

int pageTable[PAGE_SIZE]; // return the index's frame number. if not assigned, return null;
bool validInvalidBit[PAGE_SIZE];
clock_t frameInsertedTime[THE_NUMBER_OF_FRAME];
clock_t pageReferencedTime[PAGE_SIZE];
int framePointer;

int TLB[THE_NUMBER_OF_TLB][2]; // [0] -> page number [1] -> frame number
clock_t TLBinsertedTime[THE_NUMBER_OF_TLB];
int TLBhit;
int filledTLB;
int pageFaultCount;
int translatedAddressCount;

int physicalFrame[THE_NUMBER_OF_FRAME];
int physicalOffset[THE_NUMBER_OF_FRAME];
int pageInformation[PAGE_SIZE][PAGE_SIZE];

//function prototype.
bool searchTLB(int pageNum);
bool searchPageTable(int pageNum);
bool searchBackingStore(int pageNum);
void updateTLB(int pageNum, int frameNum);
void replacePageWithLRU(int pageNum);
void replacePageWithFIFO(int pageNum);


int main(int argc, char *argv[])
{
	if (argc != 2) 
	{
        printf("please execute like './memory_manager addresses.txt'\n");
        return -1;
    }

	framePointer = 0;
	TLBhit = 0;
	filledTLB = 0;
	pageFaultCount = 0;
	translatedAddressCount = 0;

	int i;
	for(i = 0; i<PAGE_SIZE; i++)
	{
		pageTable[i] = -1; 
		validInvalidBit[i] = false;
	}

	if((txtLogicalAddress = fopen(argv[1], "r")) == NULL)
	{
		printf("addresses.txt open error!\n");
		return -1;	
	}
	if((BACKING_STORE = fopen("BACKING_STORE.bin", "rb")) == NULL) // read binary file
	{
		printf("BACKING_STORE.bin open error!\n");
		return -1;
	}

	if((txtPhysicalAddress = fopen("physical.txt", "w+")) == NULL)
	{
		printf("physical.txt make error!\n");
		return -1;
	}
	
	if((txtFrameTable = fopen("frame_table.txt", "w+")) == NULL)
	{
		printf("frame_table.txt make error!\n");
		return -1;
	}

	fwrite("<with LRU replacement, LRU TLB>\n", strlen("<with LRU replacement, LRU TLB>\n"), 1, 				txtPhysicalAddress);
	fwrite("<the last frame table>\n", strlen("<the last frame table>\n"), 1, txtFrameTable);

	// translation start
	while(fgets(readBuffer, 10, txtLogicalAddress) != NULL)
	{
		logicalAddress = atoi(readBuffer);
		pageNumber = logicalAddress / PAGE_SIZE;
		pageOffset = logicalAddress % PAGE_SIZE;
	
		if(framePointer < THE_NUMBER_OF_FRAME)
		{
			if(searchTLB(pageNumber)) // TLB hit.
			{ 
				// finish	
			}
			else // TLB false
			{
				if(searchPageTable(pageNumber))
				{
					printf("page table hit\n");
					updateTLB(pageNumber, frameNumber);
				}
				else // page fault.
				{
					pageFaultCount++;
					searchBackingStore(pageNumber);
					updateTLB(pageNumber, frameNumber);
				}
			}
			logicalAddressTable[pageNumber] = logicalAddress;
			physicalFrame[frameNumber] = frameNumber * PAGE_SIZE;
			physicalOffset[frameNumber] = pageOffset;
			int pa = physicalFrame[frameNumber] + physicalOffset[frameNumber];
			sprintf(writeBuffer, "logical address : %d -> physical address : %d\n", logicalAddress, pa);
			printf("Physical memory #%d : %d\n", frameNumber, 
					(physicalFrame[frameNumber] + physicalOffset[frameNumber]));
			fwrite(writeBuffer, strlen(writeBuffer), 1, txtPhysicalAddress);

			
			
			
		}
		else
		{
			if(searchTLB(pageNumber)) // TLB hit.
			{
				logicalAddressTable[pageNumber] = logicalAddress;
				physicalFrame[frameNumber] = frameNumber * PAGE_SIZE;
				physicalOffset[frameNumber] = pageOffset;
				
			}
			else
			{
				//printf("page Number : %d\n", pageNumber);
				if(searchPageTable(pageNumber))
				{
					// just not in memory!! need to swap.
					// swapped-in frame is already set.
		
					// search victim frame.
					replacePageWithLRU(pageNumber);
					// now, victim frame is set ( frameNumber ).
		
					pageTable[pageNumber] = frameNumber; // table update.
					int i;
					for(i = 0; i<PAGE_SIZE; i++)
					{
						if(pageTable[i] == frameNumber && (i != pageNumber))
						{
							// update modified state to disk.
							//printf("validInvalidBit[%d] = false\n", i); 
							validInvalidBit[i] = false; // swapped out.
							validInvalidBit[pageNumber] = true; 
							break;
						}
					}

					// update memory. ( swapped in )
					logicalAddressTable[pageNumber] = logicalAddress;
					physicalFrame[frameNumber] = frameNumber * PAGE_SIZE;
					physicalOffset[frameNumber] = pageOffset;
					//validInvalidBit[pageNumber] = true; // swapped in.
					// update TLB.
					updateTLB(pageNumber, frameNumber);					
				}
				else // page fault + frame full.
				{
					pageFaultCount++;
					//printf("Page fault!\n");
					searchBackingStore(pageNumber);

					logicalAddressTable[pageNumber] = logicalAddress;
					physicalFrame[frameNumber] = frameNumber * PAGE_SIZE;
					physicalOffset[frameNumber] = pageOffset;
					updateTLB(pageNumber, frameNumber);
				}
			}

			printf("Physical memory #%d : %d\n", frameNumber, 
					(physicalFrame[frameNumber] + physicalOffset[frameNumber]));
			int pa = physicalFrame[frameNumber] + physicalOffset[frameNumber];
			sprintf(writeBuffer, "logical address : %d -> physical address : %d\n", logicalAddress, pa);
			fwrite(writeBuffer, strlen(writeBuffer), 1, txtPhysicalAddress);
			//replacePageWithLRU(pageNumber);
		}
		translatedAddressCount++;
	}

	printf("page fault ratio: %.2f\%, TLB hit : %d\n", (double)pageFaultCount / translatedAddressCount * 100, 				TLBhit);
	printf("translated address : %d\n", translatedAddressCount);

	sprintf(writeBuffer, "\nTLB hit ratio : %d hits out of %d\n", TLBhit, translatedAddressCount);
	fwrite(writeBuffer, strlen(writeBuffer), 1, txtPhysicalAddress);
	
	sprintf(writeBuffer, "\n%s  %s  %s\n", "Page Number", "Valid bit", "Virtual address");
	fwrite(writeBuffer, strlen(writeBuffer), 1, txtFrameTable);
	int j;
	for(j = 0; j < PAGE_SIZE; j++)
	{
		int vib;
		if(validInvalidBit[j])
		{
			//printf("valivlb[%d] = true\n", j);
			vib = 1;
		}
		else
		{
			//printf("valivlb[%d] = false\n", j);
			vib = 0;
		}
		sprintf(writeBuffer, "%7d%10d%15d\n", j, vib, logicalAddressTable[j]);
		fwrite(writeBuffer, strlen(writeBuffer), 1, txtFrameTable);
	}
	fclose(txtLogicalAddress);
	fclose(BACKING_STORE);
	fclose(txtPhysicalAddress);
	fclose(txtFrameTable);

	return 0;
}

bool searchTLB(int pageNum)
{
	int i;
	for(i = 0; i<THE_NUMBER_OF_TLB; i++)
	{
		if(TLB[i][0] == pageNum)
		{
			if(framePointer < THE_NUMBER_OF_FRAME)
			{
				TLBhit++;
				frameNumber = TLB[i][1]; 
				frameInsertedTime[frameNumber] = clock();
				
				TLBinsertedTime[i] = clock();
				//printf("TLB hit!! in TLB[%d], TLBhit=%d\n", i, TLBhit);
			}
			else
			{
				TLBhit++;
				frameNumber = TLB[i][1]; 
				frameInsertedTime[frameNumber] = clock();
				framePointer++;
				TLBinsertedTime[i] = clock();
			}
			return true;
		}
	}

	return false;
}

bool searchPageTable(int pageNum)
{
	if(pageTable[pageNum] != -1)
	{
		if(framePointer < THE_NUMBER_OF_FRAME)
		{
			frameNumber = pageTable[pageNum];
			frameInsertedTime[frameNumber] = clock();
		}
		else
		{
			frameNumber = pageTable[pageNum];
			frameInsertedTime[frameNumber] = clock();
			framePointer++;
		}
		return true;
	}
	else
	{
		// pageTable[pageNum] == -1. not loaded page.
		return false;
	}
	
}

bool searchBackingStore(int pageNum)
{
	if(fseek(BACKING_STORE, pageNum * PAGE_SIZE, SEEK_SET) == 0)
	{
		
		int read = fread(backingStoreBuffer, sizeof(unsigned char), PAGE_SIZE, BACKING_STORE);
		if(read == 0)
		{
			printf("fread Error!!\n");
		}
	
		
		if(framePointer < THE_NUMBER_OF_FRAME)
		{
			int i;
			for(i = 0; i<PAGE_SIZE; i++)
			{
				pageInformation[framePointer][i] = backingStoreBuffer[i];
			}
			
			//printf("pageNum, frameNumber : (%d, %d)\n", pageNum, framePointer);
			pageTable[pageNum] = framePointer;
			validInvalidBit[pageNum] = true; // valid bit.
			frameNumber = pageTable[pageNum];
			frameInsertedTime[frameNumber] = clock();
			framePointer++;
		}
		else // case : frame full + page fault
		{

			replacePageWithLRU(pageNum);
			
			//victim frame is set.
			//printf("victim frame is [%d]\n", frameNumber);
			
			int i;
			int comp;
			for(i = 0; i<PAGE_SIZE; i++) // load page from disk. + find the victim frame's pageNumber.
			{
				pageInformation[pageNum][i] = backingStoreBuffer[i];
			}
			
			pageTable[pageNum] = frameNumber;

			for(i = 0; i<PAGE_SIZE; i++)
			{
				if((pageTable[i] == frameNumber) && (i != pageNum))
				{
					// save victim frame's modified state and swapped out.
					validInvalidBit[i] = false;
					validInvalidBit[pageNum] = true;
					break;
				}
			}
			frameInsertedTime[frameNumber] = clock();
			
			//repalcePageWithFIFO(pageNum);
		}

		return true;
	}
	else
	{
		printf("fseek error.\n");
	}
	return false;
}

void updateTLB(int pageNum, int frameNum) // using LRU algorithm.
{
	if(filledTLB < THE_NUMBER_OF_TLB)
	{
		TLBinsertedTime[filledTLB] = clock();
		TLB[filledTLB][0] = pageNum;
		TLB[filledTLB][1] = frameNum;
		filledTLB++;
		//printf("filledTLB : %d\n", filledTLB);
	}
	else // LRU needed
	{
		int i;
		long int min = TLBinsertedTime[0];
		int minIndex = 0;
		for(i = 0; i<THE_NUMBER_OF_TLB; i++)
		{
			if(min > TLBinsertedTime[i])
			{
				min = TLBinsertedTime[i];
				minIndex = i;
			}
		}
		TLBinsertedTime[minIndex] = clock();		
		TLB[minIndex][0] = pageNum;
		TLB[minIndex][1] = frameNum;
		//printf("TLB[%d] is changed because min=%ld\n", minIndex, min);
	}
}

void replacePageWithLRU(int pageNum)
{
	int i;
	long int min = frameInsertedTime[0];
	int minIndex = 0;
	
	for(i = 0; i<THE_NUMBER_OF_FRAME; i++) // find frame which has minimum time.
	{
		if(min > frameInsertedTime[i])			
		{				
			min = frameInsertedTime[i];
			minIndex = i;
		}
	}
	// minIndex frame is need to be changed.
	frameNumber = minIndex; // this frameNumber is swapped. victim frame.
	frameInsertedTime[minIndex] = clock();
	printf("frame[%d] is need to be changed.\n", minIndex);

}

void replacePageWithFIFO(int pageNum)
{
	
	// this code will run if page fault occurs.
	//TODO
}


