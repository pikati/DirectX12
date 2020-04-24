#pragma once

class Vector3
{
private:
	float x, y, z;
public:
	Vector3(float x, float y, float z);
	static Vector3 Right();
	static Vector3 Left();
	static Vector3 Up();
	static Vector3 Down();
	static Vector3 Forward();
	static Vector3 Back();
	static Vector3 One();
	static Vector3 Zero();
};

