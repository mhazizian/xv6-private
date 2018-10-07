// #include <stdio.h>
// #include "unistd.h"
#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"


#define NUMBER_OF_ELEMENTS 5

void swap(char* first_element, char* second_element)
{
	char temp = *first_element;
	*first_element = *second_element;
	*second_element = temp;
}
 
void bubble_sort(char* numbers[])
{
	int i, j;
	for (i = 0; i < NUMBER_OF_ELEMENTS - 1; i++)
		for (j = 0; j < NUMBER_OF_ELEMENTS - i - 1; j++)
			if (*numbers[j] > *numbers[j + 1])
				swap(numbers[j], numbers[j + 1]);
}
 
void write_array(char* numbers[])
{
	char* argv[] = {"echo", numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], 0};
	// if (fork() == 0)
	// {
	// 	printf("Process ID : %d\n", getpid());
	// 	exec_v()
	// }
	int file_descriptor = open("result.txt", O_CREATE|O_WRONLY);
	// int file_descriptor = open("result.txt", O_WRONLY | O_CREAT, S_IRWXU);
	
	if (file_descriptor < 0)
	{
		// perror("Something went wrong!\nThe program can't open the result.txt file!\n");	
		return;
	}

	if (fork() == 0){
		exec("echo", argv);
	} else {
		wait();
		// for (int i = 0; i < 5; i++){
		// 	write(file_descriptor, numbers[i], sizeof(char) * )
		// }
		// if (write(file_descriptor, *numbers, sizeof(char) * NUMBER_OF_ELEMENTS) < sizeof(NUMBER_OF_ELEMENTS))
		// {
		// 	// perror("Something went wrong!\nThe program can't write on the result.txt file!\n"); 
		// 	return;
		// }
	}

	// if (write(file_descriptor, *numbers, sizeof(char) * NUMBER_OF_ELEMENTS) < sizeof(NUMBER_OF_ELEMENTS))
	// {
	// 	// perror("Something went wrong!\nThe program can't write on the result.txt file!\n"); 
	// 	return;
	// }
	close(file_descriptor);
}


// void get_input(int* numbers)
// {
// 	int i;
// 	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
// 		scanf("%d", &numbers[i]);
// }

int main(int argc, char *argv[])
{
	// int numbers[NUMBER_OF_ELEMENTS];
	// get_input(argv + 1);
	// char* argv2[] = {"echo", argv[0], argv[1], 0};

	
	// wait();

	bubble_sort(&argv[1]);
	write_array(&argv[1]);
	exit();


}
