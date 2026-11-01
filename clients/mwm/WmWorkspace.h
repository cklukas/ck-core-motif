#ifndef WM_WORKSPACE_H
#define WM_WORKSPACE_H
#include "WmWsmLib/wsm_proto.h"
extern void HandleWsmConvertRequest(Widget, XtPointer, WSMRequest *, WSMReply *);
extern void WsmConnectExisting(WmScreenData *);
extern void WsmRegisterClient(ClientData *);
extern Boolean WsmClientHidden(ClientData *);
extern void WsmApplyVisibility(ClientData *);
#endif
