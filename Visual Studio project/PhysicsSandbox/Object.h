#pragma once
#include "RGB2HSV.h"
#include "imgui/imgui.h"
#include <Windows.h>

namespace sbox
{
	enum shapes {
		CIRCLE,
		SQUARE,
	};

	void drawShape(int shape, ImVec2 pos, float size, rgb color);

	class Object {
	public:
		int mShape;
		ImVec2 mPos;
		float mSize;
		rgb mColor;

		Object();
		Object(int shape, ImVec2 pos, float size, rgb color);
		static Object createRandomCircle(RECT boundingBox, float size);
		static Object createRandomCircle(RECT boundingBox, int deviationSize, int minSize);
		void draw();
	};
}