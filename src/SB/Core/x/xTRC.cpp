#include "xCommon.h"
#include "xTRC.h"

#include "xFont.h"
#include "xSnd.h"
#include "xutil.h"
#include "iCamera.h"
#include "zBusStop.h"
#include "zEntPlayerOOBState.h"
#include "zGame.h"
#include "zGameExtras.h"
#include "zGameState.h"
#include "zGlobals.h"
#include "zMenu.h"
#include "zVar.h"

#include <string.h>

TRCPadInfo gTrcPad[k_MAX_TRC_PADS];
TRCDiskInfo gTrcDisk;
static const iColor_tag yellow = { 0xFF, 0xE6, 0x00, 0xFF };

void xTRCInit()
{
    memset(gTrcPad, 0, sizeof(gTrcPad));
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++) {
        gTrcPad[i].id = i;
    }
    memset(&gTrcDisk, 0, sizeof(gTrcDisk));
}

// UNUSED: needed to match (code from BFBB PS2)
static U32 pad_message_valid()
{
    if (gGameMode == eGameMode_Boot || gGameMode == eGameMode_Intro) {
        return FALSE;
    }
    if (zMenuRunning()) {
        return FALSE;
    }
    if (zGameModeGet() == eGameMode_Save || zGameModeGet() == eGameMode_Load) {
        return TRUE;
    }
    if (gBusStopIsRunning) {
        globals.dontShowPadMessageDuringLoadingOrCutScene = 0;
        return FALSE;
    }
    if (zGameGetOstrich() != eGameOstrich_InScene) {
        return FALSE;
    }
    if (!oob_state::IsPlayerInControl()) {
        zGameStall();
        return TRUE;
    }
    if (globals.sceneCur->sceneID == IDTAG('P','G','1','2')) {
        zGameStall();
    }
    if (!globals.player.ControlOff) {
        return TRUE;
    }
    if (globals.player.ControlOff & CONTROL_OWNER_TALK_BOX) {
        return TRUE;
    }
    if (globals.sceneCur && globals.sceneCur->sceneID == IDTAG('M','N','U','3')) {
        return TRUE;
    }
    if (zGame_HackIsGallery()) {
        return TRUE;
    }
    if ((globals.player.ControlOff & CONTROL_OWNER_EVENT) && globals.sceneCur->sceneID == IDTAG('H','B','1','0')) {
        return TRUE;
    }
    if (gGameMode == eGameMode_Stall) {
        return TRUE;
    }
    globals.dontShowPadMessageDuringLoadingOrCutScene = 0;
    return FALSE;
}

static void render_message(const char* s)
{
    static xtextbox tb = xtextbox::create(xfont::create(1, NSCREENX(19.0f), NSCREENY(22.0f), 0.0f, yellow, screen_bounds), screen_bounds, 2, 0.0f, 0.0f, 0.0f, 0.0f);
    tb.set_text(s);
    tb.bounds = screen_bounds;
    tb.bounds.contract(0.1f);
    tb.bounds.h = tb.yextent(true);
    tb.bounds.y = 0.5f - 0.5f * tb.bounds.h;
    render_fill_rect(tb.font.clip, xColorFromRGBA(0x00, 0x00, 0x00, 0xC8));
    tb.render(true);
}

static const char* message_text;
static U32 standalone;

void xTRCRender()
{
    if (message_text)
        render_message(message_text);
}

void xTRCReset()
{
    message_text = NULL;
    globals.dontShowPadMessageDuringLoadingOrCutScene = 0;
    if (gGameMode == eGameMode_Stall) {
        zGameModeSwitch(eGameMode_Game);
        xSndResume();
    }
}

