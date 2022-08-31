// Mini memory editor for Dear ImGui (to embed in your game/tools)
// Get latest version at http://www.github.com/ocornut/imgui_club
//
// Right-click anywhere to access the Options menu!
// You can adjust the keyboard repeat delay/rate in ImGuiIO.
// The code assume a mono-space font for simplicity!
// If you don't use the default font, use ImGui::PushFont()/PopFont() to switch to a mono-space font before calling this.
//
// Usage:
//   // Create a window and draw memory editor inside it:
//   static MemoryEditor mem_edit_1;
//   static char data[0x10000];
//   size_t data_size = 0x10000;
//   mem_edit_1.DrawWindow("Memory Editor", data, data_size);
//
// Usage:
//   // If you already have a window, use DrawContents() instead:
//   static MemoryEditor mem_edit_2;
//   ImGui::Begin("MyWindow")
//   mem_edit_2.DrawContents(this, sizeof(*this), (size_t)this);
//   ImGui::End();
//
// Changelog:
// - v0.10: initial version
// - v0.23 (2017/08/17): added to github. fixed right-arrow triggering a byte write.
// - v0.24 (2018/06/02): changed DragInt("Rows" to use a %d data format (which is desirable since imgui 1.61).
// - v0.25 (2018/07/11): fixed wording: all occurrences of "Rows" renamed to "Columns".
// - v0.26 (2018/08/02): fixed clicking on hex region
// - v0.30 (2018/08/02): added data preview for common data types
// - v0.31 (2018/10/10): added OptUpperCaseHex option to select lower/upper casing display [@samhocevar]
// - v0.32 (2018/10/10): changed signatures to use void* instead of unsigned char*
// - v0.33 (2018/10/10): added OptShowOptions option to hide all the interactive option setting.
// - v0.34 (2019/05/07): binary preview now applies endianness setting [@nicolasnoble]
// - v0.35 (2020/01/29): using ImGuiDataType available since Dear ImGui 1.69.
// - v0.36 (2020/05/05): minor tweaks, minor refactor.
// - v0.40 (2020/10/04): fix misuse of ImGuiListClipper API, broke with Dear ImGui 1.79. made cursor position appears on left-side of edit box. option popup appears on mouse release. fix MSVC warnings where _CRT_SECURE_NO_WARNINGS wasn't working in recent versions.
// - v0.41 (2020/10/05): fix when using with keyboard/gamepad navigation enabled.
// - v0.42 (2020/10/14): fix for . character in ASCII view always being greyed out.
// - v0.43 (2021/03/12): added OptFooterExtraHeight to allow for custom drawing at the bottom of the editor [@leiradel]
// - v0.44 (2021/03/12): use ImGuiInputTextFlags_AlwaysOverwrite in 1.82 + fix hardcoded width.
// - v0.50 (2021/11/12): various fixes for recent dear imgui versions (fixed misuse of clipper, relying on SetKeyboardFocusHere() handling scrolling from 1.85). added default size.
//
// Todo/Bugs:
// - This is generally old/crappy code, it should work but isn't very good.. to be rewritten some day.
// - PageUp/PageDown are supported because we use _NoNav. This is a good test scenario for working out idioms of how to mix natural nav and our own...
// - Arrows are being sent to the InputText() about to disappear which for LeftArrow makes the text cursor appear at position 1 for one frame.
// - Using InputText() is awkward and maybe overkill here, consider implementing something custom.

#pragma once

#include <stdio.h>      // sprintf, scanf
#include <stdint.h>     // uint8_t, etc.
#include <imgui.h>

#ifdef _MSC_VER
#define _PRISizeT   "I"
#define ImSnprintf  _snprintf
#else
#define _PRISizeT   "z"
#define ImSnprintf  snprintf
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4996) // warning C4996: 'sprintf': This function or variable may be unsafe.
#endif

struct MemoryEditor
{
    enum DataFormat
    {
        DataFormat_Bin = 0,
        DataFormat_Dec = 1,
        DataFormat_Hex = 2,
        DataFormat_COUNT
    };

    // Settings
    bool            Open;                                       // = true   // set to false when DrawWindow() was closed. ignore if not using DrawWindow().
    bool            ReadOnly;                                   // = false  // disable any editing.
    int             Cols;                                       // = 16     // number of columns to display.                     // = 0      // space to reserve at the bottom of the widget to add custom widgets
    ImU32           HighlightColor;                             //          // background color of highlighted bytes.
    ImU8            (*ReadFn)(const ImU8* data, size_t off);    // = 0      // optional handler to read bytes.
    void            (*WriteFn)(ImU8* data, size_t off, ImU8 d); // = 0      // optional handler to write bytes.
    bool            (*HighlightFn)(const ImU8* data, size_t off);//= 0      // optional handler to return Highlight property (to support non-contiguous highlighting).

