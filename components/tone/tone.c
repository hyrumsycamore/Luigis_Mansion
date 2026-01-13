#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "sound.h"
#include "tone.h"

// Global Variables
#define AMPLITUDE 127
#define PI_F 3.1415926
#define BIAS 0x80
#define MAX_LEVEL 0xFF
#define FIRST_HALF .5f
#define FIRST_QUARTER .25f
#define THIRD_QUARTER .75f

typedef uint8_t wave_t;
static wave_t *soundbuffer;
uint32_t sample_rate;
uint32_t size;
// Initialize the tone driver. Must be called before using.
// May be called again to change sample rate.
// sample_hz: sample rate in Hz to playback tone.
// Return zero if successful, or non-zero otherwise.
int32_t tone_init(uint32_t sample_hz)
{
    // Check if the sample rate is at least twice the lowest Nyquist frequency
    if (sample_hz < (2 * LOWEST_FREQ))
    {
        return 1;
    }
    //  Set the sample rate to be the given sample hz for the remainder of the program
    sample_rate = sample_hz;
    sound_init(sample_hz);
    // Allocate buffer
    size = sample_hz / LOWEST_FREQ;
    soundbuffer = (wave_t *)malloc(size);
    // Checks if the malloc was successful 
    if (soundbuffer == NULL)
    {
        return 2;
    }
    return 0;
}

// Free resources used for tone generation (DAC, etc.).
// Return zero if successful, or non-zero otherwise.
int32_t tone_deinit(void)
{
    // Deallocate Buffer
    sound_deinit();
    free(soundbuffer);
    soundbuffer = NULL;// remove dangling pointer
    return 0;
};

// Start playing the specified tone.
// tone: one of the enumerated tone types.
// freq: frequency of the tone in Hz.
void tone_start(tone_t tone, uint32_t freq)
{
    // Checks the tone paramater to make sure the input calling one of the 4 wave forms
    if (tone < 0 || tone > 4)
    {
        return;
    }
    // Checks if the frequency is less than the lowest nyquist frequency
    if (freq < LOWEST_FREQ)
    {
        return;
    }
    uint32_t num_samples = sample_rate / freq;
    // Generates a period for the tone for each wave form
    switch (tone)
    {
    case (SINE_T):
        // turns i into a radian input and grabs the sine output and makes it from a float into a int output
        for (uint32_t i = 0; i < num_samples; i++)
        {
            float rad = ((i * 2 * PI_F) / (float)num_samples);
            float intermediate = (AMPLITUDE * sinf(rad));
            soundbuffer[i] = (float)intermediate + BIAS;
        }
        break;
    case SQUARE_T:
        // Creates a square wave
        for (uint32_t i = 0; i < num_samples; i++)
        {
            // Half the period is the max level, the other the lowest level
            if (i < (num_samples / 2))
            {
                soundbuffer[i] = MAX_LEVEL;
            }
            else
            {
                soundbuffer[i] = 0;
            }
        }
        break;
    case TRIANGLE_T:
        // Creates the triangle wave
        // Sets the slope based off of the end points for the first quarter
        float slope = (MAX_LEVEL - BIAS) / (num_samples*FIRST_QUARTER);
        for (uint32_t i = 0; i < num_samples; i++)
        {
            // Positive slope from the midpoint
            if (i < (num_samples * FIRST_QUARTER))
            {
                soundbuffer[i] = (i * slope) + BIAS;
            }
            // Second Quarter, negative slope minus the max_level
            else if (i < (num_samples * THIRD_QUARTER))
            {
                soundbuffer[i] = MAX_LEVEL - ((i-(num_samples*FIRST_QUARTER)) * slope);
            }
            // Third quarter, positive slope
            else
            {
                soundbuffer[i] = ((i-num_samples*THIRD_QUARTER) * slope);
            }
        }
        break;
    case SAW_T:
        // Generating a different slope 
        float slope_saw = ((float)(MAX_LEVEL) / (num_samples));
        for (uint32_t i = 0; i < num_samples; i++)
        {
            // Splits the period in half with positive and negative slopes.
            if (i < (num_samples * FIRST_HALF))
            {
                soundbuffer[i] = (i * slope_saw) + BIAS;

            }
            else{
                soundbuffer[i] = ((i-(num_samples/2))*slope_saw);
            }
        }
        break;
    // A catch case to do nothing if there is an unexpected input.
    default:
        break;
    }
    sound_cyclic(soundbuffer, num_samples);
}
