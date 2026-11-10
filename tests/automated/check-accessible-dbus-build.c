/*
 * The D-Bus accessibility adapter builds every reply and signal with libdbus
 * calls that can fail for lack of memory.  After a failure the message cannot
 * be completed, the iterator of a container that failed to open must not be
 * used, and the containers that did open must be abandoned.
 *
 *   check-accessible-dbus-build
 *
 * The production file lib/Xm/AccessibleDBus.c is compiled into this program.
 * dbus_message_iter_append_basic, _open_container, _close_container and
 * _abandon_container are replaced by versions that count the operations of
 * one call into the adapter and fail the Nth of them.  A container that fails
 * to open gets an iterator filled with a poison pattern; any later use of it,
 * and any container left open, is reported.  For every method and signal the
 * run is repeated with each operation failing in turn, and a reply or signal
 * that leaves the adapter must be complete: a method that cannot be answered
 * gets an out-of-memory error, and a signal is not sent at all.
 *
 * It needs a session bus (the adapter asks the bus for its unique name).
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Xm/XmAll.h>
#include <Xm/Accessible.h>
#include <Xm/ProgressBar.h>

#include "AccessibleDBus.c"

/* ------------------------------------------------------------------ */
/* Failing libdbus operations                                          */
/* ------------------------------------------------------------------ */

static int injecting;		/* count and fail operations of the adapter */
static int fail_at;		/* 1-based operation to fail; 0 counts only */
static int operations;
static int open_containers;
static int poison_uses;
static int injected;

static int is_poison(const DBusMessageIter *iter)
{
    const unsigned char * p = (const unsigned char *) iter;
    size_t i;
    for (i = 0; i < 16; i++)
	if (p[i] != 0xA5)
	    return 0;
    return 1;
}

static int next_operation_fails(void)
{
    operations++;
    if (fail_at != 0 && operations == fail_at) {
	injected++;
	return 1;
    }
    return 0;
}

dbus_bool_t dbus_message_iter_append_basic(DBusMessageIter * iter, int type, const void * value)
{
    static dbus_bool_t (*real)(DBusMessageIter *, int, const void *);
    if (!real) real = dlsym(RTLD_NEXT, "dbus_message_iter_append_basic");
    if (!injecting) return real(iter, type, value);
    if (is_poison(iter)) { poison_uses++; return FALSE; }
    if (next_operation_fails()) return FALSE;
    return real(iter, type, value);
}

dbus_bool_t dbus_message_iter_open_container(DBusMessageIter * iter, int type,
					     const char * signature, DBusMessageIter * sub)
{
    static dbus_bool_t (*real)(DBusMessageIter *, int, const char *, DBusMessageIter *);
    if (!real) real = dlsym(RTLD_NEXT, "dbus_message_iter_open_container");
    if (!injecting) return real(iter, type, signature, sub);
    if (is_poison(iter)) { poison_uses++; memset(sub, 0xA5, sizeof(*sub)); return FALSE; }
    if (next_operation_fails()) {
	memset(sub, 0xA5, sizeof(*sub));
	return FALSE;
    }
    if (!real(iter, type, signature, sub)) return FALSE;
    open_containers++;
    return TRUE;
}

dbus_bool_t dbus_message_iter_close_container(DBusMessageIter * iter, DBusMessageIter * sub)
{
    static dbus_bool_t (*real)(DBusMessageIter *, DBusMessageIter *);
    if (!real) real = dlsym(RTLD_NEXT, "dbus_message_iter_close_container");
    if (!injecting) return real(iter, sub);
    if (is_poison(iter) || is_poison(sub)) { poison_uses++; return FALSE; }
    {
	int fail = next_operation_fails();
	dbus_bool_t ok = real(iter, sub);	/* libdbus releases the container either way */
	open_containers--;
	return fail ? FALSE : ok;
    }
}

void dbus_message_iter_abandon_container(DBusMessageIter * iter, DBusMessageIter * sub)
{
    static void (*real)(DBusMessageIter *, DBusMessageIter *);
    if (!real) real = dlsym(RTLD_NEXT, "dbus_message_iter_abandon_container");
    if (!injecting) { real(iter, sub); return; }
    if (is_poison(iter) || is_poison(sub)) { poison_uses++; return; }
    open_containers--;
    real(iter, sub);
}

/* What leaves the adapter is captured instead of being sent. */
#define MAX_CAPTURED 16
static DBusMessage * captured[MAX_CAPTURED];
static int captured_count;

dbus_bool_t dbus_connection_send(DBusConnection * connection, DBusMessage * message,
				 dbus_uint32_t * serial)
{
    (void) connection;
    (void) serial;
    if (captured_count < MAX_CAPTURED)
	captured[captured_count++] = dbus_message_ref(message);
    return TRUE;
}

