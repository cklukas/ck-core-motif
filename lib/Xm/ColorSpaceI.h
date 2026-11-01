/* Private, display-independent sRGB / HSV / CIE Lab (D50) conversions.
 * Matrices include Bradford D65/D50 adaptation. Lab follows CSS Color 4;
 * output is bounded sRGB, not an ICC-managed display color. */
#ifndef _XmColorSpaceI_h
#define _XmColorSpaceI_h
#include <math.h>
#include <stdint.h>
static double color_clamp(double v,double lo,double hi){return v<lo?lo:v>hi?hi:v;}
static void color_rgb(uint32_t v,double rgb[3]){for(int i=0;i<3;++i)rgb[i]=((v>>(24-8*i))&255)/255.;}
static uint32_t color_pack(const double rgb[3],unsigned alpha){uint32_t v=0;for(int i=0;i<3;++i)v=(v<<8)|(unsigned)floor(color_clamp(rgb[i],0,1)*255+.5);return (v<<8)|(alpha&255);}
static void color_to_hsv(uint32_t v,double hsv[3]){
 double c[3];color_rgb(v,c);double hi=fmax(c[0],fmax(c[1],c[2])),lo=fmin(c[0],fmin(c[1],c[2])),d=hi-lo;
 /* Preserve hue for achromatic selections, so increasing saturation recovers it. */
 if(d>0){double h=hi==c[0]?(c[1]-c[2])/d:hi==c[1]?2+(c[2]-c[0])/d:4+(c[0]-c[1])/d;hsv[0]=fmod(h*60+360,360);}
 hsv[1]=hi?d/hi:0;hsv[2]=hi;
}
static void color_from_hsv(const double hsv[3],double rgb[3]){
 double h=hsv[0]/60,s=hsv[1],v=hsv[2],c=v*s,x=c*(1-fabs(fmod(h,2)-1)),m=v-c;
 int sector=(int)floor(h)%6;
 rgb[0]=(sector==0||sector==5?c:sector==1||sector==4?x:0)+m;
 rgb[1]=(sector==1||sector==2?c:sector==0||sector==3?x:0)+m;
 rgb[2]=(sector==3||sector==4?c:sector==2||sector==5?x:0)+m;
}
static double color_linear(double v){return v<=.04045?v/12.92:pow((v+.055)/1.055,2.4);}
static double color_encoded(double v){return v<=.0031308?12.92*v:1.055*pow(v,1/2.4)-.055;}
static double color_lab_f(double v){return v>216./24389?cbrt(v):(24389./27*v+16)/116;}
static double color_lab_inv(double v){return v>6./29?v*v*v:(116*v-16)/(24389./27);}
static void color_to_lab(uint32_t value,double lab[3]){
 double c[3];color_rgb(value,c);for(int i=0;i<3;++i)c[i]=color_linear(c[i]);
 double x=color_lab_f((.436065742824811*c[0]+.385151468833791*c[1]+.143078454422641*c[2])/.964295676429568);
 double y=color_lab_f(.222493191756237*c[0]+.716887053823882*c[1]+.060619790536166*c[2]);
 double z=color_lab_f((.013923904500943*c[0]+.097081285665746*c[1]+.714099358400515*c[2])/.82510460251046);
 lab[0]=116*y-16;lab[1]=500*(x-y);lab[2]=200*(y-z);
}
static int color_from_lab(const double lab[3],double rgb[3]){
 double y=(lab[0]+16)/116,x=color_lab_inv(y+lab[1]/500)*.964295676429568,z=color_lab_inv(y-lab[2]/200)*.82510460251046;y=color_lab_inv(y);
 rgb[0]=3.13413595699587*x-1.61738633216125*y-.490661946008354*z;
 rgb[1]=-.97879550291209*x+1.91625456725952*y+.0334427311613195*z;
 rgb[2]=.0719553798841168*x-.228976826415832*y+1.40538605832413*z;
 int gamut=1;for(int i=0;i<3;++i){if(rgb[i]<-1e-6||rgb[i]>1.000001)gamut=0;rgb[i]=color_encoded(rgb[i]);}return gamut;
}
#endif
