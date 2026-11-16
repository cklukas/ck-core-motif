/*
 * Fuzz target: the MWM workspace wire protocol (clients/mwm/WmWsmLib) and
 * .mwmrc configuration token, event, context, command and accelerator parsers
 * (clients/mwm/WmResParse.c).
 *
 * clients/mwm/WmWsmLib/{wire,pack,free,util}.c are compiled unchanged and
 * linked; the parser functions of clients/mwm/WmResParse.c are extracted into
 * production.h by run-fuzz.py.
 *
 * Each input exercises both:
 *   1. WSM wire message validation, request/reply unpacking, re-packing and
 *      deallocation across all 11 WSMRequestType protocol messages in a heap
 *      buffer of the exact wire length;
 *   2. .mwmrc line/token parsing (GetNextLine, GetString, ParseBtnEvent,
 *      ParseModifiers, ParseKeySym, ParseContext, ParseWmFuncGrpArg,
 *      ParseWmFuncNbrArg, ParseWmFuncStrArg, ParseWmFuncMaybeStrArg,
 *      ParseWmFuncCCIArgs, FixMenuItem, ParseClientCommand, ProcessAccelText).
 */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>

#define NO_SHAPE
#define NO_MESSAGE_CATALOG
#include "WmGlobal.h"
#include "wsm_proto.h"

/* Stubs for WmResParse.c error reporting and globals */
WmGlobalData wmGD;

void PWarning(char *message)
{
    if (message) (void) strlen(message);
}

static Boolean F_Beep(String args, ClientData *pCD, XEvent *event)
{
    (void) args; (void) pCD; (void) event;
    return True;
}
#define F_Exec F_Beep
#define F_Menu F_Beep
#define F_Lower F_Beep
#define F_Raise F_Beep
#define F_Raise_Lower F_Beep
#define F_Screen F_Beep
#define F_Nop F_Beep

/* Forward declarations needed by extracted WmResParse.c functions */
static void StoreExclusion(MenuSpec *menuSpec, String string);
Boolean IsClientCommand(String string);
static Boolean ParseClientCommand(unsigned char **linePP, MenuSpec *menuSpec,
                                  MenuItem *menuItem, unsigned char *string,
                                  Boolean *use_separators);
static void FixMenuItem(MenuSpec *menuSpec, MenuItem *menuItem);
unsigned char *GetNextLine(void);
unsigned char *GetString(unsigned char **linePP);
Boolean ParseBtnEvent(unsigned char **linePP, unsigned int *eventType,
                      unsigned int *button, unsigned int *state, Boolean *fClick);
static Boolean ParseContext(unsigned char **linePP, Context *context, Context *subContext);
static Boolean ParseModifiers(unsigned char **linePP, unsigned int *state);
static Boolean LookupModifier(unsigned char *name, unsigned int *valueP);
static Boolean ParseImmed(unsigned char **linePP, unsigned int closure, unsigned int *detail);
static Boolean ParseKeySym(unsigned char **linePP, unsigned int closure, unsigned int *detail);
static Boolean StrToNum(unsigned char *str, unsigned int *value);
static Boolean StrToHex(unsigned char *str, unsigned int *value);
static Boolean StrToOct(unsigned char *str, unsigned int *value);
void ScanAlphanumeric(unsigned char **linePP);
void ScanWhitespace(unsigned char **linePP);
void ToLower(unsigned char *string);

#include FUZZ_TYPES_H
static Boolean GetCCIModifier(String modString, CCIEntryModifier *mod);
static Boolean ParseEvent(unsigned char **linePP, EventTableEntry *table,
                          unsigned int *eventType, unsigned int *detail,
                          unsigned int *state, Boolean *fClick);
static Boolean ParseEventType(unsigned char **linePP, EventTableEntry *table,
                              unsigned int *eventType, Cardinal *ix);
#define fprintf(...) ((void) 0)
#include FUZZ_PRODUCTION_H
#undef fprintf

/* Stand-ins for WmWsmLib screen/display format lookup */
static WSMAttribute default_window_attrs[4];
static WSMAttribute default_global_attrs[2];
static WSMAttribute default_icon_attrs[2];
static WSMScreenInfo fuzz_scr_info;
static WSMDispInfo fuzz_disp_info;

