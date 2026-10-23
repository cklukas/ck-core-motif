"""Real registry non-global key listener: ordering, consumption and fail-open."""
import json
import os
import pathlib
import select
import subprocess
import sys
import time
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

DBusGMainLoop(set_as_default=True)
fixture, receipt = map(pathlib.Path, sys.argv[1:3])
process = None
records = []
passed = False
try:
    address = str(dbus.SessionBus().get_object('org.a11y.Bus', '/org/a11y/bus').GetAddress(dbus_interface='org.a11y.Bus'))
    runtime = pathlib.Path(os.environ['XDG_RUNTIME_DIR']).resolve()
    assert address.startswith('unix:path=' + str(runtime) + '/'), (address, runtime)
    bus = dbus.bus.BusConnection(address)
    listener_bus = dbus.bus.BusConnection(address)
    registry = listener_bus.get_object('org.a11y.atspi.Registry', '/org/a11y/atspi/registry/deviceeventcontroller', introspect=False)
    controller = 'org.a11y.atspi.DeviceEventController'
    access = 'org.a11y.atspi.Accessible'
    process = subprocess.Popen([str(fixture.resolve())], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    context = GLib.MainContext.default()
    def iterate():
        while context.pending(): context.iteration(False)
    def drain(seconds):
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            iterate(); time.sleep(.001)
    def response():
        deadline = time.monotonic() + 10
        while time.monotonic() < deadline:
            iterate()
            if select.select([process.stdout], [], [], .001)[0]:
                line = process.stdout.readline()
                assert line, 'fixture exited'
                return json.loads(line)
        raise AssertionError('fixture response timeout')
    nodes = response()
    def obj(node):
        return bus.get_object(nodes['bus'], '/org/a11y/atspi/accessible/' + str(node), introspect=False)
    mode = 'observe'
    events = []
    errors = []
    class Listener(dbus.service.Object):
        @dbus.service.method('org.a11y.atspi.DeviceEventListener', in_signature='(uiuuisb)', out_signature='b', async_callbacks=('reply', 'error'))
        def NotifyEvent(self, event, reply, error):
            try:
                kind, keysym, hardware, modifiers, timestamp, label, is_text = event
                # Registry output remains unsigned for hardware/modifiers even
                # though its NotifyListenersSync input parser requires signed32.
                assert isinstance(kind, dbus.UInt32) and isinstance(keysym, dbus.Int32)
                assert isinstance(hardware, dbus.UInt32) and isinstance(modifiers, dbus.UInt32)
                assert isinstance(timestamp, dbus.Int32) and isinstance(label, dbus.String)
                assert isinstance(is_text, dbus.Boolean)
                # A synchronous query back into the paused application must work.
                value = str(obj(nodes['field']).GetText(0, -1, dbus_interface='org.a11y.atspi.Text'))
                record = dict(type=int(kind), keysym=int(keysym), hardware=int(hardware),
                    modifiers=int(modifiers), timestamp=int(timestamp), text=str(label),
                    is_text=bool(is_text), before=value, mode=mode)
                events.append(record)
                if mode == 'destroy' and int(kind) == 0:
                    assert obj(nodes['control']).DoAction(0, dbus_interface='org.a11y.atspi.Action')
                if mode == 'nested' and int(kind) == 0:
                    assert obj(nodes['control']).DoAction(1, dbus_interface='org.a11y.atspi.Action')
                if mode == 'hung' and int(kind) == 0:
                    def late():
                        reply(True); record['late_reply'] = True; return False
                    GLib.timeout_add(1200, late)
                else:
                    reply(mode == 'consume')
            except Exception as exception:
                errors.append(str(exception)); error(exception)
    listener = Listener(listener_bus, '/native_key_listener')
    empty_keys = dbus.Array([], signature='(iisi)')
    def register_listener():
        result = registry.RegisterKeystrokeListener(dbus.ObjectPath('/native_key_listener'), empty_keys,
            dbus.UInt32(0), dbus.UInt32(3), dbus.Struct((True, True, False), signature='bbb'), dbus_interface=controller)
        # Registry 2.56.2 returns FALSE unconditionally after adding a listener
        # (registryd/deviceeventcontroller.c). Prove the actual registration,
        # including ownership and non-global consumption mode, by readback.
        listeners = registry.GetKeystrokeListeners(dbus_interface=controller)
        matching = [entry for entry in listeners if str(entry[0]) == listener_bus.get_unique_name()
                    and str(entry[1]) == '/native_key_listener']
        assert len(matching) == 1, listeners
        entry = matching[0]
        assert int(entry[2]) == 0 and int(entry[3]) == 3 and not entry[4] and int(entry[5]) == 0 and tuple(map(bool, entry[6])) == (True, True, False), entry
        records.append(dict(registration_reply=bool(result), registered_bus=str(entry[0]),
                            registered_path=str(entry[1]), registered_mode=list(map(bool, entry[6]))))
    register_listener()
    def command(text):
        process.stdin.write(text + '\n'); process.stdin.flush(); return response()
    def focus(which):
        window = nodes['window' if which == 1 else 'second']
        subprocess.run(['xdotool', 'windowfocus', '--sync', str(window)], check=True)
        command('focus' + str(which)); drain(.05)
    def key(name):
        before = len(events)
        started = time.monotonic()
        subprocess.run(['xdotool', 'key', '--clearmodifiers', name], check=True)
        state = command('state')
        elapsed = time.monotonic() - started
        drain(.05)
        assert not errors, errors
        records.append(dict(key=name, mode=mode, events=events[before:], state=state, elapsed_seconds=elapsed))
        return events[before:], state, elapsed
    focus(1)
    captured, state, _ = key('a')
    assert state['value'] == 'a', state
    assert [(e['type'], e['keysym'], e['text'], e['is_text']) for e in captured] == [(0, ord('a'), 'a', True), (1, ord('a'), 'a', True)], captured
    assert captured[0]['before'] == '' and captured[1]['before'] == 'a'
    assert all(e['hardware'] > 0 and e['timestamp'] and not e['modifiers'] for e in captured)
    # Prove the very same ordinary native binding before and after consumption.
    # F10 is reserved by Motif's virtual menu-bar key mapping.
    captured, state, _ = key('F9')
    assert state['marks'] == 1 and len(captured) == 2, (captured, state)
    mode = 'consume'
    captured, state, _ = key('F9')
    assert state['marks'] == 1 and len(captured) == 2, (captured, state)
    mode = 'observe'
    captured, state, _ = key('F9')
    assert state['marks'] == 2 and len(captured) == 2, (captured, state)
    focus(2)
    captured, state, _ = key('x')
    assert state['other'] == 'x' and len(captured) == 2, (captured, state)
    command('close2'); focus(1)
    mode = 'nested'
    captured, state, _ = key('d')
    assert state['value'] == 'and' and len(captured) == 2, (captured, state)
    mode = 'hung'
    captured, state, elapsed = key('b')
    assert state['value'] == 'andb' and elapsed < 1.1, (state, elapsed)
    drain(1.3)
    assert any(e.get('late_reply') for e in captured), captured
    assert command('state')['value'] == 'andb', 'late consume changed already-delivered key'
    mode = 'observe'
    # Drop the listener connection without unregistering, as after a crash.
    listener.remove_from_connection(); listener_bus.close()
    drain(.05)
    captured, state, _ = key('c')
    assert state['value'] == 'andbc' and not captured, (captured, state)
    # Re-register on a new real bus client, then destroy the bridge from a
    # listener's reentrant action. The native key still reaches the live widget.
    listener_bus = dbus.bus.BusConnection(address)
    listener = Listener(listener_bus, '/native_key_listener')
    registry = listener_bus.get_object('org.a11y.atspi.Registry', '/org/a11y/atspi/registry/deviceeventcontroller', introspect=False)
    register_listener()
    mode = 'destroy'
    captured, state, _ = key('z')
    assert state['value'] == 'andbcz' and not state['bridge'], (captured, state)
    assert len(captured) == 1, captured
    captured, state, _ = key('q')
    assert state['value'] == 'andbczq' and not captured, (captured, state)
    listener.remove_from_connection(); listener_bus.close()
    process.stdin.write('quit\n'); process.stdin.flush()
    stdout, stderr = process.communicate(timeout=10)
    assert process.returncode == 0, stderr
    passed = True
finally:
    if process and process.poll() is None:
        process.terminate(); process.wait(timeout=5)
    receipt.write_text(json.dumps(dict(passed=passed, records=records,
        wire_contract=dict(registry_input='(uiiiisb)', listener_output='(uiuuisb)',
            registry_source='GNOME/at-spi2-core 2.56.2 registryd/marshal-dbus.c',
            registry_source_sha256='c92dbc7750295ec06733c1ce56cfdce667d457575c412a8bec0cfbf4daaa97c5'),
        scope='real private registry synchronous non-global listener and native X input; not Orca acceptance'), indent=2) + '\n')
