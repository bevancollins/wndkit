// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

// This file is based on code from the winlamb library https://github.com/rodrigocfd/winlamb
// Copyright (c) 2017-present, Rodrigo Cesar de Freitas Dias
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <windows.h>

namespace wndkit {

struct message_params {
  WPARAM wparam{};
  LPARAM lparam{};
};

struct pos_params : public message_params {
  [[nodiscard]] POINT pos() const noexcept {
    return {
      static_cast<int>(static_cast<short>(LOWORD(lparam))),
      static_cast<int>(static_cast<short>(HIWORD(lparam)))
    };
  }
};

struct command_params : public message_params {
  [[nodiscard]] bool is_from_menu() const noexcept        { return HIWORD(wparam) == 0; }
  [[nodiscard]] bool is_from_accelerator() const noexcept { return HIWORD(wparam) == 1; }
  [[nodiscard]] bool is_from_control() const noexcept     { return !is_from_menu() && !is_from_accelerator(); }
  [[nodiscard]] WORD id() const noexcept                  { return LOWORD(wparam); }
  [[nodiscard]] WORD control_notif_code() const noexcept  { return HIWORD(wparam); }
  [[nodiscard]] HWND control_hwnd() const noexcept        { return reinterpret_cast<HWND>(lparam); }
};

struct notify_params : public message_params {
  [[nodiscard]] NMHDR& nmhdr() const noexcept { return *reinterpret_cast<NMHDR*>(lparam); }
};

struct activate_params : public message_params {
  [[nodiscard]] bool is_being_activated() const noexcept           { return wparam != WA_INACTIVE; }
  [[nodiscard]] bool activated_not_by_mouse_click() const noexcept { return wparam == WA_ACTIVE; }
  [[nodiscard]] bool activated_by_mouse_click() const noexcept     { return wparam == WA_CLICKACTIVE; }
  [[nodiscard]] HWND swapped_window() const noexcept               { return reinterpret_cast<HWND>(lparam); }
};

struct activateapp_params : public message_params {
  [[nodiscard]] bool is_being_activated() const noexcept { return wparam != FALSE; }
  [[nodiscard]] DWORD thread_id() const noexcept         { return static_cast<DWORD>(lparam); }
};

struct askcbformatname_params : public message_params {
  [[nodiscard]] UINT szbuffer() const noexcept   { return static_cast<UINT>(wparam); }
  [[nodiscard]] wchar_t* buffer() const noexcept { return reinterpret_cast<wchar_t*>(lparam); }
};

using cancelmode_params = message_params;

struct capturechanged_params : public message_params {
  [[nodiscard]] HWND window_gaining_mouse() const noexcept { return reinterpret_cast<HWND>(lparam); }
};

struct changecbchain_params : public message_params {
  [[nodiscard]] HWND window_being_removed() const noexcept { return reinterpret_cast<HWND>(wparam); }
  [[nodiscard]] HWND next_window() const noexcept          { return reinterpret_cast<HWND>(lparam); }
  [[nodiscard]] bool is_last_window() const noexcept       { return next_window() == nullptr; }
};

struct char_params : public message_params {
  [[nodiscard]] WORD char_code() const noexcept           { return static_cast<WORD>(wparam); }
  [[nodiscard]] WORD repeat_count() const noexcept        { return LOWORD(lparam); }
  [[nodiscard]] BYTE scan_code() const noexcept           { return LOBYTE(HIWORD(lparam)); }
  [[nodiscard]] bool is_extended_key() const noexcept     { return (lparam >> 24) & 1; }
  [[nodiscard]] bool has_alt_key() const noexcept         { return (lparam >> 29) & 1; }
  [[nodiscard]] bool key_previously_down() const noexcept { return (lparam >> 30) & 1; }
  [[nodiscard]] bool key_being_released() const noexcept  { return (lparam >> 31) & 1; }
};

struct chartoitem_params : public message_params {
  [[nodiscard]] WORD char_code() const noexcept         { return LOWORD(wparam); }
  [[nodiscard]] WORD current_caret_pos() const noexcept { return HIWORD(wparam); }
  [[nodiscard]] HWND hlistbox() const noexcept          { return reinterpret_cast<HWND>(lparam); }
};

using childactivate_params = message_params;
using close_params         = message_params;

struct compacting_params : public message_params {
  [[nodiscard]] UINT cpu_time_ratio() const noexcept { return static_cast<UINT>(wparam); }
};

struct compareitem_params : public message_params {
  [[nodiscard]] WORD control_id() const noexcept                      { return static_cast<WORD>(wparam); }
  [[nodiscard]] COMPAREITEMSTRUCT& compareitemstruct() const noexcept { return *reinterpret_cast<COMPAREITEMSTRUCT*>(lparam); }
};

struct contextmenu_params : public pos_params {
  [[nodiscard]] HWND target() const noexcept { return reinterpret_cast<HWND>(wparam); }
};

struct copydata_params : public message_params {
  [[nodiscard]] COPYDATASTRUCT& copydatastruct() const noexcept { return *reinterpret_cast<COPYDATASTRUCT*>(lparam); }
};

struct create_params : public message_params {
  [[nodiscard]] auto createstruct() const noexcept { return reinterpret_cast<CREATESTRUCTW*>(lparam); }
};

struct ctlcolorbtn_params : public message_params {
  [[nodiscard]] HDC hdc() const noexcept   { return reinterpret_cast<HDC>(wparam); }
  [[nodiscard]] HWND hctl() const noexcept { return reinterpret_cast<HWND>(lparam); }
};

using ctlcolordlg_params       = ctlcolorbtn_params;
using ctlcoloredit_params      = ctlcolorbtn_params;
using ctlcolorlistbox_params   = ctlcolorbtn_params;
using ctlcolorscrollbar_params = ctlcolorbtn_params;
using ctlcolorstatic_params    = ctlcolorbtn_params;

struct deadchar_params : public message_params {
  [[nodiscard]] WORD char_code() const noexcept           { return static_cast<WORD>(wparam); }
  [[nodiscard]] WORD repeat_count() const noexcept        { return LOWORD(lparam); }
  [[nodiscard]] BYTE scan_code() const noexcept           { return LOBYTE(HIWORD(lparam)); }
  [[nodiscard]] bool is_extended_key() const noexcept     { return (lparam >> 24) & 1; }
  [[nodiscard]] bool has_alt_key() const noexcept         { return (lparam >> 29) & 1; }
  [[nodiscard]] bool key_previously_down() const noexcept { return (lparam >> 30) & 1; }
  [[nodiscard]] bool key_being_released() const noexcept  { return (lparam >> 31) & 1; }
};

struct deleteitem_params : public message_params {
  [[nodiscard]] WORD control_id() const noexcept                    { return static_cast<WORD>(wparam); }
  [[nodiscard]] DELETEITEMSTRUCT& deleteitemstruct() const noexcept { return *reinterpret_cast<DELETEITEMSTRUCT*>(lparam); }
};

using destroy_params          = message_params;
using destroyclipboard_params = message_params;

struct devmodechange_params : public message_params {
  [[nodiscard]] const wchar_t* device_name() const noexcept { return reinterpret_cast<const wchar_t*>(lparam); }
};

struct displaychange_params : public message_params {
  [[nodiscard]] UINT bits_per_pixel() const noexcept { return static_cast<UINT>(wparam); }
  [[nodiscard]] SIZE sz() const noexcept             { return {LOWORD(lparam), HIWORD(lparam)}; }
};

using drawclipboard_params = message_params;

struct drawitem_params : public message_params {
  [[nodiscard]] WORD control_id() const noexcept                { return static_cast<WORD>(wparam); }
  [[nodiscard]] bool is_from_menu() const noexcept              { return control_id() == 0; }
  [[nodiscard]] DRAWITEMSTRUCT& drawitemstruct() const noexcept { return *reinterpret_cast<DRAWITEMSTRUCT*>(lparam); }
};

struct dropfiles_params : public message_params {
  [[nodiscard]] HDROP hdrop() const noexcept { return reinterpret_cast<HDROP>(wparam); }
};

struct enable_params : public message_params {
  [[nodiscard]] bool has_been_enabled() const noexcept { return wparam != FALSE; }
};

struct endsession_params : public message_params {
  [[nodiscard]] bool is_session_being_ended() const noexcept { return wparam != FALSE; }
  [[nodiscard]] bool is_system_issue() const noexcept        { return (lparam & ENDSESSION_CLOSEAPP) != 0; }
  [[nodiscard]] bool is_forced_critical() const noexcept     { return (lparam & ENDSESSION_CRITICAL) != 0; }
  [[nodiscard]] bool is_logoff() const noexcept              { return (lparam & ENDSESSION_LOGOFF) != 0; }
  [[nodiscard]] bool is_shutdown() const noexcept            { return lparam == 0; }
};

struct enteridle_params : public message_params {
  [[nodiscard]] bool is_menu_displayed() const noexcept { return wparam == MSGF_MENU; }
  [[nodiscard]] HWND hwindow() const noexcept           { return reinterpret_cast<HWND>(lparam); }
};

struct entermenuloop_params : public message_params {
  [[nodiscard]] bool uses_trackpopupmenu() const noexcept { return wparam != FALSE; }
};

using entersizemove_params = message_params;

struct erasebkgnd_params : public message_params {
  [[nodiscard]] HDC hdc() const noexcept { return reinterpret_cast<HDC>(wparam); }
};

struct exitmenuloop_params : public message_params {
  [[nodiscard]] bool is_shortcut_menu() const noexcept { return wparam != FALSE; }
};

using exitsizemove_params = message_params;
using fontchange_params   = message_params;

struct getdlgcode_params : public message_params {
  [[nodiscard]] BYTE vkey_code() const noexcept { return static_cast<BYTE>(wparam); }
  [[nodiscard]] bool is_query() const noexcept  { return lparam == 0; }
  [[nodiscard]] MSG* msg() const noexcept       { return is_query() ? nullptr : reinterpret_cast<MSG*>(lparam); }
};

using getfont_params   = message_params;
using gethotkey_params = message_params;

struct geticon_params : public message_params {
  [[nodiscard]] bool is_big() const noexcept       { return wparam == ICON_BIG; }
  [[nodiscard]] bool is_small() const noexcept     { return wparam == ICON_SMALL; }
  [[nodiscard]] bool is_small_app() const noexcept { return wparam == ICON_SMALL2; }
  [[nodiscard]] UINT dpi() const noexcept          { return static_cast<UINT>(lparam); }
};

struct getminmaxinfo_params : public message_params {
  [[nodiscard]] MINMAXINFO& minmaxinfo() const noexcept { return *reinterpret_cast<MINMAXINFO*>(lparam); }
};

struct gettext_params : public message_params {
  [[nodiscard]] UINT buffer_size() const noexcept { return static_cast<UINT>(wparam); }
  [[nodiscard]] wchar_t* buffer() const noexcept  { return reinterpret_cast<wchar_t*>(lparam); }
};

using gettextlength_params = message_params;

struct help_params : public message_params {
  [[nodiscard]] HELPINFO& helpinfo() const noexcept { return *reinterpret_cast<HELPINFO*>(lparam); }
};

struct hotkey_params : public message_params {
  [[nodiscard]] bool is_snap_desktop() const noexcept { return wparam == IDHOT_SNAPDESKTOP; }
  [[nodiscard]] bool is_snap_window() const noexcept  { return wparam == IDHOT_SNAPWINDOW; }
  [[nodiscard]] bool has_alt() const noexcept         { return (LOWORD(lparam) & MOD_ALT) != 0; }
  [[nodiscard]] bool has_ctrl() const noexcept        { return (LOWORD(lparam) & MOD_CONTROL) != 0; }
  [[nodiscard]] bool has_shift() const noexcept       { return (LOWORD(lparam) & MOD_SHIFT) != 0; }
  [[nodiscard]] bool has_win() const noexcept         { return (LOWORD(lparam) & MOD_WIN) != 0; }
  [[nodiscard]] BYTE vkey_code() const noexcept       { return static_cast<BYTE>(HIWORD(lparam)); }
};

struct hscroll_params : public message_params {
  [[nodiscard]] WORD scroll_request() const noexcept { return LOWORD(wparam); }
  [[nodiscard]] WORD scroll_pos() const noexcept     { return HIWORD(wparam); }
  [[nodiscard]] HWND scrollbar() const noexcept      { return reinterpret_cast<HWND>(lparam); }
};

using vscroll_params = hscroll_params;

struct hscrollclipboard_params : public message_params {
  [[nodiscard]] HWND clipboard_viewer() const noexcept { return reinterpret_cast<HWND>(wparam); }
  [[nodiscard]] WORD scroll_event() const noexcept     { return LOWORD(lparam); }
  [[nodiscard]] WORD scroll_pos() const noexcept       { return HIWORD(lparam); }
};

using vscrollclipboard_params = hscrollclipboard_params;

struct iconerasebkgnd_params : public message_params {
  [[nodiscard]] HDC hdc() const noexcept { return reinterpret_cast<HDC>(wparam); }
};

struct initdialog_params : public message_params {
  [[nodiscard]] HWND focused_ctrl() const noexcept { return reinterpret_cast<HWND>(wparam); }
};

struct initmenu_params : public message_params {
  [[nodiscard]] HMENU hmenu() const noexcept { return reinterpret_cast<HMENU>(wparam); }
};

struct initmenupopup_params : public message_params {
  [[nodiscard]] HMENU hmenu() const noexcept             { return reinterpret_cast<HMENU>(wparam); }
  [[nodiscard]] short relative_pos() const noexcept      { return LOWORD(lparam); }
  [[nodiscard]] bool is_window_menu() const noexcept     { return HIWORD(lparam) != FALSE; }
};

struct inputlangchange_params : public message_params {
  [[nodiscard]] DWORD new_charset() const noexcept   { return static_cast<DWORD>(wparam); }
  [[nodiscard]] HKL keyboard_layout() const noexcept { return reinterpret_cast<HKL>(lparam); }
};

struct inputlangchangerequest_params : public message_params {
  [[nodiscard]] bool previous_chosen() const noexcept      { return (wparam & INPUTLANGCHANGE_BACKWARD) != 0; }
  [[nodiscard]] bool next_chosen() const noexcept          { return (wparam & INPUTLANGCHANGE_FORWARD) != 0; }
  [[nodiscard]] bool can_be_used_with_sys() const noexcept { return (wparam & INPUTLANGCHANGE_SYSCHARSET) != 0; }
  [[nodiscard]] HKL keyboard_layout() const noexcept       { return reinterpret_cast<HKL>(lparam); }
};

struct keydown_params : public message_params {
  [[nodiscard]] BYTE vkey_code() const noexcept           { return static_cast<BYTE>(wparam); }
  [[nodiscard]] WORD repeat_count() const noexcept        { return LOWORD(lparam); }
  [[nodiscard]] BYTE scan_code() const noexcept           { return LOBYTE(HIWORD(lparam)); }
  [[nodiscard]] bool is_extended_key() const noexcept     { return (lparam >> 24) & 1; }
  [[nodiscard]] bool key_previously_down() const noexcept { return (lparam >> 30) & 1; }
};

using keyup_params = keydown_params;

struct killfocus_params : public message_params {
  [[nodiscard]] HWND focused_window() const noexcept { return reinterpret_cast<HWND>(wparam); }
};

struct lbuttondblclk_params : public pos_params {
  [[nodiscard]] bool has_ctrl() const noexcept       { return (wparam & MK_CONTROL) != 0; }
  [[nodiscard]] bool has_left_btn() const noexcept   { return (wparam & MK_LBUTTON) != 0; }
  [[nodiscard]] bool has_middle_btn() const noexcept { return (wparam & MK_MBUTTON) != 0; }
  [[nodiscard]] bool has_right_btn() const noexcept  { return (wparam & MK_RBUTTON) != 0; }
  [[nodiscard]] bool has_shift() const noexcept      { return (wparam & MK_SHIFT) != 0; }
  [[nodiscard]] bool has_xbtn1() const noexcept      { return (wparam & MK_XBUTTON1) != 0; }
  [[nodiscard]] bool has_xbtn2() const noexcept      { return (wparam & MK_XBUTTON2) != 0; }
};

using lbuttondown_params   = lbuttondblclk_params;
using lbuttonup_params     = lbuttondblclk_params;
using mbuttondblclk_params = lbuttondblclk_params;
using mbuttondown_params   = lbuttondblclk_params;
using mbuttonup_params     = lbuttondblclk_params;
using mousehover_params    = lbuttondblclk_params;
using mousemove_params     = lbuttondblclk_params;
using rbuttondblclk_params = lbuttondblclk_params;
using rbuttondown_params   = lbuttondblclk_params;
using rbuttonup_params     = lbuttondblclk_params;

struct mdiactivate_params : public message_params {
  [[nodiscard]] HWND activated_child() const noexcept   { return reinterpret_cast<HWND>(wparam); }
  [[nodiscard]] HWND deactivated_child() const noexcept { return reinterpret_cast<HWND>(lparam); }
};

struct measureitem_params : public message_params {
  [[nodiscard]] MEASUREITEMSTRUCT& measureitemstruct() const noexcept { return *reinterpret_cast<MEASUREITEMSTRUCT*>(lparam); }
};

struct menuchar_params : public message_params {
  [[nodiscard]] WORD char_code() const noexcept      { return LOWORD(wparam); }
  [[nodiscard]] bool is_window_menu() const noexcept { return HIWORD(wparam) == MF_SYSMENU; }
  [[nodiscard]] HMENU hmenu() const noexcept         { return reinterpret_cast<HMENU>(lparam); }
};

struct menudrag_params : public message_params {
  [[nodiscard]] UINT initial_pos() const noexcept { return static_cast<UINT>(wparam); }
  [[nodiscard]] HMENU hmenu() const noexcept      { return reinterpret_cast<HMENU>(lparam); }
};

struct menugetobject_params : public message_params {
  [[nodiscard]] MENUGETOBJECTINFO& menugetobjectinfo() const noexcept { return *reinterpret_cast<MENUGETOBJECTINFO*>(lparam); }
};

struct menurbuttonup_params : public message_params {
  [[nodiscard]] UINT index() const noexcept  { return static_cast<UINT>(wparam); }
  [[nodiscard]] HMENU hmenu() const noexcept { return reinterpret_cast<HMENU>(lparam); }
};

struct menuselect_params : public message_params {
  [[nodiscard]] WORD item() const noexcept              { return LOWORD(wparam); }
  [[nodiscard]] bool has_bitmap() const noexcept        { return (HIWORD(wparam) & MF_BITMAP) != 0; }
  [[nodiscard]] bool is_checked() const noexcept        { return (HIWORD(wparam) & MF_CHECKED) != 0; }
  [[nodiscard]] bool is_disabled() const noexcept       { return (HIWORD(wparam) & MF_DISABLED) != 0; }
  [[nodiscard]] bool is_grayed() const noexcept         { return (HIWORD(wparam) & MF_GRAYED) != 0; }
  [[nodiscard]] bool is_highlighted() const noexcept    { return (HIWORD(wparam) & MF_HILITE) != 0; }
  [[nodiscard]] bool mouse_selected() const noexcept    { return (HIWORD(wparam) & MF_MOUSESELECT) != 0; }
  [[nodiscard]] bool is_owner_draw() const noexcept     { return (HIWORD(wparam) & MF_OWNERDRAW) != 0; }
  [[nodiscard]] bool opens_popup() const noexcept       { return (HIWORD(wparam) & MF_POPUP) != 0; }
  [[nodiscard]] bool is_sysmenu() const noexcept        { return (HIWORD(wparam) & MF_SYSMENU) != 0; }
  [[nodiscard]] bool system_has_closed() const noexcept { return HIWORD(wparam) == 0xFFFF && !lparam; }
  [[nodiscard]] HMENU hmenu() const noexcept            { return (opens_popup() || is_sysmenu()) ? reinterpret_cast<HMENU>(lparam) : nullptr; }
};

struct mouseactivate_params : public message_params {
  [[nodiscard]] short hit_test_code() const noexcept { return static_cast<short>(LOWORD(lparam)); }
  [[nodiscard]] WORD  mouse_msg_id() const noexcept  { return HIWORD(lparam); }
};

using mouseleave_params = message_params;

struct mousewheel_params : public pos_params {
  [[nodiscard]] short wheel_delta() const noexcept   { return GET_WHEEL_DELTA_WPARAM(wparam); }
  [[nodiscard]] bool has_ctrl() const noexcept       { return (LOWORD(wparam) & MK_CONTROL) != 0; }
  [[nodiscard]] bool has_left_btn() const noexcept   { return (LOWORD(wparam) & MK_LBUTTON) != 0; }
  [[nodiscard]] bool has_middle_btn() const noexcept { return (LOWORD(wparam) & MK_MBUTTON) != 0; }
  [[nodiscard]] bool has_right_btn() const noexcept  { return (LOWORD(wparam) & MK_RBUTTON) != 0; }
  [[nodiscard]] bool has_shift() const noexcept      { return (LOWORD(wparam) & MK_SHIFT) != 0; }
  [[nodiscard]] bool has_xbtn1() const noexcept      { return (LOWORD(wparam) & MK_XBUTTON1) != 0; }
  [[nodiscard]] bool has_xbtn2() const noexcept      { return (LOWORD(wparam) & MK_XBUTTON2) != 0; }
};

using move_params = pos_params;

struct moving_params : public message_params {
  [[nodiscard]] RECT& screen_coords() const noexcept { return *reinterpret_cast<RECT*>(lparam); }
};

struct ncactivate_params : public message_params {
  [[nodiscard]] bool is_active() const noexcept { return wparam == TRUE; }
};

struct nccalcsize_params : public message_params {
  [[nodiscard]] bool is_nccalcsize() const noexcept        { return wparam == TRUE; }
  [[nodiscard]] bool is_rect() const noexcept              { return wparam == FALSE; }
  [[nodiscard]] NCCALCSIZE_PARAMS& params() const noexcept { return *reinterpret_cast<NCCALCSIZE_PARAMS*>(lparam); }
  [[nodiscard]] RECT& rect() const noexcept                { return *reinterpret_cast<RECT*>(lparam); }
};

struct nccreate_params : public message_params {
  [[nodiscard]] auto createstruct() const noexcept { return reinterpret_cast<CREATESTRUCTW*>(lparam); }
};

using ncdestroy_params = message_params;
using nchittest_params = pos_params;

struct nclbuttondblclk_params : public pos_params {
  [[nodiscard]] short hit_test_code() const noexcept { return static_cast<short>(wparam); }
};

using nclbuttondown_params   = nclbuttondblclk_params;
using nclbuttonup_params     = nclbuttondblclk_params;
using ncmbuttondblclk_params = nclbuttondblclk_params;
using ncmbuttondown_params   = nclbuttondblclk_params;
using ncmbuttonup_params     = nclbuttondblclk_params;
using ncmousemove_params     = nclbuttondblclk_params;
using ncrbuttondblclk_params = nclbuttondblclk_params;
using ncrbuttondown_params   = nclbuttondblclk_params;
using ncrbuttonup_params     = nclbuttondblclk_params;

struct ncpaint_params : public message_params {
  [[nodiscard]] HRGN hrgn() const noexcept { return reinterpret_cast<HRGN>(wparam); }
};

struct nextdlgctl_params : public message_params {
  [[nodiscard]] bool has_ctrl_receiving_focus() const noexcept { return LOWORD(lparam) != FALSE; }
  [[nodiscard]] HWND ctrl_receiving_focus() const noexcept     { return LOWORD(lparam) ? reinterpret_cast<HWND>(wparam) : nullptr; }
  [[nodiscard]] bool focus_next() const noexcept               { return wparam == 0; }
};

struct nextmenu_params : public message_params {
  [[nodiscard]] BYTE vkey_code() const noexcept           { return static_cast<BYTE>(wparam); }
  [[nodiscard]] MDINEXTMENU& mdinextmenu() const noexcept { return *reinterpret_cast<MDINEXTMENU*>(lparam); }
};

struct notifyformat_params : public message_params {
  [[nodiscard]] HWND window_from() const noexcept           { return reinterpret_cast<HWND>(wparam); }
  [[nodiscard]] bool is_query_from_control() const noexcept { return lparam == NF_QUERY; }
  [[nodiscard]] bool is_requery_to_control() const noexcept { return lparam == NF_REQUERY; }
};

using paint_params = message_params;

struct paintclipboard_params : public message_params {
  [[nodiscard]] HWND clipboard_viewer() const noexcept          { return reinterpret_cast<HWND>(wparam); }
  [[nodiscard]] const PAINTSTRUCT& paintstruct() const noexcept { return *reinterpret_cast<const PAINTSTRUCT*>(lparam); }
};

struct palettechanged_params : public message_params {
  [[nodiscard]] HWND window_origin() const noexcept { return reinterpret_cast<HWND>(wparam); }
};

using paletteischanging_params = palettechanged_params;

struct parentnotify_params : public pos_params {
  [[nodiscard]] UINT event_message() const noexcept { return static_cast<UINT>(LOWORD(wparam)); }
  [[nodiscard]] WORD child_id() const noexcept      { return HIWORD(wparam); }
  [[nodiscard]] HWND child_hwnd() const noexcept    { return reinterpret_cast<HWND>(lparam); }
  [[nodiscard]] bool is_xbutton1() const noexcept   { return HIWORD(wparam) == XBUTTON1; }
  [[nodiscard]] WORD pointer_flag() const noexcept  { return HIWORD(wparam); }
};

struct powerbroadcast_params : public message_params {
  [[nodiscard]] bool is_power_status_change() const noexcept           { return wparam == PBT_APMPOWERSTATUSCHANGE; }
  [[nodiscard]] bool is_resuming() const noexcept                      { return wparam == PBT_APMRESUMEAUTOMATIC; }
  [[nodiscard]] bool is_suspending() const noexcept                    { return wparam == PBT_APMSUSPEND; }
  [[nodiscard]] bool is_power_setting_change() const noexcept          { return wparam == PBT_POWERSETTINGCHANGE; }
  [[nodiscard]] POWERBROADCAST_SETTING& power_setting() const noexcept { return *reinterpret_cast<POWERBROADCAST_SETTING*>(lparam); }
};

struct print_params : public message_params {
  [[nodiscard]] HDC hdc() const noexcept    { return reinterpret_cast<HDC>(wparam); }
  [[nodiscard]] UINT flags() const noexcept { return static_cast<UINT>(lparam); }
};

using printclient_params   = print_params;
using querydragicon_params = message_params;

struct queryendsession_params : public message_params {
  [[nodiscard]] bool is_system_issue() const noexcept    { return (lparam & ENDSESSION_CLOSEAPP) != 0; }
  [[nodiscard]] bool is_forced_critical() const noexcept { return (lparam & ENDSESSION_CRITICAL) != 0; }
  [[nodiscard]] bool is_logoff() const noexcept          { return (lparam & ENDSESSION_LOGOFF) != 0; }
  [[nodiscard]] bool is_shutdown() const noexcept        { return lparam == 0; }
};

using querynewpalette_params  = message_params;
using queryopen               = message_params;
using renderallformats_params = message_params;

struct renderformat_params : public message_params {
  [[nodiscard]] WORD clipboard_format() const noexcept { return static_cast<WORD>(wparam); }
};

struct setcursor_params : public message_params {
  [[nodiscard]] HWND cursor_owner() const noexcept   { return reinterpret_cast<HWND>(wparam); }
  [[nodiscard]] short hit_test_code() const noexcept { return static_cast<short>(LOWORD(lparam)); }
  [[nodiscard]] WORD mouse_msg_id() const noexcept   { return HIWORD(lparam); }
};

struct setfocus_params : public message_params {
  [[nodiscard]] HWND unfocused_window() const noexcept { return reinterpret_cast<HWND>(wparam); }
};

struct setfont_params : public message_params {
  [[nodiscard]] HFONT hfont() const noexcept        { return reinterpret_cast<HFONT>(wparam); }
  [[nodiscard]] bool should_redraw() const noexcept { return LOWORD(lparam) != FALSE; }
  void set_should_redraw(bool f) noexcept           { lparam = MAKELONG(HIWORD(lparam), LOWORD(f ? TRUE : FALSE)); }
};

struct sethotkey_params : public message_params {
  [[nodiscard]] BYTE vkey_code() const noexcept    { return static_cast<BYTE>(LOWORD(wparam)); }
  [[nodiscard]] bool has_alt() const noexcept      { return (HIWORD(wparam) & 0x04) != 0; }
  [[nodiscard]] bool has_ctrl() const noexcept     { return (HIWORD(wparam) & 0x02) != 0; }
  [[nodiscard]] bool has_extended() const noexcept { return (HIWORD(wparam) & 0x08) != 0; }
  [[nodiscard]] bool has_shift() const noexcept    { return (HIWORD(wparam) & 0x01) != 0; }
};

struct seticon_params : public message_params {
  [[nodiscard]] bool is_small() const noexcept   { return wparam == ICON_SMALL; }
  [[nodiscard]] HICON hicon() const noexcept     { return reinterpret_cast<HICON>(lparam); }
  [[nodiscard]] bool is_removed() const noexcept { return hicon() == nullptr; }
};

struct setredraw_params : public message_params {
  [[nodiscard]] bool can_redraw() const noexcept { return wparam != FALSE; }
};

struct settext_params : public message_params {
  [[nodiscard]] const wchar_t* text() const noexcept { return reinterpret_cast<const wchar_t*>(lparam); }
};

struct settingchange_params : public message_params {
  [[nodiscard]] const wchar_t* string_id() const noexcept { return reinterpret_cast<const wchar_t*>(lparam); }
  [[nodiscard]] bool is_policy() const noexcept           { return 0 == lstrcmpW(string_id(), L"Policy"); }
  [[nodiscard]] bool is_locale() const noexcept           { return 0 == lstrcmpW(string_id(), L"intl"); }
  [[nodiscard]] bool is_environment_vars() const noexcept { return 0 == lstrcmpW(string_id(), L"Environment"); }
};

struct showwindow_params : public message_params {
  [[nodiscard]] bool is_being_shown() const noexcept           { return wparam != FALSE; }
  [[nodiscard]] bool is_other_away() const noexcept            { return lparam == SW_OTHERUNZOOM; }
  [[nodiscard]] bool is_other_over() const noexcept            { return lparam == SW_OTHERZOOM; }
  [[nodiscard]] bool is_owner_being_minimized() const noexcept { return lparam == SW_PARENTCLOSING; }
  [[nodiscard]] bool is_owner_being_restored() const noexcept  { return lparam == SW_PARENTOPENING; }
};

struct size_params : public message_params {
  [[nodiscard]] bool is_other_maximized() const noexcept { return wparam == 4; }
  [[nodiscard]] bool is_maximized() const noexcept       { return wparam == 2; }
  [[nodiscard]] bool is_other_restored() const noexcept  { return wparam == 3; }
  [[nodiscard]] bool is_minimized() const noexcept       { return wparam == 1; }
  [[nodiscard]] bool is_restored() const noexcept        { return wparam == 0; }
  [[nodiscard]] SIZE sz() const noexcept                 { return {LOWORD(lparam), HIWORD(lparam)}; }
};

struct sizeclipboard_params : public message_params {
  [[nodiscard]] HWND clipboard_viewer() const noexcept      { return reinterpret_cast<HWND>(wparam); }
  [[nodiscard]] const RECT& clipboard_rect() const noexcept { return *reinterpret_cast<const RECT*>(lparam); }
};

struct sizing_params : public message_params {
  [[nodiscard]] WORD edge() const noexcept           { return static_cast<WORD>(wparam); }
  [[nodiscard]] RECT& screen_coords() const noexcept { return *reinterpret_cast<RECT*>(lparam); }
};

struct spoolerstatus_params : public message_params {
  [[nodiscard]] UINT status_flag() const noexcept    { return static_cast<UINT>(wparam); }
  [[nodiscard]] WORD remaining_jobs() const noexcept { return LOWORD(lparam); }
};

struct stylechanged_params : public message_params {
  [[nodiscard]] bool is_style() const noexcept                  { return (wparam & GWL_STYLE) != 0; }
  [[nodiscard]] bool is_ex_style() const noexcept               { return (wparam & GWL_EXSTYLE) != 0; }
  [[nodiscard]] const STYLESTRUCT& stylestruct() const noexcept { return *reinterpret_cast<const STYLESTRUCT*>(lparam); }
};

using stylechanging_params = stylechanged_params;
using syschar_params       = char_params;

struct syscommand_params : public pos_params {
  [[nodiscard]] WORD command_type() const noexcept { return static_cast<WORD>(wparam); }
};

using sysdeadchar_params = deadchar_params;
using syskeydown_params  = keydown_params;
using syskeyup_params    = keyup_params;

struct tcard_params : public message_params {
  [[nodiscard]] UINT action_id() const noexcept   { return static_cast<UINT>(wparam); }
  [[nodiscard]] long action_data() const noexcept { return static_cast<long>(lparam); }
};

using timechange_params = message_params;

struct timer_params : public message_params {
  [[nodiscard]] UINT_PTR timer_id() const noexcept  { return static_cast<UINT_PTR>(wparam); }
  [[nodiscard]] TIMERPROC callback() const noexcept { return reinterpret_cast<TIMERPROC>(lparam); }
};

struct uninitmenupopup_params : public message_params {
  [[nodiscard]] HMENU hmenu() const noexcept  { return reinterpret_cast<HMENU>(wparam); }
  [[nodiscard]] WORD menu_id() const noexcept { return HIWORD(lparam); }
};

using userchanged_params = message_params;

struct vkeytoitem_params : public message_params {
  [[nodiscard]] BYTE vkey_code() const noexcept         { return static_cast<BYTE>(LOWORD(wparam)); }
  [[nodiscard]] WORD current_caret_pos() const noexcept { return HIWORD(wparam); }
  [[nodiscard]] HWND hlistbox() const noexcept          { return reinterpret_cast<HWND>(lparam); }
};

struct windowposchanged_params : public message_params {
  [[nodiscard]] WINDOWPOS& windowpos() const noexcept { return *reinterpret_cast<WINDOWPOS*>(lparam); }
};

using windowposchanging_params = windowposchanged_params;

struct quit_params : public message_params {
  [[nodiscard]] int exit_code() const noexcept { return static_cast<int>(wparam); }
};

struct dpichanged_params : public message_params {
  [[nodiscard]] WORD dpi_x() const noexcept { return LOWORD(wparam); }
  [[nodiscard]] WORD dpi_y() const noexcept { return HIWORD(wparam); }
  [[nodiscard]] RECT* suggested_rect() const noexcept { return reinterpret_cast<RECT*>(lparam); }
};

}
