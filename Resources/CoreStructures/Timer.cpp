#include "Timer.h"


Timer::Timer()
{
	this->sampleLenght = 0.5;
	this->deltaTime = 0.0;
	this->lastFrame = 0.0;
	this->currentFrame = 0.0f;
	this->numberOfFrames = 0;
	this->numberOfFramesSPF = 0;
}

void Timer::tick()
{
	this->currentFrame = glfwGetTime();
	this->numberOfFrames++;
	this->numberOfFramesSPF++;
}

void Timer::updateDeltaTime()
{
	this->deltaTime = this->currentFrame - this->lastFrame;
	this->lastFrame = this->currentFrame;
}

double Timer::getDeltaTimeSeconds()
{
	return deltaTime;
}

double Timer::getDeltaTimeMiliseconds()
{
	return deltaTime / 1000;
}

double Timer::averageFPS()
{
	this->tempDeltaTimeFPS += this->currentFrame - this->lastFrame;

	if(this->tempDeltaTimeFPS >= this->sampleLenght)
	{
		this->FPS = double(this->numberOfFrames / this->tempDeltaTimeFPS);
		this->numberOfFrames = 0;
		this->tempDeltaTimeFPS = 0;
	}

	return this->FPS;
}

double Timer::currentSPF()
{
	this->tempDeltaTimeSPF += this->currentFrame - this->lastFrame;

	if (this->tempDeltaTimeSPF >= this->sampleLenght)
	{
		this->SPF = tempDeltaTimeSPF / this->numberOfFramesSPF;
		this->numberOfFramesSPF = 0;
		this->tempDeltaTimeSPF = 0;
	}

	return this->SPF;
}