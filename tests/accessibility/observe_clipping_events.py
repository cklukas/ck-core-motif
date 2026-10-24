"""Ancestor-clipping StateChanged regression on a private AT-SPI bus/display."""
import json
import os
import pathlib
import select
import signal
import stat
import subprocess
import sys
import time
import xml.etree.ElementTree as ET

import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

DBusGMainLoop(set_as_default=True)
fixture, receipt = map(pathlib.Path, sys.argv[1:3])
process = None
services = []
records = []
passed = False
error = None
registration = None
nodes = None
address = None


def identity(pid):
    directory = pathlib.Path('/proc') / str(pid)
    # Everything following the final ')' starts with process state (field 3).
    fields = (directory / 'stat').read_text().rsplit(')', 1)[1].split()
    return dict(pid=pid, executable=str((directory / 'exe').resolve()),
                start_ticks=int(fields[19]))


try:
    runtime = pathlib.Path(os.environ['XDG_RUNTIME_DIR']).resolve()
    assert runtime.is_absolute() and stat.S_IMODE(runtime.stat().st_mode) == 0o700
    assert not str(runtime).startswith('/run/user/'), 'a fresh private runtime is required'
    session = dbus.SessionBus()
    address = str(session.get_object('org.a11y.Bus', '/org/a11y/bus').GetAddress(
        dbus_interface='org.a11y.Bus'))
    bus = dbus.bus.BusConnection(address)
    daemon_interface = 'org.freedesktop.DBus'
    session_daemon = session.get_object(daemon_interface, '/org/freedesktop/DBus')
    bus_daemon = bus.get_object(daemon_interface, '/org/freedesktop/DBus')
    launcher_pid = int(session_daemon.GetConnectionUnixProcessID(
        'org.a11y.Bus', dbus_interface=daemon_interface))
    services.append(identity(launcher_pid))
    services.append(identity(int(bus_daemon.GetConnectionUnixProcessID(
        daemon_interface, dbus_interface=daemon_interface))))
    assert address.startswith('unix:path=' + str(runtime) + '/'), (address, runtime)
    registry = bus.get_object('org.a11y.atspi.Registry', '/org/a11y/atspi/registry')
    # Introspection activates only the registry on this authenticated private bus.
    xml = str(registry.Introspect(dbus_interface='org.freedesktop.DBus.Introspectable'))
    services.append(identity(int(bus_daemon.GetConnectionUnixProcessID(
        'org.a11y.atspi.Registry', dbus_interface=daemon_interface))))
    iface = 'org.a11y.atspi.Registry'
    tree = ET.fromstring(xml)
    method = tree.find("./interface[@name='%s']/method[@name='RegisterEvent']" % iface)
    assert method is not None, 'private registry has no RegisterEvent'
    signature = ''.join(arg.attrib['type'] for arg in method.findall('arg')
                        if arg.attrib.get('direction', 'in') == 'in')
    arguments = {
        's': ['object:state-changed:showing'],
        'sas': ['object:state-changed:showing', dbus.Array([], signature='s')],
        'sass': ['object:state-changed:showing', dbus.Array([], signature='s'), ''],
    }
    assert signature in arguments, ('unsupported authoritative RegisterEvent signature', signature)
    registry.RegisterEvent(*arguments[signature], dbus_interface=iface)
    registration = dict(event='object:state-changed:showing', signature=signature)
    environment = dict(os.environ, AT_SPI_BUS_ADDRESS=address)
    process = subprocess.Popen([str(fixture.resolve())], stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                               text=True, env=environment)

    def response():
        assert select.select([process.stdout], [], [], 30)[0], 'fixture response timeout'
        line = process.stdout.readline()
        assert line, 'fixture exited before response'
        return json.loads(line)

    nodes = response()
    provider_pid = int(bus_daemon.GetConnectionUnixProcessID(nodes['bus'],
                                                           dbus_interface=daemon_interface))
    assert provider_pid == process.pid == int(nodes['pid']), 'provider process mismatch'
    target_path = '/org/a11y/atspi/accessible/' + str(nodes['target'])
    target = bus.get_object(nodes['bus'], target_path, introspect=False)
    events = []

    def changed(detail, first, second, payload, properties, path=None):
        events.append(dict(path=str(path), detail=str(detail), enabled=int(first),
                           second=int(second)))

    bus.add_signal_receiver(changed, signal_name='StateChanged',
                            dbus_interface='org.a11y.atspi.Event.Object',
                            bus_name=nodes['bus'], path_keyword='path')
    context = GLib.MainContext.default()

    def drain(duration=.3):
        deadline = time.monotonic() + duration
        while time.monotonic() < deadline:
            while context.pending():
                context.iteration(False)
            time.sleep(.001)

    def state(expected):
        words = [int(word) for word in target.GetState(
            dbus_interface='org.a11y.atspi.Accessible')]
        assert words[0] & (1 << 30), ('mapped target lost VISIBLE', words)
        assert bool(words[0] & (1 << 25)) == expected, (expected, words)
        return words

    initial = state(False)
    drain()
    events.clear()
    for command, showing, expected_x in [('reveal', True, 30), ('hide', False, 180)]:
        process.stdin.write(command + '\n')
        process.stdin.flush()
        reply = response()
        assert reply['done'] and reply['target_x_in_clip'] == expected_x, reply
        words = state(showing)  # Ordered provider round trip after ancestor mutation.
        drain()
        relevant = [event for event in events
                    if event['path'] == target_path and event['detail'] == 'showing']
        assert [event['enabled'] for event in relevant] == [int(showing)], (command, events)
        records.append(dict(command=command, reply=reply, state=words, events=list(events)))
        events.clear()
    process.stdin.write('stop\n')
    process.stdin.flush()
    stdout, stderr = process.communicate(timeout=10)
    assert process.returncode == 0, (process.returncode, stderr)
    assert not stdout.strip(), stdout
    passed = True
except BaseException as caught:
    error = repr(caught)
    raise
finally:
    if process and process.poll() is None:
        process.terminate()
        process.wait(timeout=5)
    cleanup = []
    # PIDs are obtained only from the explicitly private bus. Recheck identities
    # before touching any process, so PID reuse cannot affect unrelated services.
    for service in reversed(services):
        try:
            if identity(service['pid']) == service:
                os.kill(service['pid'], signal.SIGTERM)
                cleanup.append(dict(service, stop_requested=True))
        except (FileNotFoundError, ProcessLookupError):
            cleanup.append(dict(service, already_exited=True))
    receipt.write_text(json.dumps(dict(passed=passed, error=error, address=address,
        registration=registration, provider=nodes, records=records,
        child_exit=None if process is None else process.returncode,
        private_services=services, cleanup=cleanup,
        scope='actual private-bus descendant SHOWING transitions after native ancestor moves; not screen-reader acceptance'),
        indent=2) + '\n')
