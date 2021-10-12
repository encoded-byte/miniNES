#include <algorithm>
#include <cstring>
#include <iterator>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <system_error>
#include <windows.h>
#include <d2d1.h>
#include <shlobj.h>
#include <xaudio2.h>
#include <xinput.h>
#include "machine/Machine.h"


// Constants
constexpr int WND_WIDTH = 800;
constexpr int WND_HEIGHT = 600;
constexpr int VID_WIDTH = 256;
constexpr int VID_HEIGHT = 240;
constexpr int VID_PIXEL_SIZE = 4;
constexpr int VID_PITCH = VID_WIDTH * VID_PIXEL_SIZE;
constexpr int SND_BITRATE = 48000;
constexpr int SND_SAMPLE_SIZE = 2;
constexpr int SND_FRAME_SIZE = SND_BITRATE * SND_SAMPLE_SIZE / 60;
constexpr int SND_BUFFERS = 8;

// Menu
enum Menu : int
{
	FILE_LOAD = 100,
	FILE_UNLOAD,
	FILE_QUIT,
	SYSTEM_PAUSE,
	SYSTEM_RESET,
	HELP_ABOUT,
};

// Main
HWND hWnd = NULL;
Machine machine;
bool machinePaused = 0;

// Video
ID2D1Factory* pFactory = NULL;
ID2D1HwndRenderTarget* pRenderTarget = NULL;
ID2D1Bitmap* pBitmap = NULL;

// Audio
IXAudio2* pAudio = NULL;
IXAudio2MasteringVoice* pMasterVoice = NULL;
IXAudio2SourceVoice* pSourceVoice = NULL;
BYTE audioBuffer[SND_BUFFERS][SND_FRAME_SIZE] = { 0 };
int currentBuffer = 0;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowErrorDialog(std::runtime_error& err);


void CreateMainWindow()
{
	WNDCLASS wc = { 0 };
	wc.lpszClassName = L"miniNES Window";
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	HMENU hMenu, hTopMenu = CreateMenu();
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, Menu::FILE_LOAD, L"&Load");
	AppendMenu(hMenu, MF_GRAYED, Menu::FILE_UNLOAD, L"&Unload");
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, Menu::FILE_QUIT, L"&Quit");
	AppendMenu(hTopMenu, MF_POPUP, (UINT_PTR)hMenu, L"&File");
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_GRAYED, Menu::SYSTEM_PAUSE, L"&Pause");
	AppendMenu(hMenu, MF_GRAYED, Menu::SYSTEM_RESET, L"&Reset");
	AppendMenu(hTopMenu, MF_POPUP, (UINT_PTR)hMenu, L"&System");
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, Menu::HELP_ABOUT, L"&About");
	AppendMenu(hTopMenu, MF_POPUP, (UINT_PTR)hMenu, L"&Help");

	hWnd = CreateWindowEx(
		0, wc.lpszClassName, L"miniNES", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT,
		NULL, hTopMenu, wc.hInstance, NULL);

	if (!hWnd) throw std::system_error(std::error_code());
}

void CreateGraphicsResources()
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
	if (FAILED(hr)) throw std::system_error(std::error_code());

	RECT rc;
	GetClientRect(hWnd, &rc);
	hr = pFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right, rc.bottom)),
		&pRenderTarget);
	if (FAILED(hr)) throw std::system_error(std::error_code());

	hr = pRenderTarget->CreateBitmap(
		D2D1::SizeU(VID_WIDTH, VID_HEIGHT),
		NULL,
		0,
		D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),
		&pBitmap);
	if (FAILED(hr)) throw std::system_error(std::error_code());
}

void CreateSoundResources()
{
	HRESULT hr = XAudio2Create(&pAudio);
	if (FAILED(hr)) throw std::system_error(std::error_code());

	hr = pAudio->CreateMasteringVoice(&pMasterVoice);
	if (FAILED(hr)) throw std::system_error(std::error_code());

	WAVEFORMATEX wf = { 0 };
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 1;
	wf.nSamplesPerSec = SND_BITRATE;
	wf.nAvgBytesPerSec = SND_BITRATE * SND_SAMPLE_SIZE;
	wf.nBlockAlign = SND_SAMPLE_SIZE;
	wf.wBitsPerSample = SND_SAMPLE_SIZE * 8;
	hr = pAudio->CreateSourceVoice(&pSourceVoice, &wf);
	if (FAILED(hr)) throw std::system_error(std::error_code());

	hr = pSourceVoice->Start();
	if (FAILED(hr)) throw std::system_error(std::error_code());
}

void DestroyResources()
{
	if (pBitmap) pBitmap->Release();
	if (pRenderTarget) pRenderTarget->Release();
	if (pFactory) pFactory->Release();
	if (pAudio) pAudio->Release();
}

void OnPaint()
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);
	pRenderTarget->BeginDraw();

	if (machine.is_ready())
	{
		D2D1_SIZE_F szTarget = pRenderTarget->GetSize();
		D2D1_RECT_F rcTarget = D2D1::RectF(0, 0, szTarget.width, szTarget.height);
		pRenderTarget->DrawBitmap(pBitmap, rcTarget);
	}
	else pRenderTarget->Clear();

	pRenderTarget->EndDraw();
	EndPaint(hWnd, &ps);
}

