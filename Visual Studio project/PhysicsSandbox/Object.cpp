#pragma once
#include "Object.h"
using namespace sbox;

void sbox::drawShape(int shape, ImVec2 pos, float size, rgb color) {
	if (shape == CIRCLE) {
		ImGui::GetBackgroundDrawList()->AddCircleFilled(pos, size, ImColor(color.r, color.g, color.b, 1.f));
	}
	if (shape == SQUARE) {
		ImGui::GetBackgroundDrawList()->AddRectFilled({ pos.x - size, pos.y - size }, { pos.x + size, pos.y + size }, ImColor(color.r, color.g, color.b, 1.f));
	}
}

Object::Object() {
	mShape = CIRCLE; mPos = { 0,0 }; mSize = 10.f; mColor = { 1.f,1.f,1.f };
}

Object::Object(int shape, ImVec2 pos, float size, rgb color) {
	mShape = shape;	mPos = pos;	mSize = size; mColor = color;
}

Object Object::createRandomCircle(RECT boundingBox, float size) {
	int shape = CIRCLE;
	ImVec2 pos = { rand() % boundingBox.right + 0.f, rand() % boundingBox.bottom + 0.f };
	float r = rand() % 255 / 255.f;
	float g = rand() % 255 / 255.f;
	float b = rand() % 255 / 255.f;
	rgb color = { r, g, b };
	return Object(shape, pos, size, color);
}

Object Object::createRandomCircle(RECT boundingBox, int deviationSize, int minSize) {
	int shape = CIRCLE;
	float size = rand() % deviationSize + minSize;
	ImVec2 pos = { rand() % boundingBox.right + 0.f, rand() % boundingBox.bottom + 0.f };
	float r = rand() % 255 / 255.f;
	float g = rand() % 255 / 255.f;
	float b = rand() % 255 / 255.f;
	rgb color = { r, g, b };
	return Object(shape, pos, size, color);
}

void Object::draw() {
	sbox::drawShape(mShape, mPos, mSize, mColor);
}
