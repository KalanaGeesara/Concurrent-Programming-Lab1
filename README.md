Open the terminal in the project directory.

1.Serial Program
	Compile	: gcc -g -Wall -o serial serial.c my_rand.c
	run	: ./serial

2. One Mutex for entire list
	Compile	: gcc -g -Wall -o mutex mutex.c my_rand.c -lpthread
	run	: ./mutex <Thread_count> (Relace <Thread_count> with number of threads)

3. Read Write lock for entire list
	Compile	: gcc -g -Wall -o rwl rwl.c my_rand.c -lpthread
	run	: ./rwl <Thread_count> (Relace <Thread_count> with number of threads)

After runnig the program:

Input sequence: 
		How many keys should be inserted in the main thread (n)? => Insert the number elements you want to add to Link list first

		How many keys should be inserted in the main thread (n)? => Insert how many number of operations to be extecuted on the Link list

		
		Percent of ops that should be searches? (between 0 and 1) => Insert the percantage of member operations to be performed on the Link list as a number between 0 and 1.

		Percent of ops that should be inserts? (between 0 and 1) => Insert the percantage of Insert operations to be performed on the Link list as a number between 0 and 1.
