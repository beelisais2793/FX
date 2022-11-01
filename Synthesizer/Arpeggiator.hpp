#pragma once

enum NoteOrder {
	PITCH,
	TIME
};

enum Mode {
	UP,
	DOWN,
	UPDOWN1,
	DOWNUP1,
	UPDOWN2,
	DOWNUP2,
	RANDOM,
	NUM_MODES
};


struct Arpeggiator {

/* Examples:
Range = 2, number_of_notes = 3
UP:
0 1 2 0 1 2 0 1 2 0 1 2 ...
0 0 0 1 1 1 0 0 0 1 1 1 ...
DOWN:
2 1 0 2 1 0 2 1 0 2 1 0 ...
1 1 1 0 0 0 1 1 1 0 0 0 ...
UPDOWN1:
0 1 2 0 1  2 1 0 2 1  0 1 2 0 1  2 ...
0 0 0 1 1  1 1 1 0 0  0 0 0 1 1  1 ...
UPDOWN2:
0 1 2 0 1 2  2 1 0 2 1 0  0 1 2 0 1 2 ...
0 0 0 1 1 1  1 1 1 0 0 0  0 0 0 1 1 1 ...
*/

public:

	Arpeggiator() {mode = Mode::UP; current = 0; octave = 0; range = 0; step = 1;};

	inline int getIndex(int num_of_notes) {return current<0?0:(current<num_of_notes?current:num_of_notes-1) ;}
	inline int getOctave() {return octave;}

	inline void reset() {
		switch(mode) {
			case UP:		
			case UPDOWN1: 
			case UPDOWN2:   current = -1; octave = 0;     step =  1; break;
			case DOWN:		
			case DOWNUP1:
			case DOWNUP2:   current = -2; octave = range; step = -1; break;
			default: current = 0; octave = 0; step = 0;
		}
	}

	inline int rand_range(int min, int max) {float rnd=rand()/(RAND_MAX-1.0); return min + roundf(rnd*(max-min)); }


	inline void setMode(Mode m) {

		if(mode!=m) {
			switch(m) {
				case UP:     step =  1; break;
				case DOWN:   step = -1; break;
				case RANDOM: step =  0; break;
				case UPDOWN1:
				case UPDOWN2: if(step==0) step = 1; break;
				case DOWNUP1:
				case DOWNUP2: if(step==0) step = -1; break;
				default: ;	
			};
			mode = m;
			// reset();
		}
	}


	inline void setRange(int r) {range=r;}

	int process(int);


private:

	Mode mode;
	int range;
	int step;

	int current;
	int octave;

};


int Arpeggiator::process(int number_of_notes) {

	// possible states at beginning of process():
	//
	// counter = -2: init state for DOWN modes
	// counter = -1: init state for UP modes
	// counter = 0-N (where N is number_of_notes-1 during last call and might be above current number_of_notes)
	//
	// octave = 0-N  (where N is range during last call)
	//
	// step = -1, 0, 1
	
	if(current < -1) current = number_of_notes;

	current += step;

	switch(mode) {

		case UP:	if(current >= number_of_notes) {
						current = 0; octave += step;
					}
					if(current<0) current=0;
					if(octave > range) octave = 0;
					break;

		case DOWN:	if(current < 0) {
						current = number_of_notes-1; octave += step;
					};
					if(current>number_of_notes-1) current=number_of_notes-1;
					if(octave<0) octave = range;
					break;

		case UPDOWN1:
		case DOWNUP1:
					if(octave >= range && current >= number_of_notes-1 ) {
						step = -1;
					}
					if(current >= number_of_notes) {
						current = 0; 
						if(octave<range) octave += step; // in this case step= 1
					}

					if(octave <= 0 && current < 1 ) {
						step = 1;
					}
					if(current < 0) {
						current = number_of_notes-1; 
						if(octave>=0) octave += step;   // in this case step=-1
					};
					break;
		case UPDOWN2:
		case DOWNUP2:
					if(current >= number_of_notes && octave > range-1) {
						current -= step;
						step = -1;
					}
					if(current >= number_of_notes) {
						current = 0; 
						if(octave<range) octave += step;
					}

					if(current < 0 && octave < 1) {
						current += 1;
						step = 1;
					}
					if(current < 0) {
						current = number_of_notes-1; 
						if(octave>0) octave += step;
					};
					break;
		case RANDOM: 
					current = rand_range(0, number_of_notes-1);
					octave =  rand_range(0, range);
					break;
		default: ;
	}

	if(octave>range) octave = range;
    return current;
}


template<typename T>
void Pan(T pan, T * input, T * outL, T * outR)
{
    T * pL = outL;
    T * pR = outR;
    for(T& tick : input) {
        *pL++ = tick * sin((1-pan)*M_PI/2) ;
        *pR++ = tick * cos(pan*M_PI/2)     ;
    }
}