static void release_captured(void)
{
    while (captured_count > 0)
	dbus_message_unref(captured[--captured_count]);
}

/* ------------------------------------------------------------------ */

static int failures;
static int checks;
#define CHECK(cond, ...) \
    do { \
	checks++; \
	if (!(cond)) { \
	    failures++; \
	    fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
	    fprintf(stderr, __VA_ARGS__); \
	    fprintf(stderr, "\n"); \
	} \
    } while (0)

static Widget shell, board, button, scale, field;
static XmAccessibility * bridge;
static Transport * transport;
static DBusConnection * connection;
static XmAccessibleId scale_id, button_id, field_id;

static void start(void)
{
    int argc = 1;
    char * argv[] = { "accessible-dbus-build", NULL };
    XtAppContext app;
    DBusError error;

    shell = XtVaAppInitialize(&app, "AccessibleDBusBuild", NULL, 0, &argc, argv, NULL,
			      XtNwidth, 300, XtNheight, 200, NULL);
    board = XtVaCreateManagedWidget("board", xmRowColumnWidgetClass, shell, NULL);
    button = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, board, NULL);
    scale = XtVaCreateManagedWidget("progress", xmProgressBarWidgetClass, board,
				    XmNvalue, 10, NULL);
    field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, board,
				    XmNvalue, "text", NULL);
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);

    bridge = XmAccessibilityCreate(shell, "Accessible build test");
    scale_id = XmAccessibilityWidget(bridge, scale);
    button_id = XmAccessibilityWidget(bridge, button);
    field_id = XmAccessibilityWidget(bridge, field);

    dbus_error_init(&error);
    connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
    if (connection == NULL) {
	fprintf(stderr, "no session bus: %s\n", error.message ? error.message : "?");
	exit(2);
    }
    dbus_connection_set_exit_on_disconnect(connection, FALSE);
    transport = (Transport *) calloc(1, sizeof(*transport));
    transport->owner = bridge;
    transport->connection = connection;
    bridge->transport = (XtPointer) transport;
    bridge->status = XmACCESSIBILITY_CONNECTED;
}

static DBusMessage * make_call(XmAccessibleId id, const char * iface, const char * member,
			       int first_type, ...)
{
    char path[96];
    DBusMessage * call;
    va_list ap;
    int type = first_type;

    if (id == 0) snprintf(path, sizeof(path), "%s", CACHEPATH);
    else path_for(id, path, sizeof(path));
    call = dbus_message_new_method_call(NULL, path, iface, member);
    va_start(ap, first_type);
    while (type != DBUS_TYPE_INVALID) {
	if (type == DBUS_TYPE_STRING) {
	    const char * s = va_arg(ap, const char *);
	    dbus_message_append_args(call, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID);
	} else if (type == DBUS_TYPE_UINT32) {
	    dbus_uint32_t v = (dbus_uint32_t) va_arg(ap, int);
	    dbus_message_append_args(call, DBUS_TYPE_UINT32, &v, DBUS_TYPE_INVALID);
	} else {
	    dbus_int32_t v = va_arg(ap, int);
	    dbus_message_append_args(call, DBUS_TYPE_INT32, &v, DBUS_TYPE_INVALID);
	}
	type = va_arg(ap, int);
    }
    va_end(ap);
    /* A method return needs a call that carries a serial, which sending gives. */
    {
	static dbus_bool_t (*real_send)(DBusConnection *, DBusMessage *, dbus_uint32_t *);
	dbus_uint32_t serial = 0;
	if (!real_send) real_send = dlsym(RTLD_NEXT, "dbus_connection_send");
	dbus_message_set_destination(call, "org.freedesktop.DBus");
	real_send(connection, call, &serial);
    }
    return call;
}

static void run_call(DBusMessage * call)
{
    release_captured();
    dispatch_inner(connection, call, transport);
}

typedef struct {
    const char * name;
    XmAccessibleId id;
    const char * iface;
    const char * member;
    int args;			/* 0 none, 1 or 2 strings, 3 a coordinate type */
    const char * arg1;
    const char * arg2;
} Scenario;

