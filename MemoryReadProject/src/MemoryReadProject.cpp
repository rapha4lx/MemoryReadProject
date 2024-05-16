#include "../Include/global.h"


char ProcessName[128];
int currentDrawModuleIndex{ 0 };


int FindValueIndex{ 0 };
const char* FindValueTypes[]
{
	"int", "float"
};

char FindValue[60];

//template <typename T>
//GetType(const char* type) {
//	switch (type)
//	{
//	case "int":
//		return int;
//
//	case "float":
//		return float;
//
//	default:
//		break;
//	}
//}

void menu() {
	if (ImGui::BeginTabBar("##open selector")) {

		if (ImGui::BeginTabItem("Select Process"))
		{
			ImGui::InputText("Process Name", ProcessName, IM_ARRAYSIZE(ProcessName));

			Process::GetAllProcess();

			std::map<std::wstring, PID>::iterator it = Process::currentsProcess.begin();

			ImGui::BeginChild("Process", ImVec2(500, 150));
			{
				int ProcessNameCount = 0;
				for (int i = 0; i < ProcessName[i] != '\0'; i++)
				{
					++ProcessNameCount;
				}

				bool print = false;
				for (; it != Process::currentsProcess.end();) {
					if (ProcessNameCount > 0) {
						for (int i = 0; i < ProcessNameCount;) {
							try
							{
								if (it->first[i] == ProcessName[i]) {
									++i;
								}
								else
								{
									break;
								}
								if (i == ProcessNameCount) {
									print = true;
								}
							}
							catch (const std::exception&)
							{
								print = false;
								break;
							}
						}

						if (print) {
							if (it->second == Process::selectedProcess.pid) {
								ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
							}
							else
							{
								ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
							}

							ImGui::Text("ProcessName: %ls / PID: %d", it->first.c_str(), it->second);
							if (ImGui::IsItemClicked()) {
								Process::selectedProcess.pid = it->second;
								Process::selectedProcess.openProcess();
								Process::GetMemoryPages();
								Process::GetModules();
							}
							ImGui::PopStyleColor();
							print = false;
						}
						++it;
					}
					else
					{
						if (it->second == Process::selectedProcess.pid) {
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
						}

						ImGui::Text("ProcessName: %ls / PID: %d", it->first.c_str(), it->second);
						if (ImGui::IsItemClicked()) {
							Process::selectedProcess.pid = it->second;
							Process::selectedProcess.openProcess();
							Process::GetMemoryPages();
							Process::GetModules();
						}
						ImGui::PopStyleColor();
						++it;
					}

				}

			}ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (Process::selectedProcess.pid) {
			//Memory
			if (ImGui::BeginTabItem("Memory Page Info")) {
				if (ImGui::Button("Update Pages")) {
					Process::GetMemoryPages();
				}

				ImGui::Text("Memory Pages");
				ImGui::BeginChild("Memory Page", ImVec2(700, 300));
				{
					if (currentDrawModuleIndex * 30 < Process::currentMemoryModulesInfo.size()) {
						for (int i = currentDrawModuleIndex * 30, count = 0;
							i < Process::currentMemoryModulesInfo.size() && count < 30;
							i++, count++)
						{
							Process::DrawMemoryPages(i);
						}
					}
				}ImGui::EndChild();//Get modules end

				//Memory Pages
				ImGui::BeginChild("Memory Pages Count", ImVec2(700, 50), 0, ImGuiWindowFlags_HorizontalScrollbar);
				{
					for (int i = 0; i < Process::currentMemoryModulesInfo.size() / 30; i++)
					{
						char buffer[12];
#pragma warning(suppress: 4996)
						std::sprintf(buffer, "%d", i);
						if (ImGui::Button(buffer, ImVec2(50, 30))) {
							currentDrawModuleIndex = i;
						}
						ImGui::SameLine();
					}
				}ImGui::EndChild();


				ImGui::EndTabItem(); //End Memory tab
			}

			if (ImGui::BeginTabItem("Modules Info")) {

				for (int i = 0; i < Process::currentProcessModules.size(); i++)
				{
					Process::DrawModules(i);
				}
				
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Find Value")) {
				ImGui::InputText("Find Value", FindValue, IM_ARRAYSIZE(FindValue));
				
				ImGui::Combo("Type", &FindValueIndex, FindValueTypes, 2);
				

				/*if (Process::findedCurrentProcessMemory<GetType("int")>) {

				}*/





				ImGui::EndTabItem();
			}
		}


		ImGui::EndTabBar();
	} //endTabNar
}

void startGui() {
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, UI::WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!UI::CreateDeviceD3D(hwnd))
	{
		UI::CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(UI::g_pd3dDevice);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (UI::g_ResizeWidth != 0 && UI::g_ResizeHeight != 0)
		{
			UI::g_d3dpp.BackBufferWidth = UI::g_ResizeWidth;
			UI::g_d3dpp.BackBufferHeight = UI::g_ResizeHeight;
			UI::g_ResizeWidth = UI::g_ResizeHeight = 0;
			UI::ResetDevice();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("MemoryReadyProject", nullptr);

		menu();

		ImGui::End();

		// Rendering
		ImGui::EndFrame();
		UI::g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		UI::g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		UI::g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		UI::g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (UI::g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			UI::g_pd3dDevice->EndScene();
		}
		HRESULT result = UI::g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && UI::g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			UI::ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	UI::CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	//Process::selectedProcess.Destroy();
}


// Main code
int main(int, char**)
{

	startGui();
	return 0;
}

