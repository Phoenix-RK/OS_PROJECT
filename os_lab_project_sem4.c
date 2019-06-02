#include <pthread.h>		
#include <time.h>			
#include <unistd.h>	
#include <semaphore.h>	
#include <stdlib.h>			
#include <stdio.h>		

pthread_t *Wizards;		
pthread_t Dumbledore;			

int ChairsCount = 0;
int CurrentIndex = 0;


sem_t Dumbledore_Sleep;
sem_t Wizard_Sem;
sem_t ChairsSem[2];
pthread_mutex_t ChairAccess;


void *Dumbledore_Activity();
void *Wizard_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_wizards;	
	int id;
	srand(time(NULL));
	printf("\nWELCOME TO WIZARDING WORLD!!!!!");
	
	sem_init(&Dumbledore_Sleep, 0, 0);
	sem_init(&Wizard_Sem, 0, 0);
	for(id = 0; id < 2; ++id)			
		sem_init(&ChairsSem[id], 0, 0);

	pthread_mutex_init(&ChairAccess, NULL);
	
	if(argc<2)
	{
		printf("\nDumbledore allows\n");
		number_of_wizards=3;
	}
	else
	{
		printf("\nNumber of wizards specified. Creating %d threads.\n", number_of_wizards);
		number_of_wizards = atoi(argv[1]);
	}
		
	
	Wizards = (pthread_t*) malloc(sizeof(pthread_t)*number_of_wizards);

	
	pthread_create(&Dumbledore, NULL, Dumbledore_Activity, NULL);	
	for(id = 0; id < number_of_wizards; id++)
		pthread_create(&Wizards[id], NULL, Wizard_Activity,(void*) (long)id);

	
	pthread_join(Dumbledore, NULL);
	for(id = 0; id < number_of_wizards; id++)
		pthread_join(Wizards[id], NULL);

	
	free(Wizards); 
	return 0;
}

void *Dumbledore_Activity()
{
	while(1)
	{
		sem_wait(&Dumbledore_Sleep);		
		printf("\nDumbledore has been called by a wizard.\n");

		while(1)
		{
			
			pthread_mutex_lock(&ChairAccess);
			if(ChairsCount == 0) 
			{
				
				pthread_mutex_unlock(&ChairAccess);
				break;
			}
			sem_post(&ChairsSem[CurrentIndex]);
			ChairsCount--;
			printf("\nWizard left the chair.\n Remaining Chairs %d\n", 2 - ChairsCount);
			CurrentIndex = (CurrentIndex + 1) % 2;
			pthread_mutex_unlock(&ChairAccess);
			

			printf("\t Dumbledore is currently seeing pensive with the the wizard.\n");
			sleep(5);
			sem_post(&Wizard_Sem);
			usleep(1000);
		}
	}
}

void *Wizard_Activity(void *threadID) 
{
	int ProgrammingTime;

	while(1)
	{
		printf("\nWizard %ld is preparing for O.W.L.S.\n", (long)threadID);
		ProgrammingTime = rand() % 10 + 1;
		sleep(ProgrammingTime);		

		printf("\nWizard %ld wishes to see pensive with the Dumbledore\n", (long)threadID);
		
		pthread_mutex_lock(&ChairAccess);
		int count = ChairsCount;
		pthread_mutex_unlock(&ChairAccess);

		if(count < 2)		
		{
			if(count == 0)		
				sem_post(&Dumbledore_Sleep);
			else
				printf("\nWizard %ld sat on a chair waiting for the Dumbledore to finish. \nWizard %ld practices the toughest spell *EXPECTO PATRONUM*", (long)threadID,(long)threadID);

			
			pthread_mutex_lock(&ChairAccess);
			int index = (CurrentIndex + ChairsCount) % 2;
			ChairsCount++;
			printf("\nChairs Remaining: %d\n", 2 - ChairsCount);
			pthread_mutex_unlock(&ChairAccess);
			

			sem_wait(&ChairsSem[index]);		
			printf("\t Dumbledore let  Wizard %ld to see pensive. \n", (long)threadID);
			sem_wait(&Wizard_Sem);		
			printf("\nWizard %ld left Dumbledore’s room.\n",(long)threadID);
		}
		else 
			printf("\nWizard %ld will return at another time. \n", (long)threadID);
			
	}
}
