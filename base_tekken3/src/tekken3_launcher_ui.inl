/* Game-owned native frontend. Included after the shared provider-backed panels.
 * No webview, remote assets, ROM copying, or second settings format. */
enum T3Page { T3_PLAY,T3_MODS,T3_KEYBOARD,T3_CONTROLLER,T3_DISCS,T3_SETTINGS,T3_SAVES,T3_ONLINE,T3_UPDATES };
static T3Page t3_page=T3_PLAY;
static tekken3::DiscLibrary t3_discs;
static std::string t3_observed_disc;
static int t3_remove_disc=-1;
static bool t3_reset_keys=false;
static std::filesystem::path t3_update_root;
static bool t3_update_enabled=false,t3_update_asked=false,t3_update_auto_pending=false;
static std::string t3_update_phase,t3_update_message,t3_update_version,t3_update_notes;
static unsigned long long t3_update_received=0,t3_update_total=0;
static double t3_update_poll_at=0;

static std::string t3_read_text(const std::filesystem::path& path,size_t limit=131072) {
    std::ifstream in(path,std::ios::binary);
    if(!in)return {};
    std::string result(limit,'\0');in.read(result.data(),(std::streamsize)limit);
    result.resize((size_t)in.gcount());return result;
}
static std::string t3_json_string(const std::string& json,const char* key) {
    const std::string needle=std::string("\"")+key+"\"";
    size_t pos=json.find(needle);if(pos==std::string::npos)return {};
    pos=json.find(':',pos+needle.size());if(pos==std::string::npos)return {};
    pos=json.find('"',pos+1);if(pos==std::string::npos)return {};
    std::string value;
    for(++pos;pos<json.size();++pos) {
        char c=json[pos];if(c=='"')break;
        if(c=='\\' && pos+1<json.size()) {
            c=json[++pos];if(c=='n')c='\n';else if(c=='r')c='\r';else if(c=='t')c='\t';
        }
        value.push_back(c);
    }
    return value;
}
static unsigned long long t3_json_number(const std::string& json,const char* key) {
    const std::string needle=std::string("\"")+key+"\"";
    size_t pos=json.find(needle);if(pos==std::string::npos)return 0;
    pos=json.find(':',pos+needle.size());if(pos==std::string::npos)return 0;
    while(++pos<json.size() && json[pos]==' '){}
    unsigned long long value=0;
    while(pos<json.size() && json[pos]>='0' && json[pos]<='9') {
        value=value*10+(unsigned)(json[pos]-'0');++pos;
    }
    return value;
}
static bool t3_update_spawn(const char* command,const std::string& version="") {
#if defined(_WIN32)
    const auto script=t3_update_root/"launcher"/"update_backend.py";
    auto python=t3_update_root/".runtime"/"python"/"python.exe";
    if(!std::filesystem::is_regular_file(python))python=L"python.exe";
    if(!std::filesystem::is_regular_file(script)) {
        t3_update_phase="error";t3_update_message="Updater files are missing from this installation.";return false;
    }
    const auto utf16=[](const std::string& s) {
        if(s.empty())return std::wstring();
        int count=MultiByteToWideChar(CP_UTF8,0,s.c_str(),(int)s.size(),nullptr,0);
        std::wstring out((size_t)count,L'\0');
        if(count)MultiByteToWideChar(CP_UTF8,0,s.c_str(),(int)s.size(),out.data(),count);
        return out;
    };
    std::wstring line=L"\""+python.wstring()+L"\" -I \""+script.wstring()+L"\" "+utf16(command);
    if(!version.empty())line+=L" --version \""+utf16(version)+L"\" --parent-pid "+std::to_wstring(GetCurrentProcessId());
    std::vector<wchar_t> mutable_line(line.begin(),line.end());mutable_line.push_back(0);
    STARTUPINFOW startup{};startup.cb=sizeof(startup);PROCESS_INFORMATION process{};
    const std::wstring app=std::filesystem::is_regular_file(python)?python.wstring():L"python.exe";
    if(!CreateProcessW(app==L"python.exe"?nullptr:app.c_str(),mutable_line.data(),nullptr,nullptr,FALSE,
                       CREATE_NO_WINDOW,nullptr,t3_update_root.wstring().c_str(),&startup,&process)) {
        t3_update_phase="error";t3_update_message="Could not start the updater. Check that Python is available.";return false;
    }
    CloseHandle(process.hThread);CloseHandle(process.hProcess);
    t3_update_phase=command;t3_update_message=command[0]=='c'?"Checking GitHub releases...":"Starting installer...";
    return true;
#else
    (void)command;(void)version;t3_update_phase="error";t3_update_message="Updater requires Windows.";return false;
#endif
}
static void t3_update_save_preference(bool enabled) {
    t3_update_enabled=enabled;t3_update_asked=true;
    std::error_code ec;std::filesystem::create_directories(t3_update_root/".setup",ec);
    std::ofstream file(t3_update_root/".setup"/"updates.json",std::ios::binary|std::ios::trunc);
    if(file)file<<"{\"enabled\":"<<(enabled?"true":"false")<<"}\n";
    if(enabled)t3_update_auto_pending=true;
}
static void t3_update_poll() {
    if(ImGui::GetTime()<t3_update_poll_at)return;
    t3_update_poll_at=ImGui::GetTime()+.5;
    const std::string data=t3_read_text(t3_update_root/".setup"/"update-status.json");
    if(data.empty())return;
    const std::string phase=t3_json_string(data,"phase");
    if(phase.empty())return;
    t3_update_phase=phase;t3_update_message=t3_json_string(data,"message");
    t3_update_version=t3_json_string(data,"version");
    t3_update_notes=t3_json_string(data,"notes");
    t3_update_received=t3_json_number(data,"received");t3_update_total=t3_json_number(data,"total");
}