    // [Internal State]
    bool            ContentsWidthChanged;
    size_t          DataPreviewAddr;
    size_t          DataEditingAddr;
    bool            DataEditingTakeFocus;
    char            DataInputBuf[32];
    char            AddrInputBuf[32];
    size_t          GotoAddr;
    size_t          HighlightMin, HighlightMax;
    int             PreviewEndianess;
    ImGuiDataType   PreviewDataType;

    MemoryEditor()
    {
        // Settings
        Open = true;
        ReadOnly = true;
        Cols = 16;
        HighlightColor = IM_COL32(255, 255, 255, 50);
        ReadFn = NULL;
        WriteFn = NULL;
        HighlightFn = NULL;

        // State/Internals
        ContentsWidthChanged = false;
        DataPreviewAddr = DataEditingAddr = (size_t)-1;
        DataEditingTakeFocus = false;
        memset(DataInputBuf, 0, sizeof(DataInputBuf));
        memset(AddrInputBuf, 0, sizeof(AddrInputBuf));
        GotoAddr = (size_t)-1;
        HighlightMin = HighlightMax = (size_t)-1;
        PreviewEndianess = 0;
        PreviewDataType = ImGuiDataType_S32;
    }

    void GotoAddrAndHighlight(size_t addr_min, size_t addr_max)
    {
        GotoAddr = addr_min;
        HighlightMin = addr_min;
        HighlightMax = addr_max;
    }

    struct Sizes
    {
        int     AddrDigitsCount;
        float   LineHeight;
        float   GlyphWidth;
        float   HexCellWidth;
        float   SpacingBetweenMidCols;
        float   PosHexStart;
        float   PosHexEnd;
        float   PosAsciiStart;
        float   PosAsciiEnd;
        float   WindowWidth;

        Sizes() { memset(this, 0, sizeof(*this)); }
    };

    void CalcSizes(Sizes& s, size_t mem_size, size_t base_display_addr)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        s.AddrDigitsCount = 0;
        if (s.AddrDigitsCount == 0)
            for (size_t n = base_display_addr + mem_size - 1; n > 0; n >>= 4)
                s.AddrDigitsCount++;
        s.LineHeight = ImGui::GetTextLineHeight();
        s.GlyphWidth = ImGui::CalcTextSize("F").x + 1;                  // We assume the font is mono-space
        s.HexCellWidth = (float)(int)(s.GlyphWidth * 2.5f);             // "FF " we include trailing space in the width to easily catch clicks everywhere
        s.SpacingBetweenMidCols = (float)(int)(s.HexCellWidth * 0.25f); // Every OptMidColsCount columns we add a bit of extra spacing
        s.PosHexStart = (s.AddrDigitsCount + 2) * s.GlyphWidth;
        s.PosHexEnd = s.PosHexStart + (s.HexCellWidth * Cols);
        s.PosAsciiStart = s.PosAsciiEnd = s.PosHexEnd;
        
        s.PosAsciiStart = s.PosHexEnd + s.GlyphWidth * 1;
        s.PosAsciiEnd = s.PosAsciiStart + Cols * s.GlyphWidth;
        
