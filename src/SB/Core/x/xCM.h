#ifndef XCM_H
#define XCM_H

#include "xColor.h"

#include <rwcore.h>

struct xBase;
struct xCreditsData;

void xCMupdate(F32 dt);
void xCMrender();
void xCMstart(xCreditsData* data, F32, xBase* parent);
void xCMstop();

#endif