static void reset_default_formats(void)
{
    default_global_attrs[0].nameq = XrmStringToQuark("ws_count");
    default_global_attrs[0].size = 16;
    default_global_attrs[0].is_list = False;
    default_global_attrs[1].nameq = XrmStringToQuark("ws_names");
    default_global_attrs[1].size = 8;
    default_global_attrs[1].is_list = True;

    default_window_attrs[0].nameq = XrmStringToQuark("workspaces");
    default_window_attrs[0].size = 32;
    default_window_attrs[0].is_list = False;
    default_window_attrs[1].nameq = XrmStringToQuark("state");
    default_window_attrs[1].size = 16;
    default_window_attrs[1].is_list = True;
    default_window_attrs[2].nameq = XrmStringToQuark("tag");
    default_window_attrs[2].size = 8;
    default_window_attrs[2].is_list = True;
    default_window_attrs[3].nameq = XrmStringToQuark("flags");
    default_window_attrs[3].size = 32;
    default_window_attrs[3].is_list = True;

    default_icon_attrs[0].nameq = XrmStringToQuark("icon_x");
    default_icon_attrs[0].size = 16;
    default_icon_attrs[0].is_list = False;
    default_icon_attrs[1].nameq = XrmStringToQuark("icon_y");
    default_icon_attrs[1].size = 8;
    default_icon_attrs[1].is_list = False;
}

void _WSMClearConfigScreenInfo(Display *dpy, int screen_num)
{
    (void) dpy; (void) screen_num;
    if (fuzz_scr_info.global.attr_list &&
        fuzz_scr_info.global.attr_list != default_global_attrs)
        XtFree((char *) fuzz_scr_info.global.attr_list);
    if (fuzz_scr_info.window.attr_list &&
        fuzz_scr_info.window.attr_list != default_window_attrs)
        XtFree((char *) fuzz_scr_info.window.attr_list);
    if (fuzz_scr_info.icon.attr_list &&
        fuzz_scr_info.icon.attr_list != default_icon_attrs)
        XtFree((char *) fuzz_scr_info.icon.attr_list);
    fuzz_scr_info.global.attr_list = NULL;
    fuzz_scr_info.global.num_attrs = 0;
    fuzz_scr_info.window.attr_list = NULL;
    fuzz_scr_info.window.num_attrs = 0;
    fuzz_scr_info.icon.attr_list = NULL;
    fuzz_scr_info.icon.num_attrs = 0;
}

static void install_default_formats(void)
{
    _WSMClearConfigScreenInfo(NULL, 0);
    fuzz_scr_info.global.attr_list = default_global_attrs;
    fuzz_scr_info.global.num_attrs = 2;
    fuzz_scr_info.window.attr_list = default_window_attrs;
    fuzz_scr_info.window.num_attrs = 4;
    fuzz_scr_info.icon.attr_list = default_icon_attrs;
    fuzz_scr_info.icon.num_attrs = 2;
}

WSMScreenInfo *_WSMGetScreenInfo(Display *dpy, int screen_num)
{
    (void) dpy; (void) screen_num;
    return &fuzz_scr_info;
}

WSMConfigFormatData *_WSMGetConfigFormat(Display *dpy, int screen_num, WSMConfigFormatType type)
{
    (void) dpy; (void) screen_num;
    switch (type) {
    case WSM_GLOBAL_FMT: return &fuzz_scr_info.global;
    case WSM_WINDOW_FMT: return &fuzz_scr_info.window;
    case WSM_ICON_FMT:   return &fuzz_scr_info.icon;
    default:             return NULL;
    }
}

WSMDispInfo *_WSMGetDispInfo(Display *dpy)
{
    (void) dpy;
    return &fuzz_disp_info;
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void) argc; (void) argv;
    setlocale(LC_ALL, "C");
    reset_default_formats();
    install_default_formats();
    return 0;
}

static void free_exclusions(MenuSpec *spec)
{
    MenuExclusion *ex = spec->exclusions;
    while (ex) {
        MenuExclusion *next = ex->nextExclusion;
        XtFree(ex->command_string);
        XtFree((char *) ex);
        ex = next;
    }
    spec->exclusions = NULL;
}

