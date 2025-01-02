#include "Aplikacja.h"
#include "iostream"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "cstdlib"
#include "string"
#include "fstream"
#include "sstream"
#include "vector"
#include "filesystem"

using namespace std;
namespace fs = std::filesystem;


struct CarAttributes {
    char Marka[128] = "";
    char Model[128] = "";
    char Generacja[128] = "";
    char Nadwozie[128] = "";
    char Liczba_miejsc[128] = "";
    char Srednica_zawracania[128] = "";
    char Dlugosc[128] = "";
    char Szerokosc[128] = "";
    char Wysokosc[128] = "";
    char Rozstaw_osi[128] = "";
    char Przeswit[128] = "";
    char Pojemnosc_Bagaznika[128] = "";
    char Pojemnosc_Silnika[128] = "";
    char Typ_silnika[128] = "";
    char Moc_silnika[128] = "";
    char Moment_obrotowy_silnika[128] = "";
    char Montaz_silnika[128] = "";
    char Doladowanie[128] = "";
    char Liczba_cylindrow[128] = "";
    char Rodzaj_wtrysku[128] = "";
    char Rodzaj_skrzyni_biegow[128] = "";
    char Liczba_biegow[128] = "";
    char Naped[128] = "";
    char Predkosc_maksymalna[128] = "";
    char Przyspieszenie[128] = "";
    char Srednie_spalanie[128] = "";
    char Pojemnosc_zbiornika_paliwa[128] = "";
    char Zasieg[128] = "";
    char Emisja_Co2[128] = "";
    char Masa[128] = "";
};


namespace MojaApka
{
    const ImWchar* GetPolishGlyphRanges() {
        static const ImWchar polish_ranges[] = {
            0x0020, 0xFFFF, // Pe³ny zakres znaków Unicode
            0
        };
        return polish_ranges;
    }

    void SetupImGuiWithFont()
    {
        ImGuiIO& io = ImGui::GetIO();

        // Opcjonalnie wyczyœæ domyœlne czcionki
        io.Fonts->Clear();

        // Œcie¿ka do pliku czcionki (upewnij siê, ¿e wskazuje na odpowiedni plik .ttf)
        const char* fontPath = "G:\\Project_PI_FINAL\\Projekt-PI\\examples\\example_win32_directx12\\OpenSans-VariableFont_wdth,wght.ttf";

        ImFontConfig font_cfg;
        font_cfg.OversampleH = 3; // Lepsza jakoœæ pozioma
        font_cfg.OversampleV = 1; // Lepsza jakoœæ pionowa

        // Dodaj czcionkê z pe³nym zakresem znaków Unicode
        ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath, 16.0f, &font_cfg, GetPolishGlyphRanges());
        if (!font)
        {
            cerr << "Nie uda³o siê za³adowaæ czcionki: " << fontPath << endl;
        }

