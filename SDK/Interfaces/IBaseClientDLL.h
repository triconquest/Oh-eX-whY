#pragma once

#include "../../Utils/vector.h"
#include "Entities/IClientEntity.h"
#include "../Includes/const.h"
#include "../Includes/checksum_crc.h"
#include "GlobalVars_Base.h"

#include <Windows.h>
#include <cstdint>

// virtual int	Init(CreateInterfaceFn appSystemFactory,CreateInterfaceFn physicsFactory, CGlobalVarsBase* pGlobals) = 0;

typedef struct player_info_s
{
	char			name[MAX_PLAYER_NAME_LENGTH];
	int				userID;
	char			guid[SIGNED_GUID_LEN + 1];
	uint32_t		friendsID;
	char			friendsName[MAX_PLAYER_NAME_LENGTH];
	bool			fakeplayer;
	bool			ishltv;
	bool			isreplay;
	CRC32_t			customFiles[MAX_CUSTOM_FILES];
	unsigned char	filesDownloaded;
} player_info_t;

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

class IBaseClientDLL
{
public:
	virtual int				Init(void* appSystemFactory, void* physicsFactory, CGlobalVarsBase* pGlobals) = 0;
	virtual void			PostInit() = 0;
	virtual void			Shutdown(void) = 0;
	virtual bool			ReplayInit(void* replayFactory) = 0;
	virtual bool			ReplayPostInit() = 0;
	virtual void			LevelInitPreEntity(char const* pMapName) = 0;
	virtual void			LevelInitPostEntity() = 0;
	virtual void			LevelShutdown(void) = 0;
	virtual ClientClass* GetAllClasses(void) = 0;
	virtual int				HudVidInit(void) = 0;
	virtual void			HudProcessInput(bool bActive) = 0; // 10
	virtual void			HudUpdate(bool bActive) = 0;
	virtual void			HudReset(void) = 0;
	virtual void			HudText(const char* message) = 0;
	virtual void			IN_ActivateMouse(void) = 0;
	virtual void			IN_DeactivateMouse(void) = 0;
	virtual void			IN_Accumulate(void) = 0;
	virtual void			IN_ClearStates(void) = 0;
	virtual bool			IN_IsKeyDown(const char* name, bool& isdown) = 0;
	virtual void			IN_OnMouseWheeled(int nDelta) = 0;
	virtual int				IN_KeyEvent(int eventcode, void* keynum, const char* pszCurrentBinding) = 0;
	virtual void			CreateMove(int sequence_number, float input_sample_frametime, bool active) = 0;
	virtual void			ExtraMouseSample(float frametime, bool active) = 0;
	virtual bool			WriteUsercmdDeltaToBuffer(void* buf, int from, int to, bool isnewcommand) = 0;
	virtual void			EncodeUserCmdToBuffer(int& buf, int slot) = 0;
	virtual void			DecodeUserCmdFromBuffer(int& buf, int slot) = 0;
	virtual void			View_Render(void* rect) = 0;
	virtual void			RenderView(const Vector2& view, int nClearFlags, int whatToDraw) = 0;
	virtual void			View_Fade(void* pSF) = 0;
	virtual void			SetCrosshairAngle(const Vector2& angle) = 0;
	virtual void			InitSprite(void* pSprite, const char* loadname) = 0;
	virtual void			ShutdownSprite(void* pSprite) = 0;
	virtual int				GetSpriteSize(void) const = 0;
	virtual void			VoiceStatus(int entindex, bool bTalking) = 0;
	virtual void			InstallStringTableCallback(char const* tableName) = 0;
	virtual void			FrameStageNotify(ClientFrameStage_t curStage) = 0; // 35
	virtual bool			DispatchUserMessage(int msg_type, void* msg_data) = 0;
	virtual int*			SaveInit(int size) = 0;
	virtual void			SaveWriteFields(void*, const char*, void*, void*, void*, int) = 0;
	virtual void			SaveReadFields(void*, const char*, void*, void*, void*, int) = 0;
	virtual void			PreSave(void*) = 0;
	virtual void			Save(void*) = 0;
	virtual void			WriteSaveHeaders(void*) = 0;
	virtual void			ReadRestoreHeaders(void*) = 0;
	virtual void			Restore(void*, bool) = 0;
	virtual void			DispatchOnRestore() = 0;
	virtual void*			GetStandardRecvProxies() = 0;
	virtual void			WriteSaveGameScreenshot(const char* pFilename) = 0;
	virtual void			EmitSentenceCloseCaption(char const* tokenstream) = 0;
	virtual void			EmitCloseCaption(char const* captionname, float duration) = 0;
	virtual bool			CanRecordDemo(char* errorMsg, int length) const = 0;
	virtual void			OnDemoRecordStart(char const* pDemoBaseName) = 0;
	virtual void			OnDemoRecordStop() = 0;
	virtual void			OnDemoPlaybackStart(char const* pDemoBaseName) = 0;
	virtual void			OnDemoPlaybackStop() = 0;
	virtual bool			ShouldDrawDropdownConsole() = 0;
	virtual int				GetScreenWidth() = 0;
	virtual int				GetScreenHeight() = 0;
	virtual void			WriteSaveGameScreenshotOfSize(const char* pFilename, int width, int height, bool bCreatePowerOf2Padded = false, bool bWriteVTF = false) = 0;
	virtual bool			GetPlayerView(Vector2& playerView) = 0;
	virtual void			SetupGameProperties(int contexts, Vector2 properties) = 0;
	virtual unsigned int	GetPresenceID(const char* pIDName) = 0;
	virtual const char*		GetPropertyIdString(const unsigned int id) = 0;
	virtual void			GetPropertyDisplayString(unsigned int id, unsigned int value, char* pOutput, int nBytes) = 0;
	virtual void			StartStatsReporting(HANDLE handle, bool bArbitrated) = 0;
	virtual void			InvalidateMdlCache() = 0;
	virtual void			IN_SetSampleTime(float frametime) = 0;
	virtual void			ReloadFilesInList(void* pFilesToReload) = 0;
	virtual bool			HandleUiToggle() = 0;
	virtual bool			ShouldAllowConsole() = 0;
	virtual void*			GetRenamedRecvTableInfos() = 0;
	virtual void*			GetClientUIMouthInfo() = 0;
	virtual void			FileReceived(const char* fileName, unsigned int transferID) = 0;
	virtual const char*		TranslateEffectForVisionFilter(const char* pchEffectType, const char* pchEffectName) = 0;
	virtual void			ClientAdjustStartSoundParams(struct StartSoundParams_t& params) = 0;
	virtual bool			DisconnectAttempt(void) = 0;
	virtual bool			IsConnectedUserInfoChangeAllowed(void* pCvar) = 0;
};