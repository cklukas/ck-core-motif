/* Test-only LD_PRELOAD fault injection into real libdbus event construction.
 * Fail one selected operation of one selected TextChanged message. No toolkit
 * implementation is copied here; libdbus performs every nonfailed operation. */
#define _GNU_SOURCE
#include <dbus/dbus.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* POSIX dlsym supplies the symbol address. Copy its representation into a
 * same-size function pointer without a C object/function pointer cast. */
static void load_symbol(void *destination, size_t size, const char *name) {
  void *symbol;
  const char *error;
  (void)dlerror();
  symbol = dlsym(RTLD_NEXT, name);
  error = dlerror();
  if (error || !symbol || size != sizeof(symbol)) {
    fprintf(stderr, "text-event interposer cannot resolve %s: %s\n", name,
            error ? error : "null symbol or unsupported pointer representation");
    _Exit(127);
  }
  memcpy(destination, &symbol, size);
}
#define LOAD_SYMBOL(variable, name) do { \
  _Static_assert(sizeof(variable) == sizeof(void *), \
                 "POSIX symbol and function pointer sizes must match"); \
  load_symbol(&(variable), sizeof(variable), (name)); \
} while (0)
static DBusMessage *target;
static unsigned int messages, step;
static int active, failed;
static int fail_operation(void) {
  const char *value = getenv("XM_TEST_TEXT_FAIL_STEP");
  if (!active || failed) return 0;
  ++step;
  if (value && step == strtoul(value, NULL, 10)) {
    failed = 1;
    fprintf(stderr, "text-event injected failure step %u\n", step);
    return 1;
  }
  return 0;
}
DBusMessage *dbus_message_new_signal(const char *path, const char *interface,
                                    const char *member) {
  DBusMessage *(*real)(const char *, const char *, const char *);
  LOAD_SYMBOL(real, "dbus_message_new_signal");
  const char *which = getenv("XM_TEST_TEXT_FAIL_MESSAGE");
  unsigned long selected = which ? strtoul(which, NULL, 10) : 1;
  if (!strcmp(interface, "org.a11y.atspi.Event.Object") && !strcmp(member, "TextChanged") && ++messages == selected) {
    active = 1; step = 0;
    if (fail_operation()) { active = 0; return NULL; }
    target = real(path, interface, member);
    return target;
  }
  return real(path, interface, member);
}
void dbus_message_iter_init_append(DBusMessage *message, DBusMessageIter *iter) {
  void (*real)(DBusMessage *, DBusMessageIter *);
  LOAD_SYMBOL(real, "dbus_message_iter_init_append");
  active = target && message == target;
  real(message, iter);
}
dbus_bool_t dbus_message_iter_append_basic(DBusMessageIter *iter, int type, const void *value) {
  dbus_bool_t (*real)(DBusMessageIter *, int, const void *);
  LOAD_SYMBOL(real, "dbus_message_iter_append_basic");
  if (fail_operation()) return FALSE;
  return real(iter, type, value);
}
dbus_bool_t dbus_message_iter_open_container(DBusMessageIter *iter, int type,
                                            const char *signature, DBusMessageIter *child) {
  dbus_bool_t (*real)(DBusMessageIter *, int, const char *, DBusMessageIter *);
  LOAD_SYMBOL(real, "dbus_message_iter_open_container");
  if (fail_operation()) return FALSE;
  return real(iter, type, signature, child);
}
dbus_bool_t dbus_message_iter_close_container(DBusMessageIter *iter, DBusMessageIter *child) {
  dbus_bool_t (*real)(DBusMessageIter *, DBusMessageIter *);
  LOAD_SYMBOL(real, "dbus_message_iter_close_container");
  if (fail_operation()) return FALSE;
  return real(iter, child);
}
dbus_bool_t dbus_connection_send(DBusConnection *connection, DBusMessage *message, dbus_uint32_t *serial) {
  dbus_bool_t (*real)(DBusConnection *, DBusMessage *, dbus_uint32_t *);
  LOAD_SYMBOL(real, "dbus_connection_send");
  active = target && message == target;
  if (fail_operation()) return FALSE;
  return real(connection, message, serial);
}

void dbus_message_unref(DBusMessage *message) {
  void (*real)(DBusMessage *);
  LOAD_SYMBOL(real, "dbus_message_unref");
  if (message == target) { target = NULL; active = 0; }
  real(message);
}
