#include <stdio.h>
#include <iostream>
#include <time.h>

#define GRIDSIZE_X 40
#define GRIDSIZE_Y 40

using namespace std;

string dead_cell =  "  ";
string alive_cell = "# ";

typedef enum { OUT_OF_BOUND = -999 } sentinel;

// zipped return type for the cell status of a cell and its neigbors
// used in 
struct cell_status {
	int current; // current cell status [0, 1]
	int neighbor; // living nearby cells [0, 8]
};

// cells for ca to exist in
// each cell would be either dead or alive
bool cell_space[GRIDSIZE_X][GRIDSIZE_Y];

// number of iterations CA has been running for
int generation;

// bool to integer
// helper function
int btoi(bool b) {
	if (b == true) {
		return 1;
	}
	else {
		return 0;
	}
}

void wait(int time_limit) {
	time_t now = time(NULL);
	int wait_time = now + time_limit;

	while (wait_time - now > 0) {
		now = time(NULL);
	}
}

// initialize cell_board
void initialize() {
	int mid_x = GRIDSIZE_X / 2;
	int mid_y = GRIDSIZE_Y / 2;
	
	// a guees
	for (int x = (mid_x - 4); x < (mid_x + 2); x++) {
		for (int y = (mid_y - 8); y < (mid_y + 2); y++) {
			cell_space[x][y] = true;
		}
	}

	cell_space[1][1] = true;
	cell_space[2][2] = true;
	cell_space[2][1] = true;

	cell_space[20][20] = true;
	cell_space[21][20] = true;
	cell_space[20][21] = true;

	for (int y = 4; y < 10; y++) {
		cell_space[mid_x + 8][y] = true;
	}

	for (int x = (mid_x + 7); x < GRIDSIZE_X; x++) {
		cell_space[x][mid_y - 8] = true;
	}
}

// std out display in cmd
void display() {

	int census = 0;
	cout << "\n";

	for (int x = 0; x < GRIDSIZE_X; x++) {
		for (int y = 0; y < GRIDSIZE_Y; y++) {
			
			string out = dead_cell; // by default dead
			
			if (cell_space[x][y] == true) { // mark as living if alive
				census++;
				out = alive_cell;
			} 

			cout << out;
		}
		cout << "\n";
	}

	cout << "\n";
	cout << "living cells: " << census;
	cout << "\ngeneration: " << generation << "\n\n";

	if (census == 0) {
		cout << "Cells dead, simulation done.\n\n";
		exit(1);
	}
}

struct coordinates {
	int x;
	int y;
};

struct coordinate_array {
	struct coordinates c[8];
};

struct coordinate_array get_check_idx(int x, int y) {
	/*
		1. 2. 3.
		4. -. 5. 
		6. 7. 8. 
	*/
	struct coordinates ordered_indeces[8];
	

	// defaults
	// 1.
	ordered_indeces[0].x = x - 1;
	ordered_indeces[0].y = y - 1; 

	 // 2.
	ordered_indeces[1].x = x;
	ordered_indeces[1].y = y - 1;

	// 3.
	ordered_indeces[2].x = x + 1;
	ordered_indeces[2].y = y - 1;

	// 4.
	ordered_indeces[3].x = x - 1;
	ordered_indeces[3].y = y;

	// 5.
	ordered_indeces[4].x = x + 1;
	ordered_indeces[4].y = y;

	// 6.
	ordered_indeces[5].x = x - 1;
	ordered_indeces[5].y = y + 1;

	// 7.
	ordered_indeces[6].x = x;
	ordered_indeces[6].y = y + 1;

	// 8.
	ordered_indeces[7].x = x + 1;
	ordered_indeces[7].y = y + 1;

	// check edge cases and reassign if needed
	// just need to make sure we have no indeces are out of bounds [0, GRIDSIZE__] 
	for (int i = 0; i < 8; i++) {

		if (ordered_indeces[i].x < 0 || ordered_indeces[i].x > GRIDSIZE_X) {
			ordered_indeces[i].x = OUT_OF_BOUND;
		}
		if (ordered_indeces[i].y < 0 || ordered_indeces[i].y > GRIDSIZE_Y) {
			ordered_indeces[i].y = OUT_OF_BOUND;
		}
	}

	// return wrapper of indeces
	struct coordinate_array ca;
	for (int i = 0; i < 8; i++) {
		ca.c[i] = ordered_indeces[i];
	}

	return ca;
}

// get the cell status of a cell at position (x,y) in the coordinate plane
struct cell_status get_cell_status(int x, int y) {

	struct cell_status status;
	status.current = btoi(cell_space[x][y]); // set status for current cell



	struct coordinate_array indeces = get_check_idx(x, y);
	int neighbor = 0;

	for (int i = 0; i < 8; i++) { // check 8 sets of indeces
		for (int j = 0; j < 1; j++) { // 2D coords
			int x = indeces.c[i].x;
			int y = indeces.c[i].y;

			if (x != OUT_OF_BOUND && y != OUT_OF_BOUND) { // out of bounds cell counts as dead
				if (cell_space[x][y]) {
					neighbor++;
				}
			}
		}
	}

	status.neighbor = neighbor;

	return status;
}

// rules for a generation
// 1. Death: if the count is less than 2 or greater than 3, the current cell is switched off.
// 2. Survival: if (a) the count is exactly 2, or (b) the count is exactly 3 and the current cell is on, the current cell is left unchanged
// 3. Birth: if the current cell is off and the count is exactly 3, the current cell is switched on. 
void next_generation() {
	generation++;

	bool updated_cell_space[GRIDSIZE_X][GRIDSIZE_X]; // new cell space

	struct cell_status cs;

	// check each cell and apply rules to it
	for (int x = 0; x < GRIDSIZE_X; x++) {
		for (int y = 0; y < GRIDSIZE_Y; y++) {
			updated_cell_space[x][y] = cell_space[x][y]; // first copy value from previous iteration
			cs = get_cell_status(x, y);

			// rule for death
			if (cs.neighbor < 2 || cs.neighbor > 3) {
				updated_cell_space[x][y] = false;
			}
			else {
				// rule for survival
				// if (cs.neighbor == 2 || (cs.neighbor == 3 && cs.current == 1))
					// unchanged
				// rule for birth
				if (cs.current == 0 && cs.neighbor == 3) {
					updated_cell_space[x][y] = true;
				}
				
			}
		}
	}

	// do deep copy
	// cell_space = updated_cell_space;
	for (int i = 0; i < GRIDSIZE_X; i++) {
		for (int j = 0; j < GRIDSIZE_Y; j++) {
			cell_space[i][j] = updated_cell_space[i][j];
		}
	}
}

int main() {
	initialize();
	display();
	while(generation < 500) {
		wait(1);
		next_generation();
		display();
	}

	return 0;
}