// UNUSED: needed to match (code from BFBB PS2)
static S32 DisplayMessage(TRCState state)
{
    iColor_tag clear = { 0, 0, 0, 0 };
    switch (state) {
    case TRC_Unknown:
        message_text = NULL;
        break;
    case TRC_PadMissing:
        if (!pad_message_valid()) {
            return FALSE;
        }
        message_text = "The Controller in Controller Socket 1 has been removed. Reconnect the Controller to continue with the game.";
        break;
    case TRC_PadInvalidType:
        message_text = "The Controller in Controller Socket 1 is an unknown type. Connect a standard Controller to continue with the game.";
        break;
    case TRC_DiskNotIdentified:
        message_text = "Please wait... Checking for the 'SpongeBob SquarePants: Battle for Bikini Bottom' Game Disc.";
        break;
    case TRC_DiskTrayOpen:
        message_text = "The Disc Cover is open. If you want to continue the game, please close the Disc Cover.";
        break;
    case TRC_DiskNoDisk:
        message_text = "Please insert the 'SpongeBob SquarePants: Battle for Bikini Bottom' Game Disc.";
        break;
    case TRC_DiskInvalid:
        message_text = "This is not the 'SpongeBob SquarePants: Battle for Bikini Bottom' Game Disc. Please insert the 'SpongeBob SquarePants: Battle for Bikini Bottom' Game Disc.";
        break;
    case TRC_DiskRetry:
        message_text = "The Game Disc could not be read. Please read the Nintendo GameCube\x99 Instruction Booklet for more information.";
        break;
    case TRC_DiskFatal:
        message_text = "An error has occurred. Turn the power off and refer to the Nintendo GameCube\x99 Instruction Booklet for further instructions.";
        break;
    default:
        return FALSE;
    }
    if (globals.cmgr && globals.cmgr->csn->Time > 1.0f) {
        globals.cmgr->stop = 1;
    }
    if (standalone || RwCameraGetCurrentCamera()) {
        RwCamera* cam = NULL;
        if (standalone) {
            cam = iCameraCreate(640, 480, FALSE);
            RwRGBA bg = { 0, 0, 0, 0 };
            RwCameraClear(cam, &bg, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
            RwCameraBeginUpdate(cam);
        }
        render_message(message_text);
        if (standalone) {
            RwCameraEndUpdate(cam);
            RwCameraShowRaster(cam, NULL, rwRASTERFLIPWAITVSYNC);
            iCameraDestroy(cam);
        }
    }
    return TRUE;
}

void xTRCPad(S32 pad_id, TRCState state)
{
}

void xTRCDisk(TRCState state)
{
    if (state != TRC_DiskNotIdentified) {
        gTrcDisk.state = state;
        gTrcDisk.id = TRC_DiskIdentified;
    } else {
        gTrcDisk.id = TRC_DiskNotIdentified;
    }
}

void render_mem_card_no_space(S32 needed, S32 available, S32 neededFiles, bool enabled)
{
    if (available < 0 && neededFiles != -1 && needed != -1) {
        available = 0;
    }
    bad_card_needed = needed;
    bad_card_available = available;
    const char* error_text = "{i:text_mem_card_no_space}";
    if (neededFiles == 0 && needed > available) {
        error_text = "{i:text_mem_card_no_space_overwrite}";
    }
    else if ((neededFiles > 0 && needed > available) || neededFiles == -1 || needed > available) {
        error_text = "{i:text_mem_card_no_space_no_save}";
    }
    RenderText(error_text, enabled);
}

void RenderText(const char* text, bool enabled)
{
    static xtextbox tb = xtextbox::create(xfont::create(1, NSCREENX(19.0f), NSCREENY(22.0f), 0.0f, xColorFromRGBA(0xFF, 0xE6, 0x00, 0xFF), screen_bounds), screen_bounds, 2, 0.0f, 0.0f, 0.0f, 0.0f);
    tb.set_text(enabled ? text : "");
    tb.bounds = screen_bounds;
    tb.bounds.contract(0.1f);
    tb.bounds.h = tb.yextent(true);
    tb.bounds.y = 0.5f - 0.5f * tb.bounds.h;
    tb.render(true);
    if (!enabled)
        render_fill_rect(tb.font.clip, xColorFromRGBA(0x00, 0x00, 0x00, 0x96));
}