        // Zbuduj atlas czcionek
        unsigned char* tex_pixels = nullptr;
        int tex_width, tex_height;
        io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);

        // Mo¿esz tutaj przes³aæ dane tekstury do GPU, jeœli to konieczne dla renderera
        // Przyk³ad dla OpenGL:
        // GLuint tex_id;
        // glGenTextures(1, &tex_id);
        // glBindTexture(GL_TEXTURE_2D, tex_id);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_pixels);
    }

    static bool refresh_files = true;
    void RenderUI()
    {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_AutoHideTabBar;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        static bool show_search = false;
        static bool show_profile = false;
        static bool show_sort = false;
        static bool show_add = false;

        if (show_search == true)
        {
            ImGui::Begin("Wyszukiwarka", &show_search, ImGuiWindowFlags_HorizontalScrollbar);

            ImGui::Text("Witaj w naszej bazie danych!");
            ImGui::Text("Test polskich znaków: ¹, æ, ê, ³, ñ, ó, œ, Ÿ, ¿");
            static char tekst[128] = "";
            static string wyswietlany_tekst = "";

            // Wczytywanie zawartoœci wielu plików
            static const std::string folder_path = "G:\\Project_PI_FINAL\\Projekt-PI\\examples\\example_win32_directx12\\Baza danych\\Dane\\"; //Baza danych\\Dane
            static std::string file_content = "";

            if (refresh_files) {
                file_content.clear();
                for (const auto& entry : fs::directory_iterator(folder_path)) {
                    if (entry.is_regular_file()) {
                        std::ifstream file(entry.path().string());
                        if (file.is_open()) {
                            std::string line;
                            while (std::getline(file, line)) {
                                file_content += line + " | ";
                            }
                            file.close();
                            file_content += "\n"; // Separator miêdzy plikami
                        }
                        else {
                            file_content += "Nie mo¿na otworzyc pliku: " + entry.path().string() + "\n";
                        }
                    }
                }
                refresh_files = false;
            }

            static std::string file_kryteria = "";
            if (file_kryteria.empty()) {
                std::ifstream file("G:\\Project_PI_FINAL\\Projekt-PI\\examples\\example_win32_directx12\\Baza danych\\kryteria.txt");
                if (file.is_open()) {
                    std::string line;
                    while (std::getline(file, line)) {
                        file_kryteria += line + " | "; // Dodaj now¹ liniê po ka¿dym wierszu
                    }
                    file.close();
                }
                else {
                    file_kryteria = "Nie mo¿na otworzyc pliku.";
                }
            }

            // Dodanie InputText z placeholderem
            ImGui::Text("Co chcesz wyszukac?");
            if (ImGui::InputText("##InputFiled", tekst, IM_ARRAYSIZE(tekst), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
            {
                wyswietlany_tekst = tekst;
            }

            if (ImGui::Button("Wyszukaj")) {
                wyswietlany_tekst = tekst;
            }

            if (ImGui::Button("Filtruj")) {
                show_sort = true;
                ImGui::SetWindowFocus("Filtruj");
            }

            if (show_sort == true) {
                ImGui::Begin("Filtruj", &show_sort);

                if (ImGui::Button("Wroc"))
                {
                    show_sort = false;
                }

                ImGui::End();
            }

            if (ImGui::Button("Dodaj auto")) {
                show_add = true;
                ImGui::SetWindowFocus("Dodaj auto");
            }

            if (show_add == true) {
                ImGui::Begin("Dodaj auto", &show_add);

                static CarAttributes carAttributes;

                ImGui::Text("Marka:");
                ImGui::InputText("##Marka", carAttributes.Marka, IM_ARRAYSIZE(carAttributes.Marka), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Model:");
                ImGui::InputText("##Model", carAttributes.Model, IM_ARRAYSIZE(carAttributes.Model), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Generacja:");
                ImGui::InputText("##Generacja", carAttributes.Generacja, IM_ARRAYSIZE(carAttributes.Generacja), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Nadwozie:");
                ImGui::InputText("##Nadwozie", carAttributes.Nadwozie, IM_ARRAYSIZE(carAttributes.Nadwozie), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Liczba miejsc:");
                ImGui::InputText("##Liczba_miejsc", carAttributes.Liczba_miejsc, IM_ARRAYSIZE(carAttributes.Liczba_miejsc), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Srednica zawracania:");
                ImGui::InputText("##Srednica_zawracania", carAttributes.Srednica_zawracania, IM_ARRAYSIZE(carAttributes.Srednica_zawracania), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Dlugosc:");
                ImGui::InputText("##Dlugosc", carAttributes.Dlugosc, IM_ARRAYSIZE(carAttributes.Dlugosc), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Szerokosc:");
                ImGui::InputText("##Szerokosc", carAttributes.Szerokosc, IM_ARRAYSIZE(carAttributes.Szerokosc), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Wysokosc:");
                ImGui::InputText("##Wysokosc", carAttributes.Wysokosc, IM_ARRAYSIZE(carAttributes.Wysokosc), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Rozstaw osi:");
                ImGui::InputText("##Rozstaw_osi", carAttributes.Rozstaw_osi, IM_ARRAYSIZE(carAttributes.Rozstaw_osi), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Przeswit:");
                ImGui::InputText("##Przeswit", carAttributes.Przeswit, IM_ARRAYSIZE(carAttributes.Przeswit), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Pojemnosc Bagaznika:");
                ImGui::InputText("##Pojemnosc_Bagaznika", carAttributes.Pojemnosc_Bagaznika, IM_ARRAYSIZE(carAttributes.Pojemnosc_Bagaznika), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Pojemnosc Silnika:");
                ImGui::InputText("##Pojemnosc_Silnika", carAttributes.Pojemnosc_Silnika, IM_ARRAYSIZE(carAttributes.Pojemnosc_Silnika), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Typ silnika:");
                ImGui::InputText("##Typ_silnika", carAttributes.Typ_silnika, IM_ARRAYSIZE(carAttributes.Typ_silnika), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Moc silnika:");
                ImGui::InputText("##Moc_silnika", carAttributes.Moc_silnika, IM_ARRAYSIZE(carAttributes.Moc_silnika), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Moment obrotowy silnika:");
                ImGui::InputText("##Moment_obrotowy_silnika", carAttributes.Moment_obrotowy_silnika, IM_ARRAYSIZE(carAttributes.Moment_obrotowy_silnika), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Montaz silnika:");
                ImGui::InputText("##Montaz_silnika", carAttributes.Montaz_silnika, IM_ARRAYSIZE(carAttributes.Montaz_silnika), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Doladowanie:");
                ImGui::InputText("##Doladowanie", carAttributes.Doladowanie, IM_ARRAYSIZE(carAttributes.Doladowanie), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Liczba cylindrow:");
                ImGui::InputText("##Liczba_cylindrow", carAttributes.Liczba_cylindrow, IM_ARRAYSIZE(carAttributes.Liczba_cylindrow), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Rodzaj wtrysku:");
                ImGui::InputText("##Rodzaj_wtrysku", carAttributes.Rodzaj_wtrysku, IM_ARRAYSIZE(carAttributes.Rodzaj_wtrysku), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Rodzaj skrzyni biegow:");
                ImGui::InputText("##Rodzaj_skrzyni_biegow", carAttributes.Rodzaj_skrzyni_biegow, IM_ARRAYSIZE(carAttributes.Rodzaj_skrzyni_biegow), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Liczba biegow:");
                ImGui::InputText("##Liczba_biegow", carAttributes.Liczba_biegow, IM_ARRAYSIZE(carAttributes.Liczba_biegow), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Naped:");
                ImGui::InputText("##Naped", carAttributes.Naped, IM_ARRAYSIZE(carAttributes.Naped), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Predkosc maksymalna:");
                ImGui::InputText("##Predkosc_maksymalna", carAttributes.Predkosc_maksymalna, IM_ARRAYSIZE(carAttributes.Predkosc_maksymalna), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Przyspieszenie:");
                ImGui::InputText("##Przyspieszenie", carAttributes.Przyspieszenie, IM_ARRAYSIZE(carAttributes.Przyspieszenie), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Srednie spalanie:");
                ImGui::InputText("##Srednie_spalanie", carAttributes.Srednie_spalanie, IM_ARRAYSIZE(carAttributes.Srednie_spalanie), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Pojemnosc zbiornika paliwa:");
                ImGui::InputText("##Pojemnosc_zbiornika_paliwa", carAttributes.Pojemnosc_zbiornika_paliwa, IM_ARRAYSIZE(carAttributes.Pojemnosc_zbiornika_paliwa), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Zasieg:");
                ImGui::InputText("##Zasieg", carAttributes.Zasieg, IM_ARRAYSIZE(carAttributes.Zasieg), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Emisja Co2:");
                ImGui::InputText("##Emisja_Co2", carAttributes.Emisja_Co2, IM_ARRAYSIZE(carAttributes.Emisja_Co2), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Text("Masa:");
                ImGui::InputText("##Masa", carAttributes.Masa, IM_ARRAYSIZE(carAttributes.Masa), ImGuiInputTextFlags_CharsNoBlank);

                static bool show_error_message = false;

                if (ImGui::Button("Dodaj"))
                {
                    bool all_fields_filled = true;
                    for (auto& field : { carAttributes.Marka, carAttributes.Model, carAttributes.Generacja, carAttributes.Nadwozie, carAttributes.Liczba_miejsc, carAttributes.Srednica_zawracania, carAttributes.Dlugosc, carAttributes.Szerokosc, carAttributes.Wysokosc, carAttributes.Rozstaw_osi, carAttributes.Przeswit, carAttributes.Pojemnosc_Bagaznika, carAttributes.Pojemnosc_Silnika, carAttributes.Typ_silnika, carAttributes.Moc_silnika, carAttributes.Moment_obrotowy_silnika, carAttributes.Montaz_silnika, carAttributes.Doladowanie, carAttributes.Liczba_cylindrow, carAttributes.Rodzaj_wtrysku, carAttributes.Rodzaj_skrzyni_biegow, carAttributes.Liczba_biegow, carAttributes.Naped, carAttributes.Predkosc_maksymalna, carAttributes.Przyspieszenie, carAttributes.Srednie_spalanie, carAttributes.Pojemnosc_zbiornika_paliwa, carAttributes.Zasieg, carAttributes.Emisja_Co2, carAttributes.Masa }) {
                        if (strlen(field) == 0) {
                            all_fields_filled = false;
                            break;
                        }
                    }

                    if (all_fields_filled) {
                        int file_number = 51;
                        std::string file_name;
                        do {
                            file_name = folder_path + "car_" + std::to_string(file_number++) + ".txt";
                        } while (fs::exists(file_name));

                        std::ofstream file(file_name);

                        if (file.is_open()) {
                            file << carAttributes.Marka << "\n";
                            file << carAttributes.Model << "\n";
                            file << carAttributes.Generacja << "\n";
                            file << carAttributes.Nadwozie << "\n";
                            file << carAttributes.Liczba_miejsc << "\n";
                            file << carAttributes.Srednica_zawracania << "\n";
                            file << carAttributes.Dlugosc << "\n";
                            file << carAttributes.Szerokosc << "\n";
                            file << carAttributes.Wysokosc << "\n";
                            file << carAttributes.Rozstaw_osi << "\n";
                            file << carAttributes.Przeswit << "\n";
                            file << carAttributes.Pojemnosc_Bagaznika << "\n";
                            file << carAttributes.Pojemnosc_Silnika << "\n";
                            file << carAttributes.Typ_silnika << "\n";
                            file << carAttributes.Moc_silnika << "\n";
                            file << carAttributes.Moment_obrotowy_silnika << "\n";
                            file << carAttributes.Montaz_silnika << "\n";
                            file << carAttributes.Doladowanie << "\n";
                            file << carAttributes.Liczba_cylindrow << "\n";
                            file << carAttributes.Rodzaj_wtrysku << "\n";
                            file << carAttributes.Rodzaj_skrzyni_biegow << "\n";
                            file << carAttributes.Liczba_biegow << "\n";
                            file << carAttributes.Naped << "\n";
                            file << carAttributes.Predkosc_maksymalna << "\n";
                            file << carAttributes.Przyspieszenie << "\n";
                            file << carAttributes.Srednie_spalanie << "\n";
                            file << carAttributes.Pojemnosc_zbiornika_paliwa << "\n";
                            file << carAttributes.Zasieg << "\n";
                            file << carAttributes.Emisja_Co2 << "\n";
                            file << carAttributes.Masa << "\n";
                            file.close();
                        }
                        show_error_message = false;
                        refresh_files = true;
                    }
                    else {
                        show_error_message = true;
                    }
                }

                if (show_error_message) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Nie wszystkie pola sa wypelnione!");
                }

                if (ImGui::Button("Wroc"))
                {
                    show_add = false;
                }
 
                ImGui::End();
            }

            ImGui::Separator();
            ImGui::TextUnformatted(file_kryteria.c_str());

            // Filtrowanie zawartoœci pliku na podstawie wyszukiwanego tekstu
            ImGui::Separator();
            if (!wyswietlany_tekst.empty()) {
                std::istringstream file_stream(file_content);
                std::string line;
                while (std::getline(file_stream, line)) {
                    if (line.find(wyswietlany_tekst) != std::string::npos) {
                        ImGui::TextUnformatted(line.c_str());
                    }
                }
            }
            else {
                ImGui::TextUnformatted(file_content.c_str());
            }
            //ImGui::TextUnformatted(file_content.c_str());

            if (ImGui::Button("Wroc"))
            {
                show_search = false;
            }

            ImGui::End();
        }

        if (show_profile == true)
        {
            ImGui::Begin("Profil", &show_profile);

            ImGui::Text("Twoj profil");
            if (ImGui::Button("Edytuj"))
            {

            }

            if (ImGui::Button("Wroc"))
            {
                show_profile = false;
            }

            if (ImGui::Button("Wyloguj"))
            {

            }
            ImGui::End();
        }

        ImGui::Begin("Ustawienia");
        ImGui::Button("Witaj");
        ImGui::End();

        ImGui::Begin("Menu");
        if (ImGui::Button("Wyszukiwarka")) {
            show_search = true;
            ImGui::SetWindowFocus("Wyszukiwarka");
            refresh_files = true;
        }

        if (ImGui::Button("Profil")) {
            show_profile = true;
            ImGui::SetWindowFocus("Profil");
        }

        if (ImGui::Button("Wyjœcie")) {
            exit(0);
        }
        ImGui::End();

        ImGui::End();

    }
}
