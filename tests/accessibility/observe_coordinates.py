"""Real private-bus Component coordinates across native and semantic ancestors."""
import json
import os
import pathlib
import select
import signal
import stat
import subprocess
import sys

import dbus

fixture, receipt = map(pathlib.Path, sys.argv[1:3])
process = None
services = []
records = []
provider = None
passed = False
error = None
address = None
stderr = ''


def identity(pid):
    directory = pathlib.Path('/proc') / str(pid)
    fields = (directory / 'stat').read_text().rsplit(')', 1)[1].split()
    return dict(pid=pid, executable=str((directory / 'exe').resolve()),
                start_ticks=int(fields[19]))


try:
    runtime = pathlib.Path(os.environ['XDG_RUNTIME_DIR']).resolve()
    assert runtime.is_absolute() and stat.S_IMODE(runtime.stat().st_mode) == 0o700
    assert not str(runtime).startswith('/run/user/'), 'fresh private runtime required'
    session = dbus.SessionBus()
    address = str(session.get_object('org.a11y.Bus', '/org/a11y/bus').GetAddress(
        dbus_interface='org.a11y.Bus'))
    assert address.startswith('unix:path=' + str(runtime) + '/'), (address, runtime)
    bus = dbus.bus.BusConnection(address)
    iface = 'org.freedesktop.DBus'
    for connection, name in [(session, 'org.a11y.Bus'), (bus, iface)]:
        daemon = connection.get_object(iface, '/org/freedesktop/DBus')
        services.append(identity(int(daemon.GetConnectionUnixProcessID(name, dbus_interface=iface))))
    daemon = bus.get_object(iface, '/org/freedesktop/DBus')
    # Activate the real registry before the fixture's Embed handshake.
    bus.get_object('org.a11y.atspi.Registry', '/org/a11y/atspi/registry').Introspect(
        dbus_interface='org.freedesktop.DBus.Introspectable')
    services.append(identity(int(daemon.GetConnectionUnixProcessID(
        'org.a11y.atspi.Registry', dbus_interface=iface))))
    process = subprocess.Popen([str(fixture.resolve())], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True,
        env=dict(os.environ, AT_SPI_BUS_ADDRESS=address))

    def response():
        assert select.select([process.stdout], [], [], 30)[0], 'fixture response timeout'
        line = process.stdout.readline()
        assert line, 'fixture exited before coordinate response'
        return json.loads(line)

    provider = response()
    assert int(daemon.GetConnectionUnixProcessID(provider['bus'], dbus_interface=iface)) == process.pid == provider['pid']
    owner = identity(process.pid)
    owner['maps'] = (pathlib.Path('/proc') / str(process.pid) / 'maps').read_text()
    component = 'org.a11y.atspi.Component'

    def object_for(identifier):
        return bus.get_object(provider['bus'], '/org/a11y/atspi/accessible/' + str(identifier), introspect=False)

    def check(nodes, phase):
        for node in nodes['nodes']:
            target = object_for(node['id'])
            screen = node['screen']
            for coordinates, key in enumerate(('screen', 'window', 'parent')):
                expected = node[key][:2]
                actual = [int(n) for n in target.GetExtents(dbus.UInt32(coordinates), dbus_interface=component)]
                records.append(dict(phase=phase, node=node, coordinates=coordinates, extents=actual))
                message = ('WINDOW coordinates must use the containing native shell' if coordinates == 1
                           else 'coordinate origin mismatch')
                assert actual == expected + screen[2:], (message, node['name'], actual, expected + screen[2:])
                position = [int(n) for n in target.GetPosition(dbus.UInt32(coordinates), dbus_interface=component)]
                assert position == expected, ('GetPosition mismatch', node, coordinates, position)
            size = [int(n) for n in target.GetSize(dbus_interface=component)]
            assert size == screen[2:], ('GetSize changed with coordinate origin', node, size)

    check(provider, 'initial')
    orphan = object_for(provider['orphan'])
    orphan.GetExtents(dbus.UInt32(0), dbus_interface=component)
    for coordinates in (1, 2):
        try:
            orphan.GetExtents(dbus.UInt32(coordinates), dbus_interface=component)
        except dbus.DBusException as caught:
            assert caught.get_dbus_name() == 'org.freedesktop.DBus.Error.NotSupported', caught
        else:
            raise AssertionError('application-only semantics must not invent a window or parent origin')
    def unsupported(target, coordinates, reason):
        for method in ('GetExtents', 'GetPosition'):
            try:
                target.get_dbus_method(method, component)(dbus.UInt32(coordinates))
            except dbus.DBusException as caught:
                assert caught.get_dbus_name() == 'org.freedesktop.DBus.Error.NotSupported', caught
                records.append(dict(case=reason, method=method, rejected=True))
            else:
                raise AssertionError(reason)

    boundary = object_for(provider['boundary'])
    actual = [int(n) for n in boundary.GetExtents(dbus.UInt32(0), dbus_interface=component)]
    assert actual == [-2147483648, -2147483648, 1, 1], actual
    unsupported(boundary, 2, 'relative coordinates outside signed 32-bit range must be rejected')
    retiring = object_for(provider['retiring'])
    for method in ('GetExtents', 'GetPosition'):
        # The first parent read retires the child; following reads must also fail.
        if method == 'GetExtents':
            process.stdin.write('retire\n'); process.stdin.flush()
            assert response()['armed']
        try:
            retiring.get_dbus_method(method, component)(dbus.UInt32(2))
        except dbus.DBusException as caught:
            assert caught.get_dbus_name() in ('org.freedesktop.DBus.Error.NotSupported',
                                              'org.freedesktop.DBus.Error.UnknownObject'), caught
            records.append(dict(case='parent query retired target', method=method, rejected=True))
        else:
            raise AssertionError('parent query retired target but transport returned stale coordinates')
    process.stdin.write('move\n'); process.stdin.flush()
    moved = response()
    check(moved, 'moved second shell')
    assert moved['nodes'][3]['screen'][:2] != provider['nodes'][3]['screen'][:2], 'second native shell did not move'
    capture = receipt.with_suffix('.png')
    subprocess.run(['scrot', '--overwrite', str(capture)], check=True, timeout=10,
                   stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    process.stdin.write('stop\n'); process.stdin.flush()
    stdout, stderr = process.communicate(timeout=10)
    assert process.returncode == 0 and not stdout.strip(), (process.returncode, stdout, stderr)
    passed = True
    print('Component coordinates: two windows, nested providers, moved shell and unsupported origins passed')
except BaseException as caught:
    error = repr(caught)
    raise
finally:
    if process and process.poll() is None:
        process.terminate()
        unused, stderr = process.communicate(timeout=5)
    cleanup = []
    for service in reversed(services):
        try:
            if identity(service['pid']) == service:
                os.kill(service['pid'], signal.SIGTERM)
                cleanup.append(dict(service, stop_requested=True))
        except (FileNotFoundError, ProcessLookupError):
            cleanup.append(dict(service, already_exited=True))
    receipt.write_text(json.dumps(dict(passed=passed, error=error, address=address,
        provider=provider, process=locals().get('owner'), records=records, stderr=stderr,
        child_exit=None if process is None else process.returncode,
        capture=str(locals().get('capture', '')), private_services=services, cleanup=cleanup,
        scope='native Component screen/window/parent coordinates; not screen-reader acceptance'), indent=2) + '\n')
