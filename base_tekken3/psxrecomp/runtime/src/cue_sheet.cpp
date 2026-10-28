// cue_sheet.cpp — see cue_sheet.h.

#include "cue_sheet.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

namespace PSXRecompV4 {

namespace {

std::string upper_ascii(std::string s) {
    for (char& c : s) c = (char)std::toupper((unsigned char)c);
    return s;
}

bool equal_ci(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (std::toupper((unsigned char)a[i]) != std::toupper((unsigned char)b[i])) return false;
    }
    return true;
}

// Extract the FILE line's payload name and type token. Handles both the quoted
// form (`FILE "game.bin" BINARY`) and the bare form (`FILE game.bin BINARY`)
// that some hand-written cues use.
bool parse_file_line(const std::string& line, size_t file_kw_pos,
                     std::string& out_name, std::string& out_type) {
    const size_t after_kw = file_kw_pos + 4;  // strlen("FILE")
    const size_t q1 = line.find('"', after_kw);
    size_t rest_pos;

    if (q1 != std::string::npos) {
        const size_t q2 = line.find('"', q1 + 1);
        if (q2 == std::string::npos || q2 <= q1 + 1) return false;
        out_name = line.substr(q1 + 1, q2 - q1 - 1);
        rest_pos = q2 + 1;
    } else {
        std::istringstream iss(line.substr(after_kw));
        if (!(iss >> out_name) || out_name.empty()) return false;
        rest_pos = after_kw;
        // Advance past the name we just consumed so the type token scan below
        // starts after it.
        const size_t name_at = line.find(out_name, after_kw);
        if (name_at != std::string::npos) rest_pos = name_at + out_name.size();
    }

    std::istringstream rest(line.substr(rest_pos));
    out_type.clear();
    rest >> out_type;
    return true;
}

}  // namespace

bool CueSheet::usable() const {
    if (!opened || files.empty()) return false;
    for (const CueFileRef& f : files) {
        if (!f.is_binary || !f.exists) return false;
    }
    return true;
}

bool CueSheet::has_non_binary_file() const {
    for (const CueFileRef& f : files) {
        if (!f.is_binary) return true;
    }
    return false;
}

size_t CueSheet::data_file_index() const {
    for (const CueTrackRef& t : tracks) {
        if (!t.is_audio && t.file_index < files.size()) return t.file_index;
    }
    return 0;
}

bool path_has_extension_ci(const fs::path& p, const char* dotted_ext) {
    return equal_ci(p.extension().string(), std::string(dotted_ext));
}

fs::path resolve_cue_relative_file(const fs::path& dir, const std::string& name) {
    fs::path named(name);
    // Cues written on Windows may use backslashes; std::filesystem on POSIX
    // treats those as ordinary characters, so normalize them first.
    if (name.find('\\') != std::string::npos) {
        std::string slashed = name;
        for (char& c : slashed) if (c == '\\') c = '/';
        named = fs::path(slashed);
    }

    const fs::path literal = named.is_absolute() ? named : (dir / named);
    std::error_code ec;
    if (fs::exists(literal, ec)) return literal;

    // Case-insensitive fallback: scan the containing directory for a filename
    // that differs only by case.
    const fs::path parent = literal.parent_path();
    const std::string want = literal.filename().string();
    ec.clear();
    if (fs::is_directory(parent, ec)) {
        ec.clear();
        for (fs::directory_iterator it(parent, ec), end; !ec && it != end; it.increment(ec)) {
            if (equal_ci(it->path().filename().string(), want)) return it->path();
        }
    }
    return literal;
}

