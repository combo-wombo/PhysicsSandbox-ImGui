#pragma once
#include "PhysicsObject.h"

class PhysicsObjectContainer {
public:
	std::vector<PhysicsObject> mPhysObjData;
	RECT mBoundingBox;
	int getCount() {
		return static_cast<int>(mPhysObjData.size());
	}
	PhysicsObjectContainer() {
		RECT defaultRect;
		defaultRect.left = 0;
		defaultRect.top = 0;
		defaultRect.right = 1;
		defaultRect.bottom = 1;
		mBoundingBox = defaultRect;
	}
	PhysicsObjectContainer(RECT boundingBox) {
		mBoundingBox = boundingBox;
	};
	RECT getBoundingBox() {
		return mBoundingBox;
	}
	void setBoundingBox(RECT boundingBox) {
		mBoundingBox = boundingBox;
	}
	void addRandomObjects(int objCount, int maxObjSize, int minObjSize, int maxObjVelocity) {
		for (int i = 0; i < objCount; i++) {
			PhysicsObject obj1 = PhysicsObject::createRandomPhysObj(mBoundingBox, maxObjSize, minObjSize, maxObjVelocity);
			mPhysObjData.push_back(obj1);
		}
	}
	void addRandomObject(int maxSize, int minSize, int maxVelocity) {
		PhysicsObject obj1 = PhysicsObject::createRandomPhysObj(mBoundingBox, maxSize, minSize, maxVelocity);
		mPhysObjData.push_back(obj1);
	}
	void addRandomObjectAt(ImVec2 position, int deviationSize, int minSize, int maxVelocity) {
		PhysicsObject obj1 = PhysicsObject::createRandomPhysObj(mBoundingBox, deviationSize, minSize, maxVelocity);
		obj1.mPos = position;
		mPhysObjData.push_back(obj1);
	}
	void addRandomObjectAt(ImVec2 position, float size, int maxVelocity) {
		PhysicsObject obj1 = PhysicsObject::createRandomPhysObj(mBoundingBox, size, maxVelocity);
		obj1.mPos = position;
		mPhysObjData.push_back(obj1);
	}
	void addRandomObjectAt(ImVec2 position, float size, ImVec2 velocity) {
		PhysicsObject obj1 = PhysicsObject::createRandomPhysObj(mBoundingBox, size, velocity);
		obj1.mPos = position;
		mPhysObjData.push_back(obj1);
	}
	void addObject(PhysicsObject& obj) {
		mPhysObjData.push_back(obj);
	}
	bool removeObject() {
		if (mPhysObjData.size() > 0) {
			mPhysObjData.pop_back();
		}
		else {
			return false;
		}
		return true;
	}
	PhysicsObject getObjectAt(int index) {
		return mPhysObjData[index];
	}
	bool removeObjects(int count) {
		if (count > mPhysObjData.size()) { return false; }
		for (int i = 0; i < count; i++) {
			mPhysObjData.pop_back();
		}
		return true;
	}
	void solveObjectsCollisionAll(int times) {
		int count = getCount();
		for (int k = 0; k < times; k++) {
			for (int i = 0; i < count; i++) {
				for (int j = i + 1; j < count; j++) {
					if (mPhysObjData[i].isColliding(mPhysObjData[j])) {
						PhysicsObject::solveStaticCollide(mPhysObjData[i], mPhysObjData[j]);
						PhysicsObject::solveDynamicCollide(mPhysObjData[i], mPhysObjData[j], 0.99f);
					}
				}
				mPhysObjData[i].collideBox(mBoundingBox, times);
			}
		}
	}
	void popObjectAt(ImVec2 pos) {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			if(	(pos.x <= mPhysObjData[i].mPos.x + mPhysObjData[i].mSize) && 
				(pos.x >= mPhysObjData[i].mPos.x - mPhysObjData[i].mSize) &&
				(pos.y <= mPhysObjData[i].mPos.y + mPhysObjData[i].mSize) &&
				(pos.y >= mPhysObjData[i].mPos.y - mPhysObjData[i].mSize)
				){
				mPhysObjData.erase(mPhysObjData.begin() + i);
			}
		}
	}
	void addVelocityToAll(ImVec2 velocity) {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			mPhysObjData[i].mVelocity.x += velocity.x;
			mPhysObjData[i].mVelocity.y += velocity.y;
		}
	}
	void scaleAll(float scale) {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			mPhysObjData[i].scale(scale);
		}
	}
	void solveGravityAll() {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			mPhysObjData[i].solveGravity();
		}
	}
	void solveVelocityAll() {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			mPhysObjData[i].solveVelocity();
		}
	}
	ImVec4 getVelocityArrowFromObjectAt(int index) {
		ImVec4 velocityArrow(0, 0, 0, 0);
		int count = getCount();
		if (index < count) {
			velocityArrow.x = mPhysObjData[index].mPos.x;
			velocityArrow.y = mPhysObjData[index].mPos.y;
			velocityArrow.z = mPhysObjData[index].mPos.x + mPhysObjData[index].mVelocity.x * 3.f;
			velocityArrow.w = mPhysObjData[index].mPos.y + mPhysObjData[index].mVelocity.y * 3.f;
		}
		return velocityArrow;
	}
	void drawVelocity() {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			Object obj = Object(CIRCLE, mPhysObjData[i].mPos, mPhysObjData[i].mSize, mPhysObjData[i].mColor);
			float velValue = sqrtf(mPhysObjData[i].mVelocity.x*mPhysObjData[i].mVelocity.x + mPhysObjData[i].mVelocity.y*mPhysObjData[i].mVelocity.y);
			float hue = velValue*5.f;
			while (hue > 360.f) {
				hue -= 360.f;
			}
			float value = velValue / 5.f;
			hsv velColor = hsv(hue, 1.f, value);
			obj.mColor = hsv2rgb(velColor);
			obj.draw();
		}
	}
	void draw() {
		for (PhysicsObject& obj : mPhysObjData) {
			obj.draw();
		}
	}
};