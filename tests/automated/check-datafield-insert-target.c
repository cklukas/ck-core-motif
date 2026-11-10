/*
 * When another client asks a DataField to insert its selection
 * (INSERT_SELECTION), the DataField asks that client which targets it can
 * provide and then requests one of them.  When the client offers neither
 * COMPOUND_TEXT nor STRING the request is for TEXT; the target used to be
 * read from a variable that had not been set.
 *
 *   check-datafield-insert-target
 *
 * A second X connection, served by its own thread, owns a selection that
 * offers only UTF8_STRING and records the target of the request that follows.
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/DataF.h>

static Display * cdpy;
static Window cwin;
static Atom a_clientsel, a_targets, a_insert, a_pair, a_utf8, a_text;
static volatile int phase;		/* 0 start, 1 targets served, 2 target seen, 3 finished */
static volatile Atom second_target;
static volatile int reply_seen;
static Window app_window;

static void refuse(XSelectionRequestEvent * q)
{
    XSelectionEvent n;
    memset(&n, 0, sizeof(n));
    n.type = SelectionNotify;
    n.display = q->display;
    n.requestor = q->requestor;
    n.selection = q->selection;
    n.target = q->target;
    n.time = q->time;
    n.property = None;
    XSendEvent(cdpy, q->requestor, False, NoEventMask, (XEvent *) &n);
    XFlush(cdpy);
}

static void * client(void * arg)
{
    Atom pair[2];
    XEvent e;
    int i;

    (void) arg;
    cdpy = XOpenDisplay(NULL);
    cwin = XCreateSimpleWindow(cdpy, DefaultRootWindow(cdpy), 0, 0, 1, 1, 0, 0, 0);
    a_clientsel = XInternAtom(cdpy, "CLIENT_TEST_SELECTION", False);
    a_targets = XInternAtom(cdpy, "TARGETS", False);
    a_insert = XInternAtom(cdpy, "INSERT_SELECTION", False);
    a_pair = XInternAtom(cdpy, "ATOM_PAIR", False);
    a_utf8 = XInternAtom(cdpy, "UTF8_STRING", False);
    a_text = XInternAtom(cdpy, "TEXT", False);
    XSetSelectionOwner(cdpy, a_clientsel, cwin, CurrentTime);

    /* ask the data field, which owns PRIMARY, to insert our selection */
    pair[0] = a_clientsel;
    pair[1] = a_utf8;		/* not the locale's target */
    XChangeProperty(cdpy, cwin, a_pair, a_pair, 32, PropModeReplace, (unsigned char *) pair, 2);
    XConvertSelection(cdpy, XA_PRIMARY, a_insert, a_pair, cwin, CurrentTime);
    XFlush(cdpy);

    for (i = 0; i < 2000 && phase < 3; i++) {
	while (XPending(cdpy)) {
	    XNextEvent(cdpy, &e);
	    if (e.type == SelectionRequest && e.xselectionrequest.selection == a_clientsel) {
		XSelectionRequestEvent * q = &e.xselectionrequest;
		if (q->target == a_targets) {
		    Atom offered[1];
		    XSelectionEvent n;
		    offered[0] = a_utf8;
		    XChangeProperty(cdpy, q->requestor, q->property, XA_ATOM, 32, PropModeReplace,
				    (unsigned char *) offered, 1);
		    memset(&n, 0, sizeof(n));
		    n.type = SelectionNotify; n.display = q->display;
		    n.requestor = q->requestor; n.selection = q->selection;
		    n.target = q->target; n.time = q->time; n.property = q->property;
		    XSendEvent(cdpy, q->requestor, False, NoEventMask, (XEvent *) &n);
		    XFlush(cdpy);
		    phase = 1;
		} else {
		    second_target = q->target;
		    phase = 2;
		    refuse(q);
		}
	    } else if (e.type == SelectionNotify && e.xselection.selection == XA_PRIMARY) {
		reply_seen = 1;
		phase = 3;
	    }
	}
	usleep(5000);
    }
    phase = 3;
    return NULL;
}

int main(void)
{
    int argc = 1;
    char * argv[] = { "datafield-insert-target", NULL };
    XtAppContext app;
    Widget shell, df;
    pthread_t thread;
    int i, failures = 0;
    Atom text;

    XInitThreads();
    shell = XtVaAppInitialize(&app, "DataFieldInsert", NULL, 0, &argc, argv, NULL, NULL);
    df = XtVaCreateManagedWidget("df", xmDataFieldWidgetClass, shell,
				 XmNvalue, "hello world", NULL);
    XtRealizeWidget(shell);
    XSync(XtDisplay(shell), False);
    XmDataFieldSetSelection(df, 0, 5, CurrentTime);
    XSync(XtDisplay(shell), False);
    text = XInternAtom(XtDisplay(shell), "TEXT", False);

    app_window = XtWindow(shell);
    pthread_create(&thread, NULL, client, NULL);
    for (i = 0; i < 2000 && phase < 3; i++) {
	while (XtAppPending(app))
	    XtAppProcessEvent(app, XtIMAll);
	usleep(5000);
    }
    pthread_join(thread, NULL);

    if (phase < 2) {
	fprintf(stderr, "FAIL the data field never asked for a target (phase %d)\n", phase);
	failures++;
    } else if (second_target != text) {
	fprintf(stderr, "FAIL the data field asked for target %lu, expected TEXT (%lu)\n",
		(unsigned long) second_target, (unsigned long) text);
	failures++;
    }
    printf("datafield insert target: %s\n", failures ? "FAILED" : "ok");
    fflush(stdout);
    return failures != 0;
}
