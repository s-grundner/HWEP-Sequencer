#include "scale.h"

char key_name[12][2] = {"G ", "A ", "A*", "B ", "C ", "C*", "D ", "D*", "E ", "F ", "F*"};

uint8_t get_key_num(double freq)
{
	if(freq == 0)
	{
		return 0;
	}
	return log2(freq/CONCERT_PITCH)*12.0+49.0;
}

double get_pitch_hz(uint8_t key_num)
{
	return pow(2.0, ((double)key_num-49.0)/12.0)*(double)CONCERT_PITCH;
}

char* get_key_name(uint8_t key_num)
{
	return key_name[key_num%12];
}
