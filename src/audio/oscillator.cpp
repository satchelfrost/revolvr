/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <audio/oscillator.h>
#include <cmath>

#define TWO_PI (3.14159 * 2)
#define AMPLITUDE 0.3
#define FREQUENCY 440.0

void Oscillator::SetWaveOnAndScale(bool isWaveOn, double scale) {
    isWaveOn_.store(isWaveOn);
    sound_scale_ = scale;
}

void Oscillator::SetSampleRate(int32_t sampleRate) {
    phaseIncrement_ = (TWO_PI * FREQUENCY) / (double) sampleRate;
}

void Oscillator::Render(float *audioData, int32_t numFrames) {
    if (!isWaveOn_.load()) phase_ = 0;

    for (int i = 0; i < numFrames; i++) {

        if (isWaveOn_.load()) {

            // Calculates the next sample value for the sine wave.
            audioData[i] = (float) (sin(phase_) * AMPLITUDE);

            // Increments the phase, handling wrap around.
            phase_ += phaseIncrement_ * (sound_scale_ + 1);
            if (phase_ > TWO_PI) phase_ -= TWO_PI;

        } else {
            // Outputs silence by setting sample value to zero.
            audioData[i] = 0;
        }
    }

}
