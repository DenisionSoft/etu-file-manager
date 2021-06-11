#include <iostream>
#include <algorithm>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include <ctime>
#define _CRT_SECURE_NO_WARNINGS
#define Wait int cx = _getch();

using namespace std;
using namespace boost::filesystem;

void MenuContent(int& pos, string& path, int& fsize, string& clipboard); // Главное меню
void ContextMenuRender(int& pos, int& posCon, string& path, int& fsize, string& clipboard, bool& cut_flag);
void ContextMenuContent(int& posCon, string& path, string& pstr, int& fsize); // Контекстное меню
int Recalc(string& path, int& fsize); // Пересчёт количества элементов в папке
void new_file(string& path); // Создание нового файла
void new_dir(string& path); // Создание новой папки
void rm_file(string& path, string& file_name); // Удаление файла
void rm_dir(string& path, string& dir_name); // Удаление папки
void new_name(string& path, string& dir_name); // Переименование
void properties(string& path); // Меню свойства
void open_in_cmd(string& path); // Открытие папки в Терминале
void cut(string& path, string& clipboard, bool& cut_flag); // Вырезать
void copy(string& path, string& clipboard, bool& cut_flag); // Копировать
void paste(string& path, string& clipboard, bool& cut_flag, string& pstr2); // Вставить
void text_with_data(string& path); // Новый txt с текстом
void help(); // Страница помощи

int pos = 1; // Текущая позиция курсора
int posCon = 1; // Текущая позиция курсора в контекстном меню
int c = 0; // Нажатая клавиша в меню
int fsize = 0; // Количество элементов в текущей папке
bool update = false; // Флаг на обновление экрана
int page = 1; // Текущая страница
int pageV = 1; // Всего страниц
bool shdown = false; // Флаг на закрытие программы
string clipboard = ""; // Буфер обмена
bool cut_flag = false; // Флаг на вырезание файла

std::wstring s2ws(const std::string& s) // Функция string в wstring, используется для ShellExecute
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

