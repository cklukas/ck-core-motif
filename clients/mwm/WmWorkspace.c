/* Standalone Motif workspace protocol. This is independent of CDE's WSM build. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "WmGlobal.h"
#include "WmWorkspace.h"
#include "WmFunction.h"
#include "WmCmd.h"
#include "WmKeyFocus.h"
#include "WmWinConf.h"
#include "WmWinState.h"
#include <stdint.h>
#include <string.h>
#include <limits.h>
#ifdef MWM_WSM

static const char *window_names[] = {"x", "y", "width", "height", "iconic", "hidden", "maximized"};
static const char *icon_names[] = {"x", "y"};

static WmScreenData *screen_for_widget(Widget widget)
{
    int number = XScreenNumberOfScreen(XtScreen(widget));
    for (int i = 0; i < wmGD.numScreens; ++i)
        if (wmGD.Screens[i].managed && wmGD.Screens[i].screen == number) return &wmGD.Screens[i];
    return NULL;
}

static ClientData *find_tree(ClientData *client, Window id)
{
    if (client->client == id) return client;
    for (ClientData *child = client->transientChildren; child; child = child->transientSiblings) {
        ClientData *found = find_tree(child, id);
        if (found) return found;
    }
    return NULL;
}

static ClientData *find_client(WmScreenData *screen, Window id)
{
    id &= WIN_MASK;
    for (ClientListEntry *entry = screen->clientList; entry; entry = entry->nextSibling) {
        if (entry->type != NORMAL_STATE) continue;
        ClientData *found = find_tree(entry->pCD, id);
        if (found) return found;
    }
    return NULL;
}

Boolean WsmClientHidden(ClientData *client)
{
    for (; client; client = client->transientLeader)
        if (client->wsmHidden) return True;
    return False;
}

void WsmApplyVisibility(ClientData *client)
{
    Boolean hidden = WsmClientHidden(client);
    if (hidden) {
        if (wmGD.keyboardFocus == client) {
            Do_Focus_Key(NULL, CurrentTime, ALWAYS_SET_FOCUS);
            SetKeyboardFocus(NULL, 0);
        }
        if (client->clientFrameWin) XUnmapWindow(DISPLAY, client->clientFrameWin);
        if (ICON_FRAME_WIN(client)) XUnmapWindow(DISPLAY, ICON_FRAME_WIN(client));
    } else if (client->clientState == NORMAL_STATE || client->clientState == MAXIMIZED_STATE) {
        if (client->clientFrameWin) XMapWindow(DISPLAY, client->clientFrameWin);
    } else if (client->clientState == MINIMIZED_STATE && ICON_FRAME_WIN(client)) {
        XMapWindow(DISPLAY, ICON_FRAME_WIN(client));
    }
    for (ClientData *child = client->transientChildren; child; child = child->transientSiblings)
        WsmApplyVisibility(child);
}

typedef struct { WmScreenData *screen; Window owner; XtIntervalId timer; } WorkspaceOwner;
static WorkspaceOwner *owners;

static void release_tree(ClientData *client)
{
    client->wsmHidden = False;
    for (ClientData *child = client->transientChildren; child; child = child->transientSiblings)
        release_tree(child);
}

static void monitor_owner(XtPointer closure, XtIntervalId *timer)
{
    WorkspaceOwner *owner = closure;
    Atom selection = _WSMGetSelectionAtom(DISPLAY, owner->screen->screen, WSM_WORKSPACE_MANAGER);
    Window current = XGetSelectionOwner(DISPLAY, selection);
    (void)timer;
    if (current != owner->owner) {
        WmScreenData *previous = ACTIVE_PSD;
        ACTIVE_PSD = owner->screen;
        RemoveCommandsForClient((int)(owner->screen - wmGD.Screens), owner->owner);
        ACTIVE_PSD = previous;
        for (ClientListEntry *entry = owner->screen->clientList; entry; entry = entry->nextSibling)
            if (entry->type == NORMAL_STATE) release_tree(entry->pCD);
        for (ClientListEntry *entry = owner->screen->clientList; entry; entry = entry->nextSibling)
            if (entry->type == NORMAL_STATE) WsmApplyVisibility(entry->pCD);
        owner->owner = None;
        owner->timer = 0;
        return;
    }
    owner->timer = XtAppAddTimeOut(XtWidgetToApplicationContext(owner->screen->utmShell),
                                   500, monitor_owner, owner);
}

static void track_owner(WmScreenData *screen)
{
    int screen_count = ScreenCount(DISPLAY);

    if (!screen || screen_count <= 0 || screen->screen < 0 ||
        screen->screen >= screen_count ||
        (size_t)screen_count > UINT_MAX / sizeof(*owners))
        return;
    if (!owners) {
        owners = (WorkspaceOwner *)XtCalloc((Cardinal)screen_count,
                                             sizeof(*owners));
        if (!owners)
            return;
    }
    WorkspaceOwner *owner = &owners[screen->screen];
    Atom selection = _WSMGetSelectionAtom(DISPLAY, screen->screen, WSM_WORKSPACE_MANAGER);
    Window current = XGetSelectionOwner(DISPLAY, selection);
    if (owner->timer) XtRemoveTimeOut(owner->timer);
    owner->screen = screen; owner->owner = current; owner->timer = 0;
    if (current != None)
        owner->timer = XtAppAddTimeOut(XtWidgetToApplicationContext(screen->utmShell), 500, monitor_owner, owner);
}

static void formats(Display *display, int screen, WSMConfigFormatReply *reply)
{
    WSMConfigFormatData *window = _WSMGetConfigFormat(display, screen, WSM_WINDOW_FMT);
    WSMConfigFormatData *icon = _WSMGetConfigFormat(display, screen, WSM_ICON_FMT);
    if (!window->num_attrs) {
        window->num_attrs = XtNumber(window_names);
        window->attr_list = (WSMAttribute *)XtCalloc(window->num_attrs, sizeof(WSMAttribute));
        for (int i = 0; i < window->num_attrs; ++i) {
            window->attr_list[i].nameq = XrmStringToQuark(window_names[i]);
            window->attr_list[i].size = i < 4 ? 32 : 8;
        }
    }
    if (!icon->num_attrs) {
        icon->num_attrs = XtNumber(icon_names);
        icon->attr_list = (WSMAttribute *)XtCalloc(icon->num_attrs, sizeof(WSMAttribute));
        for (int i = 0; i < icon->num_attrs; ++i) {
            icon->attr_list[i].nameq = XrmStringToQuark(icon_names[i]);
            icon->attr_list[i].size = 32;
        }
    }
    reply->accepts_diffs = True;
    reply->window_formats = window->attr_list;
    reply->num_window_formats = window->num_attrs;
    reply->icon_formats = icon->attr_list;
    reply->num_icon_formats = icon->num_attrs;
}

static void snapshot(ClientData *client, Boolean icon, WSMWinInfo *info)
{
    int count = icon ? XtNumber(icon_names) : XtNumber(window_names);
    const char **names = icon ? icon_names : window_names;
    long values[] = {client->clientX, client->clientY, client->clientWidth,
                     client->clientHeight, client->clientState == MINIMIZED_STATE,
                     client->wsmHidden, client->maxConfig};
    if (icon) { values[0] = ICON_X(client); values[1] = ICON_Y(client); }
    info->window = client->client | (icon ? (Window)0xe0000000UL : 0);
    info->num_data_list = count;
    info->data_list = (WSMWinData *)XtCalloc(count, sizeof(WSMWinData));
    if (!info->data_list) {
        info->num_data_list = 0;
        return;
    }
    for (int i = 0; i < count; ++i) {
        info->data_list[i].nameq = XrmStringToQuark(names[i]);
        info->data_list[i].type = WSM_VALUE_DATA;
        info->data_list[i].data.value = values[i];
    }
}

static int tree_count(ClientData *client)
{
    int count = 2;
    for (ClientData *child = client->transientChildren; child; child = child->transientSiblings)
        {
            count += tree_count(child);
            if (count > 65535) return 65536;
        }
    return count;
}

static void tree_snapshot(ClientData *client, WSMWinInfo *items, int *used)
{
    snapshot(client, False, &items[(*used)++]);
    snapshot(client, True, &items[(*used)++]);
    for (ClientData *child = client->transientChildren; child; child = child->transientSiblings)
        tree_snapshot(child, items, used);
}

static void apply(ClientData *client, Window id, WSMWinData *data, int count)
{
    Boolean icon = (id & ~WIN_MASK) != 0;
    int x = icon ? ICON_X(client) : client->clientX;
    int y = icon ? ICON_Y(client) : client->clientY;
    unsigned width = client->clientWidth, height = client->clientHeight;
    Boolean iconic = client->clientState == MINIMIZED_STATE;
    Boolean maximized = client->maxConfig;
    for (int i = 0; i < count; ++i) {
        if (data[i].type != WSM_VALUE_DATA) continue;
        const char *name = XrmQuarkToString(data[i].nameq);
        int32_t value = (int32_t)(uint32_t)data[i].data.value;
        if (!name) continue;
        if (!strcmp(name, "x") && value >= SHRT_MIN && value <= SHRT_MAX) x = value;
        else if (!strcmp(name, "y") && value >= SHRT_MIN && value <= SHRT_MAX) y = value;
        else if (!icon && !strcmp(name, "width") && value > 0 && value <= USHRT_MAX) width = value;
        else if (!icon && !strcmp(name, "height") && value > 0 && value <= USHRT_MAX) height = value;
        else if (!icon && !strcmp(name, "hidden")) client->wsmHidden = value != 0;
        else if (!icon && !strcmp(name, "iconic")) iconic = value != 0;
        else if (!icon && !strcmp(name, "maximized")) maximized = value != 0;
    }
    if (icon) {
        ICON_X(client) = x; ICON_Y(client) = y;
        if (ICON_FRAME_WIN(client)) XMoveWindow(DISPLAY, ICON_FRAME_WIN(client), x, y);
    } else {
        if (x != client->clientX || y != client->clientY ||
            width != (unsigned)client->clientWidth || height != (unsigned)client->clientHeight)
            ProcessClientConfiguration(client, x, y, width, height);
        SetClientState(client, iconic ? MINIMIZED_STATE : maximized ? MAXIMIZED_STATE : NORMAL_STATE, CurrentTime);
    }
    WsmApplyVisibility(client);
}

void HandleWsmConvertRequest(Widget widget, XtPointer closure, WSMRequest *request, WSMReply *reply)
{
    int screen_number = XScreenNumberOfScreen(XtScreen(widget));
    WmScreenData *screen = screen_for_widget(widget);
    if (!screen) { reply->any.type = WSM_UNKNOWN; return; }
    (void)closure;
    memset(reply, 0, sizeof(*reply));
    reply->any.type = request->any.type;
    switch (request->any.type) {
    case WSM_CONNECT:
        track_owner(screen);
        for (int i = 0; i < request->connect.num_versions; ++i)
            if (request->connect.known_versions[i] >= 1 && request->connect.known_versions[i] <= 3) {
                reply->connect.version = request->connect.known_versions[i]; break;
            }
        break;
    case WSM_EXTENSIONS:
        _WSMClearConfigScreenInfo(XtDisplay(widget), screen_number);
        /* No optional extensions, including the old virtual-root panning extension. */
        break;
    case WSM_CONFIG_FMT:
        formats(XtDisplay(widget), screen_number, &reply->config_format);
        break;
    case WSM_GET_STATE: {
        Window id = request->get_state.window;
        int count = 0, used = 0;
        ClientData *client = id ? find_client(screen, id) : NULL;
        if (id) count = client ? 1 : 0;
        else for (ClientListEntry *entry = screen->clientList; entry; entry = entry->nextSibling)
            if (entry->type == NORMAL_STATE) {
                count += tree_count(entry->pCD);
                if (count > 65535) break;
            }
        if (count > 65535) break; /* protocol list count is CARD16 */
        reply->get_state.win_info_list = (WSMWinInfo *)XtCalloc(count, sizeof(WSMWinInfo));
        if (count != 0 && !reply->get_state.win_info_list)
            break;
        reply->any.allocated = True;
        if (id && client) snapshot(client, (id & ~WIN_MASK) != 0, reply->get_state.win_info_list + used++);
        else if (!id) for (ClientListEntry *entry = screen->clientList; entry; entry = entry->nextSibling)
            if (entry->type == NORMAL_STATE) tree_snapshot(entry->pCD, reply->get_state.win_info_list, &used);
        reply->get_state.num_win_info_list = used;
        break;
    }
    case WSM_SET_STATE:
        for (int i = 0; i < request->set_state.num_win_info_list; ++i) {
            WSMWinInfo *info = &request->set_state.win_info_list[i];
            ClientData *client = find_client(screen, info->window);
            if (client) apply(client, info->window, info->data_list, info->num_data_list);
        }
        break;
    case WSM_REG_WINDOW: {
        ClientData *client = find_client(screen, request->register_window.window);
        if (client) {
            WSMWinInfo info;
            snapshot(client, (request->register_window.window & ~WIN_MASK) != 0, &info);
            reply->any.allocated = True;
            reply->register_window.window_data = info.data_list;
            reply->register_window.num_window_data = info.num_data_list;
        }
        break;
    }
    default: break;
    }
}