        s.WindowWidth = s.PosAsciiEnd + style.ScrollbarSize + style.WindowPadding.x * 2 + s.GlyphWidth;
    }

    // Memory Editor contents only
    void DrawContents(void* mem_data_void, size_t mem_size, size_t base_display_addr = 0x0000)
    {
        if (Cols < 1)
            Cols = 1;

        ImU8* mem_data = (ImU8*)mem_data_void;
        Sizes s;
        CalcSizes(s, mem_size, base_display_addr);
        ImGuiStyle& style = ImGui::GetStyle();

        // We begin into our scrolling region with the 'ImGuiWindowFlags_NoMove' in order to prevent click from moving the window.
        // This is used as a facility since our main click detection code doesn't assign an ActiveId so the click would normally be caught as a window-move.
        const float height_separator = style.ItemSpacing.y;
        float footer_height = 0.f;
        
        footer_height += height_separator + ImGui::GetFrameHeightWithSpacing() * 1 + ImGui::GetTextLineHeightWithSpacing() * 3;
        ImGui::BeginChild("##scrolling", ImVec2(0, -footer_height), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        // We are not really using the clipper API correctly here, because we rely on visible_start_addr/visible_end_addr for our scrolling function.
        const int line_total_count = (int)((mem_size + Cols - 1) / Cols);
        ImGuiListClipper clipper;
        clipper.Begin(line_total_count, s.LineHeight);

        bool data_next = false;

        if (ReadOnly || DataEditingAddr >= mem_size)
            DataEditingAddr = (size_t)-1;
        if (DataPreviewAddr >= mem_size)
            DataPreviewAddr = (size_t)-1;

        size_t preview_data_type_size = 0;

        size_t data_editing_addr_next = (size_t)-1;
        if (DataEditingAddr != (size_t)-1)
        {
            // Move cursor but only apply on next frame so scrolling with be synchronized (because currently we can't change the scrolling while the window is being rendered)
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) && (ptrdiff_t)DataEditingAddr >= (ptrdiff_t)Cols)                     { data_editing_addr_next = DataEditingAddr - Cols; }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) && (ptrdiff_t)DataEditingAddr < (ptrdiff_t)mem_size - Cols)    { data_editing_addr_next = DataEditingAddr + Cols; }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)) && (ptrdiff_t)DataEditingAddr > (ptrdiff_t)0)                  { data_editing_addr_next = DataEditingAddr - 1; }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)) && (ptrdiff_t)DataEditingAddr < (ptrdiff_t)mem_size - 1)      { data_editing_addr_next = DataEditingAddr + 1; }
        }

        // Draw vertical separator
        ImVec2 window_pos = ImGui::GetWindowPos();
      
        draw_list->AddLine(ImVec2(window_pos.x + s.PosAsciiStart - s.GlyphWidth, window_pos.y), ImVec2(window_pos.x + s.PosAsciiStart - s.GlyphWidth, window_pos.y + 9999), ImGui::GetColorU32(ImGuiCol_Border));

        const ImU32 color_text = ImGui::GetColorU32(ImGuiCol_Text);
        const ImU32 color_disabled = ImGui::GetColorU32(ImGuiCol_TextDisabled);

        const char* format_address = "%0*" _PRISizeT "X: ";
        const char* format_data = "%0*" _PRISizeT "X";
        const char* format_byte = "%02X";
        const char* format_byte_space = "%02X ";

        while (clipper.Step())
            for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) // display only visible lines
            {
                size_t addr = (size_t)(line_i * Cols);
                ImGui::Text(format_address, s.AddrDigitsCount, base_display_addr + addr);

                // Draw Hexadecimal
                for (int n = 0; n < Cols && addr < mem_size; n++, addr++)
                {
                    float byte_pos_x = s.PosHexStart + s.HexCellWidth * n;
                    
                    byte_pos_x += (float)(n / 8) * s.SpacingBetweenMidCols;
                    ImGui::SameLine(byte_pos_x);

                    // Draw highlight
                    bool is_highlight_from_user_range = (addr >= HighlightMin && addr < HighlightMax);
                    bool is_highlight_from_user_func = (HighlightFn && HighlightFn(mem_data, addr));
                    bool is_highlight_from_preview = (addr >= DataPreviewAddr && addr < DataPreviewAddr + preview_data_type_size);
                    if (is_highlight_from_user_range || is_highlight_from_user_func || is_highlight_from_preview)
                    {
                        ImVec2 pos = ImGui::GetCursorScreenPos();
                        float highlight_width = s.GlyphWidth * 2;
                        bool is_next_byte_highlighted = (addr + 1 < mem_size) && ((HighlightMax != (size_t)-1 && addr + 1 < HighlightMax) || (HighlightFn && HighlightFn(mem_data, addr + 1)));
                        if (is_next_byte_highlighted || (n + 1 == Cols))
                        {
                            highlight_width = s.HexCellWidth;
                            if (n > 0 && (n + 1) < Cols && ((n + 1) % 8) == 0)
                                highlight_width += s.SpacingBetweenMidCols;
                        }
                        draw_list->AddRectFilled(pos, ImVec2(pos.x + highlight_width, pos.y + s.LineHeight), HighlightColor);
                    }

                    if (DataEditingAddr == addr)
                    {
                        // Display text input on current byte
                        bool data_write = false;
                        ImGui::PushID((void*)addr);
                        if (DataEditingTakeFocus)
                        {
                            ImGui::SetKeyboardFocusHere(0);
                            sprintf(AddrInputBuf, format_data, s.AddrDigitsCount, base_display_addr + addr);
                            sprintf(DataInputBuf, format_byte, ReadFn ? ReadFn(mem_data, addr) : mem_data[addr]);
                        }
                        struct UserData
                        {
                            // FIXME: We should have a way to retrieve the text edit cursor position more easily in the API, this is rather tedious. This is such a ugly mess we may be better off not using InputText() at all here.
                            static int Callback(ImGuiInputTextCallbackData* data)
                            {
                                UserData* user_data = (UserData*)data->UserData;
                                if (!data->HasSelection())
                                    user_data->CursorPos = data->CursorPos;
                                if (data->SelectionStart == 0 && data->SelectionEnd == data->BufTextLen)
                                {
                                    // When not editing a byte, always refresh its InputText content pulled from underlying memory data
                                    // (this is a bit tricky, since InputText technically "owns" the master copy of the buffer we edit it in there)
                                    data->DeleteChars(0, data->BufTextLen);
                                    data->InsertChars(0, user_data->CurrentBufOverwrite);
                                    data->SelectionStart = 0;
                                    data->SelectionEnd = 2;
                                    data->CursorPos = 0;
                                }
                                return 0;
                            }
                            char   CurrentBufOverwrite[3];  // Input
                            int    CursorPos;               // Output
                        };
                        UserData user_data;
                        user_data.CursorPos = -1;
                        sprintf(user_data.CurrentBufOverwrite, format_byte, ReadFn ? ReadFn(mem_data, addr) : mem_data[addr]);
                        ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_CallbackAlways;
#if IMGUI_VERSION_NUM >= 18104
                        flags |= ImGuiInputTextFlags_AlwaysOverwrite;
#else
                        flags |= ImGuiInputTextFlags_AlwaysInsertMode;
#endif
                        ImGui::SetNextItemWidth(s.GlyphWidth * 2);
                        if (ImGui::InputText("##data", DataInputBuf, IM_ARRAYSIZE(DataInputBuf), flags, UserData::Callback, &user_data))
                            data_write = data_next = true;
                        else if (!DataEditingTakeFocus && !ImGui::IsItemActive())
                            DataEditingAddr = data_editing_addr_next = (size_t)-1;
                        DataEditingTakeFocus = false;
                        if (user_data.CursorPos >= 2)
                            data_write = data_next = true;
                        if (data_editing_addr_next != (size_t)-1)
                            data_write = data_next = false;
                        unsigned int data_input_value = 0;
                        if (data_write && sscanf(DataInputBuf, "%X", &data_input_value) == 1)
                        {
                            if (WriteFn)
                                WriteFn(mem_data, addr, (ImU8)data_input_value);
                            else
                                mem_data[addr] = (ImU8)data_input_value;
                        }
                        ImGui::PopID();
                    }
                    else
                    {
                        // NB: The trailing space is not visible but ensure there's no gap that the mouse cannot click on.
                        ImU8 b = ReadFn ? ReadFn(mem_data, addr) : mem_data[addr];

                        
                        if (b == 0)
                            ImGui::TextDisabled("00 ");
                        else
                            ImGui::Text(format_byte_space, b);
                        
                        if (!ReadOnly && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                        {
                            DataEditingTakeFocus = true;
                            data_editing_addr_next = addr;
                        }
                    }
                }

                
                // Draw ASCII values
                ImGui::SameLine(s.PosAsciiStart);
                ImVec2 pos = ImGui::GetCursorScreenPos();
                addr = line_i * Cols;
                ImGui::PushID(line_i);
                if (ImGui::InvisibleButton("ascii", ImVec2(s.PosAsciiEnd - s.PosAsciiStart, s.LineHeight)))
                {
                    DataEditingAddr = DataPreviewAddr = addr + (size_t)((ImGui::GetIO().MousePos.x - pos.x) / s.GlyphWidth);
                    DataEditingTakeFocus = true;
                }
                ImGui::PopID();
                for (int n = 0; n < Cols && addr < mem_size; n++, addr++)
                {
                    if (addr == DataEditingAddr)
                    {
                        draw_list->AddRectFilled(pos, ImVec2(pos.x + s.GlyphWidth, pos.y + s.LineHeight), ImGui::GetColorU32(ImGuiCol_FrameBg));
                        draw_list->AddRectFilled(pos, ImVec2(pos.x + s.GlyphWidth, pos.y + s.LineHeight), ImGui::GetColorU32(ImGuiCol_TextSelectedBg));
                    }
                    unsigned char c = ReadFn ? ReadFn(mem_data, addr) : mem_data[addr];
                    char display_c = (c < 32 || c >= 128) ? '.' : c;
                    draw_list->AddText(pos, (display_c == c) ? color_text : color_disabled, &display_c, &display_c + 1);
                    pos.x += s.GlyphWidth;
                }
                
            }
        ImGui::PopStyleVar(2);
        ImGui::EndChild();

        // Notify the main window of our ideal child content size (FIXME: we are missing an API to get the contents size from the child)
        ImGui::SetCursorPosX(s.WindowWidth);

        if (data_next && DataEditingAddr + 1 < mem_size)
        {
            DataEditingAddr = DataPreviewAddr = DataEditingAddr + 1;
            DataEditingTakeFocus = true;
        }
        else if (data_editing_addr_next != (size_t)-1)
        {
            DataEditingAddr = DataPreviewAddr = data_editing_addr_next;
            DataEditingTakeFocus = true;
        }
    }
};

#undef _PRISizeT
#undef ImSnprintf

#ifdef _MSC_VER
#pragma warning (pop)
#endif