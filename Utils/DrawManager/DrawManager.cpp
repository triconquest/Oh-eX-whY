#include "DrawManager.h"
#include "../../Globals/Globals.h"
#include "../../Utils/Render/Render.h"

#include <vector>

void DrawManager::DrawFilledRect(float x, float y, float w, float h, D3DCOLOR color)
{
	D3DRECT rect = { x, y, x + w, y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawManager::DrawLine(int fromx, int fromy, int tox, int toy, int thickness, D3DCOLOR color)
{
	if (!Draw.LineL)
		D3DXCreateLine(pDevice, &Draw.LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(fromx, fromy);
	Line[1] = D3DXVECTOR2(tox, toy);
	Draw.LineL->SetWidth(thickness);
	Draw.LineL->Draw(Line, 2, color);
}

void DrawManager::DrawLine(Vector2 src, Vector2 dst, int thickness, D3DCOLOR color)
{
	DrawLine(src.x, src.y, dst.x, dst.y, thickness, color);
}

void DrawManager::DrawLine(const Vector3& from, const Vector3& to, float thickness, D3DCOLOR color)
{
	Vector2 screenfrom, screento;

	VMatrix vmatrix = I::EngineClient->WorldToScreenMatrix();

	if (!Render::WorldToScreen(from, screenfrom, vmatrix, I::iScreenWidth, I::iScreenHeight) ||
		!Render::WorldToScreen(to, screento, vmatrix, I::iScreenWidth, I::iScreenHeight))
		return;

	DrawLine(static_cast<int>(screenfrom.x), static_cast<int>(screenfrom.y),
		static_cast<int>(screento.x), static_cast<int>(screento.y),
		static_cast<int>(thickness), color);
}

void DrawManager::Draw3DLine(const Vector3& from, const Vector3& to, D3DCOLOR color)
{
	Vertex vertices[2] =
	{
		{ D3DXVECTOR3(from.x, from.y, from.z), color },
		{ D3DXVECTOR3(to.x,   to.y,   to.z),   color }
	};

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertices, sizeof(Vertex));
}

void DrawManager::DrawBox(Vector2 top, Vector2 bot, int thickness, D3DCOLOR color)
{
    float height = ABS(top.y - bot.y);
    float halfWidth = height / 4;

    float centerX = top.x;

    Vector2 topLeft = { centerX - halfWidth, top.y };
    Vector2 topRight = { centerX + halfWidth, top.y };
    Vector2 bottomLeft = { centerX - halfWidth, bot.y };
    Vector2 bottomRight = { centerX + halfWidth, bot.y };

    DrawLine(topLeft, topRight, thickness, color);       
    DrawLine(bottomLeft, bottomRight, thickness, color); 
    DrawLine(topLeft, bottomLeft, thickness, color);
    DrawLine(topRight, bottomRight, thickness, color); 
}

void DrawManager::DrawCircle(float x, float y, float radius, D3DCOLOR color, int segments)
{
	std::vector<D3DXVECTOR2> points(segments + 1);

	float angleStep = D3DX_PI * 2.0f / segments;

	for (int i = 0; i <= segments; i++)
	{
		float angle = i * angleStep;
		points[i] = D3DXVECTOR2(x + cosf(angle) * radius, y + sinf(angle) * radius);
	}

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	LPD3DXLINE pLine;
	if (SUCCEEDED(D3DXCreateLine(pDevice, &pLine))) {
		pLine->SetWidth(1.0f);
		pLine->SetAntialias(TRUE);
		pLine->Begin();
		pLine->Draw(&points[0], points.size(), color);
		pLine->End();
		pLine->Release();
	}
}

void DrawManager::DrawScreenText(const char* text, float x, float y, FontFlags fontFlag, D3DCOLOR color)
{
	if (!pFont)
		return;

	RECT rect;
	SetRect(&rect, x, y, x + 200, y + 20);

	switch (fontFlag)
	{
	case FONT_SMALL:
		pFontSmall->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, color);
		break;

	case FONT_MEDIUM:
		pFontMedium->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, color);
		break;

	case FONT_LARGE:
		pFont->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, color);
		break;
	}
}

void DrawManager::DrawOutlineScreenText(const char* text, float x, float y, FontFlags fontFlag, D3DCOLOR color)
{
	Draw.DrawScreenText(text, x - 1, y, fontFlag, D3DCOLOR_ARGB(160, 0, 0, 0));
	Draw.DrawScreenText(text, x + 1, y, fontFlag, D3DCOLOR_ARGB(160, 0, 0, 0));
	Draw.DrawScreenText(text, x, y - 1, fontFlag, D3DCOLOR_ARGB(160, 0, 0, 0));
	Draw.DrawScreenText(text, x, y + 1, fontFlag, D3DCOLOR_ARGB(160, 0, 0, 0));

	Draw.DrawScreenText(text, x, y, fontFlag, color);
}

