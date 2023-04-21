#pragma once
#include "PhysicsObject.h"
#include <stdexcept>

class PhysicsObjectContainer {
public:
	std::vector<PhysicsObject> mPhysObjData;
	std::vector<std::vector<std::vector<int>>> mGridData;
	int mGridLength = 25;
	int mGridHeight = 25;
	int mSavedObjectIndex = -1;
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
		gridDataSetup();
	}

	void gridDataSetup() {
		mGridData.clear();
		for (int i = 0; i < mGridLength; i++) {
			std::vector<std::vector<int>> mGridDataY;
			for (int j = 0; j < mGridHeight; j++) {
				std::vector<int> mData;
				mGridDataY.push_back(mData);
			}
			mGridData.push_back(mGridDataY);
		}
	}

	PhysicsObjectContainer(RECT boundingBox) {
		mBoundingBox = boundingBox;
	}

	RECT getBoundingBox() {
		return mBoundingBox;
	}

	void setBoundingBox(RECT boundingBox) {
		mBoundingBox = boundingBox;
	}

	void setCollisionGridSize(int length, int height) {
		mGridLength = length;
		mGridHeight = height;
		gridDataSetup();
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

	void solveCollision(int times) {
		solveObjectsCollision();
		solveBoxCollision();
	}

	void solveBoxCollision() {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			mPhysObjData[i].collideBox(mBoundingBox, 1);
		}
	}

	void solveObjectsCollision() {
		int count = getCount();
		float windowLength = mBoundingBox.right;
		float windowHeight = mBoundingBox.bottom;

		for (int i = 0; i < mGridLength; i++) { //clear old sector data
			for (int j = 0; j < mGridHeight; j++) {
				mGridData.at(i).at(j).clear();
			}
		}
		for (int i = 0; i < count; i++) {		//fill new sector data
			mPhysObjData[i].collideBox(mBoundingBox, 1);
			int gridX = mPhysObjData[i].mPos.x / windowLength * mGridLength;
			int gridY = mPhysObjData[i].mPos.y / windowHeight * mGridHeight;
			if (gridX >= 0 && gridY >= 0 && gridX < mGridLength && gridY < mGridHeight) {
				mGridData.at(gridX).at(gridY).push_back(i);
			}
		}
		for (int i = 0; i < mGridLength-1; i++) {
			for (int j = 0; j < mGridHeight-1; j++) {
				std::vector<int> possibleColliders;
				for (int index : mGridData.at(i).at(j)) {
					possibleColliders.push_back(index);
				}
				for (int index : mGridData.at(i).at(j+1)) {
					possibleColliders.push_back(index);
				}
				for (int index : mGridData.at(i+1).at(j)) {
					possibleColliders.push_back(index);
				}
				for (int index : mGridData.at(i+1).at(j+1)) {
					possibleColliders.push_back(index);
				}
				for (int collider1 : possibleColliders) {
					for (int collider2 : possibleColliders) {
						if (collider1 != collider2) {
							if (mPhysObjData[collider1].isColliding(mPhysObjData[collider2])) {
								PhysicsObject::solveStaticCollide(mPhysObjData[collider1], mPhysObjData[collider2]);
								PhysicsObject::solveDynamicCollide(mPhysObjData[collider1], mPhysObjData[collider2], 0.99f);
							}
						}
					}
				}
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

	int getObjectIndexAt(ImVec2 pos) {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			if ((pos.x <= mPhysObjData[i].mPos.x + mPhysObjData[i].mSize) &&
				(pos.x >= mPhysObjData[i].mPos.x - mPhysObjData[i].mSize) &&
				(pos.y <= mPhysObjData[i].mPos.y + mPhysObjData[i].mSize) &&
				(pos.y >= mPhysObjData[i].mPos.y - mPhysObjData[i].mSize)
				) {
				return i;
			}
		}
		return -1;
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

	void solveGravityAll(float dt) {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			mPhysObjData[i].solveGravity(dt);
		}
	}

	void solveVelocityAll(float dt) {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			mPhysObjData[i].solveVelocity(dt);
		}
	}

	void magnetAllTo(ImVec2 pos, float strength) {
		int count = getCount();
		for (int i = 0; i < count; i++) {
			mPhysObjData[i].magnetTo(pos, strength);
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