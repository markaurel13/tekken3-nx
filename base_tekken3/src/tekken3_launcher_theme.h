#ifndef TEKKEN3_LAUNCHER_THEME_H
#define TEKKEN3_LAUNCHER_THEME_H
/* Game-owned skin; the shared launcher remains unchanged for other games. */
static inline LauncherTheme tekken3_launcher_theme(void) {
    LauncherTheme t=launcher_theme_psx();
    t.background=lng_rgba(.055f,.055f,.063f,1);
    t.background2=t.background;
    t.panel=lng_rgba(.085f,.085f,.094f,1);
    t.panel_hovered=lng_rgba(.15f,.145f,.145f,1);
    t.control=lng_rgba(.13f,.13f,.14f,1);
    t.control_hovered=lng_rgba(.23f,.18f,.18f,1);
    t.border=lng_rgba(.27f,.26f,.26f,1);
    t.accent=lng_rgba(.73f,.13f,.12f,1);
    t.accent_dim=lng_rgba(.43f,.065f,.06f,1);
    t.accent2=lng_rgba(.86f,.72f,.47f,1);
    t.text=lng_rgba(.92f,.90f,.85f,1);
    t.text_muted=lng_rgba(.64f,.63f,.60f,1);
    t.good=lng_rgba(.56f,.73f,.53f,1);
    t.warn=lng_rgba(.91f,.67f,.36f,1);
    t.focus_ring=t.accent2;
    t.radius_sm=1;t.radius_lg=2;t.scanlines=0;
    t.font_body=17;t.spacing_lg=20;t.spacing_md=12;t.spacing_sm=7;
    return t;
}
#endif