CueSheet parse_cue_sheet(const fs::path& cue_path) {
    CueSheet sheet;

    std::ifstream cue(cue_path);
    if (!cue.is_open()) return sheet;
    sheet.opened = true;

    const fs::path dir = cue_path.parent_path();

    std::string line;
    int      cur_track_num   = -1;
    bool     cur_track_audio = false;
    uint32_t cur_index00     = 0;
    bool     cur_has_index00 = false;

    while (std::getline(cue, line)) {
        // Strip a trailing CR so cues with CRLF endings parse on POSIX.
        if (!line.empty() && line.back() == '\r') line.pop_back();

        const std::string upper = upper_ascii(line);

        const size_t file_pos = upper.find("FILE");
        if (file_pos != std::string::npos) {
            std::string name, type;
            if (parse_file_line(line, file_pos, name, type)) {
                CueFileRef ref;
                ref.name      = name;
                ref.is_binary = equal_ci(type, "BINARY");
                ref.path      = resolve_cue_relative_file(dir, name);
                std::error_code ec;
                ref.exists    = fs::exists(ref.path, ec) && !fs::is_directory(ref.path, ec);
                sheet.files.push_back(std::move(ref));
            }
            // A FILE line resets any track state carried from the previous file.
            cur_track_num   = -1;
            cur_has_index00 = false;
            cur_index00     = 0;
            continue;
        }

        int  tn = 0;
        char type_buf[32] = {0};
        if (std::sscanf(upper.c_str(), " TRACK %d %31s", &tn, type_buf) == 2) {
            cur_track_num   = tn;
            cur_track_audio = (std::string(type_buf).find("AUDIO") != std::string::npos);
            cur_index00     = 0;
            cur_has_index00 = false;
            continue;
        }

        int idx = 0, mm = 0, ss = 0, ff = 0;
        if (std::sscanf(upper.c_str(), " INDEX %d %d:%d:%d", &idx, &mm, &ss, &ff) == 4 &&
            cur_track_num >= 1 && !sheet.files.empty()) {
            const uint32_t index_lba = (uint32_t)(((mm * 60 + ss) * 75) + ff);
            if (idx == 0) {
                cur_index00     = index_lba;
                cur_has_index00 = true;
                continue;
            }
            if (idx != 1) continue;
            CueTrackRef t;
            t.number      = cur_track_num;
            t.is_audio    = cur_track_audio;
            t.file_index  = sheet.files.size() - 1;
            t.index01     = index_lba;
            t.index00     = cur_index00;
            t.has_index00 = cur_has_index00;
            sheet.tracks.push_back(t);
            cur_track_num = -1;
        }
    }

    // Smart fallback: When users rename their .bin files or .cue on an SD card
    // without updating the internal FILE strings in the .cue, match the missing
    // FILE references to binary image files present in the same directory.
    bool any_missing = false;
    for (const auto& f : sheet.files) {
        if (!f.exists) {
            any_missing = true;
            break;
        }
    }

    if (any_missing && !sheet.files.empty()) {
        std::vector<fs::path> dir_bins;
        std::error_code ec;
        for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
            if (it->is_regular_file(ec)) {
                std::string ext = it->path().extension().string();
                if (equal_ci(ext, ".bin") || equal_ci(ext, ".img") || equal_ci(ext, ".iso")) {
                    dir_bins.push_back(it->path());
                    printf("[CueSheet] Candidato en directorio: '%s' (%llu bytes)\n",
                           it->path().filename().string().c_str(),
                           (unsigned long long)fs::file_size(it->path(), ec));
                }
            }
        }

        auto natural_path_less = [](const fs::path& pa, const fs::path& pb) {
            std::string a = pa.filename().string();
            std::string b = pb.filename().string();
            size_t i = 0, j = 0;
            while (i < a.size() && j < b.size()) {
                if (std::isdigit(static_cast<unsigned char>(a[i])) &&
                    std::isdigit(static_cast<unsigned char>(b[j]))) {
                    unsigned long long na = 0, nb = 0;
                    while (i < a.size() && std::isdigit(static_cast<unsigned char>(a[i]))) {
                        na = na * 10 + (a[i] - '0');
                        i++;
                    }
                    while (j < b.size() && std::isdigit(static_cast<unsigned char>(b[j]))) {
                        nb = nb * 10 + (b[j] - '0');
                        j++;
                    }
                    if (na != nb) return na < nb;
                } else {
                    char ca = std::tolower(static_cast<unsigned char>(a[i]));
                    char cb = std::tolower(static_cast<unsigned char>(b[j]));
                    if (ca != cb) return ca < cb;
                    i++; j++;
                }
            }
            return a.size() < b.size();
        };

        if (sheet.files.size() == 1 && !dir_bins.empty()) {
            // Single-file cue: pick the largest binary file in the directory
            auto best = std::max_element(dir_bins.begin(), dir_bins.end(),
                [](const fs::path& a, const fs::path& b) {
                    std::error_code ec1, ec2;
                    return fs::file_size(a, ec1) < fs::file_size(b, ec2);
                });
            if (best != dir_bins.end()) {
                printf("[CueSheet] Auto-asociando archivo único: '%s' -> '%s'\n",
                       sheet.files[0].name.c_str(), best->filename().string().c_str());
                sheet.files[0].path = *best;
                sheet.files[0].exists = true;
                sheet.files[0].is_binary = true;
            }
        } else if (sheet.files.size() <= dir_bins.size()) {
            // Multi-file cue: Sort available bins by size descending so the
            // primary data track (600+ MB) is first.
            std::sort(dir_bins.begin(), dir_bins.end(),
                [](const fs::path& a, const fs::path& b) {
                    std::error_code ec1, ec2;
                    return fs::file_size(a, ec1) > fs::file_size(b, ec2);
                });

            size_t data_file_idx = sheet.data_file_index();
            printf("[CueSheet] Auto-asociando track de datos: '%s' -> '%s' (%llu bytes)\n",
                   sheet.files[data_file_idx].name.c_str(), dir_bins[0].filename().string().c_str(),
                   (unsigned long long)fs::file_size(dir_bins[0], ec));

            sheet.files[data_file_idx].path = dir_bins[0];
            sheet.files[data_file_idx].exists = true;
            sheet.files[data_file_idx].is_binary = true;

            // Audio tracks: sort remaining candidate bins using natural sort (biden1 < biden2 < biden10)
            std::vector<fs::path> audio_bins(dir_bins.begin() + 1, dir_bins.end());
            std::sort(audio_bins.begin(), audio_bins.end(), natural_path_less);

            // Si los candidatos de audio incluyen la pista de musica real (27.7 MB) y la pista de relleno silencioso (28.0 MB),
            // o si una contiene muestras de audio y la otra es silencio digital (ceros), asegurarse de que el Track 2 reciba
            // la pista con la musica de los creditos y el Track 3 el relleno.
            if (audio_bins.size() >= 2) {
                std::error_code ec_a, ec_b;
                uint64_t sz0 = fs::file_size(audio_bins[0], ec_a);
                uint64_t sz1 = fs::file_size(audio_bins[1], ec_b);
                if (sz0 == 28042896ULL && sz1 == 27701856ULL) {
                    std::swap(audio_bins[0], audio_bins[1]);
                    printf("[CueSheet] Ajustando orden de pistas de audio por coincidencia exacta con Tekken 3 (Track 2: 27.7MB, Track 3: 28MB)\n");
                } else {
                    // Comprobar contenido de sectores pasados el pregap (sector 200) para asegurar que Track 2 tenga audio real
                    std::ifstream f0(audio_bins[0].string(), std::ios::binary);
                    std::ifstream f1(audio_bins[1].string(), std::ios::binary);
                    if (f0.is_open() && f1.is_open()) {
                        char buf0[4096] = {0}, buf1[4096] = {0};
                        f0.seekg(2352 * 200, std::ios::beg);
                        f1.seekg(2352 * 200, std::ios::beg);
                        f0.read(buf0, sizeof(buf0));
                        f1.read(buf1, sizeof(buf1));
                        bool b0_nz = false, b1_nz = false;
                        for (size_t k = 0; k < sizeof(buf0); k++) if (buf0[k]) { b0_nz = true; break; }
                        for (size_t k = 0; k < sizeof(buf1); k++) if (buf1[k]) { b1_nz = true; break; }
                        if (!b0_nz && b1_nz) {
                            std::swap(audio_bins[0], audio_bins[1]);
                            printf("[CueSheet] Ajustando orden de pistas: '%s' contiene audio real -> asignada a Track 2\n",
                                   audio_bins[0].filename().string().c_str());
                        }
                    }
                }
            }

            size_t audio_bin_idx = 0;
            for (size_t i = 0; i < sheet.files.size() && audio_bin_idx < audio_bins.size(); ++i) {
                if (i == data_file_idx) continue;
                printf("[CueSheet] Auto-asociando track de audio #%zu: '%s' -> '%s' (%llu bytes)\n",
                       i + 1, sheet.files[i].name.c_str(), audio_bins[audio_bin_idx].filename().string().c_str(),
                       (unsigned long long)fs::file_size(audio_bins[audio_bin_idx], ec));
                sheet.files[i].path = audio_bins[audio_bin_idx++];
                sheet.files[i].exists = true;
                sheet.files[i].is_binary = true;
            }
        }
    }

    return sheet;
}

}  // namespace PSXRecompV4