static void check_scenario(const Scenario * sc)
{
    DBusMessage * call;
    int total, n;
    const char * signature = NULL;

#define NEW_CALL() \
    (sc->args == 0 ? make_call(sc->id, sc->iface, sc->member, DBUS_TYPE_INVALID) : \
     sc->args == 1 ? make_call(sc->id, sc->iface, sc->member, DBUS_TYPE_STRING, sc->arg1, DBUS_TYPE_INVALID) : \
     sc->args == 3 ? make_call(sc->id, sc->iface, sc->member, DBUS_TYPE_UINT32, 0, DBUS_TYPE_INVALID) : \
		     make_call(sc->id, sc->iface, sc->member, DBUS_TYPE_STRING, sc->arg1, DBUS_TYPE_STRING, sc->arg2, DBUS_TYPE_INVALID))

    /* Count the operations of an ordinary run. */
    call = NEW_CALL();
    injecting = 1; fail_at = 0; operations = 0; open_containers = 0;
    run_call(call);
    injecting = 0;
    total = operations;
    CHECK(captured_count == 1 && dbus_message_get_type(captured[0]) == DBUS_MESSAGE_TYPE_METHOD_RETURN,
	  "%s: an ordinary call must be answered (%d messages)", sc->name, captured_count);
    if (captured_count == 1)
	signature = dbus_message_get_signature(captured[0]);
    CHECK(total > 0 && open_containers == 0, "%s: %d operations, %d containers left open",
	  sc->name, total, open_containers);
    (void) signature;
    dbus_message_unref(call);

    for (n = 1; n <= total; n++) {
	call = NEW_CALL();
	injecting = 1; fail_at = n; operations = 0; open_containers = 0; injected = 0;
	poison_uses = 0;
	run_call(call);
	injecting = 0;
	CHECK(injected == 1, "%s/%d: the failure was not injected", sc->name, n);
	CHECK(poison_uses == 0, "%s/%d: an iterator of a failed container was used", sc->name, n);
	CHECK(open_containers == 0, "%s/%d: %d containers left open", sc->name, n, open_containers);
	CHECK(captured_count == 1 &&
	      dbus_message_get_type(captured[0]) == DBUS_MESSAGE_TYPE_ERROR &&
	      dbus_message_is_error(captured[0], DBUS_ERROR_NO_MEMORY),
	      "%s/%d: an incomplete reply must become an out-of-memory error (%d messages, type %d)",
	      sc->name, n, captured_count,
	      captured_count ? dbus_message_get_type(captured[0]) : -1);
	dbus_message_unref(call);
    }
    release_captured();
}

/* ------------------------------------------------------------------ */
/* Signals                                                             */
/* ------------------------------------------------------------------ */

static const char * signal_signature(const char * member)
{
    if (!strcmp(member, "PropertyChange") || !strcmp(member, "StateChanged") ||
	!strcmp(member, "ChildrenChanged") || !strcmp(member, "Activate") ||
	!strcmp(member, "Deactivate") || !strcmp(member, "SelectionChanged"))
	return "siiva{sv}";
    if (!strcmp(member, "PropertiesChanged")) return "sa{sv}as";
    if (!strcmp(member, "AddAccessible")) return CACHE_ITEM;
    if (!strcmp(member, "RemoveAccessible")) return "(so)";
    return NULL;
}

static void check_signals_complete(const char * what, int n)
{
    int i;
    for (i = 0; i < captured_count; i++) {
	const char * member = dbus_message_get_member(captured[i]);
	const char * expect = member ? signal_signature(member) : NULL;
	CHECK(dbus_message_get_type(captured[i]) == DBUS_MESSAGE_TYPE_SIGNAL && expect != NULL &&
	      strcmp(dbus_message_get_signature(captured[i]), expect) == 0,
	      "%s/%d: signal %s left with signature '%s'", what, n, member ? member : "?",
	      dbus_message_get_signature(captured[i]));
    }
}

static int toggle;

static void change_widgets(void)
{
    XmString label;
    toggle = !toggle;
    XtVaSetValues(scale, XmNvalue, toggle ? 70 : 30, NULL);
    label = XmStringCreateLocalized(toggle ? "volume" : "level");
    XtVaSetValues(button, XmNlabelString, label, NULL);
    XmStringFree(label);
    XtSetSensitive(button, toggle ? False : True);
}

