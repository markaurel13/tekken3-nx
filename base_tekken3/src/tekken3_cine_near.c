/* The polygons of a cinematic's actors behind or against the camera
 * (src/tekken3_cine.c).
 *
 * A fight's camera never comes near a fighter; a cinematic's does, and goes
 * past one (Roger's TTT ending: from frame ~600 Yoshimitsu is up to 1 500
 * units behind the PS2 camera). The PS2 clips those polygons at its near
 * plane. The PS1 body renderer (0x80036CAC, one call a model part) does not:
 * a vertex behind the camera gets SZ 0 and the GTE's overflowed divide, and
 * its polygons, kept by NCLIP or double-sided, cover the screen.
 *
 * During a cinematic, at the end of each part (0x800377F8, its epilogue,
 * reached through the dispatcher), every packet the part linked in the
 * display list (a1 at the call up to a1 at the epilogue, at the head of its
 * OT slot) is checked against the part's projected vertices, still in the
 * scratchpad: screen XY at 0x1F800000, SZ >> 5 at 0x1F8001B8, a word and a
 * half-word a vertex (the indices the polygon loops use, 0x80037368). A
 * packet one of whose corners is a vertex nearer than H / 2 (where the
 * divide overflows, behind the camera included) keeps its place in the list
 * with no words: the GPU skips it. Nothing changes outside cinematics or
 * when no vertex is that near. TEKKEN3_CINE_NEAR_LOG=1: a line a frame with
 * the packets left out. */
#include "mod_plugins.h"
#include "psx_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum { XY=0x1F800000u, SZ=0x1F8001B8u, VERTS=0x6E, OT_SLOTS=1024 };

extern int tekken3_cine_engine_active(void);
extern int tekken3_cine_engine_frame(void);

static uint32_t start;              /* a1 at the part's call: its first packet */
static unsigned dropped, frame_of;

void tekken3_cine_near_begin(uint32_t packets) { start=packets; }

static int log_on(void) { static int l=-1; if(l<0) l=getenv("TEKKEN3_CINE_NEAR_LOG")!=NULL; return l; }

/* The corners' XY words of a GP0 polygon packet (tag, then the command). */
static int corners(uint32_t p, uint32_t at[4]) {
    uint32_t cmd=psx_mod_read_word(p+4)>>24;
    if((cmd&0xE0)!=0x20) return 0;
    int gouraud=cmd&0x10, quad=cmd&0x08, textured=cmd&0x04, n=quad?4:3;
    uint32_t o=p+8;
    for(int v=0;v<n;v++) {
        if(v && gouraud) o+=4;
        at[v]=o; o+=4;
        if(textured) o+=4;
    }
    return n;
}

static void part_end(CPUState *cpu) {
    uint32_t end=cpu->gpr[5], ot=cpu->gpr[6], from=start;
    start=0;
    if(!from || end<=from || end-from>0x10000u) return;
    /* The part's near vertices, by their screen XY. */
    uint32_t h=cpu->gte_ctrl[26]&0xFFFFu, cut=h/2;
    uint32_t near_xy[VERTS]; unsigned nn=0;
    for(unsigned i=0;i<VERTS;i++) {
        uint32_t z=(uint32_t)psx_mod_read_half(SZ+2*i)<<5;
        if(z<cut) near_xy[nn++]=psx_mod_read_word(XY+4*i);
    }
    if(!nn) return;
    int f=tekken3_cine_engine_frame();
    if((unsigned)f!=frame_of) {
        if(log_on() && dropped) fprintf(stderr,"Cine near: frame %u, %u packets behind the camera left out\n",frame_of,dropped);
        frame_of=(unsigned)f; dropped=0;
    }
    from&=0x00FFFFFFu; end&=0x00FFFFFFu;
    for(unsigned s=0;s<OT_SLOTS;s++) {
        uint32_t p=psx_mod_read_word(ot+4*s)&0x00FFFFFFu;
        for(unsigned k=0;k<512 && p>=from && p<end;k++) {
            uint32_t tag=psx_mod_read_word(0x80000000u|p), at[4];
            int n=corners(0x80000000u|p,at), hit=0;
            for(int v=0;v<n && !hit;v++) {
                uint32_t xy=psx_mod_read_word(at[v]);
                for(unsigned j=0;j<nn;j++) if(near_xy[j]==xy) { hit=1; break; }
            }
            if(hit && tag>>24) { psx_mod_write_word(0x80000000u|p,tag&0x00FFFFFFu); dropped++; }
            p=tag&0x00FFFFFFu;
        }
    }
}

/* An actor some of whose bones are nearer the cinematic's camera than
 * NEAR_UNITS (behind it: negative), along its line of sight. What is drawn
 * from points of its own, not by the body renderer, is left out for such an
 * actor: Yoshimitsu's sword glow (0x8003172C, src/tekken3_ttt1_mod.c).
 * TEKKEN3_CINE_NEAR_LOG=1: its nearest bone every 30
 * frames. The bones' world
 * matrices: actor +0x8F4, 68 bytes each, position at +0x14. */
extern int tekken3_cine_camera(int16_t eye[3],int16_t target[3]);
int tekken3_cine_actor_near(uint32_t actor) {
    enum { BONES=22, NEAR_UNITS=200 };
    int16_t eye[3],target[3];
    if(!tekken3_cine_engine_active() || !tekken3_cine_camera(eye,target)) return 0;
    double d[3],len=0;
    for(int k=0;k<3;k++) { d[k]=target[k]-eye[k]; len+=d[k]*d[k]; }
    if(len<1) return 0;
    len=sqrt(len);
    double nearest=1e9;
    for(unsigned b=0;b<BONES;b++) {
        uint32_t m=actor+0x8f4+b*68;
        double z=0;
        for(int k=0;k<3;k++) z+=((int32_t)psx_mod_read_word(m+0x14+4*k)-eye[k])*d[k]/len;
        if(z<nearest) nearest=z;
    }
    if(log_on()) {
        static int last=-1; int f=tekken3_cine_engine_frame();
        if(f!=last && f%30==0) { last=f; fprintf(stderr,"Cine near: frame %d, actor %08X nearest bone %.0f\n",f,actor,nearest); }
    }
    return nearest<NEAR_UNITS;
}

extern void __real_func_800377F8(CPUState *cpu);
void __wrap_func_800377F8(CPUState *cpu) {
    if((cpu->pc==0 || cpu->pc==0x800377f8u) && tekken3_cine_engine_active()) part_end(cpu);
    else start=0;
    __real_func_800377F8(cpu);
}
