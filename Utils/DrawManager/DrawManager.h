#pragma once

#include "../vector.h"

#include <d3d9.h>
#include <d3dx9.h>

struct Screen2D
{
	float fScreenHeight, fScreenWidth;
};

enum FontFlags
{
	FONT_SMALL,
	FONT_MEDIUM,
	FONT_LARGE,
};

struct Vertex
{
	D3DXVECTOR3 position;
	DWORD color;
};

class DrawManager
{
public:
	void DrawFilledRect(float x, float y, float w, float h, D3DCOLOR color);
	void DrawLine(int fromx, int fromy, int tox, int toy, int thickness, D3DCOLOR color);
	void DrawLine(Vector2 src, Vector2 dst, int thickness, D3DCOLOR color);
	void DrawLine(const Vector3& from, const Vector3& to, float thickness, D3DCOLOR color);
	void Draw3DLine(const Vector3& from, const Vector3& to, D3DCOLOR color);
	void DrawBox(Vector2 top, Vector2 bot, int thickness, D3DCOLOR color);
	void DrawCircle(float x, float y, float radius, D3DCOLOR color, int segments = 64);
	void DrawScreenText(const char* text, float x, float y, FontFlags fontFlag, D3DCOLOR color);
	void DrawOutlineScreenText(const char* text, float x, float y, FontFlags fontFlag, D3DCOLOR color);
	void DrawCrosshairOnPosition(float x, float y);
	void DrawCornerBox(Vector2 feet, Vector2 head, int thickness, D3DCOLOR color);
	void DrawFilledBackground(const char* text, int x, int y, D3DCOLOR color);
	D3DCOLOR FloatToD3D(float color[3]);

	inline float FOVToScreenRadius(float fovDegrees, float screenHeight, float cameraFOV = 90.0f)
	{
		return tanf(D3DXToRadian(fovDegrees) / 2.0f) / tanf(D3DXToRadian(cameraFOV) / 2.0f) * (screenHeight / 2.0f);
	}

	SIZE GetTextSize(const char* text);
	void GetDevice(IDirect3DDevice9* dev) { pDevice = dev; }
	void InitFonts();
	void Destroy();

private:
	IDirect3DDevice9* pDevice;
	ID3DXLine* LineL;
	ID3DXLine* pCircleLine = nullptr;
	ID3DXFont* pFont = nullptr;
	ID3DXFont* pFontSmall = nullptr;
	ID3DXFont* pFontMedium = nullptr;

	LPDIRECT3DVERTEXBUFFER9 g_pVB;
};

inline DrawManager Draw;