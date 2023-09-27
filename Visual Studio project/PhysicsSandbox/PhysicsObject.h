#pragma once
#include "Object.h"
#include <cmath>

class PhysicsObject : public Object {
public:
	ImVec2 mVelocity;
	float mMass;
	bool mGravity = true;
	float mGravityScale = .5f;

	PhysicsObject() {
		mVelocity = { 0.f,0.f };
		mMass = mSize * 5;
	}
	PhysicsObject(Object obj, ImVec2 velocity) : Object(obj.mShape, obj.mPos, obj.mSize, obj.mColor) {
		mVelocity = velocity;
		mMass = obj.mSize * 5;
	}
	static PhysicsObject createRandomPhysObj(RECT boundingBox, float size, int maxVel) {
		float randVelX = rand() % maxVel * 2 - maxVel;
		float randVelY = rand() % maxVel * 2 - maxVel;
		ImVec2 randVelocity = { randVelX, randVelY };
		return PhysicsObject(Object::createRandomCircle(boundingBox, size), randVelocity);
	}
	static PhysicsObject createRandomPhysObj(RECT boundingBox, float size, ImVec2 velocity) {
		return PhysicsObject(Object::createRandomCircle(boundingBox, size), velocity);
	}
	static PhysicsObject createRandomPhysObj(RECT boundingBox, int deviationSize, int minSize, int maxVel) {
		float randVelX = rand() % maxVel * 2 - maxVel;
		float randVelY = rand() % maxVel * 2 - maxVel;
		ImVec2 randVelocity = { randVelX, randVelY };
		return PhysicsObject(Object::createRandomCircle(boundingBox, deviationSize, minSize), randVelocity);
	}
	void scale(float proportions) {
		mSize = mSize * proportions;
		if (mSize < 1.f) {
			mSize = 1.f;
		}
		mMass = mSize * 5;
	}
	void invertVel(bool xy) {
		if (xy) {
			mVelocity = { -1.f * mVelocity.x, mVelocity.y };
		}
		else {
			mVelocity = { mVelocity.x, -1.f * mVelocity.y };
		}
	}
	void applyFriction(ImVec2 friction) {
		mVelocity = { mVelocity.x * friction.x, mVelocity.y * friction.y };
	}
	void solveGravity(float dt) {
		if (mGravity) {
			dt += 1;
			mVelocity = { mVelocity.x, mVelocity.y + mGravityScale * dt };
		}
	}
	void solveVelocity(float dt) {
		dt += 1;
		mPos.x = mPos.x + mVelocity.x * dt;
		mPos.y = mPos.y + mVelocity.y * dt;
	}
	void collideBox(RECT boxRect, int iterationCount, bool useApplyFriction = true) {
		if (mPos.x + mSize >= boxRect.right) {
			//collision right border;
			mPos = { boxRect.right - mSize, mPos.y };
			if (mVelocity.x >= 6.f) {
				mVelocity = { -1.f * mVelocity.x, mVelocity.y };
			}
			if (useApplyFriction) {
				applyFriction({ 1.f - (0.19f / iterationCount), 1.f });
			}
		}
		if (mPos.x - mSize <= boxRect.left) {
			//collision left border;
			mPos = { boxRect.left + mSize, mPos.y };
			if (mVelocity.x <= -6.f) {
				mVelocity = { -1.f * mVelocity.x, mVelocity.y };
			}
			if (useApplyFriction) {
				applyFriction({ 1.f - (0.19f / iterationCount), 1.f });
			}
		}
		if (mPos.y + mSize >= boxRect.bottom) {
			//bouncing off ground
			mPos = { mPos.x, boxRect.bottom - mSize };
			if (mVelocity.y >= 6.f) {
				mVelocity = { mVelocity.x, -1.f * mVelocity.y };
			}
			if (useApplyFriction) {
				applyFriction({ 1.f - (0.1f / iterationCount), 1.f - (0.5f / iterationCount) });
			}
		}
		if (mPos.y - mSize <= boxRect.top) {
			//bouncing off roof
			mPos = { mPos.x, boxRect.top + mSize };
			if (mVelocity.y <= -6.f) {
				mVelocity = { mVelocity.x, -1.f * mVelocity.y };
			}
			if (useApplyFriction) {
				applyFriction({ 1.f - (0.1f / iterationCount), 1.f - (0.5f / iterationCount) });
			}
		}
	}
	void setPosStatic(ImVec2 pos) {
		mPos = pos;
		mVelocity = { 0, 0 };
	}
	bool isColliding(Object& obj) {
		float aX = mPos.x - obj.mPos.x;
		float aY = mPos.y - obj.mPos.y;
		float sumXY = aX * aX + aY * aY;
		float distance = sqrtf(sumXY);
		float sumSizes = mSize + obj.mSize;
		if (distance < sumSizes) {
			return true;
		}
		else {
			return false;
		}
	}