int main() {
    system("color 8F");
    setlocale(0, "");
    string dir_name;
    string file_name;
    string path = "C:\\";
    system("cls");
    fsize = Recalc(path, fsize);
    MenuContent(pos, path, fsize, clipboard);
    while (1) {
        if (shdown) return 0;
        cin.clear();
        if (update) {
            update = false;
            c = 111;
        }
        else c = _getch();
        system("cls");
        if (c == 224) continue;
        switch (c) {
        case 72:
            if (pos > 1) {
                pos--;
                double tem = pos / 27.0;
                if ((int)tem != tem) page = (int)(pos / 27.0) + 1;
                else page = (int)tem;
                if (page == 0) page++;
            }
            else {
                pos = fsize;
                double tem = pos / 27.0;
                if ((int)tem != tem) page = (int)(pos / 27.0) + 1;
                else page = (int)tem;
                if (page == 0) page++;
            }
            break;
        case 80:
            if (pos < fsize) {
                pos++;
                double tem = pos / 27.0;
                if ((int)tem != tem) page = (int)(pos / 27.0) + 1;
                else page = (int)tem;
            }
            else {
                pos = 1;
                page = 1;
            }
            break;
        case 75:
            if (page > 1) page--;
            else page = pageV;
            pos = (page - 1) * 27 + 1;
            break;
        case 77:
            if (page < pageV) page++;
            else page = 1;
            pos = (page - 1) * 27 + 1;
            break;
        case 27: // "Esc"
            return 0;
        case 110: //"N"
            new_file(path);
            break;
        case 109: //"M"
            new_dir(path);
            break;
        case 114: //"R"
            system("cls");
            cout << "Введите название: ";
            cin >> file_name;
            cin.clear();
            rm_file(path, file_name);
            break;
        case 116: //"T"
            system("cls");
            cout << "Введите название: ";
            cin >> dir_name;
            cin.clear();
            rm_dir(path, dir_name);
            break;
        case 117: //"U"
            system("cls");
            cout << "Введите название: ";
            cin >> dir_name;
            cin.clear();
            new_name(path, dir_name);
            break;
        case 99: //"С"
            ContextMenuRender(pos, posCon, path, fsize, clipboard, cut_flag);
            break;
        case 104: //"H"
            help();
            break;
        case 13: { // "Enter"
            int count = 0;
            for (const auto& x : directory_iterator(path)) {
                count++;
                if (count == pos) {
                    string pstr = x.path().string();
                    boost::erase_all(pstr, path);
                    string extension = boost::filesystem::extension(x.path());
                    if (is_directory(x.path())) {
                        path = x.path().string();
                        pos = 1;
                        page = 1;
                        update = true;
                    }
                    else if (is_regular_file(x.path())) {
                        path = x.path().string();
                        wstring wstr = s2ws(path);
                        LPCWSTR lr = wstr.c_str();
                        ShellExecute(0, 0, lr, 0, 0, SW_SHOW);
                        if (path.length() != 3) {
                            int start = path.find_last_of('\\');
                            if (!path[start + 1]) path.replace(start, 1, "");
                            start = path.find_last_of('\\');
                            path.replace(start + 1, 100, "");
                            update = true;
                        }
                    }
                    break;
                }
            }
            break;
        }
        case 102: { // "F"
            int count = 0;
            for (const auto& x : directory_iterator(path)) {
                count++;
                if (count == pos) {
                    string pstr = x.path().string();
                    boost::erase_all(pstr, path);
                    if (is_directory(x.path())) {
                        string str = "open";
                        LPCSTR lr = str.c_str();
                        string str2 = x.path().string();
                        LPCSTR lr2 = str2.c_str();
                        ShellExecuteA(0, lr, lr2, 0, 0, SW_SHOW); // Открывает в проводнике
                    }
                    else if (is_regular_file(x.path())) {
                        string str = "open";
                        LPCSTR lr = str.c_str();
                        string str2 = path;
                        LPCSTR lr2 = str2.c_str();
                        ShellExecuteA(0, lr, lr2, 0, 0, SW_SHOW);
                    }
                    break;
                }
            }
            break;
        }
        case 8: { // "backspace"
            page = 1;
            if (path.length() != 3) {
                int start = path.find_last_of('\\');
                if (!path[start + 1]) path.replace(start, 1, "");
                start = path.find_last_of('\\');
                path.replace(start + 1, 100, "");
                pos = 1;
                update = true;
            }
            else { // Если каталог уже корневой
                system("cls");
                cout << "Введите новую метку корневого каталога: ";
                string disklet;
                cin.clear();
                cin >> disklet;
                string newpath = disklet + ":\\";
                try {
                    bool x = exists(newpath);
                    if (!x) while (!x) {
                        cout << "Извините, этого диска не существует, попробуйте снова: ";
                        disklet = "";
                        cin.clear();
                        cin >> disklet;
                        newpath = disklet + ":\\";
                        x = exists(newpath);
                        if (x) break;
                    }
                    path = newpath;
                    update = true;
                }
                catch (const filesystem_error& ex) {
                    cout << "Ошибка доступа";
                }
            }
            break;
        }
        }
        try {
            MenuContent(pos, path, fsize, clipboard);
        }
        catch (const filesystem_error& ex) {
            cout << "Ошибка доступа. Нажмите любую клавишу для возврата...";
            if (path.length() != 3) {
                int start = path.find_last_of('\\');
                if (!path[start + 1]) path.replace(start, 1, "");
                start = path.find_last_of('\\');
                path.replace(start + 1, 100, "");
                pos = 1;
            }
            else path = "C:\\";
            int ch = _getch();
        }
    }
    return 0;
}

