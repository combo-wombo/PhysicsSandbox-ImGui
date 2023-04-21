# Work in progress - PhysicsSandbox-ImGui
A small DirectX-11 ImGui application that can simulate circular physical objects.

## Version hierarchy
Version 1 (27.03.2023): 
	The first version, very unstable, not optimized, basic functionality and settings.

Version 1.1 (21.04.2023):
	-Grid collision optimization (objects are now compared only to their grid "neighbors", check out that grid view in the menu). 60FPS object count before was ~800, now it's more than 6000 and still pushing (except it suddenly crashes when i go past these numbers). Although the physics functions need a remake, they can't handle all that.
	-Added ability to magnet all objects with left mouse click and drag one object with right mouse click.
	-Changed menu font to "minecraftia".
	-Lowered the gravity from 0.7 to 0.5.
	-Making steps towards delta-timing movement.

## Technologies used
- C++ 20
- DirectX 11 as a foundation for ImGui
- Dear ImGui

## Latest version screenshots
![cap 1](https://github.com/combo-wombo/PhysicsSandbox-ImGui/blob/main/Screenshots/1.jpg?raw=true)
