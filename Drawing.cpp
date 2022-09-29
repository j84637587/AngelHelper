#include "Drawing.h"
#include <cstdio>

LPCSTR Drawing::lpWindowName = "Maplestory Shield";
ImVec2 Drawing::vWindowSize = { 500, 500 };
ImGuiWindowFlags Drawing::WindowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize;
bool Drawing::bDraw = true;

void Drawing::Active()
{
	bDraw = true;
}

bool Drawing::isActive()
{
	return bDraw == true;
}

/// <summary>
/// 顯示子配置選單
/// </summary>
static void ShowMenuFile()
{
    if (ImGui::MenuItem(u8"新配置")) {}
    if (ImGui::MenuItem(u8"載入配置", "Ctrl+O")) {}
    if (ImGui::BeginMenu(u8"配置清單"))
    {
        ImGui::MenuItem(u8"配置1");
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem(u8"保存配置", "Ctrl+S")) {}
    if (ImGui::MenuItem(u8"另存配置")) {}
}

/// <summary>
/// 顯示配置選單
/// </summary>
static void ShowAppMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

enum MyItemColumnID
{
    MyItemColumnID_ID,
    MyItemColumnID_Name,
    MyItemColumnID_Action,
    MyItemColumnID_Quantity,
    MyItemColumnID_Description
};

struct MyItem
{
    int         ID;
    const char* Name;
    int         Quantity;

    static const ImGuiTableSortSpecs* s_current_sort_specs;

    // Compare function to be used by qsort()
    static int __cdecl CompareWithSortSpecs(const void* lhs, const void* rhs)
    {
        const MyItem* a = (const MyItem*)lhs;
        const MyItem* b = (const MyItem*)rhs;
        for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
        {
            // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
            // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
            const ImGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
            int delta = 0;
            switch (sort_spec->ColumnUserID)
            {
            case MyItemColumnID_ID:             delta = (a->ID - b->ID);                break;
            case MyItemColumnID_Name:           delta = (strcmp(a->Name, b->Name));     break;
            case MyItemColumnID_Quantity:       delta = (a->Quantity - b->Quantity);    break;
            case MyItemColumnID_Description:    delta = (strcmp(a->Name, b->Name));     break;
            default: IM_ASSERT(0); break;
            }
            if (delta > 0)
                return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
            if (delta < 0)
                return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
        }

        // qsort() is instable so always return a way to differenciate items.
        // Your own compare function may want to avoid fallback on implicit sort specs e.g. a Name compare if it wasn't already part of the sort specs.
        return (a->ID - b->ID);
    }
};

const ImGuiTableSortSpecs* MyItem::s_current_sort_specs = NULL;

static const char* template_items_names[] =
{
    "Banana", "Apple", "Cherry", "Watermelon", "Grapefruit", "Strawberry", "Mango",
    "Kiwi", "Orange", "Pineapple", "Blueberry", "Plum", "Coconut", "Pear", "Apricot"
};

static void showPacketList()
{
    // Create item list
    static ImVector<MyItem> items;
    static ImVector<int> selection;
    if (items.Size == 0)
    {
        items.resize(50, MyItem());
        for (int n = 0; n < items.Size; n++)
        {
            const int template_n = n % IM_ARRAYSIZE(template_items_names);
            MyItem& item = items[n];
            item.ID = n;
            item.Name = template_items_names[template_n];
            item.Quantity = (n * n - n) % 20; // Assign default quantities
        }
    }

    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    if (ImGui::BeginTable("table_sorting", 4, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 15), 0.0f))
    {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_ID);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
        ImGui::TableSetupColumn("Copy", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
        ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Quantity);
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        // Sort our data if sort specs have been changed!
        if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
            if (sorts_specs->SpecsDirty)
            {
                MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                if (items.Size > 1)
                    qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
                MyItem::s_current_sort_specs = NULL;
                sorts_specs->SpecsDirty = false;
            }

        // Demonstrate using clipper for large vertical lists
        ImGuiListClipper clipper;
        clipper.Begin(items.Size);
        while (clipper.Step())
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
            {
                // Display a data item
                MyItem* item = &items[row_n];
                ImGui::PushID(item->ID);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                const bool item_is_selected = selection.contains(item->ID);
                char label[32];
                sprintf(label, "%04d", item->ID);
                ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
                if (ImGui::Selectable(label, item_is_selected, selectable_flags, ImVec2(0, 0.0f)))
                {
                    if (ImGui::GetIO().KeyCtrl)
                    {
                        if (item_is_selected)
                            selection.find_erase_unsorted(item->ID);
                        else
                            selection.push_back(item->ID);
                    }
                    else
                    {
                        selection.clear();
                        selection.push_back(item->ID);
                    }
                }

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(item->Name);
                ImGui::TableNextColumn();
                ImGui::SmallButton("None");
                ImGui::TableNextColumn();
                ImGui::Text("%d", item->Quantity);
                ImGui::PopID();
            }
        ImGui::EndTable();
    }
}

void Drawing::ShowWindow()
{
    // 確認Imgui資源都在
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context.");

    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;
    static bool no_docking = false;
    static bool unsaved_document = false;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    if (isActive()) {
        // 主視窗
        if (!ImGui::Begin(lpWindowName, &bDraw, window_flags))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }

        ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
    
        // 選單列
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(u8"配置"))
            {
                ShowMenuFile();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem(u8"封包"))
            {
                ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
                showPacketList();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        // End of ShowDemoWindow()
        ImGui::PopItemWidth();
        ImGui::End();
    }

    HotKeys();
}

/// <summary>
/// 
/// </summary>
void Drawing::HotKeys() {
    if (GetAsyncKeyState(VK_INSERT) & 1)
        bDraw = !bDraw;
}

/// <summary>
/// 
/// </summary>
void Draw_Confirm()
{
    ImGui::SetNextWindowSize(ImVec2(250.000f, 100.000f), ImGuiCond_Once);
    ImGui::Begin(u8"確定", NULL, 2);

    ImGui::SameLine();
    ImGui::Button(u8"繼續", ImVec2(95.000f, 50.000f));

    ImGui::SameLine();
    ImGui::Button(u8"取消", ImVec2(95.000f, 50.000f));

    ImGui::End();
}