void Render(int& pos, string& path, int& fsize, int& page, string& clipboard) {
    int count = 0;
    for (const auto& x : directory_iterator(path)) {
        string pstr = x.path().string();
        boost::erase_all(pstr, path);
        boost::erase_all(pstr, "\\");
        if (pstr.length() > 90) pstr = pstr.substr(0, 90);
        count++;
        if (page > 1 && count <= ((page - 1) * 27)) {
            continue;
        }
        if (count > page * 27) {
            continue;
        }
        string extension = boost::filesystem::extension(x.path());
        if (is_directory(x.path())) extension = "dir";
        boost::erase_all(extension, ".");
        bool NAF = false;
        if (extension != "dir" && is_regular_file(x.path())) NAF = true;
        string measure;
        long long filsize;
        if (NAF) {
            filsize = file_size(x.path());
            measure = " bytes";
            if (int(filsize / 1024) > 0) { filsize = long long(filsize / 1024); measure = " KB"; };
            if (int(filsize / 1024) > 0) { filsize = long long(filsize / 1024); measure = " MB"; };
            if (int(filsize / 1024) > 0) { filsize = long long(filsize / 1024); measure = " GB"; };
        }
        if (pos == count) {
            if (NAF) {

                cout << " > [" << extension << "] " << pstr << " | " << filsize << measure << endl;
            }
            else
                cout << " > [" << extension << "] " << pstr << endl;
        }
        else {
            if (NAF) {

                cout << "   [" << extension << "] " << pstr << " | " << filsize << measure << endl;
            }
            else
                cout << "   [" << extension << "] " << pstr << endl;
        }
    }
    if (count == 0) {
        cout << "   Пусто.";
    }
    int gap = 27 - (fsize - ((page - 1) * 27));
    for (gap; gap > 0; gap--) {
        cout << endl;
    }
    string opnlocal = clipboard;
    int start = opnlocal.find_last_of('\\');
    opnlocal.replace(0, start, "");
    boost::erase_all(opnlocal, "\\");
    if (opnlocal == "") cout << "Страница: " << page << "/" << pageV << " | Элементов: " << fsize << " | Буфер обмена: пуст" << endl;
    else cout << "Страница: " << page << "/" << pageV << " | Элементов: " << fsize << " | Буфер обмена: " << opnlocal << endl;
    cout << "H - страница помощи. C - вызов контекстного меню. Esc - выход.";
    count = 1;
}

void MenuContent(int& pos, string& path, int& fsize, string& clipboard) {
    fsize = Recalc(path, fsize);
    int altcount = 0;
    cout << path << endl;
    for (const auto& x : directory_iterator(path)) altcount++;
    if (altcount < 28) {
        Render(pos, path, fsize, page, clipboard);
        pageV = 1;
    }
    else {
        float tmp = altcount / 27.0;
        while (pageV < tmp) {
            pageV++;
        }
        Render(pos, path, fsize, page, clipboard);
    }
}

