"""Exact text-change payload regression on a private real AT-SPI bus/display."""
import json
import os
import pathlib
import select
import subprocess
import sys
import time
import dbus
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
    process = subprocess.Popen([str(fixture.resolve())], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    def response():
        assert select.select([process.stdout], [], [], 30)[0], 'fixture timeout'
        line = process.stdout.readline()
        assert line, 'fixture exited'
        return json.loads(line)
    nodes = response()
    events = []
    def event(detail, first, second, payload, properties, path=None, member=None):
        tail = path.rsplit('/', 1)[1]
        events.append(dict(node=1 if tail == 'root' else int(tail), member=str(member),
                           detail=str(detail), offset=int(first), length=int(second), payload=str(payload)))
    bus.add_signal_receiver(event, dbus_interface='org.a11y.atspi.Event.Object',
                            bus_name=nodes['bus'], path_keyword='path', member_keyword='member')
    context = GLib.MainContext.default()
    def drain():
        deadline = time.monotonic() + .2
        while time.monotonic() < deadline:
            while context.pending(): context.iteration(False)
            time.sleep(.001)
    drain(); events.clear()
    def run(command, expected, carets=None):
        events.clear()
        process.stdin.write(command + '\n'); process.stdin.flush()
        assert response()['done']
        # A method round trip is ordered after all fixture-side queued signals.
        bus.get_object(nodes['bus'], '/org/a11y/atspi/accessible/root', introspect=False).GetRole(dbus_interface='org.a11y.atspi.Accessible')
        drain()
        actual = [(e['node'], e['detail'], e['offset'], e['length'], e['payload']) for e in events if e['member'] == 'TextChanged']
        assert actual == expected, (command, actual, expected)
        if carets is not None:
            assert [(e['node'], e['offset']) for e in events if e['member'] == 'TextCaretMoved'] == carets, (command, events)
        records.append(dict(command=command, events=list(events)))
    field, text, custom = [int(nodes[k]) for k in ('field', 'text', 'custom')]
    run('insert', [(field, 'insert', 1, 2, 'β😀')])
    run('replace', [(field, 'delete', 1, 2, 'β😀'), (field, 'insert', 1, 2, 'é')])
    run('clear', [(field, 'delete', 0, 4, 'AéB')])
    run('noop', [], [])
    run('caret', [], [(text, 1)])
    run('readonly', [(text, 'delete', 1, 1, 'b'), (text, 'insert', 1, 1, 'z')])
    run('rejected', [], [])
    run('long', [(text, 'delete', 1, 2, 'zc'), (text, 'insert', 1, 4999, 'a' * 4999)])
    run('middle', [(text, 'delete', 2047, 1, 'a'), (text, 'insert', 2047, 1, '😀')])
    run('custom', [(custom, 'delete', 0, 7, 'initial'), (custom, 'insert', 0, 11, 'replacement')])
    run('unicode', [(custom, 'delete', 1, 9, 'eplacemen'), (custom, 'insert', 1, 2, 'β😀')])
    run('nested', [], [])
    run('recover', [], [])
    run('invalid', [], [])
    run('recover', [], [])
    run('password', [], [])
    password = bus.get_object(nodes['bus'], '/org/a11y/atspi/accessible/' + str(nodes['password']), introspect=False)
    assert str(password.GetRoleName(dbus_interface='org.a11y.atspi.Accessible')) == 'password text'
    try:
        password.GetText(0, 1, dbus_interface='org.a11y.atspi.Text')
        raise AssertionError('password content exposed')
    except dbus.DBusException:
        pass
    run('remove', [], [])
    events.clear()
    process.stdin.write('destroy\n'); process.stdin.flush()
    assert response()['done']
    drain()
    assert not [e for e in events if e['member'] == 'TextChanged'], events
    records.append(dict(command='destroy bridge from text callback', events=list(events)))
    process.stdin.write('quit\n'); process.stdin.flush()
    stdout, stderr = process.communicate(timeout=10)
    assert process.returncode == 0, stderr
    passed = True
finally:
    if process and process.poll() is None:
        process.terminate(); process.wait(timeout=5)
    receipt.write_text(json.dumps(dict(passed=passed, records=records,
        scope='native Text/TextField and provider transport deltas; not screen-reader acceptance'), indent=2) + '\n')
