/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Coordinate transport must cross semantic ancestors to the owning shell. */
#include <Xm/Accessible.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <X11/Shell.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct { Widget anchor; int x, y, width, height; } CoordinateRegion;
typedef struct {
  int x, y;
  XmAccessibility *bridge;
  XmAccessibleId retire;
} EdgeRegion;
typedef struct {
  XtAppContext app;
  Widget shell[2], area[2], label[2];
  XmAccessibility *bridge;
  XmAccessibleId panel[2], child[2], orphan, boundary, retiring;
  EdgeRegion edges[4];
  CoordinateRegion regions[5];
  unsigned int attempts;
} Fixture;
static void require(int ok, const char *message) {
  if (!ok) { fprintf(stderr, "%s\n", message); exit(3); }
}
static Boolean query(XtPointer data, XmAccessibleInfo *info) {
  CoordinateRegion *r = data;
  Position x, y;
  XtTranslateCoords(r->anchor, (Position)r->x, (Position)r->y, &x, &y);
  info->role = XmACCESSIBLE_PANEL;
  info->name = XtNewString("Semantic coordinate region");
  info->has_extents = True;
  info->screen_x = x; info->screen_y = y;
  info->width = r->width; info->height = r->height;
  return True;
}
static Boolean edge_query(XtPointer data, XmAccessibleInfo *info) {
  EdgeRegion *r=data;
  info->role=XmACCESSIBLE_PANEL;
  info->has_extents=True;
  info->screen_x=r->x; info->screen_y=r->y;
  info->width=info->height=1;
  if (r->retire) {
    XmAccessibleId id=r->retire;
    r->retire=XmACCESSIBLE_NONE;
    XmAccessibilityRemove(r->bridge,id);
  }
  return True;
}
static void rect(Widget w, int r[4]) {
  Position x, y;
  Dimension width, height;
  XtTranslateCoords(w, 0, 0, &x, &y);
  XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);
  r[0] = x; r[1] = y; r[2] = width; r[3] = height;
}
static void item(const char *name, XmAccessibleId id, const int r[4],
                 const int window[4], const int *parent, int comma) {
  printf("%s{\"name\":\"%s\",\"id\":%u,\"screen\":[%d,%d,%d,%d],"
         "\"window\":[%d,%d],\"parent\":", comma ? "," : "", name, id,
         r[0], r[1], r[2], r[3], r[0]-window[0], r[1]-window[1]);
  if (parent) printf("[%d,%d]", r[0]-parent[0], r[1]-parent[1]);
  else fputs("null", stdout);
  fputs("}", stdout);
}
static void report(Fixture *f) {
  int i;
  XSync(XtDisplay(f->shell[0]), False);
  printf("{\"bus\":\"%s\",\"pid\":%ld,\"nodes\":[",
         XmAccessibilityBusName(f->bridge), (long)getpid());
  for (i=0; i<2; ++i) {
    int shell[4], area[4], label[4], panel[4], child[4];
    XmAccessibleInfo info={0};
    rect(f->shell[i], shell); rect(f->area[i], area); rect(f->label[i], label);
    require(query(&f->regions[i*2], &info), "panel geometry unavailable");
    panel[0]=info.screen_x; panel[1]=info.screen_y;
    panel[2]=info.width; panel[3]=info.height; XmAccessibleInfoClear(&info);
    require(query(&f->regions[i*2+1], &info), "child geometry unavailable");
    child[0]=info.screen_x; child[1]=info.screen_y;
    child[2]=info.width; child[3]=info.height; XmAccessibleInfoClear(&info);
    item("native label", XmAccessibilityWidget(f->bridge,f->label[i]), label,shell,area,i!=0);
    item("semantic panel",f->panel[i],panel,shell,area,1);
    item("nested semantic child",f->child[i],child,shell,panel,1);
  }
  printf("],\"orphan\":%u,\"boundary\":%u,\"retiring\":%u}\n",f->orphan,f->boundary,f->retiring); fflush(stdout);
}
static void command(XtPointer data, int *fd, XtInputId *input) {
  Fixture *f=data; char line[32]; (void)fd; (void)input;
  if (!fgets(line,sizeof(line),stdin) || !strcmp(line,"stop\n")) {
    XtAppSetExitFlag(f->app); return;
  }
  if (!strcmp(line,"retire\n")) {
    f->edges[2].retire=f->retiring;
    puts("{\"armed\":true}"); fflush(stdout); return;
  }
  require(!strcmp(line,"move\n"), "unknown coordinate command");
  XtVaSetValues(f->shell[1],XmNx,480,XmNy,190,NULL);
  XmAccessibilityRefresh(f->bridge);
  report(f);
}
static void connected(XtPointer data, XtIntervalId *timer) {
  Fixture *f=data; (void)timer;
  if (XmAccessibilityGetStatus(f->bridge)==XmACCESSIBILITY_CONNECTED) {
    report(f);
    XtAppAddInput(f->app,STDIN_FILENO,(XtPointer)XtInputReadMask,command,f);
  } else if (++f->attempts<100) XtAppAddTimeOut(f->app,100,connected,f);
  else { fputs("private accessibility transport did not connect\n",stderr); exit(2); }
}
int main(int argc,char **argv) {
  Fixture f={0}; int i; Display *display;
  XmAccessibleProvider provider={0}, edge_provider={0}; provider.query=query;
  edge_provider.query=edge_query;
  require(getenv("AT_SPI_BUS_ADDRESS")!=NULL,"explicit private accessibility bus required");
  f.shell[0]=XtVaAppInitialize(&f.app,"CoordinateFixture",NULL,0,&argc,argv,NULL,
      XmNx,91,XmNy,57,XmNtitle,"Coordinate primary",NULL);
  display=XtDisplay(f.shell[0]);
  f.shell[1]=XtVaAppCreateShell("second","CoordinateFixture",topLevelShellWidgetClass,
      display,XmNx,420,XmNy,110,XmNtitle,"Coordinate second",NULL);
  for(i=0;i<2;++i) {
    f.area[i]=XtVaCreateManagedWidget("area",xmDrawingAreaWidgetClass,f.shell[i],
        XmNwidth,260,XmNheight,170,XmNresizePolicy,XmRESIZE_NONE,NULL);
    f.label[i]=XtVaCreateManagedWidget("native",xmLabelWidgetClass,f.area[i],
        XmNx,17,XmNy,23,XmNwidth,100,XmNheight,25,XmNrecomputeSize,False,NULL);
    XtRealizeWidget(f.shell[i]);
    f.regions[i*2]=(CoordinateRegion){f.area[i],31,57,120,70};
    f.regions[i*2+1]=(CoordinateRegion){f.area[i],42,68,35,19};
  }
  f.bridge=XmAccessibilityCreate(f.shell[0],"Coordinate fixture");
  require(f.bridge!=NULL,"bridge creation failed");
  require(XmAccessibilityAttachShell(f.bridge,f.shell[1]),"second shell attachment failed");
  for(i=0;i<2;++i) {
    f.panel[i]=XmAccessibilityAdd(f.bridge,XmAccessibilityWidget(f.bridge,f.area[i]),&provider,&f.regions[i*2]);
    f.child[i]=XmAccessibilityAdd(f.bridge,f.panel[i],&provider,&f.regions[i*2+1]);
    require(f.panel[i] && f.child[i],"semantic registration failed");
  }
  f.regions[4]=(CoordinateRegion){f.area[0],9,11,10,10};
  f.orphan=XmAccessibilityAdd(f.bridge,XmAccessibilityRoot(f.bridge),&provider,&f.regions[4]);
  require(f.orphan!=0,"application semantic registration failed");
  f.edges[0]=(EdgeRegion){INT_MAX,INT_MAX,f.bridge,0};
  f.edges[1]=(EdgeRegion){INT_MIN,INT_MIN,f.bridge,0};
  f.edges[2]=(EdgeRegion){10,20,f.bridge,0};
  f.edges[3]=(EdgeRegion){30,40,f.bridge,0};
  {
    XmAccessibleId parent=XmAccessibilityAdd(f.bridge,
        XmAccessibilityWidget(f.bridge,f.area[0]),&edge_provider,&f.edges[0]);
    f.boundary=XmAccessibilityAdd(f.bridge,parent,&edge_provider,&f.edges[1]);
    parent=XmAccessibilityAdd(f.bridge,XmAccessibilityWidget(f.bridge,f.area[0]),
        &edge_provider,&f.edges[2]);
    f.retiring=XmAccessibilityAdd(f.bridge,parent,&edge_provider,&f.edges[3]);
    require(f.boundary && f.retiring,"edge provider registration failed");
  }
  XmAccessibilityConnect(f.bridge);
  XtAppAddTimeOut(f.app,100,connected,&f); XtAppMainLoop(f.app);
  XmAccessibilityDestroy(f.bridge);
  XtDestroyWidget(f.shell[1]); XtDestroyWidget(f.shell[0]);
  XtCloseDisplay(display); XtDestroyApplicationContext(f.app);
  return 0;
}
