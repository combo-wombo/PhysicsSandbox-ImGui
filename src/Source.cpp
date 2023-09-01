#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "Object.h"
#include "PhysicsObject.h"
#include "PhysicsObjectContainer.h"

int WINDOW_HEIGHT = 800;
int WINDOW_WIDTH = 1000;
bool enableGravity = true;
bool drawVelocityArrows = false;
bool colorizeBallsVelocity = false;
bool drawCollisionGridData = false;
bool running = true;
bool showfps = true;
int COLLISION_STEPS = 20;
int TICKRATE = 66;
float magnetStrength = 0.2f;
double ticksPerSecond = 1.0 / TICKRATE;
int ticksPerMillisecond = static_cast<int>(ticksPerSecond * 1000);
float dt = 0;
float dtReal = 0;
PhysicsObjectContainer globalContainer;

void drawText(ImVec2 pos, float scale, std::string text) {
	const char* textC = text.c_str();
	ImFont* curFont = ImGui::GetFont();
	curFont->Scale = scale;
	ImGui::PushFont(curFont);
	ImVec2 textSize = ImGui::CalcTextSize(textC);
	ImGui::GetBackgroundDrawList()->AddRectFilled({ pos.x, pos.y }, { pos.x + textSize.x, pos.y + textSize.y }, ImColor(0.f, 0.f, 0.f, 0.8f));
	ImGui::GetBackgroundDrawList()->AddText({ pos.x, pos.y }, ImColor(1.f, 1.f, 1.f, 1.f), textC);
	ImGui::PopFont();
}

void drawFps(ImVec2 pos, float scale) {
	drawText(pos, scale, "FPS: " + std::to_string(ImGui::GetIO().Framerate));
}