static void tekken3_launcher_init(LauncherModel* m) {
    t3_page=T3_PLAY;t3_remove_disc=-1;t3_reset_keys=false;
    t3_discs.load(std::filesystem::u8path(asset("disc-library.txt")));
    if(t3_discs.error.empty() && t3_discs.has_selection && !tekken3_launcher_disc_is_explicit)
        launcher_model_set_rom(m,t3_discs.selected.c_str());
    t3_observed_disc=m->rom_full;
    if(t3_discs.error.empty()) {
        t3_discs.attach(m->rom_full);t3_discs.selected=tekken3::DiscLibrary::normalize(m->rom_full);t3_discs.save();
    }
    m->s.skip_launcher=0;
    t3_update_root=std::filesystem::u8path(asset("..")).lexically_normal();
    if(!std::filesystem::is_regular_file(t3_update_root/"VERSION"))
        t3_update_root=std::filesystem::u8path(asset(".")).lexically_normal();
    const auto preference=t3_read_text(t3_update_root/".setup"/"updates.json",1024);
    t3_update_asked=preference.find("\"enabled\"")!=std::string::npos;
    t3_update_enabled=preference.find("true")!=std::string::npos;
    t3_update_auto_pending=t3_update_asked && t3_update_enabled;
    t3_update_phase.clear();t3_update_message.clear();t3_update_version.clear();t3_update_notes.clear();
    t3_update_received=0;t3_update_total=0;
    t3_update_poll_at=0;
}
static void t3_goto(LauncherModel* m,T3Page page) {
    launcher_model_cancel_capture(m);launcher_model_cancel_hk_capture(m);
    launcher_model_cancel_camera_capture(m);
    m->map_all_active=false;m->map_all_wait_release=false;
    t3_page=page;
    launcher_model_set_view(m,page==T3_MODS?LNG_VIEW_MODS:
        page==T3_SETTINGS?LNG_VIEW_SETTINGS:page==T3_ONLINE?LNG_VIEW_NETPLAY:
        (page==T3_KEYBOARD || page==T3_CONTROLLER)?LNG_VIEW_CONTROLLER:LNG_VIEW_DASHBOARD);
    if(page==T3_DISCS)t3_discs.refresh();
}
static void t3_heading(const char* text) {
    ImGui::PushFont(g_t3_heading);ImGui::TextUnformatted(text);ImGui::PopFont();
}
static bool t3_compatible(const LauncherModel* m) {
    std::string serial;
    for(const char* p=m->verify.serial;*p;p++)
        if(std::isalnum((unsigned char)*p))serial+=(char)std::toupper((unsigned char)*p);
    return serial=="SLUS00402";
}
static void t3_disc_status(LauncherModel* m,const LauncherTheme& th) {
    if(!m->rom_full[0])ImGui::TextColored(col(th.warn),"NO DISC SELECTED");
    else if(!m->rom_present || m->verify.verdict==3 || m->verify.verdict==0)
        ImGui::TextColored(col(th.warn),"DISC NOT READY - check the image and its tracks");
    else if(!t3_compatible(m))ImGui::TextColored(col(th.warn),"INCOMPATIBLE DISC - this build requires Tekken 3 USA");
    else ImGui::TextColored(col(th.good),"DISC READY  /  %s  /  %s",m->verify.serial,m->verify.region);
}
static void t3_pick_disc(LauncherModel* m) {
    static const char* patterns[]={"*.cue","*.bin","*.iso","*.chd"};
    request_rom_picker(m,"Attach a disc image",patterns,4,"PlayStation disc images",false);
}
static void t3_player_selector(LauncherModel* m) {
    for(int p=0;p<2;p++) {
        if(p)ImGui::SameLine();
        const bool selected=m->cfg_player==p;
        if(selected)ImGui::PushStyleColor(ImGuiCol_Button,col(g_th->accent_dim));
        if(ImGui::Button(p?"PLAYER 2":"PLAYER 1",ImVec2(126,34))) {
            launcher_model_cancel_capture(m);m->map_all_active=false;m->cfg_player=p;
            launcher_binds_refresh(m);
        }
        if(selected)ImGui::PopStyleColor();
    }
    ImGui::Spacing();
}
static const char* t3_move_label(const char* button) {
    if(!strcmp(button,"Square"))return "1  /  Left punch";
    if(!strcmp(button,"Triangle"))return "2  /  Right punch";
    if(!strcmp(button,"Cross"))return "3  /  Left kick";
    if(!strcmp(button,"Circle"))return "4  /  Right kick";
    if(!strcmp(button,"Start"))return "Pause / confirm";
    return "";
}
static void t3_keyboard(LauncherModel* m,const LauncherTheme& th) {
    t3_heading("KEYBOARD MAPPING");
    ImGui::TextColored(col(th.text_muted),"Keys and controller buttons are stored separately.");
    ImGui::TextWrapped("Each player uses only their assigned input device. P2 keyboard defaults: I/J/K/L movement, numpad 1/2/4/5 attacks, U/O shoulders.");
    ImGui::Spacing();t3_player_selector(m);
    const int p=m->cfg_player;
    ImGui::Text("Active input: %s",launcher_model_player_src_label(m,p));
    if(m->s.player_src[p]!=1) {
        ImGui::SameLine();
        if(ImGui::Button("Use keyboard for this player"))
            launcher_model_set_source(m,p,1,0,nullptr,nullptr);
    }
    ImGui::TextColored(col(th.text_muted),"Select a binding, then press a key. Esc cancels; Clear removes that binding.");
    ImGui::Spacing();
    const auto* prof=(const SystemProfile*)m->profile;
    const float height=std::max(160.0f,ImGui::GetContentRegionAvail().y-62);
    if(ImGui::BeginTable("keyboard-map",4,ImGuiTableFlags_RowBg|ImGuiTableFlags_BordersInnerH|
            ImGuiTableFlags_ScrollY|ImGuiTableFlags_SizingStretchProp,ImVec2(0,height))) {
        ImGui::TableSetupColumn("PS1 INPUT",ImGuiTableColumnFlags_WidthFixed,130);
        ImGui::TableSetupColumn("TEKKEN",ImGuiTableColumnFlags_WidthFixed,158);
        ImGui::TableSetupColumn("PRIMARY KEY");ImGui::TableSetupColumn("ALTERNATE KEY");
        ImGui::TableSetupScrollFreeze(0,1);ImGui::TableHeadersRow();
        for(int b=0;b<prof->controller.button_count && b<LNG_MAX_BUTTONS;b++) {
            ImGui::PushID(b);ImGui::TableNextRow();ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();ImGui::TextUnformatted(prof->controller.buttons[b].label);
            ImGui::TableNextColumn();ImGui::AlignTextToFramePadding();
            ImGui::TextColored(col(th.text_muted),"%s",t3_move_label(prof->controller.buttons[b].label));
            for(int slot=0;slot<2;slot++) {
                ImGui::TableNextColumn();ImGui::PushID(slot);
                const bool capture=m->capturing && !m->capture_pad && m->capture_btn==b && m->capture_slot==slot;
                const char* text=capture?"PRESS A KEY...":slot?m->binds_alt[p][b]:m->binds[p][b];
                if(capture)ImGui::PushStyleColor(ImGuiCol_Button,col(th.accent_dim));
                if(ImGui::Button(text[0]?text:"Unbound",ImVec2(std::max(60.f,ImGui::GetContentRegionAvail().x-66),0)))
                    launcher_model_begin_capture_slot(m,b,slot);
                if(capture)ImGui::PopStyleColor();
                ImGui::SameLine();
                if(ImGui::SmallButton("Clear"))launcher_binds_set_button_slot(m,p+1,b,slot,0);
                ImGui::PopID();
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    if(ImGui::Button("Reset keyboard defaults"))t3_reset_keys=true;
    ImGui::SameLine();ImGui::TextColored(col(th.text_muted),"Bindings save as you map them.");
    if(t3_reset_keys)ImGui::OpenPopup("Reset keyboard?");
    if(ImGui::BeginPopupModal("Reset keyboard?",nullptr,ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Reset Player %d keyboard keys? Controller profiles are unchanged.",p+1);
        if(ImGui::Button("Reset")) {launcher_binds_reset_keyboard(m,p+1);t3_reset_keys=false;ImGui::CloseCurrentPopup();}
        ImGui::SameLine();if(ImGui::Button("Cancel")){t3_reset_keys=false;ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }
}
static void t3_library(LauncherModel* m,const LauncherTheme& th) {
    t3_heading("DISC LIBRARY");
    ImGui::TextWrapped("Attach your own disc images. CUE files keep multi-track BIN images together. Detaching never deletes a ROM or its tracks.");
    ImGui::Spacing();
    if(ImGui::Button("+ Attach image",ImVec2(156,36)))t3_pick_disc(m);
    ImGui::SameLine();if(ImGui::Button("Recheck selected",ImVec2(166,36))) {
        std::string path=m->rom_full;launcher_model_set_rom(m,path.c_str());t3_discs.refresh();
    }
    if(m->rom_full[0] && m->verify.verdict!=1 && m->setup_wizard_supported &&
       (m->prepare_disc_cb || m->prepare_with_progress_cb)) {
        ImGui::SameLine();
        if(ImGui::Button("Image setup...",ImVec2(150,36))) {
            m->setup_wizard_open=true;m->setup_page=m->setup_needs_toolchain?0:1;
        }
    }
    ImGui::Spacing();t3_disc_status(m,th);
    if(!t3_discs.error.empty())ImGui::TextWrapped("%s",t3_discs.error.c_str());
    ImGui::Separator();ImGui::Spacing();
    if(t3_discs.entries.empty())ImGui::TextDisabled("No attached images. Use Attach image to choose your Tekken 3 disc.");
    for(size_t i=0;i<t3_discs.entries.size();i++) {
        const auto& entry=t3_discs.entries[i];const bool active=tekken3::DiscLibrary::same(entry.path,m->rom_full);
        ImGui::PushID((int)i);
        std::string name=std::filesystem::u8path(entry.path).filename().u8string();
        if(ImGui::Selectable(name.c_str(),active,0,ImVec2(0,30)))launcher_model_set_rom(m,entry.path.c_str());
        ImGui::TextColored(col(entry.present?th.text_muted:th.warn),"%s",entry.present?(active?"SELECTED":"ATTACHED"):"FILE MISSING");
        ImGui::SameLine();
        if(ImGui::SmallButton("Detach")){t3_remove_disc=(int)i;ImGui::OpenPopup("Detach image?");}
        ImGui::TextWrapped("%s",entry.path.c_str());
        if(ImGui::BeginPopupModal("Detach image?",nullptr,ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted("Remove this library reference? The disc files will stay on disk.");
            if(ImGui::Button("Detach reference")) {
                if(t3_discs.detach(i)) {
                    if(active)launcher_model_set_rom(m,t3_discs.entries.empty()?"":t3_discs.entries[0].path.c_str());
                    t3_observed_disc=m->rom_full;
                }
                t3_remove_disc=-1;ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();if(ImGui::Button("Cancel")){t3_remove_disc=-1;ImGui::CloseCurrentPopup();}
            ImGui::EndPopup();
        }
        ImGui::Separator();ImGui::PopID();
    }
    ImGui::Spacing();
    ImGui::TextColored(col(th.text_muted),"This recompilation runs Tekken 3 USA (SLUS-00402), not arbitrary PS1 games.");
}
static void t3_home(LauncherModel* m,const LauncherTheme& th) {
    /* Reuse the shipped digital PlayStation pad artwork, without generating
     * new character art or baking copyrighted disc pixels into the launcher. */
    if(g_pad_digital.id && g_pad_digital.w>0 && ImGui::GetContentRegionAvail().x>790) {
        const float w=260,h=w*g_pad_digital.h/g_pad_digital.w;
        const ImVec2 pos=ImGui::GetCursorScreenPos();
        const float x=pos.x+ImGui::GetContentRegionAvail().x-w-14;
        ImGui::GetWindowDrawList()->AddImage(tid(g_pad_digital),ImVec2(x,pos.y+22),
            ImVec2(x+w,pos.y+22+h),ImVec2(0,0),ImVec2(1,1),IM_COL32(255,255,255,190));
    }
    ImGui::TextColored(col(th.accent2),"THE KING OF IRON FIST TOURNAMENT");
    ImGui::PushFont(g_t3_title);ImGui::TextUnformatted("READY TO FIGHT");ImGui::PopFont();
    ImGui::TextColored(col(th.text_muted),"Choose your setup. Launch when you're ready.");
    ImGui::Dummy(ImVec2(0,22));
    t3_heading("GAME DISC");
    if(m->rom_file[0])ImGui::TextWrapped("%s",m->rom_file);
    t3_disc_status(m,th);
    ImGui::Spacing();
    if(ImGui::Button("Manage disc images",ImVec2(205,36)))t3_goto(m,T3_DISCS);
    ImGui::Dummy(ImVec2(0,14));ImGui::Separator();ImGui::Dummy(ImVec2(0,14));
    t3_heading("PLAYER SETUP");
    for(int p=0;p<2;p++) {
        ImGui::PushID(p);ImGui::AlignTextToFramePadding();
        ImGui::Text("P%d",p+1);ImGui::SameLine(50);
        ImGui::SetNextItemWidth(std::min(300.f,ImGui::GetContentRegionAvail().x-150));
        if(ImGui::BeginCombo("##input",launcher_model_player_src_label(m,p))) {
            draw_source_selectables(m,p);ImGui::EndCombo();
        }
        ImGui::SameLine();
        if(ImGui::Button("Mapping",ImVec2(108,0))) {
            m->cfg_player=p;t3_goto(m,m->s.player_src[p]==2?T3_CONTROLLER:T3_KEYBOARD);
        }
        ImGui::PopID();
    }
    ImGui::Dummy(ImVec2(0,14));ImGui::Separator();ImGui::Dummy(ImVec2(0,14));
    t3_heading("MOD CONFIGURATION");
    int count=0,enabled=0;
    if(m->mods && m->mods->feature_count && m->mods->feature_get) {
        count=m->mods->feature_count(m->mods->ctx);
        for(int i=0;i<count;i++){RecompLauncherCModFeature f{};if(m->mods->feature_get(m->mods->ctx,i,&f) && f.enabled)enabled++;}
    }
    ImGui::Text("%d enabled / %d available features",enabled,count);
    ImGui::SameLine();if(ImGui::Button("Open mod manager"))t3_goto(m,T3_MODS);
    ImGui::TextColored(col(th.text_muted),"Review character imports, outfits and enhancements before playing.");
}
static void t3_settings(LauncherModel* m,const LauncherTheme& th) {
    t3_heading("GAME SETTINGS");
    if(ImGui::BeginTabBar("GameSettings")) {
        if(ImGui::BeginTabItem("DISPLAY")){ImGui::Spacing();draw_display_controls(m,th);ImGui::EndTabItem();}
        if(ImGui::BeginTabItem("AUDIO")){ImGui::Spacing();draw_audio_controls(m,th);ImGui::EndTabItem();}
        if(ImGui::BeginTabItem("SYSTEM")){ImGui::Spacing();draw_system_controls(m,th);draw_input_controls(m,th);ImGui::EndTabItem();}
        if(ImGui::BeginTabItem("HOTKEYS")){ImGui::Spacing();draw_hotkeys_controls(m,th);ImGui::EndTabItem();}
        ImGui::EndTabBar();
    }
    ImGui::Spacing();
    if(launcher_model_can_restore_defaults(m) && ImGui::Button("Restore game settings defaults"))
        launcher_model_request_restore_defaults(m);
}
static void t3_updates(LauncherModel* m,const LauncherTheme& th) {
    t3_heading("GAME PATCHES");
    std::string installed=t3_read_text(t3_update_root/"VERSION",80);
    while(!installed.empty() && (installed.back()=='\n'||installed.back()=='\r'))installed.pop_back();
    ImGui::Text("Current patch: %s",installed.empty()?"unknown":installed.c_str());
    ImGui::Spacing();
    bool enabled=t3_update_enabled;
    if(ImGui::Checkbox("Check GitHub for updates at every launcher start",&enabled))
        t3_update_save_preference(enabled);
    ImGui::TextColored(col(th.text_muted),"Updates install from FishB0nes98/Tekken3Recompiled releases.");
    ImGui::Spacing();
    if(ImGui::Button("CHECK FOR UPDATES",ImVec2(190,38)))t3_update_spawn("check");
    ImGui::SameLine();
    if(t3_update_phase=="available") {
        ImGui::PushStyleColor(ImGuiCol_Button,col(th.accent));
        if(ImGui::Button("INSTALL UPDATE",ImVec2(190,38))) {
            t3_update_spawn("install",t3_update_version);
        }
        ImGui::PopStyleColor();
    } else if(t3_update_phase=="checking" || t3_update_phase=="install" || t3_update_phase=="downloading" ||
              t3_update_phase=="verifying" || t3_update_phase=="building") {
        ImGui::TextUnformatted("Working...");
    }
    if(!t3_update_message.empty()) {
        ImGui::Spacing();ImGui::TextWrapped("%s",t3_update_message.c_str());
    }
    if(t3_update_total && (t3_update_phase=="downloading" || t3_update_phase=="verifying"))
        ImGui::ProgressBar((float)std::min(1.0,(double)t3_update_received/(double)t3_update_total),ImVec2(-1,18));
    if(t3_update_phase=="available") {
        ImGui::TextColored(col(th.good),"Available patch: %s",t3_update_version.c_str());
        ImGui::TextColored(col(th.text_muted),"Files download here. The launcher then closes to install and rebuild, and reopens when ready.");
    }
    ImGui::Dummy(ImVec2(0,12));ImGui::Separator();ImGui::Dummy(ImVec2(0,8));
    t3_heading(t3_update_phase=="available"?"AVAILABLE PATCH NOTES":"CURRENT PATCH");
    std::string notes;
    if(t3_update_phase=="available") {
        notes=t3_update_notes;
    } else {
        std::string version=installed;
        if(!version.empty() && (version[0]=='v' || version[0]=='V'))version.erase(0,1);
        if(!version.empty() && std::all_of(version.begin(),version.end(),[](char c) {
            return (c>='0' && c<='9') || c=='.';
        }))notes=t3_read_text(t3_update_root/("CHANGELOG_v"+version+".md"));
        if(notes.empty()) {
            const std::string current=t3_read_text(t3_update_root/".setup"/"current-patch.json");
            if(t3_json_string(current,"version")=="v"+version)
                notes=t3_json_string(current,"notes");
        }
    }
    if(notes.empty())notes="Changelog for this patch is not available yet.";
    ImGui::BeginChild("PatchNotes",ImVec2(0,std::max(120.f,ImGui::GetContentRegionAvail().y)),ImGuiChildFlags_Borders);
    ImGui::TextWrapped("%s",notes.c_str());
    ImGui::EndChild();
}
static void tekken3_draw_launcher(LauncherModel* m,const LauncherTheme& th,int,int) {
    if(t3_update_auto_pending) {
        t3_update_auto_pending=false;t3_update_spawn("check");
    }
    t3_update_poll();
    if(t3_update_phase=="ready-to-apply")m->action=LNG_ACTION_QUIT;
    if(t3_observed_disc!=m->rom_full) {
        t3_observed_disc=m->rom_full;
        if(t3_discs.error.empty()) {
            t3_discs.attach(m->rom_full);t3_discs.selected=tekken3::DiscLibrary::normalize(m->rom_full);t3_discs.save();
        }
    }
    m->s.skip_launcher=0;
    const auto* vp=ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);ImGui::SetNextWindowSize(vp->Size);
    ImGui::Begin("##TekkenLauncher",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|
        ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::PushFont(g_t3_title);ImGui::TextUnformatted("TEKKEN 3");ImGui::PopFont();
    ImGui::SameLine();ImGui::SetCursorPosY(43);
    ImGui::TextColored(col(th.text_muted),"RECOMPILED  /  PLAYSTATION");
    if(t3_update_phase=="available") {
        ImGui::SameLine();ImGui::SetCursorPosX(std::max(420.f,ImGui::GetWindowWidth()-250.f));
        if(ImGui::SmallButton("UPDATE AVAILABLE"))t3_goto(m,T3_UPDATES);
    }
    ImGui::SetCursorPosY(87);
    ImVec2 rule=ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddRectFilled(rule,ImVec2(rule.x+ImGui::GetContentRegionAvail().x,rule.y+3),imcol(th.accent));
    ImGui::Dummy(ImVec2(0,17));
    const float body_h=std::max(200.f,ImGui::GetContentRegionAvail().y-77);
    const float nav_w=vp->Size.x<1050?164.f:202.f;
    ImGui::BeginChild("Menu",ImVec2(nav_w,body_h),ImGuiChildFlags_None);
    ImGui::TextColored(col(th.text_muted),"MAIN MENU");ImGui::Dummy(ImVec2(0,12));
    const char* names[]={"PLAY","MOD MANAGER","KEYBOARD","CONTROLLER","DISC LIBRARY","SETTINGS","MEMORY CARDS","ONLINE","PATCHES & UPDATES"};
    for(int page=0;page<9;page++) {
        if(page==T3_ONLINE && !m->netplay_supported)continue;
        const bool selected=(int)t3_page==page;
        if(selected)ImGui::PushStyleColor(ImGuiCol_Button,col(th.accent_dim));
        else ImGui::PushStyleColor(ImGuiCol_Button,col(th.background));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign,ImVec2(.08f,.5f));
        if(ImGui::Button(names[page],ImVec2(-1,44)))t3_goto(m,(T3Page)page);
        ImGui::PopStyleVar();ImGui::PopStyleColor();
    }
    ImGui::Dummy(ImVec2(0,20));
    ImGui::TextColored(col(th.text_muted),"LOCAL SETUP");
    ImGui::TextWrapped("Shown before every game launch.");
    ImGui::EndChild();ImGui::SameLine(0,20);
    ImGui::BeginChild("Content",ImVec2(0,body_h),ImGuiChildFlags_Borders);
    switch(t3_page) {
        case T3_PLAY:t3_home(m,th);break;
        case T3_MODS:
            t3_heading("MOD MANAGER");
            ImGui::TextColored(col(th.text_muted),"Install packages, choose features and review compatibility.");
            ImGui::Spacing();if(m->mods)draw_mods(m,th);else ImGui::TextDisabled("Mod service unavailable in this build.");break;
        case T3_KEYBOARD:t3_keyboard(m,th);break;
        case T3_CONTROLLER:t3_heading("CONTROLLER MAPPING");t3_player_selector(m);draw_controller(m,th);break;
        case T3_DISCS:t3_library(m,th);break;
        case T3_SETTINGS:t3_settings(m,th);break;
        case T3_SAVES:t3_heading("MEMORY CARDS");panel_save_draw(m,&th);break;
        case T3_ONLINE:draw_netplay(m,th);break;
        case T3_UPDATES:t3_updates(m,th);break;
    }
    ImGui::EndChild();
    ImGui::Dummy(ImVec2(0,12));ImGui::Separator();ImGui::Dummy(ImVec2(0,5));
    if(t3_page==T3_ONLINE) {
        draw_footer(m,th,58);
    } else {
    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(col(th.text_muted),"Setup applies on launch.  |  Esc cancels mapping.");
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-330);
    if(ImGui::Button("EXIT",ImVec2(100,42)))m->action=LNG_ACTION_QUIT;
    ImGui::SameLine();
    const bool ready=(launcher_model_can_launch(m)||launcher_model_bios_blocks_play(m)) && t3_compatible(m);
    ImGui::BeginDisabled(!ready || m->capturing || m->hk_capturing);
    ImGui::PushStyleColor(ImGuiCol_Button,col(th.accent));
    if(ImGui::Button("LAUNCH GAME",ImVec2(210,42))) {
        if(launcher_model_bios_blocks_play(m))launcher_model_bios_play_prompt(m);
        else if(mod_commit_launch(m))m->action=LNG_ACTION_LAUNCH;
        else t3_goto(m,T3_MODS);
    }
    ImGui::PopStyleColor();ImGui::EndDisabled();
    }
    draw_setup_wizard_modal(m,th);draw_bios_confirm_modal(m,th);draw_bios_play_modal(m,th);
    draw_pgo_confirm_modal(m,th);draw_fmv_timing_confirm_modal(m,th);
    draw_standalone_builtin_rom_picker(m,th);draw_restore_defaults_modal(m);
    draw_netplay_player_modal(m);draw_netplay_network_modal(m,th);draw_netplay_host_modal(m,th);
    draw_netplay_password_modal(m,th);draw_netplay_direct_modal(m,th);draw_netplay_room_modal(m,th);
    if(!t3_update_asked)ImGui::OpenPopup("Automatic updates?");
    if(ImGui::BeginPopupModal("Automatic updates?",nullptr,ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextWrapped("Check GitHub for new Tekken 3 patches whenever this launcher opens?");
        ImGui::TextWrapped("You can change this later in Patches & Updates. Installing a patch is always your choice.");
        if(ImGui::Button("Enable checks",ImVec2(150,36))) {
            t3_update_save_preference(true);ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button("Not now",ImVec2(120,36))) {
            t3_update_save_preference(false);ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}