void ContextMenuRender(int& pos, int& posCon, string& path, int& fsize, string& clipboard, bool& cut_flag) {
    system("cls");
    fsize = Recalc(path, fsize);
    int altcount = 0;
    string pstr = "";
    for (const auto& x : directory_iterator(path)) {
        altcount++;
        if (altcount == pos) {
            pstr = x.path().string();
            break;
        }
    }
    string ogpathname = clipboard;
    int start = ogpathname.find_last_of('\\');
    ogpathname.replace(0, start, "");
    ContextMenuContent(posCon, path, pstr, fsize);
    while (1) {
        cin.clear();
        int cc = _getch();
        system("cls");
        if (cc == 224) continue;
        if (cc == 72) {
            if (posCon > 1) posCon--;
            else posCon = 14;
        }
        if (cc == 80) {
            if (posCon < 14) posCon++;
            else posCon = 1;
        }
        if (cc == 13)
            switch (posCon)
            {
            case 1:
                if (is_directory(pstr)) {
                    path = pstr;
                    pos = 1;
                    page = 1;
                    update = true;
                }
                else if (is_regular_file(pstr)) {
                    path = pstr;
                    wstring wstr = s2ws(path);
                    LPCWSTR lr = wstr.c_str();
                    ShellExecute(0, 0, lr, 0, 0, SW_SHOW);
                    if (path.length() != 3) {
                        int start = path.find_last_of('\\');
                        if (!path[start + 1]) path.replace(start, 1, "");
                        start = path.find_last_of('\\');
                        path.replace(start + 1, 100, "");
                        update = true;
                    }
                }
                posCon = 1;
                return;
                break;
            case 2:
                if (is_directory(pstr)) {
                    string str = "open";
                    LPCSTR lr = str.c_str();
                    string str2 = pstr;
                    LPCSTR lr2 = str2.c_str();
                    ShellExecuteA(0, lr, lr2, 0, 0, SW_SHOW);
                }
                else if (is_regular_file(pstr)) {
                    string str = "open";
                    LPCSTR lr = str.c_str();
                    string str2 = path;
                    LPCSTR lr2 = str2.c_str();
                    ShellExecuteA(0, lr, lr2, 0, 0, SW_SHOW);
                }
                posCon = 1;
                return;
                break;
            case 3:
                if (is_directory(pstr)) open_in_cmd(pstr);
                else { cout << "Это доступно только для папок. Нажмите любую клавишу для продолжения..."; Wait }
                posCon = 1;
                return;
                break;
            case 4:
                new_name(path, pstr);
                posCon = 1;
                return;
                break;
            case 5:
                if (is_directory(pstr)) rm_dir(path, pstr);
                else if (is_regular_file(pstr)) rm_file(path, pstr);
                posCon = 1;
                return;
                break;
            case 6:
                properties(pstr);
                posCon = 1;
                return;
                break;
            case 7:
                new_dir(path);
                posCon = 1;
                return;
                break;
            case 8:
                new_file(path);
                posCon = 1;
                return;
                break;
            case 9:
                text_with_data(path);
                posCon = 1;
                return;
                break;
            case 10:
                cut(pstr, clipboard, cut_flag);
                posCon = 1;
                return;
                break;
            case 11:
                copy(pstr, clipboard, cut_flag);
                posCon = 1;
                return;
                break;
            case 12:
                if (is_directory(pstr)) paste(pstr, clipboard, cut_flag, ogpathname);
                else paste(path, clipboard, cut_flag, ogpathname);
                posCon = 1;
                return;
                break;
            case 13:
                posCon = 1;
                return;
            case 14:
                shdown = true;
                system("cls");
                return;
            }
        ContextMenuContent(posCon, path, pstr, fsize);
    }
}

