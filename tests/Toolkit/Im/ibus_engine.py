"""Select a global engine on one explicit private IBus bus, without XKB changes.

IBus 1.5.32 src/ibusbus.c implements SetGlobalEngine success as receipt of its
void method reply. GlobalEngine is independently deserialized via libIBus.
Neither observation proves a widget/InputContext-to-engine mapping.
"""
import re
import threading
import time
import gi

gi.require_version('IBus', '1.0')
from gi.repository import Gio, GLib, IBus


def select_global_engine(address, name, timeout_ms=3000):
    if not isinstance(address, str) or not address.startswith('unix:abstract=') or ';' in address:
        raise ValueError('An explicit private abstract IBus endpoint is required')
    if not isinstance(name, str) or not re.fullmatch(r'[A-Za-z0-9_:+.-]{1,128}', name):
        raise ValueError('Invalid engine identifier')
    if type(timeout_ms) is not int or not 1 <= timeout_ms <= 10000:
        raise ValueError('Invalid bounded IBus timeout')
    result = dict(requested=name, endpoint=address, started_ns=time.monotonic_ns(),
                  accepted=False, setter_error=None, global_name=None, getter_error=None,
                  matched=False, context_mapping_proven=False)
    cancellable = Gio.Cancellable()
    timer = threading.Timer(timeout_ms / 1000, cancellable.cancel)
    timer.daemon = True
    connection = None
    phase = 'connect'
    timer.start()
    try:
        connection = Gio.DBusConnection.new_for_address_sync(address,
            Gio.DBusConnectionFlags.AUTHENTICATION_CLIENT | Gio.DBusConnectionFlags.MESSAGE_BUS_CONNECTION,
            None, cancellable)
        result['connection_unique_name'] = connection.get_unique_name()
        def call(destination, interface, method, args, reply):
            return connection.call_sync(destination, '/org/freedesktop/IBus' if destination != 'org.freedesktop.DBus' else '/org/freedesktop/DBus',
                interface, method, args, GLib.VariantType.new(reply), Gio.DBusCallFlags.NONE,
                timeout_ms, cancellable)
        phase = 'owner'
        owner = call('org.freedesktop.DBus', 'org.freedesktop.DBus', 'GetNameOwner',
                     GLib.Variant('(s)', ('org.freedesktop.IBus',)), '(s)').unpack()[0]
        result['provider_unique_name'] = owner
        phase = 'setter'
        try:
            call(owner, 'org.freedesktop.IBus', 'SetGlobalEngine',
                 GLib.Variant('(s)', (name,)), '()')
            result['accepted'] = True
        except GLib.Error as error:
            result['setter_error'] = dict(message=str(error), remote=Gio.DBusError.get_remote_error(error))
        # Read independently even after a setter error: an old matching global
        # value must never be substituted for successful mutation admission.
        phase = 'getter'
        try:
            reply = call(owner, 'org.freedesktop.DBus.Properties', 'Get',
                         GLib.Variant('(ss)', ('org.freedesktop.IBus', 'GlobalEngine')), '(v)')
            IBus.init()
            property_value = reply.get_child_value(0).get_variant()
            if property_value.get_type_string() != 'v':
                raise ValueError('GlobalEngine property was not the declared variant')
            engine = IBus.Serializable.deserialize_object(property_value.get_variant())
            if not isinstance(engine, IBus.EngineDesc):
                raise ValueError('GlobalEngine did not deserialize as IBusEngineDesc')
            result['global_name'] = engine.get_name()
        except (GLib.Error, ValueError, TypeError) as error:
            result['getter_error'] = dict(message=str(error))
        phase = 'owner-readback'
        final_owner = call('org.freedesktop.DBus', 'org.freedesktop.DBus', 'GetNameOwner',
                           GLib.Variant('(s)', ('org.freedesktop.IBus',)), '(s)').unpack()[0]
        result['provider_stable'] = owner == final_owner
        result['matched'] = result['accepted'] and result['global_name'] == name and result['provider_stable']
    except GLib.Error as error:
        result['error'] = dict(phase=phase, message=str(error), remote=Gio.DBusError.get_remote_error(error))
    finally:
        # Teardown shares the same deadline as connect/set/get. In particular,
        # do not disable cancellation before a synchronous transport close.
        try:
            if connection:
                try: connection.close_sync(cancellable)
                except GLib.Error as error: result['close_error'] = str(error)
        finally:
            timer.cancel()
            timer.join()
        result['ended_ns'] = time.monotonic_ns()
    return result