static void check_changed_signals(void)
{
    int total, n, with_events;

    /* The first report announces the nodes; later ones report changes. */
    _XmAccessibilityTransportChanged(bridge, scale_id);
    _XmAccessibilityTransportChanged(bridge, button_id);
    release_captured();

    change_widgets();
    injecting = 1; fail_at = 0; operations = 0; open_containers = 0;
    release_captured();
    _XmAccessibilityTransportChanged(bridge, scale_id);
    _XmAccessibilityTransportChanged(bridge, button_id);
    injecting = 0;
    total = operations;
    with_events = captured_count;
    CHECK(with_events >= 3, "changes of the progress bar and the button must produce signals (%d)", with_events);
    check_signals_complete("changed", 0);

    for (n = 1; n <= total; n++) {
	change_widgets();
	injecting = 1; fail_at = n; operations = 0; open_containers = 0; injected = 0;
	poison_uses = 0;
	release_captured();
	_XmAccessibilityTransportChanged(bridge, scale_id);
	_XmAccessibilityTransportChanged(bridge, button_id);
	injecting = 0;
	CHECK(injected == 1, "changed/%d: the failure was not injected", n);
	CHECK(poison_uses == 0, "changed/%d: an iterator of a failed container was used", n);
	CHECK(open_containers == 0, "changed/%d: %d containers left open", n, open_containers);
	check_signals_complete("changed", n);
	/* the signal that failed is the only one missing */
	CHECK(captured_count < with_events + 1, "changed/%d: more signals than before", n);
    }
    release_captured();

    /* a node that is removed */
    {
	injecting = 1; fail_at = 0; operations = 0; open_containers = 0;
	release_captured();
	_XmAccessibilityTransportRemoved(bridge, field_id, 1, 2);
	injecting = 0;
	total = operations;
	CHECK(captured_count == 2, "removal must produce two signals (%d)", captured_count);
	check_signals_complete("removed", 0);
	for (n = 1; n <= total; n++) {
	    injecting = 1; fail_at = n; operations = 0; open_containers = 0; injected = 0;
	    poison_uses = 0;
	    release_captured();
	    _XmAccessibilityTransportRemoved(bridge, field_id, 1, 2);
	    injecting = 0;
	    CHECK(injected == 1 && poison_uses == 0 && open_containers == 0,
		  "removed/%d: injected %d poison %d open %d", n, injected, poison_uses, open_containers);
	    check_signals_complete("removed", n);
	    CHECK(captured_count < 2, "removed/%d: both signals sent after a failure (%d)", n, captured_count);
	}
    }
    release_captured();
}

int main(void)
{
    static const Scenario scenarios[] = {
	{ "root children", 1, ACCESS, "GetChildren", 0, NULL, NULL },
	{ "root interfaces", 1, ACCESS, "GetInterfaces", 0, NULL, NULL },
	{ "root state", 1, ACCESS, "GetState", 0, NULL, NULL },
	{ "root application", 1, ACCESS, "GetApplication", 0, NULL, NULL },
	{ "relation set", 1, ACCESS, "GetRelationSet", 0, NULL, NULL },
	{ "attributes", 1, ACCESS, "GetAttributes", 0, NULL, NULL },
	{ "role name", 1, ACCESS, "GetRoleName", 0, NULL, NULL },
	{ "introspect", 1, "org.freedesktop.DBus.Introspectable", "Introspect", 0, NULL, NULL },
	{ "all properties", 1, "org.freedesktop.DBus.Properties", "GetAll", 1, ACCESS, NULL },
	{ "parent property", 1, "org.freedesktop.DBus.Properties", "Get", 2, ACCESS, "Parent" },
	{ "name property", 1, "org.freedesktop.DBus.Properties", "Get", 2, ACCESS, "Name" },
	{ "child count property", 1, "org.freedesktop.DBus.Properties", "Get", 2, ACCESS, "ChildCount" },
	{ "cache", 0, CACHE, "GetItems", 0, NULL, NULL },
	{ "cache introspect", 0, "org.freedesktop.DBus.Introspectable", "Introspect", 0, NULL, NULL },
    };
    size_t i;

    start();
    for (i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); i++)
	check_scenario(&scenarios[i]);

    {
	Scenario value[] = {
	    { "value properties", 0, "org.freedesktop.DBus.Properties", "GetAll", 1, VALUE, NULL },
	    { "current value", 0, "org.freedesktop.DBus.Properties", "Get", 2, VALUE, "CurrentValue" },
	    { "value text", 0, "org.freedesktop.DBus.Properties", "Get", 2, VALUE, "Text" },
	    { "progress state", 0, ACCESS, "GetState", 0, NULL, NULL },
	    { "progress interfaces", 0, ACCESS, "GetInterfaces", 0, NULL, NULL },
	    { "progress extents", 0, COMPONENT, "GetExtents", 3, NULL, NULL },
	    { "progress position", 0, COMPONENT, "GetPosition", 3, NULL, NULL },
	};
	for (i = 0; i < sizeof(value) / sizeof(value[0]); i++) {
	    value[i].id = scale_id;
	    check_scenario(&value[i]);
	}
    }

    check_changed_signals();

    printf("accessible D-Bus build: %d checks, %d failures\n", checks, failures);
    fflush(stdout);
    return failures != 0;
}