void ContextMenuContent(int& posCon, string& path, string& pstr, int& fsize) {
    cout << pstr << endl << endl;
    string pstr2 = pstr;
    boost::erase_all(pstr2, path);
    boost::erase_all(pstr2, "\\");
    if (pstr2.length() > 90) pstr2 = pstr2.substr(0, 90);
    string extension = boost::filesystem::extension(pstr);
    if (is_directory(pstr)) extension = "dir";
    boost::erase_all(extension, ".");
    bool NAF = false;
    if (extension != "dir" && is_regular_file(pstr)) NAF = true;
    string measure;
    long long filsize;
    if (NAF) {
        filsize = file_size(pstr);
        measure = " байт";
        if (int(filsize / 1024) > 0) { filsize = long long(filsize / 1024); measure = " КБ"; };
        if (int(filsize / 1024) > 0) { filsize = long long(filsize / 1024); measure = " МБ"; };
        if (int(filsize / 1024) > 0) { filsize = long long(filsize / 1024); measure = " ГБ"; };
    }
    if (NAF) cout << "   [" << extension << "] " << pstr2 << " | " << filsize << measure << endl;
    else cout << "   [" << extension << "] " << pstr2 << endl;

    cout << endl;
    if (posCon == 1) cout << " > Открыть" << endl;
    else cout << "   Открыть" << endl;
    if (posCon == 2) cout << " > Открыть в Проводнике Windows" << endl;
    else cout << "   Открыть в Проводнике Windows" << endl;
    if (posCon == 3) cout << " > Открыть в Терминале" << endl;
    else cout << "   Открыть в Терминале" << endl;
    if (posCon == 4) cout << " > Переименовать" << endl;
    else cout << "   Переименовать" << endl;
    if (posCon == 5) cout << " > Удалить" << endl;
    else cout << "   Удалить" << endl;
    if (posCon == 6) cout << " > Свойства" << endl;
    else cout << "   Свойства" << endl;
    cout << "   ----" << endl;
    if (posCon == 7) cout << " > Создать папку" << endl;
    else cout << "   Создать папку" << endl;
    if (posCon == 8) cout << " > Создать файл" << endl;
    else cout << "   Создать файл" << endl;
    if (posCon == 9) cout << " > Создать текстовый файл и редактировать его" << endl;
    else cout << "   Создать текстовый файл и редактировать его" << endl;
    cout << "   ----" << endl;
    if (posCon == 10) cout << " > Вырезать" << endl;
    else cout << "   Вырезать" << endl;
    if (posCon == 11) cout << " > Копировать" << endl;
    else cout << "   Копировать" << endl;
    if (posCon == 12) cout << " > Вставить" << endl;
    else cout << "   Вставить" << endl;
    cout << "   ----" << endl;
    if (posCon == 13) cout << " > Выйти из меню" << endl;
    else cout << "   Выйти из меню" << endl;
    if (posCon == 14) cout << " > Выйти из программы" << endl;
    else cout << "   Выйти из программы" << endl;

}

int Recalc(string& path, int& fsize) {
    int count = 0;
    for (const auto& x : directory_iterator(path)) count++;
    return count;
}

void new_file(string& path) {
    system("cls");
    cout << "Введите название: ";
    string file_name;
    cin >> file_name;
    cin.clear();
    std::ofstream(path + "\\" + file_name);
    update = true;
}

void new_dir(string& path) {
    system("cls");
    cout << "Введите название: ";
    string dir_name;
    cin >> dir_name;
    cin.clear();
    string res_name = path + "\\" + dir_name;
    try {
        create_directory(res_name);
    }
    catch (const filesystem_error& ex) {
        cout << "Недопустимое название! Нажмите любую клавишу для продолжения...";
        char cc = _getch();
    }
    update = true;
}

void rm_file(string& path, string& file_name) {
    try {
        string res_name;
        if (file_name.find(":\\") != string::npos) res_name = file_name;
        else res_name = path + "\\" + file_name;
        if (!exists(res_name)) {
            cout << "Элемента несуществует. Нажмите любую клавишу для продолжения...";
            char cc = _getch();
            update = true;
            return;
        }
        cout << endl << "Вы уверены, что хотите удалить этот файл? Y/y: ";
        string sure;
        cin >> sure;
        cin.clear();
        if (sure == "Y" || sure == "y") {
            remove(res_name);
        }
    }
    catch (const filesystem_error& ex) {
        cout << "Невозможно удалить элемент. Нажмите любую клавишу для продолжения...";
        char cc = _getch();
    }
    update = true;
}

void rm_dir(string& path, string& dir_name) {
    try {
        string res_name;
        if (dir_name.find(":\\") != string::npos) res_name = dir_name;
        else res_name = path + "\\" + dir_name;
        if (!exists(res_name)) {
            cout << "Папки несуществует. Нажмите любую клавишу для продолжения...";
            char cc = _getch();
            update = true;
            return;
        }
        cout << endl << "Вы уверены, что хотите удалить эту папку? Это затронет все вложенные файлы! Y/y: ";
        string sure;
        cin >> sure;
        cin.clear();
        if (sure == "Y" || sure == "y") {
            remove_all(res_name);
        }
    }
    catch (const filesystem_error& ex) {
        cout << "Невозможно удалить папку. Нажмите любую клавишу для продолжения...";
        char cc = _getch();
    }
    update = true;
}