void DrawManager::DrawCrosshairOnPosition(float x, float y)
{

	D3DCOLOR d3Color = FloatToD3D(G::fRecoilCrosshairColor);

	D3DCOLOR outline = D3DCOLOR_ARGB(255, 0, 0, 0);

	float lineWidth = 1.0f;
	float size = 10.f;
	float outlineThickness = 1.0f;

	Draw.DrawFilledRect(x - size - outlineThickness, y - (lineWidth / 2) - outlineThickness,
		(size * 2) + (outlineThickness * 2), lineWidth + (outlineThickness * 2), outline);

	Draw.DrawFilledRect(x - (lineWidth / 2) - outlineThickness, y - size - outlineThickness,
		lineWidth + (outlineThickness * 2), (size * 2) + (outlineThickness * 2), outline);

	Draw.DrawFilledRect(x - size, y - (lineWidth / 2),
		size * 2, lineWidth, d3Color);

	// Draw main crosshair (vertical)
	Draw.DrawFilledRect(x - (lineWidth / 2), y - size,
		lineWidth, size * 2, d3Color);
}

void DrawManager::DrawCornerBox(Vector2 feet, Vector2 head, int thickness, D3DCOLOR color)
{
	float height = feet.y - head.y;
	float width = height / 2.0f;

	float x = head.x - width / 2.0f;
	float y = head.y;

	float lineW = width / 3.5f;
	float lineH = height / 4.0f;

	DrawLine({ x, y }, { x + lineW, y }, thickness, color);          
	DrawLine({ x, y }, { x, y + lineH }, thickness, color);          

	// Top right
	DrawLine({ x + width - lineW, y }, { x + width, y }, thickness, color);  
	DrawLine({ x + width, y }, { x + width, y + lineH }, thickness, color);   

	// Bottom left
	DrawLine({ x, y + height - lineH }, { x, y + height }, thickness, color);
	DrawLine({ x, y + height }, { x + lineW, y + height }, thickness, color);

	// Bottom right
	DrawLine({ x + width - lineW, y + height }, { x + width, y + height }, thickness, color);
	DrawLine({ x + width, y + height - lineH }, { x + width, y + height }, thickness, color);
}

void DrawManager::DrawFilledBackground(const char* text, int x, int y, D3DCOLOR color)
{
	constexpr int paddingX = 4;
	constexpr int paddingY = 2;

	RECT rect = { 0, 0, 0, 0 };
	pFontMedium->DrawTextA(nullptr, text, -1, &rect, DT_CALCRECT, 0);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	int bX = x - paddingX;
	int bY = y - paddingY;
	int boxW = width + (paddingX * 2);
	int boxH = height + (paddingY * 2);

	struct Vertex2
	{
		float x, y, z, rhw;
		D3DCOLOR color;
	};

	Vertex2 vertices[4] = {
		{ static_cast<float>(bX),          static_cast<float>(bY),          0.0f, 1.0f, color },
		{ static_cast<float>(bX + boxW),   static_cast<float>(bY),          0.0f, 1.0f, color },
		{ static_cast<float>(bX),          static_cast<float>(bY + boxH),   0.0f, 1.0f, color },
		{ static_cast<float>(bX + boxW),   static_cast<float>(bY + boxH),   0.0f, 1.0f, color },
	};

	DWORD oldFVF;
	pDevice->GetFVF(&oldFVF);

	pDevice->SetTexture(0, nullptr);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(Vertex2));

	pDevice->SetFVF(oldFVF);
}

D3DCOLOR DrawManager::FloatToD3D(float color[3])
{
	return D3DCOLOR_ARGB(255, (int)(color[0] * 255), (int)(color[1] * 255), (int)(color[2] * 255));
}

SIZE DrawManager::GetTextSize(const char* text)
{
	RECT rect = { 0, 0, 0, 0 };
	pFont->DrawTextA(nullptr, text, -1, &rect, DT_CALCRECT, 0);

	SIZE size;
	size.cx = rect.right - rect.left;
	size.cy = rect.bottom - rect.top;
	return size;
}

void DrawManager::InitFonts()
{
	if (!pFont)
	{
		D3DXCreateFont(pDevice, 24, 0, FW_NORMAL, 1, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &pFont);
	}

	if (!pFontSmall)
	{
		D3DXCreateFont(pDevice, 12, 0, FW_NORMAL, 1, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &pFontSmall);
	}

	if (!pFontMedium)
	{
		D3DXCreateFont(pDevice, 18, 0, FW_NORMAL, 1, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &pFontMedium);
	}
}

void DrawManager::Destroy()
{
	if (pFont)
	{
		pFont->Release();
		pFont = nullptr;
	}

	if (pCircleLine)
	{
		pCircleLine->Release();
		pCircleLine = nullptr;
	}
}
