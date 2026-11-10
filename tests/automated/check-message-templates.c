/*
 * Message templates from a message catalog or the error database are text
 * to expand, not printf formats.
 *
 *   check-message-templates
 *
 * Drives _XmExpandMessage directly with hostile templates, then runs
 * templates through the Motif warning handler (_XmWarningMsg, XmeWarning)
 * and through the DataField and RepType warnings, capturing the text that
 * reaches the toolkit's warning handler.  The earlier toolkit used these
 * texts as formats for snprintf, so a template with %n wrote through its
 * argument.
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/DataF.h>
#include <Xm/RepType.h>
#include <Xm/XmP.h>

extern void _XmWarningMsg(Widget, char *, char *, char **, Cardinal);

static int failures;
#define CHECK(cond, ...) \
    do { \
	if (!(cond)) { \
	    failures++; \
	    fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
	    fprintf(stderr, __VA_ARGS__); \
	    fprintf(stderr, "\n"); \
	} \
    } while (0)

static char captured[8192];
static int captures;

static void capture(String msg)
{
    snprintf(captured, sizeof(captured), "%s", msg);
    captures++;
}

typedef size_t (*expand_fn)(char *, size_t, const char *, const char * const *, Cardinal);

static void test_expansion(void)
{
    expand_fn expand = (expand_fn) dlsym(RTLD_DEFAULT, "_XmExpandMessage");
    static const char * const two[] = { "one", "two" };
    static const struct { const char * tmpl; const char * expect; } table[] = {
	{ "plain", "plain" },
	{ "a %s b %s c", "a one b two c" },
	{ "%s%s%s%s", "onetwo" },
	{ "%n", "one" },
	{ "%99999999d|%*d|%-5.3ls|%1$s|%hhn", "one|two|||" },
	{ "100%% sure", "100% sure" },
	{ "ends with %", "ends with %" },
	{ "ends with %99", "ends with %99" },
	{ "%c%d%x%p%f%.3s", "onetwo" },
	{ "", "" },
    };
    char buffer[256], small[8];
    size_t i, needed;

    if (expand == NULL) {
	fprintf(stderr, "note: _XmExpandMessage not present (earlier toolkit)\n");
	return;
    }
    for (i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
	needed = expand(buffer, sizeof(buffer), table[i].tmpl, two, 2);
	CHECK(strcmp(buffer, table[i].expect) == 0 && needed == strlen(table[i].expect),
	      "template '%s' expanded to '%s' (%lu), expected '%s'", table[i].tmpl,
	      buffer, (unsigned long) needed, table[i].expect);
    }
    /* sizing call and truncation */
    needed = expand(NULL, 0, "%s and %s", two, 2);
    CHECK(needed == strlen("one and two"), "sizing call returned %lu", (unsigned long) needed);
    memset(small, 'x', sizeof(small));
    needed = expand(small, sizeof(small), "%s and %s", two, 2);
    CHECK(needed == strlen("one and two") && strcmp(small, "one and") == 0,
	  "truncation: '%s' %lu", small, (unsigned long) needed);
    memset(small, 'x', sizeof(small));
    needed = expand(small, 1, "abc", two, 2);
    CHECK(needed == 3 && small[0] == '\0' && small[1] == 'x', "one byte buffer");
    needed = expand(small, 0, "abc", two, 2);
    CHECK(needed == 3 && small[0] == '\0', "empty buffer is untouched");
    /* missing and NULL arguments */
    {
	const char * const holes[] = { NULL, "b" };
	expand(buffer, sizeof(buffer), "[%s][%s][%s]", holes, 2);
	CHECK(strcmp(buffer, "[][b][]") == 0, "NULL and missing arguments: '%s'", buffer);
	expand(buffer, sizeof(buffer), "[%s]", NULL, 0);
	CHECK(strcmp(buffer, "[]") == 0, "no arguments: '%s'", buffer);
	needed = expand(buffer, sizeof(buffer), NULL, two, 2);
	CHECK(needed == 0 && buffer[0] == '\0', "NULL template");
    }
}

int main(void)
{
    int argc = 1;
    char * argv[] = { "message-templates", NULL };
    XtAppContext app;
    Widget shell, df;
    char * params[1] = { "alpha" };
    XmRepTypeId id;

    test_expansion();

    XtSetLanguageProc(NULL, NULL, NULL);	/* a multibyte locale */
    shell = XtVaAppInitialize(&app, "MessageTemplates", NULL, 0, &argc, argv, NULL,
			      NULL);
    XtSetWarningHandler(capture);

    /* A hostile template reaches the warning handler as the default text. */
    captures = 0;
    _XmWarningMsg(shell, "template", "%n|%s|%s|%s|%1$s|%99999999d|%*d|%ls", params, 1);
    CHECK(captures == 1 && strstr(captured, "alpha|||||||") != NULL,
	  "hostile template through the warning handler: '%s'", captured);
    CHECK(strstr(captured, "Name:") != NULL && strstr(captured, "Class:") != NULL,
	  "the warning keeps its Name and Class header: '%s'", captured);

    /* A message without parameters is shown as it is. */
    captures = 0;
    XmeWarning(shell, "100% done %n %s");
    CHECK(captures == 1 && strstr(captured, "100% done %n %s") != NULL,
	  "message without parameters: '%s'", captured);

    /* Messages with a number: the value is shown as a string. */
    captures = 0;
    id = XmRepTypeGetId(XmRAlignment);
    CHECK(id != XmREP_TYPE_INVALID, "representation type Alignment");
    (void) XmRepTypeValidValue(id, 250, shell);
    CHECK(captures == 1 && strstr(captured, "Illegal value (250) for rep type XmRAlignment") != NULL,
	  "representation type warning: '%s'", captured);

    /* A byte that is not a character of the locale. */
    df = XtVaCreateManagedWidget("df", xmDataFieldWidgetClass, shell, NULL);
    captures = 0;
    XtVaSetValues(df, XmNvalue, "a\377b", NULL);
    CHECK(captures >= 1 && strstr(captured, "Character '\\377', not supported") != NULL,
	  "data field warning (%d): '%s'", captures, captured);

    printf("message templates: %s\n", failures ? "FAILED" : "ok");
    return failures != 0;
}