void new_name(string& path, string& dir_name) {
    try {
        string res_name;
        if (dir_name.find(":\\") != string::npos) res_name = dir_name;
        else res_name = path + "\\" + dir_name;
        if (!exists(res_name)) {
            cout << "Элемента несуществует. Нажмите любую клавишу для продолжения...";
            char cc = _getch();
            update = true;
            return;
        }
        cout << "Введите новое название: ";
        string dir_name2;
        cin >> dir_name2;
        cin.clear();
        string res_name2 = path + "\\" + dir_name2;
        // Нельзя переименовать папку в файл и наоборот
        if ((extension(res_name) != extension(res_name2)) && (extension(res_name) == "" || extension(res_name2) == "")) {
            cout << "Невозможно переобразовать тип элемента. Нажмите любую клавишу для продолжения...";
            char cc = _getch();
            update = true;
            return;
        }
        if (extension(res_name) != extension(res_name2)) {
            cout << endl << "Вы уверены, что хотите сменить расширение файла? Он может оказаться недоступным. Y/y: ";
            string sure;
            cin >> sure;
            cin.clear();
            if (sure == "Y" || sure == "y") {
                rename(res_name, res_name2);
                update = true;
                return;
            }
            else {
                update = true;
                return;
            }
        }
        rename(res_name, res_name2);
    }
    catch (const filesystem_error& ex) {
        cout << "Невозможно переименовать. Нажмите любую клавишу для продолжения...";
        char cc = _getch();
    }
    update = true;
}

void properties(string& path) {
    cout << path << endl << endl;
    cout << "   СВОЙСТВА" << endl << endl;

    if (is_regular_file(path)) {
        string ex = extension(path);
        boost::erase_all(ex, ".");
        if (ex == "mid" || ex == "mp3" || ex == "mpa" || ex == "ogg" || ex == "wav") { cout << "   Музыкальный файл ." + ex; }
        if (ex == "7z" || ex == "rar" || ex == "zip" || ex == "gz" || ex == "pkg") { cout << "   Архив ." + ex; }
        if (ex == "msi" || ex == "bat" || ex == "bin" || ex == "exe" || ex == "jar") { cout << "   Исполняемый файл ." + ex; }
        if (ex == "gif" || ex == "jpeg" || ex == "jpg" || ex == "png" || ex == "bmp") { cout << "   Изображение ." + ex; }
        if (ex == "doc" || ex == "docx" || ex == "ppt" || ex == "pptx" || ex == "xlsx") { cout << "   Файл Microsoft Office ." + ex; }
        if (ex == "c" || ex == "cpp" || ex == "py" || ex == "java" || ex == "cs") { cout << "   Файл исходного кода ." + ex; }
        if (ex == "avi" || ex == "flv" || ex == "m4v" || ex == "mkv" || ex == "mp4") { cout << "   Видеофайл ." + ex; }
        if (ex == "pdf" || ex == "txt" || ex == "rtf" || ex == "odt" || ex == "tex") { cout << "   Текстовый документ ." + ex; }
        cout << endl << endl;
    }

    if (is_regular_file(path)) {
        long long filsize = file_size(path);
        string measure = " байт";
        cout << "   Размер файла:" << endl;
        cout << "    " << filsize << measure << endl;
        if (int(filsize / 1024) > 0) {
            filsize = long long(filsize / 1024); measure = " КБ"; cout << "   ~" << filsize << measure << endl;
        };
        if (int(filsize / 1024) > 0) {
            filsize = long long(filsize / 1024); measure = " МБ"; cout << "   ~" << filsize << measure << endl;
        };
        if (int(filsize / 1024) > 0) {
            filsize = long long(filsize / 1024); measure = " ГБ"; cout << "   ~" << filsize << measure << endl;
        };
    }

    time_t uu = creation_time(path);
    cout << endl << "   Время создания: " << asctime(gmtime(&uu));
    uu = last_write_time(path);
    cout << "   Время последнего изменения: " << asctime(gmtime(&uu));
    cout << endl << "Для продолжения, нажмите любую клавишу...";
    Wait
}