void WsmConnectExisting(WmScreenData *screen)
{
    Atom selection = _WSMGetSelectionAtom(DISPLAY, screen->screen, WSM_WORKSPACE_MANAGER);
    if (XGetSelectionOwner(DISPLAY, selection) == None) return;
    short versions[] = {3, 2, 1};
    WSMRequest request = {0};
    request.connect.type = WSM_CONNECT;
    request.connect.known_versions = versions;
    request.connect.num_versions = XtNumber(versions);
    track_owner(screen);
    WSMSendMessage(screen->utmShell, WSM_WORKSPACE_MANAGER, &request, NULL, NULL);
}

static void registered(Widget widget, XtPointer closure, WSMReply *reply, WSMErrorCode error)
{
    Window id = (Window)(uintptr_t)closure;
    WmScreenData *screen = screen_for_widget(widget);
    ClientData *client = screen ? find_client(screen, id) : NULL;
    if (client && error == WSM_SUCCESS && reply)
        apply(client, id, reply->register_window.window_data, reply->register_window.num_window_data);
}

void WsmRegisterClient(ClientData *client)
{
    WmScreenData *screen = PSD_FOR_CLIENT(client);
    int number = screen->screen;
    Atom selection = _WSMGetSelectionAtom(DISPLAY, number, WSM_WORKSPACE_MANAGER);
    if (!_WSMGetConfigFormat(DISPLAY, number, WSM_WINDOW_FMT)->num_attrs ||
        XGetSelectionOwner(DISPLAY, selection) == None) return;
    for (int icon = 0; icon < 2; ++icon) {
        WSMRequest request = {0};
        request.register_window.type = WSM_REG_WINDOW;
        request.register_window.window = client->client | (icon ? (Window)0xe0000000UL : 0);
        WSMSendMessage(screen->utmShell, WSM_WORKSPACE_MANAGER, &request, registered,
                       (XtPointer)(uintptr_t)request.register_window.window);
    }
    WsmApplyVisibility(client);
}
#endif
