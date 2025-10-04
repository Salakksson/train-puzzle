#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#define MAX_SIZE 1000
#define MIN_SIZE 1

struct {
	bool* lights;
	int size;
	int position;
	int direction;
} train;

void print_state()
{
	for (int i = 0; i < train.size; i++)
	{
		bool light = train.lights[i];
		char c = light ? 't' : 'f';
		if (i == train.position)
			c = toupper(c);

		if (i != 0) printf(" ");
		printf("%c", c);
	}
	printf("\n");
}

void set_light(bool set)
{
	train.lights[train.position] = set;
}

bool get_light()
{
	return train.lights[train.position];
}

void move_forward()
{
	train.position += train.direction;
	if (train.position == -1)
		train.position = train.size - 1;
	if (train.position == train.size)
		train.position = 0;
}

void switch_direction()
{
	train.direction = -train.direction;
}

void init_train(int size)
{
	train.lights = malloc(size);
	train.size = size;
	train.position = 0;
	train.direction = 1;

	for (int i = 0; i < size; i++)
	{
		train.lights[i] = rand() % 2;
	}
}

int guess_size()
{
	// check if size is less than 5
	for (int i = 0; i < 5; i++)
	{
		move_forward();
		set_light(false);
	}
	switch_direction();
	for (int i = 0; i < 5; i++)
	{
		if (get_light()) return i;
		set_light(true);
		move_forward();
	}
	puts("not less than 5");

	switch_direction();
	int count = 3;
	for (int i = 0; i < count; i++)
	{
		move_forward();
		set_light(false);
	}
	// now we are at a state ? [f f F] t t ?
	// where [] is the "explored" region
	// we need atleast one light on to the right of
	// the explored region to be able to count it reliably
	// end goal is setting the whole train to false

	switch_direction();

	// this doesnt assume a maximum size
	while (true)
	{
		// assume we are in ? [f ... ←F] t
		for (int i = 0; i < count; i++)
		{
			// should be in explored region
			if (get_light() != false)
			{
				puts("invalid?");
				print_state();
				return count;
			}
			move_forward();
		}
		// now we are on <-(?) [f f ...] t ?
		// we will set the ? to false to extend the region
		set_light(false);
		move_forward();
		set_light(true);

		// now we are moving right starting on the first
		// light of the explored region
		switch_direction();
		move_forward();

		// count number of lights to our right
		int recount = 0;
		while (get_light() == false)
		{
			move_forward();
			recount++;
			if (recount > count + 1)
				return count + 1;
		}
		// expected state: t [f f ...] T→
		// recount = number of f's in []
		if (recount != count + 1) return count + 1;
		count++;
		switch_direction();
		move_forward();
	}

	return 100;
}

int main()
{
	time_t seed = time(NULL);
	srand(seed);

	int size = rand() % MAX_SIZE;
	if (size < MIN_SIZE) size = MIN_SIZE;
	init_train(size);

	// guess with high iteration count in case of disaster
	int guess = guess_size(MAX_SIZE * 5);

	printf("size is %i\n", size);
	printf("guessed %i\n", guess);

	return (guess != size);
}