void open_in_cmd(string& path) {
    std::ofstream ofs("tmp.txt");
    string tt = path;
    ofs << "start  /D \"" + tt + "\" CMD.EXE";
    ofs.close();
    rename("tmp.txt", "tmp.bat");
    ShellExecuteA(0, NULL, "tmp.bat", NULL, NULL, SW_RESTORE);
    Sleep(1000);
    remove("tmp.bat");
}

void cut(string& path, string& clipboard, bool& cut_flag) {
    clipboard = path;
    cut_flag = true;
}

void copy(string& path, string& clipboard, bool& cut_flag) {
    clipboard = path;
    cut_flag = false;
}

void paste(string& path, string& clipboard, bool& cut_flag, string& pstr2) {
    try {
        if (clipboard == "") {
            cout << "Буфер обмена пуст. Нажмите любую клавишу для продолжения...";
            char cc = _getch();
            return;
        }
        if (exists(clipboard) && is_directory(clipboard)) {
            path = path + pstr2;
            if (clipboard == path) {
                cout << "Невозможно скопировать в то же место. ";
                char cc = _getch();
                return;
            }
            copy_directory(clipboard, path);
            if (cut_flag == true) {
                remove_all(clipboard);
                cut_flag = false;
                clipboard = "";
            }
        }
        else if (exists(clipboard) && is_regular_file(clipboard)) {
            path = path + pstr2;
            if (clipboard == path) {
                cout << "Невозможно скопировать в то же место. ";
                char cc = _getch();
                return;
            }
            copy_file(clipboard, path);
            if (cut_flag == true) {
                remove(clipboard);
                cut_flag = false;
                clipboard = "";
            }
        }
        else {
            cout << "Невозможно совершить операцию. Нажмите любую клавишу для продолжения...";
            char cc = _getch();
            return;
        }
    }
    catch (const filesystem_error& ex) {
        cout << "Невозможно совершить операцию. Нажмите любую клавишу для продолжения...";
        char cc = _getch();
        return;
    }
}

void text_with_data(string& path) {
    string file_name;
    string file_data;
    system("cls");
    cout << "Введите название: ";
    cin >> file_name;
    cin.clear();
    file_name = file_name + ".txt";
    std::ofstream ofs(path + "\\" + file_name);
    cout << endl << "Введите текст (только символы английского алфавита): " << endl;
    cin.clear();
    std::getline(std::cin >> std::ws, file_data);
    ofs << file_data;
    ofs.close();
    cin.clear();
}

void help() {
    system("cls");
    cout << "Файловый менеджер. Разработан в качестве курсовой работы во II семестре курса Программирование." << endl;
    cout << "Медведев Денис, группа 0374, ФКТИ СПбГЭТУ \"ЛЭТИ\"." << endl << endl;
    cout << "Горячие клавиши:" << endl;
    cout << "   C - контекстное меню" << endl;
    cout << "   H - помощь" << endl;
    cout << "   F - открыть в Проводнике Windows" << endl;
    cout << "   N - создать файл" << endl;
    cout << "   M - создать папку" << endl;
    cout << "   R - удалить файл" << endl;
    cout << "   T - удалить папку" << endl;
    cout << "   U - переименовать" << endl;
    cout << "   Backspace (обратный пробел) - возвращение назад" << endl;
    cout << "   Стрелки влево и вправо для перемещения между страниц" << endl;
    cout << "   Esc - выйти из программы" << endl << endl;
    cout << "Для смены корневого каталога, вернитесь назад из корня диска." << endl << endl;
    cout << "Нажмите на любую клавишу для возвращения.";
    Wait
    system("cls");
}