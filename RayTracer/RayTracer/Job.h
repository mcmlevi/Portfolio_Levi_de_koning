#pragma once

enum class Status
{
	WAITING,
	GO,
	DONE,
	EXIT,
	WORKING
};


struct Job
{
	Job(int x, int y, int size) :yPos(y), xPos(x), tileSize(size){}
	Job() = default;
	~Job() = default;
	int yPos;
	int xPos;
	int tileSize;
	
};