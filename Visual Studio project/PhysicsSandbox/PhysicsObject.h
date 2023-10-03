#pragma once
#include "Object.h"
#include <cmath>

namespace sbox
{
	class PhysicsObject : public Object {
	public:
		ImVec2 mVelocity;
		float mMass;
		bool mGravity = true;
		float mGravityScale = .5f;

		PhysicsObject();
		PhysicsObject(Object obj, ImVec2 velocity);
		static PhysicsObject createRandomPhysObj(RECT boundingBox, float size, int maxVel);
		static PhysicsObject createRandomPhysObj(RECT boundingBox, float size, ImVec2 velocity);
		static PhysicsObject createRandomPhysObj(RECT boundingBox, int deviationSize, int minSize, int maxVel);
		void scale(float proportions);
		void invertVel(bool xy);
		void applyFriction(ImVec2 friction);
		void solveGravity(float dt);
		void solveVelocity(float dt);
		void collideBox(RECT boxRect, int iterationCount, bool useApplyFriction = true);
		void setPosStatic(ImVec2 pos);
		bool isColliding(Object& obj);
		void magnetTo(ImVec2 pos, float strength);
		void static solveStaticCollide(PhysicsObject& obj1, PhysicsObject& obj2);
		void static solveDynamicCollide(PhysicsObject& obj1, PhysicsObject& obj2, float efficiency = 1.f);
	};

}

