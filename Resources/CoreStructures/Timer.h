#ifndef TIMER_H
#define TIMER_H

#include <GLFW/glfw3.h>

class Timer
{
private:
	double	sampleLenght;		// Time interval at which frame are sampled at
	double	deltaTime;			// Time between current frame and last frame
	double	tempDeltaTimeFPS;	// Time between current frame and last frame over the sample interval for FPS
	double	tempDeltaTimeSPF;	// Time between current frame and last frame over the sample interval for SPF
	double	lastFrame;			// Time of last frame
	double	currentFrame;		// Time of Current frame
	int		numberOfFrames;		// Number of frames rendered since last render call for FPS
	int		numberOfFramesSPF;	// Number of frames rendered since last render call for SPF
	double	FPS;				// Frames per second
	double	SPF;				// Seconds per frame

public:
	Timer();
	void tick();

	void updateDeltaTime();
	double getDeltaTimeSeconds();
	double getDeltaTimeMiliseconds();
	double averageFPS();
	double currentSPF();
};

#endif
