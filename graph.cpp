// graph.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "graph.h"
#include <string>
#include <sstream>
#include <cwctype>
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

std::vector <std::vector<int>> coordinates{
    {0, 3, 3, 8, 6, 10, 16, 16, 16, 23, 24, 24},
    {1, 1, 7, 19, 22, 22, 7, 1, 9, 19, 2, 16} 
};
Graph graph(coordinates);

enum class MouseMode {
    AddVertex,
    SelectStart,
    SelectEnd
};

MouseMode mouse_mode = MouseMode::AddVertex;
std::wstring status_text = L"Режим: добавление вершины";

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    EnterCoordinates(HWND, UINT, WPARAM, LPARAM);

void RebuildGraph()
{
    graph = Graph(coordinates);
    graph.FormMatrix();
    graph.BuildTreeEdgesPrim();
}

void RebuildPath()
{
    graph.FormMatrix();
    graph.BuildTreeEdgesPrim();
}

int FindVertexByMouse(int mouse_x, int mouse_y, int client_height)
{
    float scale = 30.0f;
    int scale_int = static_cast<int>(scale);
    float radius = 4 * (scale / 15);
    auto vertices = graph.GetVertices();

    for (size_t i = 0; i < vertices.size(); ++i) {
        int x = vertices[i].x_ * scale_int + 10;
        int y = -vertices[i].y_ * scale_int + client_height - 10;
        float dist = std::sqrt(std::pow(mouse_x - x, 2) + std::pow(mouse_y - y, 2));
        if (dist <= radius + 8) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPH));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 
       CW_USEDEFAULT, 0, 
       1000, 800, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   RebuildPath();

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ENTER_COORDINATES:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ENTER_COORDINATES), hWnd, EnterCoordinates);
                break;
            case IDM_MODE_ADD_VERTEX:
                mouse_mode = MouseMode::AddVertex;
                status_text = L"Режим: добавление вершины";
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            case IDM_MODE_SELECT_START:
                mouse_mode = MouseMode::SelectStart;
                status_text = L"Режим: выбор начальной вершины";
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            case IDM_MODE_SELECT_END:
                mouse_mode = MouseMode::SelectEnd;
                status_text = L"Режим: выбор конечной вершины";
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            case IDM_BUILD_PATH:
                RebuildPath();
                status_text = L"Путь построен";
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            case IDM_CLEAR_GRAPH:
                coordinates.clear();
                coordinates.resize(2);
                RebuildGraph();
                status_text = L"Граф очищен";
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_SIZE:
    {
        RebuildPath();
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT client_rect;
            GetClientRect(hWnd, &client_rect);
            int clientHeight = client_rect.bottom - client_rect.top;
            int clientWidth = client_rect.right - client_rect.left;

            HBRUSH brush_black = CreateSolidBrush(RGB(0, 0, 0));
            HBRUSH brush_blue = CreateSolidBrush(RGB(21, 101, 192));
            HBRUSH brush_green = CreateSolidBrush(RGB(0, 255, 0));
            HBRUSH brush_red = CreateSolidBrush(RGB(255, 0, 0));
            HPEN pen_grid = CreatePen(PS_SOLID, 1, RGB(235, 235, 235));
            HPEN pen_axis = CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
            HPEN pen_tree = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
            HPEN pen_path = CreatePen(PS_SOLID, 3, RGB(46, 125, 50));
            HPEN pen_old = (HPEN)SelectObject(hdc, pen_grid);
            HBRUSH brush_old = (HBRUSH)SelectObject(hdc, brush_black);

            float scale{ 30 };
            int scale_int{ static_cast<int>(scale) };
            int radius{ 12 };

            int origin_x = 10;
            int origin_y = clientHeight - 10;

            for (int x = origin_x; x < clientWidth; x += scale_int) {
                MoveToEx(hdc, x, 0, nullptr);
                LineTo(hdc, x, origin_y);
            }
            for (int y = origin_y; y > 0; y -= scale_int) {
                MoveToEx(hdc, origin_x, y, nullptr);
                LineTo(hdc, clientWidth, y);
            }

            SelectObject(hdc, pen_axis);
            MoveToEx(hdc, origin_x, 0, nullptr);
            LineTo(hdc, origin_x, origin_y);
            MoveToEx(hdc, origin_x, origin_y, nullptr);
            LineTo(hdc, clientWidth, origin_y);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(80, 80, 80));
            TextOutW(hdc, clientWidth - 25, origin_y - 20, L"X", 1);
            TextOutW(hdc, origin_x + 8, 8, L"Y", 1);

            for (int i = 0; i <= 30; i += 5) {
                std::wstring label = std::to_wstring(i);
                int x = origin_x + i * scale_int;
                int y = origin_y - i * scale_int;

                MoveToEx(hdc, x, origin_y, nullptr);
                LineTo(hdc, x, origin_y + 5);
                TextOutW(hdc, x - 8, origin_y - 18, label.c_str(),
                    static_cast<int>(label.size()));

                MoveToEx(hdc, origin_x, y, nullptr);
                LineTo(hdc, origin_x + 5, y);
                TextOutW(hdc, origin_x + 8, y - 8, label.c_str(),
                    static_cast<int>(label.size()));
            }

            auto tree_edges{ graph.GetTree() };
            auto path_edges{ graph.GetPath() };
            int start_v = graph.GetStartVertex();
            int end_v = graph.GetEndVertex();
            auto vertices = graph.GetVertices();

            SelectObject(hdc, pen_tree);
            for (const auto& edge : tree_edges) {
                int x1 = edge.first_.x_ * scale_int + 10;
                int y1 = -edge.first_.y_ * scale_int + clientHeight - 10;
                int x2 = edge.second_.x_ * scale_int + 10;
                int y2 = -edge.second_.y_ * scale_int + clientHeight - 10;

                MoveToEx(hdc, x1, y1, nullptr);
                LineTo(hdc, x2, y2);
            }

            SelectObject(hdc, pen_path);
            for (const auto& edge : path_edges) {
                int x1 = edge.first_.x_ * scale_int + 10;
                int y1 = -edge.first_.y_ * scale_int + clientHeight - 10;
                int x2 = edge.second_.x_ * scale_int + 10;
                int y2 = -edge.second_.y_ * scale_int + clientHeight - 10;

                MoveToEx(hdc, x1, y1, nullptr);
                LineTo(hdc, x2, y2);
            }

            for (size_t i = 0; i < vertices.size(); ++i) {
                int x = vertices[i].x_ * scale_int + 10;
                int y = -vertices[i].y_ * scale_int + clientHeight - 10;

                if ((int)i == start_v) {
                    SelectObject(hdc, brush_green);
                }
                else if ((int)i == end_v) {
                    SelectObject(hdc, brush_red);
                }
                else {
                    SelectObject(hdc, brush_blue);
                }

                Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

                std::wstring number = std::to_wstring(i);
                SetTextColor(hdc, RGB(255, 255, 255));
                TextOutW(hdc, x - 5, y - 8, number.c_str(),
                    static_cast<int>(number.size()));

                SetTextColor(hdc, RGB(0, 120, 0));
                if ((int)i == start_v) {
                    TextOutW(hdc, x - 20, y - radius - 18, L"START", 5);
                }
                SetTextColor(hdc, RGB(180, 0, 0));
                if ((int)i == end_v) {
                    TextOutW(hdc, x - 14, y - radius - 18, L"END", 3);
                }
            }

            std::wstringstream status;
            status << status_text << L" | Старт: " << start_v
                << L" | Конец: " << end_v
                << L" | Длина пути: " << graph.GetPathLength();
            std::wstring status_line = status.str();
            SetTextColor(hdc, RGB(0, 0, 0));
            TextOutW(hdc, 10, 10, status_line.c_str(),
                static_cast<int>(status_line.size()));

            SelectObject(hdc, pen_old);
            SelectObject(hdc, brush_old);
            DeleteObject(brush_black);
            DeleteObject(brush_blue);
            DeleteObject(brush_green);
            DeleteObject(brush_red);
            DeleteObject(pen_grid);
            DeleteObject(pen_axis);
            DeleteObject(pen_tree);
            DeleteObject(pen_path);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_LBUTTONDOWN:
    {
        int mouse_x = GET_X_LPARAM(lParam);
        int mouse_y = GET_Y_LPARAM(lParam);

        RECT client_rect;
        GetClientRect(hWnd, &client_rect);
        int clientHeight = client_rect.bottom - client_rect.top;

        int vertex = FindVertexByMouse(mouse_x, mouse_y, clientHeight);

        if (mouse_mode == MouseMode::AddVertex) {
            if (vertex != -1) {
                status_text = L"Тут уже есть вершина";
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            }

            float scale = 30.0f;
            int graph_x = static_cast<int>((mouse_x - 10) / scale + 0.5f);
            int graph_y = static_cast<int>((clientHeight - 10 - mouse_y) / scale + 0.5f);

            if (graph_x < 0 || graph_y < 0) {
                status_text = L"Вершина должна быть в положительной области координат";
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            }

            if (coordinates.size() < 2) {
                coordinates.resize(2);
            }
            coordinates[0].push_back(graph_x);
            coordinates[1].push_back(graph_y);
            RebuildGraph();
            status_text = L"Вершина добавлена";
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        }

        if (vertex == -1) {
            status_text = L"Кликните по существующей вершине";
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        }

        if (mouse_mode == MouseMode::SelectStart) {
            graph.SetStartVertex(vertex);
            status_text = L"Начальная вершина выбрана";
        }
        else if (mouse_mode == MouseMode::SelectEnd) {
            graph.SetEndVertex(vertex);
            status_text = L"Конечная вершина выбрана";
        }

        RebuildPath();
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик окна ввода координат.
static bool ParseTwoColumnCoordinates(const std::wstring& text, std::vector<std::vector<int>>& out)
{
    std::wistringstream sin(text);
    std::wstring line;
    std::vector<int> xs, ys;

    while (std::getline(sin, line)) {
        // Trim line
        size_t start = 0;
        while (start < line.size() && iswspace(line[start])) ++start;
        size_t end = line.size();
        while (end > start && iswspace(line[end-1])) --end;
        if (start >= end) continue;

        std::wstring trimmed = line.substr(start, end - start);
        std::wistringstream ls(trimmed);
        int x, y;
        if (ls >> x >> y) {
            xs.push_back(x);
            ys.push_back(y);
        }
        else {
            // try parse comma or semicolon separated
            // replace commas and semicolons with spaces and try again
            for (auto& ch : trimmed) if (ch == L',' || ch == L';' || ch == L'\t') ch = L' ';
            std::wistringstream ls2(trimmed);
            if (ls2 >> x >> y) { xs.push_back(x); ys.push_back(y); }
        }
    }

    if (xs.empty()) return false;
    out.clear(); out.resize(2);
    out[0] = xs; out[1] = ys;
    return true;
}

INT_PTR CALLBACK EnterCoordinates(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        std::wstring text;
        if (!coordinates.empty() && coordinates[0].size() == coordinates[1].size()) {
            for (size_t i = 0; i < coordinates[0].size(); ++i) {
                if (i > 0) text += L"\r\n";
                text += std::to_wstring(coordinates[0][i]);
                text += L"\t";
                text += std::to_wstring(coordinates[1][i]);
            }
        }
        SetDlgItemTextW(hDlg, IDC_EDIT_COORDINATES, text.c_str());
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            int len = GetWindowTextLengthW(GetDlgItem(hDlg, IDC_EDIT_COORDINATES));
            std::wstring buf;
            buf.resize(len + 1);
            GetDlgItemTextW(hDlg, IDC_EDIT_COORDINATES, &buf[0], len + 1);

            // Parse two-column input: each line "x y" (tab/space separated)
            std::vector<std::vector<int>> new_coords;
            if (ParseTwoColumnCoordinates(buf, new_coords)) {
                coordinates = new_coords;
                RebuildGraph();
                status_text = L"Координаты обновлены";

                HWND parent = GetParent(hDlg);
                if (!parent) parent = GetWindow(hDlg, GW_OWNER);
                if (parent) InvalidateRect(parent, nullptr, TRUE);
            }

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
