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
#include "algorithm"
#include "functional"
#include "regex"
#include "locale"
#include "codecvt"

using namespace std;
namespace fs = std::filesystem;

struct CarAttributes {
    std::string Marka = "";
    std::string Model = "";
    int Generacja = 0;
    std::string Nadwozie = "";
    int Liczba_miejsc = 0;
    double Srednica_zawracania = 0.0;
    double Dlugosc = 0.0;
    double Szerokosc = 0.0;
    double Wysokosc = 0.0;
    double Rozstaw_osi = 0.0;
    double Przeswit = 0.0;
    double Pojemnosc_Bagaznika = 0.0;
    double Pojemnosc_Silnika = 0.0;
    std::string Typ_silnika = "";
    double Moc_silnika = 0.0;
    double Moment_obrotowy_silnika = 0.0;
    std::string Montaz_silnika = "";
    std::string Doladowanie = "";
    int Liczba_cylindrow = 0;
    std::string Rodzaj_wtrysku = "";
    std::string Rodzaj_skrzyni_biegow = "";
    int Liczba_biegow = 0;
    std::string Naped = "";
    double Predkosc_maksymalna = 0.0;
    double Przyspieszenie = 0.0;
    double Srednie_spalanie = 0.0;
    double Pojemnosc_zbiornika_paliwa = 0.0;
    double Zasieg = 0.0;
    double Emisja_Co2 = 0.0;
    double Masa = 0.0;
};
std::vector<CarAttributes> cars;
static std::string current_user = "";
static std::string current_email = "";
static std::string current_birth_date = "";

namespace MojaApka
{
    // Funkcja szyfruj¹ca has³o
    std::string EncryptPassword(const std::string& password) {
        std::string encrypted;
        int shift = 3;  // Szyfr Cezara
        for (char ch : password) {
            encrypted += isalnum(ch) ? ch + shift : ch;
        }
        return encrypted;
    }

    // Funkcja deszyfruj¹ca has³o
    std::string DecryptPassword(const std::string& encryptedPassword) {
        std::string decrypted;
        int shift = 3;  // Szyfr Cezara
        for (char ch : encryptedPassword) {
            decrypted += isalnum(ch) ? ch - shift : ch;
        }
        return decrypted;
    }