static void fuzz_wsm_wire(const uint8_t *data, size_t size)
{
    if (size < 1) return;
    Display *fake_dpy = (Display *) &fuzz_disp_info;
    WSMRequestType type = (WSMRequestType) (data[0] % 11);
    size_t msg_len = size - 1;
    unsigned char *msg = malloc(msg_len ? msg_len : 1);
    if (!msg) abort();
    if (msg_len) memcpy(msg, data + 1, msg_len);

    install_default_formats();
    (void) _WSMReqTypeToName(type);
    (void) _WSMRequiresConfigFormat(type);
    (void) _WSMValidateMessage(fake_dpy, 0, (MessageData) msg, msg_len, type, False);
    (void) _WSMValidateMessage(fake_dpy, 0, (MessageData) msg, msg_len, type, True);

    WSMRequest req;
    memset(&req, 0, sizeof(req));
    _WSMUnpackRequest(fake_dpy, 0, (MessageData) msg, msg_len, type, &req);
    if (req.any.type != WSM_UNKNOWN) {
        if (type == WSM_CONNECT || type == WSM_CONFIG_FMT)
            _WSMClearConfigScreenInfo(fake_dpy, 0);
        unsigned long packed_len = 0;
        WSMErrorCode err = WSM_SUCCESS;
        MessageData packed = _WSMPackRequest(fake_dpy, 0, &req, &packed_len, &err);
        if (packed) {
            if (!_WSMValidateMessage(fake_dpy, 0, packed, packed_len, type, False))
                abort();
            XtFree((char *) packed);
        }
        FreeRequest(&req);
    }

    install_default_formats();
    WSMReply rep;
    memset(&rep, 0, sizeof(rep));
    _WSMUnpackReply(fake_dpy, 0, (MessageData) msg, msg_len, type, &rep);
    if (rep.any.type != WSM_UNKNOWN) {
        unsigned long packed_len = 0;
        MessageData packed = _WSMPackReply(fake_dpy, 0, &rep, &packed_len);
        if (packed) {
            if (!_WSMValidateMessage(fake_dpy, 0, packed, packed_len, type, True))
                abort();
            XtFree((char *) packed);
        }
        FreeReply(&rep);
    }
    _WSMClearConfigScreenInfo(fake_dpy, 0);
    free(msg);
}

static void fuzz_mwmrc_parsers(const uint8_t *data, size_t size)
{
    unsigned char *buf = malloc(size + 1);
    if (!buf) abort();
    if (size) memcpy(buf, data, size);
    buf[size] = '\0';

    /* 1. Button event + context + function arguments */
    {
        unsigned char *p = buf;
        unsigned int eventType = 0, button = 0, state = 0;
        Boolean fClick = False;
        Context ctx = 0, subCtx = 0;
        (void) ParseBtnEvent(&p, &eventType, &button, &state, &fClick);
        (void) ParseContext(&p, &ctx, &subCtx);
    }

    /* 2. Modifiers + KeySym + numeric/group/string arguments */
    {
        unsigned char *p = buf;
        unsigned int state = 0, detail = 0, num = 0;
        String arg = NULL;
        (void) ParseModifiers(&p, &state);
        (void) ParseKeySym(&p, 0, &detail);
        (void) StrToNum(p, &num);
        p = buf;
        (void) ParseWmFuncGrpArg(&p, NULL, &arg);
        p = buf;
        (void) ParseWmFuncNbrArg(&p, NULL, &arg);
    }

    /* 3. Accelerator text processing */
    {
        size_t cap = 16;
        unsigned char *dest = (unsigned char *) XtMalloc((Cardinal) cap);
        if (dest) {
            dest[0] = '\0';
            if (ProcessAccelText(buf, buf + size, &dest, &cap))
                (void) strlen((char *) dest);
            XtFree((char *) dest);
        }
    }

    /* 4. Multi-line GetNextLine / GetString / ParseClientCommand / CCI */
    if (size < MAXLINE) {
        unsigned char *work = malloc(size + 1);
        if (!work) abort();
        memcpy(work, buf, size + 1);
        cfileP = NULL;
        linec = 0;
        parseP = work;
        while (GetNextLine() != NULL) {
            unsigned char *lineP = line;
            unsigned char *tok = GetString(&lineP);
            if (!tok) continue;
            if (IsClientCommand((String) tok)) {
                MenuSpec spec;
                MenuItem item;
                Boolean use_sep = False;
                memset(&spec, 0, sizeof(spec));
                memset(&item, 0, sizeof(item));
                if (ParseClientCommand(&lineP, &spec, &item, tok, &use_sep))
                    XtFree(item.label);
                free_exclusions(&spec);
            } else {
                String arg = NULL;
                unsigned char *argP = lineP;
                if (ParseWmFuncStrArg(&argP, F_Exec, &arg) && arg) {
                    (void) strlen(arg);
                    XtFree(arg);
                }
                arg = NULL;
                argP = lineP;
                if (ParseWmFuncMaybeStrArg(&argP, F_Lower, &arg) && arg) {
                    (void) strlen(arg);
                    XtFree(arg);
                }
                arg = NULL;
                argP = lineP;
                if (ParseWmFuncCCIArgs(&argP, F_Nop, &arg) && arg) {
                    MenuSpec spec;
                    MenuItem item;
                    memset(&spec, 0, sizeof(spec));
                    memset(&item, 0, sizeof(item));
                    item.label = XtNewString((char *) tok);
                    item.wmFuncArgs = arg;
                    FixMenuItem(&spec, &item);
                    XtFree(item.label);
                    XtFree(item.clientCommandName);
                    XtFree(arg);
                    free_exclusions(&spec);
                }
            }
        }
        free(work);
    }

    free(buf);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    fuzz_wsm_wire(data, size);
    fuzz_mwmrc_parsers(data, size);
    return 0;
}