	void magnetTo(ImVec2 pos, float strength) {
		float xDiff = (pos.x - mPos.x) * 0.02f * strength;
		float yDiff = (pos.y - mPos.y) * 0.02f * strength;
		mVelocity = {
			mVelocity.x + xDiff,
			mVelocity.y + yDiff
		};
	}

	void static solveStaticCollide(PhysicsObject& obj1, PhysicsObject& obj2) {
		float distance = sqrtf((obj2.mPos.x - obj1.mPos.x) * (obj2.mPos.x - obj1.mPos.x) + (obj2.mPos.y - obj1.mPos.y) * (obj2.mPos.y - obj1.mPos.y));
		if (distance >= 1.f) {
			float overlap = 0.51f * (distance - obj1.mSize - obj2.mSize);
			obj1.mPos = { obj1.mPos.x - overlap * (obj1.mPos.x - obj2.mPos.x) / distance, obj1.mPos.y - overlap * (obj1.mPos.y - obj2.mPos.y) / distance };
			obj2.mPos = { obj2.mPos.x + overlap * (obj1.mPos.x - obj2.mPos.x) / distance, obj2.mPos.y + overlap * (obj1.mPos.y - obj2.mPos.y) / distance };
		}
		else {
			float overlap = 0.51f * (obj1.mSize - obj2.mSize) + 1.f;
			obj1.mPos = { obj1.mPos.x - overlap, obj1.mPos.y + overlap };
			obj2.mPos = { obj2.mPos.x + overlap, obj2.mPos.y - overlap };
		}
	}
	void static solveDynamicCollide(PhysicsObject& obj1, PhysicsObject& obj2, float efficiency = 1.f) {
		float distance = sqrtf((obj2.mPos.x - obj1.mPos.x) * (obj2.mPos.x - obj1.mPos.x) + (obj2.mPos.y - obj1.mPos.y) * (obj2.mPos.y - obj1.mPos.y));
		if (distance == 0) {
			float overlap = 0.5f * (obj1.mSize - obj2.mSize);
			obj1.mPos = { obj1.mPos.x - overlap, obj1.mPos.y - overlap };
			obj2.mPos = { obj2.mPos.x + overlap, obj2.mPos.y + overlap };
		}
		else {
			float normalX = (obj2.mPos.x - obj1.mPos.x) / distance;
			float normalY = (obj2.mPos.y - obj1.mPos.y) / distance;
			float tanX = -normalY;
			float dotProductTan1 = obj1.mVelocity.x * tanX + obj1.mVelocity.y * normalX;
			float dotProductTan2 = obj2.mVelocity.x * tanX + obj2.mVelocity.y * normalX;
			float dotProductNormal1 = obj1.mVelocity.x * normalX + obj1.mVelocity.y * normalY;
			float dotProductNormal2 = obj2.mVelocity.x * normalX + obj2.mVelocity.y * normalY;
			float momentum1 = (dotProductNormal1 * (obj1.mMass - obj2.mMass) + 2.0f * obj2.mMass * dotProductNormal2) / (obj1.mMass + obj2.mMass);
			float momentum2 = (dotProductNormal2 * (obj2.mMass - obj1.mMass) + 2.0f * obj1.mMass * dotProductNormal1) / (obj1.mMass + obj2.mMass);
			if (efficiency < 1.f) {
				momentum1 *= efficiency;
				momentum2 *= efficiency;
			}
			obj1.mVelocity = { tanX * dotProductTan1 + normalX * momentum1,	normalX * dotProductTan1 + normalY * momentum1 };
			obj2.mVelocity = { tanX * dotProductTan2 + normalX * momentum2,	normalX * dotProductTan2 + normalY * momentum2 };
		}
	}
};