    bool IsLoginUnique(const std::string& login) {
        std::ifstream file("users.txt");
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(",");
            if (pos != std::string::npos && line.substr(0, pos) == login) {
                return false;
            }
        }
        return true;
    }

    bool VerifyLogin(const std::string& login, const std::string& password) {
        std::ifstream file("users.txt");
        std::string line;
        while (std::getline(file, line)) {
            size_t pos_login = line.find(",");
            size_t pos_password = line.find_last_of(",");
            if (pos_login != std::string::npos && pos_password != std::string::npos) {
                std::string stored_login = line.substr(0, pos_login);
                std::string encrypted_password = line.substr(pos_password + 1);
                if (stored_login == login && DecryptPassword(encrypted_password) == password) {
                    return true;
                }
            }
        }
        return false;
    }

    // Funkcja waliduj¹ca has³o
    bool IsPasswordValid(const std::string& password) {
        std::regex password_regex(R"((?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{7,})");
        return std::regex_match(password, password_regex);
    }

    // Funkcja zapisuj¹ca dane u¿ytkownika do pliku
    void SaveUserData(const std::string& login, const std::string& email, const std::string& password, const std::string& birth_date) {
        std::ofstream file("users.txt", std::ios::app);
        if (file.is_open()) {
            file << login << "," << email << "," << birth_date << "," << EncryptPassword(password) << "\n";
        }
    }

    int partition(std::vector<CarAttributes>& cars, int low, int high,
        const std::function<bool(const CarAttributes&, const CarAttributes&)>& comparator) {
        CarAttributes pivot = cars[high];
        int i = low - 1;

        for (int j = low; j < high; ++j) {
            if (comparator(cars[j], pivot)) {
                ++i;
                std::swap(cars[i], cars[j]);
            }
        }
        std::swap(cars[i + 1], cars[high]);
        return i + 1;
    }

    void quicksort(std::vector<CarAttributes>& cars, int low, int high,
        const std::function<bool(const CarAttributes&, const CarAttributes&)>& comparator) {
        if (low < high) {
            int pivotIndex = partition(cars, low, high, comparator);
            quicksort(cars, low, pivotIndex - 1, comparator);
            quicksort(cars, pivotIndex + 1, high, comparator);
        }
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

        static bool show_login = true;
        static bool show_register = false;
        static bool show_search = false;
        static bool show_profile = false;
        static bool show_profile_bar = false;
        static bool show_filtersort = false;
        static bool show_add = false;

        static int choice_filtering = 0;
        static int choice_min_max = 0;
        static std::string search = "";
        static double filtering_parameter = 0.0;
        static std::vector<CarAttributes> filtered_cars;

        if (show_login == true) {
            ImGui::Begin("Logowanie", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

            // Rozmiar okna i obliczanie pozycji centralnej
            ImVec2 window_size = ImGui::GetWindowSize();
            ImVec2 content_size = ImGui::GetContentRegionAvail();
            float center_x = (content_size.x - 200) / 2.0f; // 200 to szerokoœæ elementów (dopasuj do potrzeb)

            // Œrodek pionowy
            ImGui::SetCursorPosY(content_size.y * 0.2f); // 20% poni¿ej górnej krawêdzi

            // Zawartoœæ okna
            static char login[128] = "";
            static char password[128] = "";
            static std::string message = "";

            // Wycentrowanie pola "Login"
            ImGui::SetCursorPosX(center_x);
            ImGui::Text("Login");
            ImGui::SetCursorPosX(center_x);
            ImGui::InputText("##login", login, IM_ARRAYSIZE(login));

            // Wycentrowanie pola "Has³o"
            ImGui::SetCursorPosX(center_x);
            ImGui::Text("Haslo");
            ImGui::SetCursorPosX(center_x);
            ImGui::InputText("##password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);

            // Wyœwietlanie komunikatu b³êdu
            if (!message.empty()) {
                ImGui::SetCursorPosX(center_x);
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", message.c_str());
            }

            // Przyciski "Zaloguj" i "Zarejestruj" obok siebie
            ImGui::SetCursorPosX(center_x);
            if (ImGui::Button("Zaloguj", ImVec2(100, 30))) {
                if (VerifyLogin(login, password)) {
                    current_user = login;

                    // Odczyt danych z pliku
                    std::ifstream file("users.txt");
                    std::string line;
                    while (std::getline(file, line)) {
                        std::istringstream iss(line);
                        std::string stored_login, stored_email, birth_date, stored_password;

                        if (std::getline(iss, stored_login, ',') &&
                            std::getline(iss, stored_email, ',') &&
                            std::getline(iss, birth_date, ',') &&
                            std::getline(iss, stored_password)) {
                            if (stored_login == login) {
                                current_email = stored_email;
                                current_birth_date = birth_date;
                                break;
                            }
                        }
                    }
                    file.close();

                    show_login = false;
                    show_search = true;
                    show_profile_bar = true;
                }
                else {
                    message = "Podany login lub haslo s¹ niepoprawne.";
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Zarejestruj", ImVec2(100, 30))) {
                show_register = true;
                show_login = false;
            }

            // Przycisk "Wyjœcie" na œrodku pod przyciskami
            ImGui::SetCursorPosX(center_x);
            if (ImGui::Button("Wyjscie", ImVec2(208, 30))) {
                exit(0);
            }

            ImGui::End();
        }

        if (show_register == true) {
            ImGui::Begin("Rejestracja");

            // Rozmiar okna i pozycjonowanie
            ImVec2 window_size = ImGui::GetWindowSize();
            float center_x = window_size.x / 2.0f;

            // Zmienne dla pól wejœciowych
            static char login[128] = "";
            static char email[128] = "";
            static char password[128] = "";
            static char confirm_password[128] = "";
            static std::string error_message = "";

            // Zmienne dla daty urodzenia
            static int day = 1;
            static int selected_month = 0; // Indeks wybranego miesi¹ca
            static int year = 2000; // Domyœlny rok
            const char* months[] = {
                "Styczen", "Luty", "Marzec", "Kwiecien", "Maj", "Czerwiec",
                "Lipiec", "Sierpien", "Wrzesien", "Pazdziernik", "Listopad", "Grudzien"
            };

            // Szerokoœæ inputów
            float input_width = 200.0f; // Nowa szerokoœæ inputów
            // Offset dla labeli
            float login_offset = center_x - (input_width / 2) + 80.0;
            float email_offset = center_x - (input_width / 2) + 80.0;
            float password_offset = center_x - (input_width / 2) + 80.0;
            float confirm_password_offset = center_x - (input_width / 2) + 55.0;
            float date_offset = center_x - (input_width / 2) + 50.0;
            float error_offset = center_x - (input_width / 2) + 10.0;
            float input_offset = center_x - (input_width / 2);         // Offset dla inputów

            // Nag³ówek
            ImGui::SetCursorPosX(center_x - 95.0f);
            ImGui::Text("Podaj dane do rejestracji:");

            // Login
            ImGui::SetCursorPosX(login_offset);
            ImGui::Text("Login");
            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            ImGui::InputText("##login", login, IM_ARRAYSIZE(login));

            // Email
            ImGui::SetCursorPosX(email_offset);
            ImGui::Text("Email");
            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            ImGui::InputText("##email", email, IM_ARRAYSIZE(email));

            // Has³o
            ImGui::SetCursorPosX(password_offset);
            ImGui::Text("Haslo");
            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            ImGui::InputText("##password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);

            // Powtórz has³o
            ImGui::SetCursorPosX(confirm_password_offset);
            ImGui::Text("Powtorz haslo");
            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            ImGui::InputText("##confirm_password", confirm_password, IM_ARRAYSIZE(confirm_password), ImGuiInputTextFlags_Password);

            // Data urodzenia
            ImGui::SetCursorPosX(date_offset);
            ImGui::Text("Data urodzenia");

            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            if (ImGui::BeginCombo("Dzien", std::to_string(day).c_str())) {
                for (int i = 1; i <= 31; i++) {
                    bool is_selected = (day == i);
                    if (ImGui::Selectable(std::to_string(i).c_str(), is_selected)) {
                        day = i;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            if (ImGui::BeginCombo("Miesiac", months[selected_month])) {
                for (int i = 0; i < 12; i++) {
                    bool is_selected = (selected_month == i);
                    if (ImGui::Selectable(months[i], is_selected)) {
                        selected_month = i;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            if (ImGui::BeginCombo("Rok", std::to_string(year).c_str())) {
                for (int i = 2025; i >= 1900; i--) {
                    bool is_selected = (year == i);
                    if (ImGui::Selectable(std::to_string(i).c_str(), is_selected)) {
                        year = i;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            // Przycisk "Zarejestruj"
            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            if (ImGui::Button("Zarejestruj", ImVec2(input_width, 30.0f))) {
                if (std::string(password) != std::string(confirm_password)) {
                    error_message = "Hasla sie nie zgadzaja!";
                }
                else if (!IsLoginUnique(login)) {
                    error_message = "Login jest juz zajety!";
                }
                else if (!IsPasswordValid(password)) {
                    error_message = "Haslo nie spelnia wymagan!";
                }
                else {
                    // Formatowanie daty urodzenia
                    std::string birth_date = std::to_string(day) + "-" + months[selected_month] + "-" + std::to_string(year);

                    SaveUserData(login, email, password, birth_date); // Zapisz dane z dat¹ urodzenia
                    error_message = "Rejestracja zakonczona!";
                    show_register = false;
                    show_login = true;
                }
            }

            // Wyœwietlanie b³êdów
            ImGui::SetCursorPosX(error_offset);
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", error_message.c_str());

            // Przycisk "Logowanie"
            ImGui::SetCursorPosX(input_offset);
            ImGui::SetNextItemWidth(input_width);
            if (ImGui::Button("Logowanie", ImVec2(input_width, 30.0f))) {
                show_register = false;
                show_login = true;
            }

            ImGui::End();
        }

        if (show_search == true)
        {
            if (show_profile_bar) {
                ImGui::Begin("Pasek profilu");

                // Oblicz dostêpne miejsce i przesuñ kursor do prawej krawêdzi
                float button_width = 100.0f; // Szerokoœæ przycisku (dopasuj do swoich potrzeb)
                float available_width = ImGui::GetContentRegionAvail().x;
                ImGui::SetCursorPosX(available_width - button_width);

                if (ImGui::Button("Profil", ImVec2(button_width, 30.0f))) { // Dodaj wymiary przycisku
                    show_profile_bar = false;
                    show_profile = true;
                    ImGui::SetWindowFocus("Profil");
                }

                ImGui::End();
            }

            ImGui::Begin("Wyszukiwarka", &show_search);

            ImGui::Text("Witaj w naszej bazie danych!");
            //ImGui::Text("Test polskich znaków: ¹, æ, ê, ³, ñ, ó, œ, Ÿ, ¿");
            static char tekst[128] = "";
            static string wyswietlany_tekst = "";

            // Wczytywanie zawartoœci wielu plików
            static const std::string folder_path = "F:\\Project_PI_FINAL\\Projekt-PI\\examples\\example_win32_directx12\\Baza danych\\Dane\\"; //Baza danych\\Dane
            static std::vector<std::string> lines;

            if (refresh_files) {
                cars.clear();
                for (const auto& entry : fs::directory_iterator(folder_path)) {
                    if (entry.is_regular_file()) {
                        std::ifstream file(entry.path().string());
                        if (file.is_open()) {
                            CarAttributes car;
                            std::string line;
                            std::getline(file, car.Marka);
                            std::getline(file, car.Model);
                            file >> car.Generacja;
                            file.ignore(); // Ignore newline after int
                            std::getline(file, car.Nadwozie);
                            file >> car.Liczba_miejsc;
                            file.ignore();
                            file >> car.Srednica_zawracania;
                            file.ignore();
                            file >> car.Dlugosc;
                            file.ignore();
                            file >> car.Szerokosc;
                            file.ignore();
                            file >> car.Wysokosc;
                            file.ignore();
                            file >> car.Rozstaw_osi;
                            file.ignore();
                            file >> car.Przeswit;
                            file.ignore();
                            file >> car.Pojemnosc_Bagaznika;
                            file.ignore();
                            file >> car.Pojemnosc_Silnika;
                            file.ignore();
                            std::getline(file, car.Typ_silnika);
                            file >> car.Moc_silnika;
                            file.ignore();
                            file >> car.Moment_obrotowy_silnika;
                            file.ignore();
                            std::getline(file, car.Montaz_silnika);
                            std::getline(file, car.Doladowanie);
                            file >> car.Liczba_cylindrow;
                            file.ignore();
                            std::getline(file, car.Rodzaj_wtrysku);
                            std::getline(file, car.Rodzaj_skrzyni_biegow);
                            file >> car.Liczba_biegow;
                            file.ignore();
                            std::getline(file, car.Naped);
                            file >> car.Predkosc_maksymalna;
                            file.ignore();
                            file >> car.Przyspieszenie;
                            file.ignore();
                            file >> car.Srednie_spalanie;
                            file >> car.Pojemnosc_zbiornika_paliwa;
                            file.ignore();
                            file >> car.Zasieg;
                            file.ignore();
                            file >> car.Emisja_Co2;
                            file >> car.Masa;
                            file.ignore();

                            cars.push_back(car);
                            file.close();
                        }
                    }
                }
                refresh_files = false;
            }

            ImGui::Text("Co chcesz wyszukac?");
            if (ImGui::InputText("##InputFiled", tekst, IM_ARRAYSIZE(tekst), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
            {
                wyswietlany_tekst = tekst;
            }

            if (ImGui::Button("Wyszukaj")) {
                wyswietlany_tekst = tekst;
            }

            if (ImGui::Button("Filtruj i sortuj")) {
                show_filtersort = true;
                ImGui::SetWindowFocus("Filtruj i sortuj");
            }

            if (ImGui::Button("Dodaj auto")) {
                show_add = true;
                ImGui::SetWindowFocus("Dodaj auto");
            }

            ImGui::Separator();

            // Filtrowanie zawartoœci pliku na podstawie wyszukiwanego tekstu
            if (ImGui::BeginTable("Car Table", 30, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX)) {
                ImGui::TableSetupScrollFreeze(2, 0);
                // Nag³ówki tabeli
                ImGui::TableSetupColumn("Marka");
                ImGui::TableSetupColumn("Model");
                ImGui::TableSetupColumn("Generacja");
                ImGui::TableSetupColumn("Nadwozie");
                ImGui::TableSetupColumn("Liczba miejsc");
                ImGui::TableSetupColumn("Srednica zawracania");
                ImGui::TableSetupColumn("Dlugosc");
                ImGui::TableSetupColumn("Szerokosc");
                ImGui::TableSetupColumn("Wysokosc");
                ImGui::TableSetupColumn("Rozstaw osi");
                ImGui::TableSetupColumn("Przeswit");
                ImGui::TableSetupColumn("Pojemnosc bagaznika");
                ImGui::TableSetupColumn("Pojemnosc silnika");
                ImGui::TableSetupColumn("Typ silnika");
                ImGui::TableSetupColumn("Moc silnika");
                ImGui::TableSetupColumn("Moment obrotowy silnika");
                ImGui::TableSetupColumn("Montaz silnika");
                ImGui::TableSetupColumn("Doladowanie");
                ImGui::TableSetupColumn("Liczba cylindrow");
                ImGui::TableSetupColumn("Rodzaj wtrysku");
                ImGui::TableSetupColumn("Rodzaj skrzyni biegow");
                ImGui::TableSetupColumn("Liczba biegow");
                ImGui::TableSetupColumn("Naped");
                ImGui::TableSetupColumn("Predkosc maksymalna");
                ImGui::TableSetupColumn("Przyspieszenie");
                ImGui::TableSetupColumn("Srednie spalanie");
                ImGui::TableSetupColumn("Pojemnosc zbiornika paliwa");
                ImGui::TableSetupColumn("Zasieg");
                ImGui::TableSetupColumn("Emisja CO2");
                ImGui::TableSetupColumn("Masa");
                ImGui::TableHeadersRow();

                // Dane samochodów
                for (const auto& car : cars) {
                    bool match = true;

                    // Filtrowanie wed³ug wyboru u¿ytkownika
                    if (choice_filtering == 1 && !search.empty() && car.Marka != search) {
                        match = false;
                    }
                    else if (choice_filtering == 2 && !search.empty() && car.Model != search) {
                        match = false;
                    }
                    else if (choice_filtering == 3) {
                        if ((choice_min_max == 1 && car.Moc_silnika < filtering_parameter) ||
                            (choice_min_max == 2 && car.Moc_silnika > filtering_parameter)) {
                            match = false;
                        }
                    }
                    else if (choice_filtering == 4) {
                        if ((choice_min_max == 1 && car.Liczba_cylindrow < filtering_parameter) ||
                            (choice_min_max == 2 && car.Liczba_cylindrow > filtering_parameter)) {
                            match = false;
                        }
                    }

                    // Wyszukiwanie w wierszu
                    std::ostringstream car_info;
                    car_info << car.Marka << " " << car.Model << " " << car.Generacja << " "
                        << car.Nadwozie << " " << car.Liczba_miejsc << " " << car.Srednica_zawracania << " "
                        << car.Dlugosc << " " << car.Szerokosc << " " << car.Wysokosc << " "
                        << car.Rozstaw_osi << " " << car.Przeswit << " " << car.Pojemnosc_Bagaznika << " "
                        << car.Pojemnosc_Silnika << " " << car.Typ_silnika << " " << car.Moc_silnika << " "
                        << car.Moment_obrotowy_silnika << " " << car.Montaz_silnika << " " << car.Doladowanie << " "
                        << car.Liczba_cylindrow << " " << car.Rodzaj_wtrysku << " " << car.Rodzaj_skrzyni_biegow << " "
                        << car.Liczba_biegow << " " << car.Naped << " " << car.Predkosc_maksymalna << " "
                        << car.Przyspieszenie << " " << car.Srednie_spalanie << " " << car.Pojemnosc_zbiornika_paliwa
                        << " " << car.Zasieg << " " << car.Emisja_Co2 << " " << car.Masa;

                    if (!wyswietlany_tekst[0] == '\0' && car_info.str().find(wyswietlany_tekst) == std::string::npos) {
                        match = false;
                    }

                    if (match) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(car.Marka.c_str());
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(car.Model.c_str());
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", car.Generacja);
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextUnformatted(car.Nadwozie.c_str());
                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("%d", car.Liczba_miejsc);
                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%.2f", car.Srednica_zawracania);
                        ImGui::TableSetColumnIndex(6);
                        ImGui::Text("%.2f", car.Dlugosc);
                        ImGui::TableSetColumnIndex(7);
                        ImGui::Text("%.2f", car.Szerokosc);
                        ImGui::TableSetColumnIndex(8);
                        ImGui::Text("%.2f", car.Wysokosc);
                        ImGui::TableSetColumnIndex(9);
                        ImGui::Text("%.2f", car.Rozstaw_osi);
                        ImGui::TableSetColumnIndex(10);
                        ImGui::Text("%.2f", car.Przeswit);
                        ImGui::TableSetColumnIndex(11);
                        ImGui::Text("%.2f", car.Pojemnosc_Bagaznika);
                        ImGui::TableSetColumnIndex(12);
                        ImGui::Text("%.2f", car.Pojemnosc_Silnika);
                        ImGui::TableSetColumnIndex(13);
                        ImGui::TextUnformatted(car.Typ_silnika.c_str());
                        ImGui::TableSetColumnIndex(14);
                        ImGui::Text("%.2f", car.Moc_silnika);
                        ImGui::TableSetColumnIndex(15);
                        ImGui::Text("%.2f", car.Moment_obrotowy_silnika);
                        ImGui::TableSetColumnIndex(16);
                        ImGui::TextUnformatted(car.Montaz_silnika.c_str());
                        ImGui::TableSetColumnIndex(17);
                        ImGui::TextUnformatted(car.Doladowanie.c_str());
                        ImGui::TableSetColumnIndex(18);
                        ImGui::Text("%d", car.Liczba_cylindrow);
                        ImGui::TableSetColumnIndex(19);
                        ImGui::TextUnformatted(car.Rodzaj_wtrysku.c_str());
                        ImGui::TableSetColumnIndex(20);
                        ImGui::TextUnformatted(car.Rodzaj_skrzyni_biegow.c_str());
                        ImGui::TableSetColumnIndex(21);
                        ImGui::Text("%d", car.Liczba_biegow);
                        ImGui::TableSetColumnIndex(22);
                        ImGui::TextUnformatted(car.Naped.c_str());
                        ImGui::TableSetColumnIndex(23);
                        ImGui::Text("%.2f", car.Predkosc_maksymalna);
                        ImGui::TableSetColumnIndex(24);
                        ImGui::Text("%.2f", car.Przyspieszenie);
                        ImGui::TableSetColumnIndex(25);
                        ImGui::Text("%.2f", car.Srednie_spalanie);
                        ImGui::TableSetColumnIndex(26);
                        ImGui::Text("%.2f", car.Pojemnosc_zbiornika_paliwa);
                        ImGui::TableSetColumnIndex(27);
                        ImGui::Text("%.2f", car.Zasieg);
                        ImGui::TableSetColumnIndex(28);
                        ImGui::Text("%.2f", car.Emisja_Co2);
                        ImGui::TableSetColumnIndex(29);
                        ImGui::Text("%.2f", car.Masa);
                    }
                }

                ImGui::EndTable();
            }

            if (show_filtersort == true) {
                ImGui::Begin("Filtruj i sortuj", &show_filtersort);

                static int selected_sort_option = -1; // -1 means no option selected

                ImGui::Text("Wybierz jak chcesz posortowac \nsamochody:");

                ImGui::RadioButton("Liczba miejsc, rosnaco", &selected_sort_option, 1);
                ImGui::RadioButton("Liczba miejsc, malejaco", &selected_sort_option, 2);
                ImGui::RadioButton("Producent A-Z", &selected_sort_option, 3);
                ImGui::RadioButton("Producent Z-A", &selected_sort_option, 4);
                ImGui::RadioButton("Model A-Z", &selected_sort_option, 5);
                ImGui::RadioButton("Model Z-A", &selected_sort_option, 6);
                ImGui::RadioButton("Generacja, rosnaco", &selected_sort_option, 7);
                ImGui::RadioButton("Generacja, malejaco", &selected_sort_option, 8);
                ImGui::RadioButton("Moc, rosnaco", &selected_sort_option, 9);
                ImGui::RadioButton("Moc, malejaco", &selected_sort_option, 10);
                ImGui::RadioButton("Masa, rosnaco", &selected_sort_option, 11);

                if (ImGui::Button("Zastosuj")) {
                    if (selected_sort_option != -1) {
                        switch (selected_sort_option) {
                        case 1: // Liczba miejsc, rosn¹co
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Liczba_miejsc < b.Liczba_miejsc;
                                });
                            break;
                        case 2: // Liczba miejsc, malej¹co
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Liczba_miejsc > b.Liczba_miejsc;
                                });
                            break;
                        case 3: // Producent A-Z (Marka)
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Marka < b.Marka;
                                });
                            break;
                        case 4: // Producent Z-A (Marka)
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Marka > b.Marka;
                                });
                            break;
                        case 5: // Model A-Z
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Model < b.Model;
                                });
                            break;
                        case 6: // Model Z-A
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Model > b.Model;
                                });
                            break;
                        case 7: // Generacja rosn¹co
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Generacja < b.Generacja;
                                });
                            break;
                        case 8: // Generacja malej¹co
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Generacja > b.Generacja;
                                });
                            break;
                        case 9: // Moc silnika rosn¹co
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Moc_silnika < b.Moc_silnika;
                                });
                            break;
                        case 10: // Moc silnika malej¹co
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Moc_silnika > b.Moc_silnika;
                                });
                            break;
                        case 11: // Masa rosn¹co
                            quicksort(cars, 0, cars.size() - 1, [](const CarAttributes& a, const CarAttributes& b) {
                                return a.Masa < b.Masa;
                                });
                            break;
                        }
                    }
                }

                ImGui::Separator();

                ImGui::Text("Wybierz, jak chcesz filtrowac:");
                ImGui::RadioButton("Marka", &choice_filtering, 1);
                ImGui::RadioButton("Model", &choice_filtering, 2);
                ImGui::RadioButton("Moc", &choice_filtering, 3);
                ImGui::RadioButton("Iloœæ cylindrow", &choice_filtering, 4);

                if (choice_filtering == 1) { // Filtrowanie po marce
                    ImGui::Text("Podaj marke:");
                    static char marka_buffer[128] = "";
                    if (ImGui::InputText("##Marka", marka_buffer, IM_ARRAYSIZE(marka_buffer))) {
                        search = marka_buffer;
                    }
                }

                if (choice_filtering == 2) { // Filtrowanie po modelu
                    ImGui::Text("Podaj model:");
                    static char model_buffer[128] = "";
                    if (ImGui::InputText("##Model", model_buffer, IM_ARRAYSIZE(model_buffer))) {
                        search = model_buffer;
                    }
                }

                if (choice_filtering == 3) { // Filtrowanie po mocy
                    ImGui::Text("Wybierz zakres:");
                    ImGui::RadioButton("Minimalna moc", &choice_min_max, 1);
                    ImGui::RadioButton("Maksymalna moc", &choice_min_max, 2);
                    ImGui::Text("Podaj wartosc mocy:");
                    ImGui::InputDouble("##Moc", &filtering_parameter);
                }

                if (choice_filtering == 4) { // Filtrowanie po iloœci cylindrów
                    ImGui::Text("Wybierz zakres:");
                    ImGui::RadioButton("Minimalna ilosc cylindrow", &choice_min_max, 1);
                    ImGui::RadioButton("Maksymalna ilosc cylindrow", &choice_min_max, 2);
                    ImGui::Text("Podaj wartosc:");
                    ImGui::InputDouble("##Cylindry", &filtering_parameter);
                }

                ImGui::Separator();

                if (ImGui::Button("Wroc"))
                {
                    show_filtersort = false;
                    selected_sort_option = -1;
                    choice_filtering = 0;
                }

                ImGui::End();
            }

            if (show_add == true) {
                ImGui::Begin("Dodaj auto", &show_add);

                static CarAttributes carAttributes;

                ImGui::Text("Marka:");
                static char marka_buffer[128];
                strcpy(marka_buffer, carAttributes.Marka.c_str());
                if (ImGui::InputText("##Marka", marka_buffer, IM_ARRAYSIZE(marka_buffer))) {
                    carAttributes.Marka = marka_buffer;
                }

                ImGui::Text("Model:");
                static char model_buffer[128];
                strcpy(model_buffer, carAttributes.Model.c_str());
                if (ImGui::InputText("##Model", model_buffer, IM_ARRAYSIZE(model_buffer))) {
                    carAttributes.Model = model_buffer;
                }

                ImGui::Text("Generacja:");
                ImGui::InputInt("##Generacja", &carAttributes.Generacja);

                ImGui::Text("Nadwozie:");
                static char nadwozie_buffer[128];
                strcpy(nadwozie_buffer, carAttributes.Nadwozie.c_str());
                if (ImGui::InputText("##Nadwozie", nadwozie_buffer, IM_ARRAYSIZE(nadwozie_buffer))) {
                    carAttributes.Nadwozie = nadwozie_buffer;
                }

                ImGui::Text("Liczba miejsc:");
                ImGui::InputInt("##Liczba_miejsc", &carAttributes.Liczba_miejsc);

                ImGui::Text("Srednica zawracania:");
                ImGui::InputDouble("##Srednica_zawracania", &carAttributes.Srednica_zawracania);

                ImGui::Text("Dlugosc:");
                ImGui::InputDouble("##Dlugosc", &carAttributes.Dlugosc);

                ImGui::Text("Szerokosc:");
                ImGui::InputDouble("##Szerokosc", &carAttributes.Szerokosc);

                ImGui::Text("Wysokosc:");
                ImGui::InputDouble("##Wysokosc", &carAttributes.Wysokosc);

                ImGui::Text("Rozstaw osi:");
                ImGui::InputDouble("##Rozstaw_osi", &carAttributes.Rozstaw_osi);

                ImGui::Text("Przeswit:");
                ImGui::InputDouble("##Przeswit", &carAttributes.Przeswit);

                ImGui::Text("Pojemnosc Bagaznika:");
                ImGui::InputDouble("##Pojemnosc_Bagaznika", &carAttributes.Pojemnosc_Bagaznika);

                ImGui::Text("Pojemnosc Silnika:");
                ImGui::InputDouble("##Pojemnosc_Silnika", &carAttributes.Pojemnosc_Silnika);

                ImGui::Text("Typ silnika:");
                static char typ_silnika_buffer[128];
                strcpy(typ_silnika_buffer, carAttributes.Typ_silnika.c_str());
                if (ImGui::InputText("##Typ_silnika", typ_silnika_buffer, IM_ARRAYSIZE(typ_silnika_buffer))) {
                    carAttributes.Typ_silnika = typ_silnika_buffer;
                }

                ImGui::Text("Moc silnika:");
                ImGui::InputDouble("##Moc_silnika", &carAttributes.Moc_silnika);

                ImGui::Text("Moment obrotowy silnika:");
                ImGui::InputDouble("##Moment_obrotowy_silnika", &carAttributes.Moment_obrotowy_silnika);

                ImGui::Text("Montaz silnika:");
                static char montaz_silnika_buffer[128];
                strcpy(montaz_silnika_buffer, carAttributes.Montaz_silnika.c_str());
                if (ImGui::InputText("##Montaz_silnika", montaz_silnika_buffer, IM_ARRAYSIZE(montaz_silnika_buffer))) {
                    carAttributes.Montaz_silnika = montaz_silnika_buffer;
                }

                ImGui::Text("Doladowanie:");
                static char doladowanie_buffer[128];
                strcpy(doladowanie_buffer, carAttributes.Doladowanie.c_str());
                if (ImGui::InputText("##Doladowanie", doladowanie_buffer, IM_ARRAYSIZE(doladowanie_buffer))) {
                    carAttributes.Doladowanie = doladowanie_buffer;
                }

                ImGui::Text("Liczba cylindrow:");
                ImGui::InputInt("##Liczba_cylindrow", &carAttributes.Liczba_cylindrow);

                ImGui::Text("Rodzaj wtrysku:");
                static char rodzaj_wtrysku_buffer[128];
                strcpy(rodzaj_wtrysku_buffer, carAttributes.Rodzaj_wtrysku.c_str());
                if (ImGui::InputText("##Rodzaj_wtrysku", rodzaj_wtrysku_buffer, IM_ARRAYSIZE(rodzaj_wtrysku_buffer))) {
                    carAttributes.Rodzaj_wtrysku = rodzaj_wtrysku_buffer;
                }

                ImGui::Text("Rodzaj skrzyni biegow:");
                static char rodzaj_skrzyni_biegow_buffer[128];
                strcpy(rodzaj_skrzyni_biegow_buffer, carAttributes.Rodzaj_skrzyni_biegow.c_str());
                if (ImGui::InputText("##Rodzaj_skrzyni_biegow", rodzaj_skrzyni_biegow_buffer, IM_ARRAYSIZE(rodzaj_skrzyni_biegow_buffer))) {
                    carAttributes.Rodzaj_skrzyni_biegow = rodzaj_skrzyni_biegow_buffer;
                }

                ImGui::Text("Liczba biegow:");
                ImGui::InputInt("##Liczba_biegow", &carAttributes.Liczba_biegow);

                ImGui::Text("Naped:");
                static char naped_buffer[128];
                strcpy(naped_buffer, carAttributes.Naped.c_str());
                if (ImGui::InputText("##Naped", naped_buffer, IM_ARRAYSIZE(naped_buffer))) {
                    carAttributes.Naped = naped_buffer;
                }

                ImGui::Text("Predkosc maksymalna:");
                ImGui::InputDouble("##Predkosc_maksymalna", &carAttributes.Predkosc_maksymalna);

                ImGui::Text("Przyspieszenie:");
                ImGui::InputDouble("##Przyspieszenie", &carAttributes.Przyspieszenie);

                ImGui::Text("Srednia spalania:");
                ImGui::InputDouble("##Srednia_spalania", &carAttributes.Srednie_spalanie);

                ImGui::Text("Pojemnosc zbiornika paliwa:");
                ImGui::InputDouble("##Pojemnosc_zbiornika_paliwa", &carAttributes.Pojemnosc_zbiornika_paliwa);

                ImGui::Text("Zasieg:");
                ImGui::InputDouble("##Zasieg", &carAttributes.Zasieg);

                ImGui::Text("Emisja Co2:");
                ImGui::InputDouble("##Emisja_Co2", &carAttributes.Emisja_Co2);

                ImGui::Text("Masa:");
                ImGui::InputDouble("##Masa", &carAttributes.Masa);

                static bool show_error_message = false;

                if (ImGui::Button("Dodaj"))
                {
                    bool all_fields_filled = true;
                    // SprawdŸ wszystkie pola typu std::string
                    for (const auto& field : { carAttributes.Marka, carAttributes.Model, carAttributes.Nadwozie, carAttributes.Typ_silnika, carAttributes.Montaz_silnika, carAttributes.Doladowanie, carAttributes.Rodzaj_wtrysku, carAttributes.Rodzaj_skrzyni_biegow, carAttributes.Naped }) {
                        if (field.empty()) {
                            all_fields_filled = false;
                            break;
                        }
                    }

                    // SprawdŸ wszystkie pola numeryczne (int i double)
                    if (all_fields_filled) {
                        if (carAttributes.Generacja == 0 || carAttributes.Liczba_miejsc == 0 || carAttributes.Srednica_zawracania == 0.0 ||
                            carAttributes.Dlugosc == 0.0 || carAttributes.Szerokosc == 0.0 || carAttributes.Wysokosc == 0.0 ||
                            carAttributes.Rozstaw_osi == 0.0 || carAttributes.Przeswit == 0.0 || carAttributes.Pojemnosc_Bagaznika == 0.0 ||
                            carAttributes.Pojemnosc_Silnika == 0.0 || carAttributes.Moc_silnika == 0.0 || carAttributes.Moment_obrotowy_silnika == 0.0 ||
                            carAttributes.Liczba_cylindrow == 0 || carAttributes.Liczba_biegow == 0 || carAttributes.Predkosc_maksymalna == 0.0 ||
                            carAttributes.Przyspieszenie == 0.0 || carAttributes.Srednie_spalanie == 0.0 || carAttributes.Pojemnosc_zbiornika_paliwa == 0.0 || carAttributes.Zasieg == 0.0 ||
                            carAttributes.Emisja_Co2 == 0.0 || carAttributes.Masa == 0.0) {
                            all_fields_filled = false;
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

            if (ImGui::Button("Wyjsce"))
            {
                //show_search = false;
                //show_login = true;
                exit(0);
            }

            ImGui::End();
        }

        if (show_profile == true)
        {
            ImGui::Begin("Profil", &show_profile);

            ImGui::Text("Twoj profil");

            ImGui::Text("Zalogowany jako:");
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", current_user.c_str());

            ImGui::Text("Email:");
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", current_email.c_str());

            ImGui::Text("Data urodzenia:");
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", current_birth_date.c_str());

            ImGui::Separator();
            if (ImGui::Button("Edytuj"))
            {
                // Funkcjonalnoœæ edycji profilu
            }

            if (ImGui::Button("Wroc"))
            {
                show_profile = false;
                show_profile_bar = true;
            }

            if (ImGui::Button("Wyloguj"))
            {
                current_user = "";
                current_email = "";
                current_birth_date = ""; // Resetowanie danych u¿ytkownika po wylogowaniu
                show_search = false;
                show_profile = false;
                show_login = true;
            }
            ImGui::End();
        }

        ImGui::End();
    }
}