void calculatePhysics() {
	while (running) {
		clock_t tStart = clock();

		if (GetAsyncKeyState(0x51))/*Q*/ { globalContainer.addRandomObjectAt(ImGui::GetMousePos(), 5, 2, 10); }
		if (GetAsyncKeyState(0x45))/*E*/ { globalContainer.addRandomObjectAt(ImGui::GetMousePos(), 10, 10); }
		if (GetAsyncKeyState(VK_SPACE)) { if (1 / dtReal > 40.f) { globalContainer.addRandomObjectAt({ 15,15 }, 9.f, { 35.f,0.f }); } }

		if (GetAsyncKeyState(VK_OEM_MINUS)) { globalContainer.scaleAll(0.99f); }
		if (GetAsyncKeyState(VK_OEM_PLUS)) { globalContainer.scaleAll(1.01f); }

		if (GetAsyncKeyState(0x46))/*F*/ { globalContainer.removeObject(); globalContainer.removeObject(); }
		if (GetAsyncKeyState(VK_DELETE)) { globalContainer.mPhysObjData.clear(); }

		if (GetAsyncKeyState(0x57)) { globalContainer.addVelocityToAll({ 0.f,-3.f }); }
		if (GetAsyncKeyState(0x53)) { globalContainer.addVelocityToAll({ 0.f,3.f }); }
		if (GetAsyncKeyState(0x41)) { globalContainer.addVelocityToAll({ -3.f,0.f }); }
		if (GetAsyncKeyState(0x44)) { globalContainer.addVelocityToAll({ 3.f,0.f }); }

		if (GetAsyncKeyState(0x47))/*G*/ {
			globalContainer.popObjectAt(ImGui::GetMousePos());
		}

		if (GetAsyncKeyState(VK_LBUTTON)) {
			globalContainer.magnetAllTo(ImGui::GetMousePos(), magnetStrength);
		}

		if (GetAsyncKeyState(VK_RBUTTON)) {
			if (globalContainer.mSavedObjectIndex == -1) {
				globalContainer.mSavedObjectIndex = globalContainer.getObjectIndexAt(ImGui::GetMousePos());
			}
			else {
				globalContainer.mPhysObjData[globalContainer.mSavedObjectIndex].magnetTo(ImGui::GetMousePos(), 10.f);
				globalContainer.mPhysObjData[globalContainer.mSavedObjectIndex].mVelocity.x *= 0.8f;
				globalContainer.mPhysObjData[globalContainer.mSavedObjectIndex].mVelocity.y *= 0.8f;
			}
		}
		else {
			globalContainer.mSavedObjectIndex = -1;
		}

		/// //////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (enableGravity && !GetAsyncKeyState(VK_INSERT)) {
			globalContainer.solveGravityAll(dt);
		}

		globalContainer.solveVelocityAll(dt);
		globalContainer.solveCollision(COLLISION_STEPS);

		/// //////////////////////////////////////////////////////////////////////////////////////////////////////////
		dt = (float)(clock() - tStart) / CLOCKS_PER_SEC;
		if (dt < ticksPerSecond) {
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((ticksPerSecond - dt) * 1000)));
		}
		dtReal = (float)(clock() - tStart) / CLOCKS_PER_SEC;
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) { return 0L; }
	if (message == WM_DESTROY) { PostQuitMessage(0); return 0L; }
	return DefWindowProcW(window, message, w_param, l_param);
}

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"comboWomboClass";
	RegisterClassExW(&wc);
	const HWND window = CreateWindowExW(WS_EX_LAYERED, wc.lpszClassName, L"Physics window", 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, wc.hInstance, nullptr);
	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);
	RECT client_area{};	GetClientRect(window, &client_area);
	RECT window_area{};	GetWindowRect(window, &window_area);
	POINT diff{};
	ClientToScreen(window, &diff);
	const MARGINS margins{ window_area.left + (diff.x - window_area.left), window_area.top + (diff.y - window_area.top), client_area.right, client_area.bottom };
	DwmExtendFrameIntoClientArea(window, &margins);
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	constexpr D3D_FEATURE_LEVEL levels[2]{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0U, levels, 2U, D3D11_SDK_VERSION, &sd, &swap_chain, &device, &level, &device_context);
	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));
	if (back_buffer) { device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view); back_buffer->Release(); }
	else { return 1; }
	ShowWindow(window, cmd_show);
	UpdateWindow(window);
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);
	RECT windowRect;
	GetWindowRect(window, &windowRect);
	std::thread physicsThread(calculatePhysics);
	RECT gameBox;
	gameBox.left = 0;
	gameBox.top = 0;
	gameBox.right = windowRect.right - windowRect.left - GetSystemMetrics(SM_CYFRAME) * 2 - GetSystemMetrics(SM_CXPADDEDBORDER) * 2;
	gameBox.bottom = windowRect.bottom - windowRect.top - (GetSystemMetrics(SM_CYFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER) * 2);
	globalContainer.setBoundingBox(gameBox);

	ImGuiIO& io = ImGui::GetIO();
	ImFont* f_minecraftia = io.Fonts->AddFontFromFileTTF("font/Minecraftia.ttf", 15);

	while (running) {
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg); DispatchMessageW(&msg);
			if (msg.message == WM_QUIT) { running = false; }
		}
		if (!running) { break; }
		ImGui_ImplDX11_NewFrame(); ImGui_ImplWin32_NewFrame(); ImGui::NewFrame();
		if (GetAsyncKeyState(VK_ESCAPE)) {
			running = false;
		}
		//...............................................................................................code.goes.down.here..............................
		if (colorizeBallsVelocity) {
			globalContainer.drawVelocity();
		}
		else {
			globalContainer.draw();
		}

		if (drawCollisionGridData) {
			float gridSectorLength = (gameBox.right) / (float)globalContainer.mGridLength;
			float gridSectorHeight = (gameBox.bottom) / (float)globalContainer.mGridHeight;
			for (int i = 0; i < globalContainer.mGridLength; i++) {
				ImGui::GetBackgroundDrawList()->AddLine({ 0.f + gridSectorLength * (1 + i), 0.f }, { 0.f + gridSectorLength * (1 + i), 0.f + windowRect.bottom }, ImColor(1.f, 1.f, 1.f));
				ImGui::GetBackgroundDrawList()->AddLine({ 0.f, 0.f + gridSectorHeight * (1 + i) }, { 0.f + windowRect.right, 0.f + gridSectorHeight * (1 + i) }, ImColor(1.f, 1.f, 1.f));
				for (int j = 0; j < globalContainer.mGridHeight; j++) {
					if (globalContainer.mGridData.at(i).at(j).size() != 0) {
						drawText({ i * gridSectorLength + gridSectorLength / 2, j * gridSectorHeight + gridSectorHeight / 2 }, 1.f, std::to_string(globalContainer.mGridData.at(i).at(j).size()));
					}
				}
			}
		}

		ImGui::Begin("Menu", (bool*)0, ImGuiWindowFlags_NoMove);
		ImGui::GetStyle().WindowRounding = 10.f;
		ImGui::GetStyle().WindowBorderSize = 1.f;
		ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;
		ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.6f);

		ImGui::SetWindowPos({ 0.f,0.f });
		ImGui::GetStyle().WindowMinSize = { 320.f,130.f };
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImColor(0.f, 0.f, 0.f, 0.2f);
		std::string fpsText = "FPS: " + std::to_string(ImGui::GetIO().Framerate);
		std::string physicsFpsText = "Physics fps: " + std::to_string(1 / dtReal);
		std::string objCount = "Object count: " + std::to_string(globalContainer.getCount());
		ImGui::Text(fpsText.c_str());
		ImGui::Text(physicsFpsText.c_str());
		ImGui::Text(objCount.c_str());
		if (ImGui::CollapsingHeader("Settings")) {
			ImGui::Checkbox("Enable gravity", &enableGravity);
			ImGui::Checkbox("Draw velocity arrows", &drawVelocityArrows);
			ImGui::Checkbox("Draw collision grid data", &drawCollisionGridData);
			ImGui::Checkbox("Velocity color", &colorizeBallsVelocity);
			ImGui::SliderFloat("Magnet strength", &magnetStrength, 0.f, 1.1f);
		}
		if (ImGui::CollapsingHeader("Controls help")) {
			ImGui::Text("Left mouse button - magnet objects");
			ImGui::Text("Right mouse button - drag one object");
			ImGui::Separator();
			ImGui::Text("Q - add small round object");
			ImGui::Text("E - add big round object");
			ImGui::Text("Spacebar - add stream of objects");
			ImGui::Separator();
			ImGui::Text("F	  - delete last added objects");
			ImGui::Text("G	  - delete object at mouse cursor");
			ImGui::Text("Delete key - clear objects");
			ImGui::Separator();
			ImGui::Text("W/A/S/D - control gravity");
			ImGui::Text("INSERT - stop gravity");
			ImGui::Text("-/+  - control scale");
			ImGui::Separator();
			ImGui::Text("ESC  - exit program");
		}
		ImGui::End();

		if (GetAsyncKeyState(VK_RBUTTON)) {
			if (globalContainer.mSavedObjectIndex == -1) {
				ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMousePos(), 10.f, ImColor(1.f, 1.f, 1.f));
			}
			else {
				ImGui::GetBackgroundDrawList()->AddLine(ImGui::GetMousePos(), globalContainer.mPhysObjData[globalContainer.mSavedObjectIndex].mPos, ImColor(1.f, 1.f, 1.f));
				ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMousePos(), globalContainer.mPhysObjData[globalContainer.mSavedObjectIndex].mSize, ImColor(1.f, 1.f, 1.f));
			}
		}


		if (drawVelocityArrows) {
			for (int i = 0; i < globalContainer.getCount(); i++) {
				ImVec4 arrow = globalContainer.getVelocityArrowFromObjectAt(i);
				ImGui::GetBackgroundDrawList()->AddLine({ arrow.x, arrow.y }, { arrow.z,arrow.w }, ImColor(1.f, 1.f, 1.f));
				ImGui::GetBackgroundDrawList()->AddCircleFilled({ arrow.z,arrow.w }, 3.f, ImColor(0.f, 0.f, 0.f));
				ImGui::GetBackgroundDrawList()->AddCircle({ arrow.z,arrow.w }, 3.f, ImColor(1.f, 1.f, 1.f));
			}
		}
		//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
		ImGui::Render();
		constexpr float color[4]{ 0.f,0.f,0.f,0.9f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(1U, 0U);
	}
	physicsThread.join();
	ImGui_ImplDX11_Shutdown(); ImGui_ImplWin32_Shutdown(); ImGui::DestroyContext();
	if (swap_chain) { swap_chain->Release(); }
	if (device_context) { device_context->Release(); }
	if (device) { device->Release(); }
	if (render_target_view) { render_target_view->Release(); }
	DestroyWindow(window); UnregisterClassW(wc.lpszClassName, wc.hInstance);
	return 0;
}