void OnLoad()
{
	IFileOpenDialog* pFileOpen = NULL;
	IShellItem* pItem = NULL;

	try
	{
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
		if (FAILED(hr)) throw hr;

		COMDLG_FILTERSPEC types[] = {{ L"NES files (*.nes;*.fds)", L"*.nes;*.fds" }};
		hr = pFileOpen->SetFileTypes(ARRAYSIZE(types), types);
		if (FAILED(hr)) throw hr;

		hr = pFileOpen->Show(NULL);
		if (FAILED(hr)) throw hr;

		hr = pFileOpen->GetResult(&pItem);
		if (FAILED(hr)) throw hr;

		PWSTR sPath;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &sPath);
		if (FAILED(hr)) throw hr;

		std::filesystem::path oPath = sPath;
		CoTaskMemFree(sPath);
		machine.load(oPath.string());
		machinePaused = 0;
		EnableMenuItem(GetMenu(hWnd), Menu::FILE_UNLOAD, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), Menu::SYSTEM_PAUSE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), Menu::SYSTEM_RESET, MF_ENABLED);
		CheckMenuItem(GetMenu(hWnd), Menu::SYSTEM_PAUSE, MF_UNCHECKED);
		SetWindowText(hWnd, std::wstring(L"miniNES - " + oPath.stem().wstring()).c_str());
	}
	catch (std::runtime_error& err)
	{
		ShowErrorDialog(err);
	}
	catch (HRESULT) {}

	if (pItem) pItem->Release();
	if (pFileOpen) pFileOpen->Release();
}

void OnUnload()
{
	machine.unload();
	machinePaused = 0;
	EnableMenuItem(GetMenu(hWnd), Menu::FILE_UNLOAD, MF_DISABLED);
	EnableMenuItem(GetMenu(hWnd), Menu::SYSTEM_PAUSE, MF_DISABLED);
	EnableMenuItem(GetMenu(hWnd), Menu::SYSTEM_RESET, MF_DISABLED);
	CheckMenuItem(GetMenu(hWnd), Menu::SYSTEM_PAUSE, MF_UNCHECKED);
	SetWindowText(hWnd, L"miniNES");
}

void OnPause()
{
	machinePaused = !machinePaused;
	CheckMenuItem(GetMenu(hWnd), Menu::SYSTEM_PAUSE, machinePaused ? MF_CHECKED : MF_UNCHECKED);
}

void OnReset()
{
	machine.reset();
	machinePaused = 0;
	CheckMenuItem(GetMenu(hWnd), Menu::SYSTEM_PAUSE, MF_UNCHECKED);
}

void OnAbout()
{
	MessageBox(
		hWnd,
		L"miniNES \u00a9 2021 encoded_byte\nA minimal Famicom/NES emulator",
		L"miniNES - About",
		MB_OK | MB_ICONINFORMATION);
}

void ShowErrorDialog(std::runtime_error& err)
{
	std::string text = err.what();
	std::wstring wtext;
	std::copy(text.begin(), text.end(), std::back_inserter(wtext));
	MessageBox(hWnd, wtext.c_str(), L"Error", MB_OK | MB_ICONERROR);
	OnUnload();
}

void EmulatorLoop()
{
	if (machine.is_ready() && !machinePaused)
	{
		for (int i = 0; i != 2; ++i)
		{
			BYTE input = 0;
			XINPUT_STATE state = { 0 };
			XInputGetState(i, &state);
			WORD& buttons = state.Gamepad.wButtons;
			input |= buttons & XINPUT_GAMEPAD_B ? 0x01 : 0;
			input |= buttons & XINPUT_GAMEPAD_A ? 0x02 : 0;
			input |= buttons & XINPUT_GAMEPAD_BACK ? 0x04 : 0;
			input |= buttons & XINPUT_GAMEPAD_START ? 0x08 : 0;
			input |= buttons & XINPUT_GAMEPAD_DPAD_UP ? 0x10 : 0;
			input |= buttons & XINPUT_GAMEPAD_DPAD_DOWN ? 0x20 : 0;
			input |= buttons & XINPUT_GAMEPAD_DPAD_LEFT ? 0x40 : 0;
			input |= buttons & XINPUT_GAMEPAD_DPAD_RIGHT ? 0x80 : 0;
			machine.set_input(i, input);
		}

		try
		{
			machine.frame();
		}
		catch (std::runtime_error& err)
		{
			ShowErrorDialog(err);
			return;
		}

		XAUDIO2_BUFFER buffer = { 0 };
		buffer.AudioBytes = SND_FRAME_SIZE;
		buffer.pAudioData = audioBuffer[currentBuffer];
		std::memcpy(audioBuffer[currentBuffer], machine.get_audio(), SND_FRAME_SIZE);
		currentBuffer = ++currentBuffer % SND_BUFFERS;
		pSourceVoice->SubmitSourceBuffer(&buffer);

		D2D1_RECT_U rcBitmap = D2D1::RectU(0, 0, VID_WIDTH, VID_HEIGHT);
		pBitmap->CopyFromMemory(&rcBitmap, machine.get_video(), VID_PITCH);
	}

	InvalidateRect(hWnd, NULL, FALSE);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case Menu::FILE_LOAD:
			OnLoad();
			return 0;
		case Menu::FILE_UNLOAD:
			OnUnload();
			return 0;
		case Menu::FILE_QUIT:
			DestroyWindow(hWnd);
			return 0;
		case Menu::SYSTEM_PAUSE:
			OnPause();
			return 0;
		case Menu::SYSTEM_RESET:
			OnReset();
			return 0;
		case Menu::HELP_ABOUT:
			OnAbout();
			return 0;
		}
		break;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) return hr;

	try
	{
		CreateMainWindow();
		CreateGraphicsResources();
		CreateSoundResources();
		ShowWindow(hWnd, nShowCmd);

		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else EmulatorLoop();
		}
	}
	catch (std::system_error) {}

	DestroyResources();
	CoUninitialize();

	return 0;
}
