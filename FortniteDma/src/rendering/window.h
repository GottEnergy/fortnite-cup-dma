#pragma once

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
		return 0L;
	}

	if (message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0L;
	}
	
	return DefWindowProc(window, message, w_param, l_param);
}

ID3D11Device* device{ nullptr };
ID3D11DeviceContext* device_context{ nullptr };
IDXGISwapChain* swap_chain{ nullptr };
ID3D11RenderTargetView* render_target_view{ nullptr };
D3D_FEATURE_LEVEL level{};
HWND window{};
WNDCLASSEXW wc{};

bool InitWindow(HINSTANCE instance, INT cmd_show)
{
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = instance;
	wc.lpszClassName = settings::window::ClassName;

	RegisterClassExW(&wc);

	HWND window;
	if (settings::config::MoonlightAim) {
		window = CreateWindowExW(
			WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
			wc.lpszClassName,
			settings::window::WindowName,
			WS_POPUP,
			0,
			0,
			settings::window::Width,
			settings::window::Height,
			nullptr,
			nullptr,
			wc.hInstance,
			nullptr
		);
	}
	else {
		window = CreateWindowExW(
			WS_EX_TOPMOST | WS_EX_TRANSPARENT,
			wc.lpszClassName,
			settings::window::WindowName,
			WS_POPUP,
			0,
			0,
			settings::window::Width,
			settings::window::Height,
			nullptr,
			nullptr,
			wc.hInstance,
			nullptr
		);
	}

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 0U;
	sd.BufferDesc.RefreshRate.Denominator = 0U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // DXGI_SWAP_EFFECT_DISCARD
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

	constexpr D3D_FEATURE_LEVEL levels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer) {
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else {
		std::cout << hue::red << "(!) " << hue::white << "Error creating render target view" << std::endl;
		return false;
	}

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);


	ImGuiIO& io = ImGui::GetIO();

	// style

	auto & colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Child

	colors[ImGuiCol_ChildBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.6f };

	// Border
	colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

	// Text
	colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
	colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

	// Popups
	colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

	// Slider
	colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
	colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

	// Seperator
	colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
	colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
	colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

	// Resize Grip
	colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
	colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

	auto& style = ImGui::GetStyle();
	style.TabRounding = 4;
	style.ScrollbarRounding = 9;
	style.WindowRounding = 7;
	style.GrabRounding = 3;
	style.FrameRounding = 3;
	style.PopupRounding = 4;
	style.ChildRounding = 4;

	// fonts

	fonts::notosans_font = io.Fonts->AddFontFromMemoryTTF(fonts::notosans, sizeof(fonts::notosans), 1.712962962962963 * (settings::window::Height/100));
	fonts::typenatural_font = io.Fonts->AddFontFromMemoryTTF(fonts::typenatural, sizeof(fonts::typenatural), 1.712962962962963 * (settings::window::Height / 100));

	// images
	int image_width = 0, image_height = 0;
	unsigned char* decodedData = stbi_load_from_memory(images::supernatural_image, sizeof(images::supernatural_image), &image_width, &image_height, NULL, 4);

	if (decodedData) {
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = image_width;
		textureDesc.Height = image_height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		ID3D11Texture2D* pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource = {};
		subResource.pSysMem = decodedData;
		subResource.SysMemPitch = image_width * 4;
		HRESULT hr = device->CreateTexture2D(&textureDesc, &subResource, &pTexture);
		if (SUCCEEDED(hr)) {
			device->CreateShaderResourceView(pTexture, nullptr, (ID3D11ShaderResourceView**)&images::supernatural);
			pTexture->Release();
		}
		else {
			std::cout << hue::yellow << "(/) " << hue::white << "Couldnt load an image" << std::endl;
		}
		
		stbi_image_free(decodedData);
	}
	else {
		std::cout << hue::yellow << "(/) " << hue::white << "Couldnt load an image" << std::endl;
	}

	return true;
}

bool UpdateWindow(void (*mainfunc)()) {

	auto start = std::chrono::high_resolution_clock::now();

	MSG msg;
	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			return false;
		}
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	mainfunc();

	ImGui::Render();

	float color[4]{ 0.f,0.f, 0.f, 0.f }; //color of background. last param is opacity. i have it transparent rn so u can see the console but we will set to 1 when we wanna use it
	
	if (settings::config::Fuser)
		color[3] = 1.f;

	device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
	device_context->ClearRenderTargetView(render_target_view, color);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swap_chain->Present(settings::config::vSync ? 1:0, 0);

	//D3D11_QUERY_DESC queryDesc = { D3D11_QUERY_EVENT, 0 };
	//ID3D11Query* frameQuery;
	//device->CreateQuery(&queryDesc, &frameQuery);

	//// Insert a fence-like query at the end of the frame
	//device_context->End(frameQuery);

	//// Wait for the GPU to signal the query has been completed
	//while (device_context->GetData(frameQuery, NULL, 0, 0) != S_OK) {
	//	// Wait (this ensures the CPU doesn't proceed prematurely)
	//}

	//frameQuery->Release();

	__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::mainThreadData.addValue(static_cast<float>(elapsed));

	return true;
}

bool DestroyWindow() {

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (swap_chain) {
		swap_chain->Release();
	}
	if (device_context) {
		device_context->Release();
	}
	if (device) {
		device->Release();
	}
	if (render_target_view) {
		render_target_view->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return true;
}