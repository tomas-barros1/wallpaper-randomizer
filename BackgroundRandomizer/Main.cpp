#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <random>
#include <algorithm>
#include <nlohmann/json.hpp>

#include <windows.h>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

const string configFileName = "wallpapersDir.json";
const string lastFileName = "lastWallpaper.json";

// normaliza barras invertidas para barras normais
string normalizePath(string path) {
    replace(path.begin(), path.end(), '\\', '/');
    return path;
}

bool isImageFile(const fs::path& p) {
    static const vector<string> validExt = { ".jpg", ".jpeg", ".png", ".bmp" };
    string ext = p.extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return find(validExt.begin(), validExt.end(), ext) != validExt.end();
}

// pergunta o caminho no console, normaliza e salva no json
string askAndSaveDir() {
    string dir;
    cout << "Nenhum diretorio de wallpapers configurado ainda." << endl;
    cout << "Digite o caminho completo da pasta com as imagens (ex: D:\\Wallpapers ou D:/Wallpapers): ";
    getline(cin, dir);

    dir = normalizePath(dir);

    json j;
    j["wallpapersDir"] = dir;

    ofstream out(configFileName);
    out << j.dump(4);
    out.close();

    cout << "Salvo em " << configFileName << "!" << endl;
    return dir;
}

bool loadWallpapersDir(string& outDir) {
    if (!fs::exists(configFileName)) {
        outDir = askAndSaveDir();
    }
    else {
        try {
            ifstream in(configFileName);
            json j;
            in >> j;
            in.close();
            outDir = j.value("wallpapersDir", "");
            outDir = normalizePath(outDir);
        }
        catch (const json::parse_error& e) {
            cout << "Arquivo " << configFileName << " estava corrompido. Vamos recriar." << endl;
            outDir = askAndSaveDir();
        }
    }

    if (outDir.empty() || !fs::exists(outDir) || !fs::is_directory(outDir)) {
        cout << "O diretorio '" << outDir << "' nao existe ou e invalido." << endl;
        cout << "Apague o arquivo " << configFileName << " e rode o programa de novo para reconfigurar." << endl;
        return false;
    }

    return true;
}

string loadLastWallpaper() {
    if (!fs::exists(lastFileName)) return "";

    try {
        ifstream in(lastFileName);
        json j;
        in >> j;
        in.close();
        string last = j.value("lastWallpaper", "");
        return normalizePath(last);
    }
    catch (const json::parse_error&) {
        return "";
    }
}

void saveLastWallpaper(const string& path) {
    json j;
    j["lastWallpaper"] = normalizePath(path);

    ofstream out(lastFileName);
    out << j.dump(4);
    out.close();
}

bool setWallpaper(const string& path) {
    fs::path absPath = fs::absolute(path);
    wstring widePath = absPath.wstring();

    BOOL result = SystemParametersInfoW(
        SPI_SETDESKWALLPAPER,
        0,
        (PVOID)widePath.c_str(),
        SPIF_UPDATEINIFILE | SPIF_SENDCHANGE
    );

    return result == TRUE;
}

int main() {
    try {
        string wallpapersDir;
        if (!loadWallpapersDir(wallpapersDir)) {
            return 0;
        }

        vector<string> images;
        for (const auto& entry : fs::directory_iterator(wallpapersDir)) {
            if (entry.is_regular_file() && isImageFile(entry.path())) {
                images.push_back(normalizePath(entry.path().string()));
            }
        }

        if (images.empty()) {
            cout << "Nenhuma imagem encontrada em: " << wallpapersDir << endl;
            return 0;
        }

        string lastWallpaper = loadLastWallpaper();

        vector<string> candidates = images;
        if (images.size() > 1 && !lastWallpaper.empty()) {
            candidates.erase(
                remove(candidates.begin(), candidates.end(), lastWallpaper),
                candidates.end()
            );
        }

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
        string chosen = candidates[dist(gen)];

        if (!setWallpaper(chosen)) {
            cout << "Falha ao definir o wallpaper. Erro: " << GetLastError() << endl;
            return 1;
        }

        saveLastWallpaper(chosen);
        cout << "Wallpaper alterado para: " << chosen << endl;
    }
    catch (const exception& e) {
        cout << "Erro inesperado: " << e.what() << endl;
        return 1;
    }

    return 0;
}