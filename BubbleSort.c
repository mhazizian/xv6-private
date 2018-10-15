#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

#define NUMBER_OF_ELEMENTS 5

void swap(char** first_element, char** second_element)
{
	char* temp = *first_element;
	*first_element = *second_element;
	*second_element = temp;
}
 
void bubble_sort(char* numbers[])
{
	int i, j;
	for (i = 0; i < NUMBER_OF_ELEMENTS - 1; i++)
		for (j = 0; j < NUMBER_OF_ELEMENTS - i - 1; j++)
			if (atoi(numbers[j]) > atoi(numbers[j + 1]))
				swap(&numbers[j], &numbers[j + 1]);
}
 
void write_array(char* numbers[])
{
	int i = 0;
	int file_descriptor = open("result.txt", O_CREATE | O_WRONLY);
	
	printf(1, "Process ID : %d\n", getpid());
	
	if (file_descriptor < 0)
	{
		printf(2, "Something went wrong!\nThe program can't open the result.txt file!\n");	
		return;
	}

	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		write(file_descriptor, numbers[i], sizeof(numbers[i]) / sizeof(char));
		write(file_descriptor, " ", sizeof(char));
	}

	write(file_descriptor, "\n", sizeof(char));
	close(file_descriptor);
}

int main(int argc, char *argv[])
{
	if (argc == (NUMBER_OF_ELEMENTS + 1))
	{
		bubble_sort(&argv[1]);
		write_array(&argv[1]);
		exit();
	}
	else
	{
		printf(2, "Invalid input!!!\n");
		exit();
	}
}
