#pragma once
#include "PhysicsObject.h"
#include <vector>
#include <stdexcept>

namespace sbox
{
	class PhysicsObjectContainer {
	public:
		std::vector<PhysicsObject> mPhysObjData;
		std::vector<std::vector<std::vector<int>>> mGridData;
		int mGridLength = 25;
		int mGridHeight = 25;
		int mSavedObjectIndex = -1;
		RECT mBoundingBox;
		
		int getCount();
		PhysicsObjectContainer();
		void gridDataSetup();
		PhysicsObjectContainer(RECT boundingBox);
		RECT getBoundingBox();
		void setBoundingBox(RECT boundingBox);
		void setCollisionGridSize(int length, int height);
		void addRandomObjects(int objCount, int maxObjSize, int minObjSize, int maxObjVelocity);
		void addRandomObject(int maxSize, int minSize, int maxVelocity);
		void addRandomObjectAt(ImVec2 position, int deviationSize, int minSize, int maxVelocity);
		void addRandomObjectAt(ImVec2 position, float size, int maxVelocity);
		void addRandomObjectAt(ImVec2 position, float size, ImVec2 velocity);
		void addObject(PhysicsObject& obj);
		bool removeObject();
		PhysicsObject getObjectAt(int index);
		bool removeObjects(int count);
		void solveCollision(int times);
		void solveBoxCollision();
		void solveObjectsCollision();
		void popObjectAt(ImVec2 pos);
		int getObjectIndexAt(ImVec2 pos);
		void addVelocityToAll(ImVec2 velocity);
		void scaleAll(float scale);
		void solveGravityAll(float dt);
		void solveVelocityAll(float dt);
		void magnetAllTo(ImVec2 pos, float strength);
		ImVec4 getVelocityArrowFromObjectAt(int index);
		void drawVelocity();
		void draw();
	};
}

