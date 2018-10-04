#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUMBER_OF_ELEMENTS 5

void swap(int* first_element, int* second_element)
{
	int temp = *first_element;
	*first_element = *second_element;
	*second_element = temp;
}
 
void bubble_sort(int* numbers)
{
	int i, j;
	for (i = 0; i < NUMBER_OF_ELEMENTS - 1; i++)
		for (j = 0; j < NUMBER_OF_ELEMENTS - i - 1; j++)
			if (numbers[j] > numbers[j + 1])
				swap(&numbers[j], &numbers[j + 1]);
}
 
void write_array(int numbers[])
{
	printf("Process ID : %d\n", getpid());
	int file_descriptor = open("result.txt", O_WRONLY | O_CREAT, S_IRWXU);
	
	if (file_descriptor < 0)
	{
		perror("Something went wrong!\nThe program can't open the result.txt file!\n");	
		return;
	}

	if (write(file_descriptor, numbers, sizeof(int) * NUMBER_OF_ELEMENTS) < sizeof(NUMBER_OF_ELEMENTS))
	{
                perror("Something went wrong!\nThe program can't write on the result.txt file!\n"); 
                return;
        }
	close(file_descriptor);
}


void get_input(int* numbers)
{
	int i;
	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
		scanf("%d", &numbers[i]);
}

int main()
{
	int numbers[NUMBER_OF_ELEMENTS];
	get_input(numbers);
	bubble_sort(numbers);
	write_array(numbers);
	return 0;
}
