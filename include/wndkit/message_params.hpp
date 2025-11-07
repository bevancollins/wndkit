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
  POINT pos() const {
    return {
      static_cast<int>(static_cast<short>(LOWORD(lparam))),
      static_cast<int>(static_cast<short>(HIWORD(lparam)))
    };
  }
};

struct command_params : public message_params {
  bool is_from_menu() const        { return HIWORD(wparam) == 0; }
  bool is_from_accelerator() const { return HIWORD(wparam) == 1; }
  bool is_from_control() const     { return !is_from_menu() && !is_from_accelerator(); }
  WORD id() const                  { return LOWORD(wparam); }
  WORD control_notif_code() const  { return HIWORD(wparam); }
  HWND control_hwnd() const        { return reinterpret_cast<HWND>(lparam); }
};

struct notify_params : public message_params {
  NMHDR& nmhdr() const { return *reinterpret_cast<NMHDR*>(lparam); }
};

struct activate_params : public message_params {
  bool is_being_activated() const           { return wparam != WA_INACTIVE; }
  bool activated_not_by_mouse_click() const { return wparam == WA_ACTIVE; }
  bool activated_by_mouse_click() const     { return wparam == WA_CLICKACTIVE; }
  HWND swapped_window() const               { return reinterpret_cast<HWND>(lparam); }
};

struct activateapp_params : public message_params {
  bool is_being_activated() const { return wparam != FALSE; }
  DWORD thread_id() const         { return static_cast<DWORD>(lparam); }
};

struct askcbformatname_params : public message_params {
  UINT szbuffer() const   { return static_cast<UINT>(wparam); }
  wchar_t* buffer() const { return reinterpret_cast<wchar_t*>(lparam); }
};

using cancelmode_params = message_params;

struct capturechanged_params : public message_params {
  HWND window_gaining_mouse() const { return reinterpret_cast<HWND>(lparam); }
};

struct changecbchain_params : public message_params {
  HWND window_being_removed() const { return reinterpret_cast<HWND>(wparam); }
  HWND next_window() const          { return reinterpret_cast<HWND>(lparam); }
  bool is_last_window() const       { return next_window() == nullptr; }
};

struct char_params : public message_params {
  WORD char_code() const           { return static_cast<WORD>(wparam); }
  WORD repeat_count() const        { return LOWORD(lparam); }
  BYTE scan_code() const           { return LOBYTE(HIWORD(lparam)); }
  bool is_extended_key() const     { return (lparam >> 24) & 1; }
  bool has_alt_key() const         { return (lparam >> 29) & 1; }
  bool key_previously_down() const { return (lparam >> 30) & 1; }
  bool key_being_released() const  { return (lparam >> 31) & 1; }
};

struct chartoitem_params : public message_params {
  WORD char_code() const         { return LOWORD(wparam); }
  WORD current_caret_pos() const { return HIWORD(wparam); }
  HWND hlistbox() const          { return reinterpret_cast<HWND>(lparam); }
};

using childactivate_params = message_params;
using close_params         = message_params;

struct compacting_params : public message_params {
  UINT cpu_time_ratio() const { return static_cast<UINT>(wparam); }
};

struct compareitem_params : public message_params {
  WORD control_id() const                      { return static_cast<WORD>(wparam); }
  COMPAREITEMSTRUCT& compareitemstruct() const { return *reinterpret_cast<COMPAREITEMSTRUCT*>(lparam); }
};

struct contextmenu_params : public pos_params {
  HWND target() const { return reinterpret_cast<HWND>(wparam); }
};

struct copydata_params : public message_params {
  COPYDATASTRUCT& copydatastruct() const { return *reinterpret_cast<COPYDATASTRUCT*>(lparam); }
};

struct create_params : public message_params {
  auto createstruct() const { return reinterpret_cast<CREATESTRUCTW*>(lparam); }
};

struct ctlcolorbtn_params : public message_params {
  HDC hdc() const   { return reinterpret_cast<HDC>(wparam); }
  HWND hctl() const { return reinterpret_cast<HWND>(lparam); }
};

using ctlcolordlg_params       = ctlcolorbtn_params;
using ctlcoloredit_params      = ctlcolorbtn_params;
using ctlcolorlistbox_params   = ctlcolorbtn_params;
using ctlcolorscrollbar_params = ctlcolorbtn_params;
using ctlcolorstatic_params    = ctlcolorbtn_params;

struct deadchar_params : public message_params {
  WORD char_code() const           { return static_cast<WORD>(wparam); }
  WORD repeat_count() const        { return LOWORD(lparam); }
  BYTE scan_code() const           { return LOBYTE(HIWORD(lparam)); }
  bool is_extended_key() const     { return (lparam >> 24) & 1; }
  bool has_alt_key() const         { return (lparam >> 29) & 1; }
  bool key_previously_down() const { return (lparam >> 30) & 1; }
  bool key_being_released() const  { return (lparam >> 31) & 1; }
};

struct deleteitem_params : public message_params {
  WORD control_id() const                    { return static_cast<WORD>(wparam); }
  DELETEITEMSTRUCT& deleteitemstruct() const { return *reinterpret_cast<DELETEITEMSTRUCT*>(lparam); }
};

using destroy_params          = message_params;
using destroyclipboard_params = message_params;

struct devmodechange_params : public message_params {
  const wchar_t* device_name() const { return reinterpret_cast<const wchar_t*>(lparam); }
};

struct displaychange_params : public message_params {
  UINT bits_per_pixel() const { return static_cast<UINT>(wparam); }
  SIZE sz() const             { return {LOWORD(lparam), HIWORD(lparam)}; }
};

using drawclipboard_params = message_params;

struct drawitem_params : public message_params {
  WORD control_id() const                { return static_cast<WORD>(wparam); }
  bool is_from_menu() const              { return control_id() == 0; }
  DRAWITEMSTRUCT& drawitemstruct() const { return *reinterpret_cast<DRAWITEMSTRUCT*>(lparam); }
};

struct dropfiles_params : public message_params {
  HDROP hdrop() const { return reinterpret_cast<HDROP>(wparam); }
};

struct enable_params : public message_params {
  bool has_been_enabled() const { return wparam != FALSE; }
};

struct endsession_params : public message_params {
  bool is_session_being_ended() const { return wparam != FALSE; }
  bool is_system_issue() const        { return (lparam & ENDSESSION_CLOSEAPP) != 0; }
  bool is_forced_critical() const     { return (lparam & ENDSESSION_CRITICAL) != 0; }
  bool is_logoff() const              { return (lparam & ENDSESSION_LOGOFF) != 0; }
  bool is_shutdown() const            { return lparam == 0; }
};

struct enteridle_params : public message_params {
  bool is_menu_displayed() const { return wparam == MSGF_MENU; }
  HWND hwindow() const           { return reinterpret_cast<HWND>(lparam); }
};

struct entermenuloop_params : public message_params {
  bool uses_trackpopupmenu() const { return wparam != FALSE; }
};

using entersizemove_params = message_params;

struct erasebkgnd_params : public message_params {
  HDC hdc() const { return reinterpret_cast<HDC>(wparam); }
};

struct exitmenuloop_params : public message_params {
  bool is_shortcut_menu() const { return wparam != FALSE; }
};

using exitsizemove_params = message_params;
using fontchange_params   = message_params;

struct getdlgcode_params : public message_params {
  BYTE vkey_code() const { return static_cast<BYTE>(wparam); }
  bool is_query() const  { return lparam == 0; }
  MSG* msg() const       { return is_query() ? nullptr : reinterpret_cast<MSG*>(lparam); }
};

using getfont_params   = message_params;
using gethotkey_params = message_params;

struct geticon_params : public message_params {
  bool is_big() const       { return wparam == ICON_BIG; }
  bool is_small() const     { return wparam == ICON_SMALL; }
  bool is_small_app() const { return wparam == ICON_SMALL2; }
  UINT dpi() const          { return static_cast<UINT>(lparam); }
};

struct getminmaxinfo_params : public message_params {
  MINMAXINFO& minmaxinfo() const { return *reinterpret_cast<MINMAXINFO*>(lparam); }
};

struct gettext_params : public message_params {
  UINT buffer_size() const { return static_cast<UINT>(wparam); }
  wchar_t* buffer() const  { return reinterpret_cast<wchar_t*>(lparam); }
};

using gettextlength_params = message_params;

struct help_params : public message_params {
  HELPINFO& helpinfo() const { return *reinterpret_cast<HELPINFO*>(lparam); }
};

struct hotkey_params : public message_params {
  bool is_snap_desktop() const { return wparam == IDHOT_SNAPDESKTOP; }
  bool is_snap_window() const  { return wparam == IDHOT_SNAPWINDOW; }
  bool has_alt() const         { return (LOWORD(lparam) & MOD_ALT) != 0; }
  bool has_ctrl() const        { return (LOWORD(lparam) & MOD_CONTROL) != 0; }
  bool has_shift() const       { return (LOWORD(lparam) & MOD_SHIFT) != 0; }
  bool has_win() const         { return (LOWORD(lparam) & MOD_WIN) != 0; }
  BYTE vkey_code() const       { return static_cast<BYTE>(HIWORD(lparam)); }
};

struct hscroll_params : public message_params {
  WORD scroll_request() const { return LOWORD(wparam); }
  WORD scroll_pos() const     { return HIWORD(wparam); }
  HWND scrollbar() const      { return reinterpret_cast<HWND>(lparam); }
};

using vscroll_params = hscroll_params;

struct hscrollclipboard_params : public message_params {
  HWND clipboard_viewer() const { return reinterpret_cast<HWND>(wparam); }
  WORD scroll_event() const     { return LOWORD(lparam); }
  WORD scroll_pos() const       { return HIWORD(lparam); }
};

using vscrollclipboard_params = hscrollclipboard_params;

struct iconerasebkgnd_params : public message_params {
  HDC hdc() const { return reinterpret_cast<HDC>(wparam); }
};

struct initdialog_params : public message_params {
  HWND focused_ctrl() const { return reinterpret_cast<HWND>(wparam); }
};

struct initmenu_params : public message_params {
  HMENU hmenu() const { return reinterpret_cast<HMENU>(wparam); }
};

struct initmenupopup_params : public message_params {
  HMENU hmenu() const             { return reinterpret_cast<HMENU>(wparam); }
  short relative_pos() const      { return LOWORD(lparam); }
  bool is_window_menu() const     { return HIWORD(lparam) != FALSE; }
};

struct inputlangchange_params : public message_params {
  DWORD new_charset() const   { return static_cast<DWORD>(wparam); }
  HKL keyboard_layout() const { return reinterpret_cast<HKL>(lparam); }
};

struct inputlangchangerequest_params : public message_params {
  bool previous_chosen() const      { return (wparam & INPUTLANGCHANGE_BACKWARD) != 0; }
  bool next_chosen() const          { return (wparam & INPUTLANGCHANGE_FORWARD) != 0; }
  bool can_be_used_with_sys() const { return (wparam & INPUTLANGCHANGE_SYSCHARSET) != 0; }
  HKL keyboard_layout() const       { return reinterpret_cast<HKL>(lparam); }
};

struct keydown_params : public message_params {
  BYTE vkey_code() const           { return static_cast<BYTE>(wparam); }
  WORD repeat_count() const        { return LOWORD(lparam); }
  BYTE scan_code() const           { return LOBYTE(HIWORD(lparam)); }
  bool is_extended_key() const     { return (lparam >> 24) & 1; }
  bool key_previously_down() const { return (lparam >> 30) & 1; }
};

using keyup_params = keydown_params;

struct killfocus_params : public message_params {
  HWND focused_window() const { return reinterpret_cast<HWND>(wparam); }
};

struct lbuttondblclk_params : public pos_params {
  bool has_ctrl() const       { return (wparam & MK_CONTROL) != 0; }
  bool has_left_btn() const   { return (wparam & MK_LBUTTON) != 0; }
  bool has_middle_btn() const { return (wparam & MK_MBUTTON) != 0; }
  bool has_right_btn() const  { return (wparam & MK_RBUTTON) != 0; }
  bool has_shift() const      { return (wparam & MK_SHIFT) != 0; }
  bool has_xbtn1() const      { return (wparam & MK_XBUTTON1) != 0; }
  bool has_xbtn2() const      { return (wparam & MK_XBUTTON2) != 0; }
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
  HWND activated_child() const   { return reinterpret_cast<HWND>(wparam); }
  HWND deactivated_child() const { return reinterpret_cast<HWND>(lparam); }
};

struct measureitem_params : public message_params {
  MEASUREITEMSTRUCT& measureitemstruct() const { return *reinterpret_cast<MEASUREITEMSTRUCT*>(lparam); }
};

struct menuchar_params : public message_params {
  WORD char_code() const      { return LOWORD(wparam); }
  bool is_window_menu() const { return HIWORD(wparam) == MF_SYSMENU; }
  HMENU hmenu() const         { return reinterpret_cast<HMENU>(lparam); }
};

struct menudrag_params : public message_params {
  UINT initial_pos() const { return static_cast<UINT>(wparam); }
  HMENU hmenu() const      { return reinterpret_cast<HMENU>(lparam); }
};

struct menugetobject_params : public message_params {
  MENUGETOBJECTINFO& menugetobjectinfo() const { return *reinterpret_cast<MENUGETOBJECTINFO*>(lparam); }
};

struct menurbuttonup_params : public message_params {
  UINT index() const  { return static_cast<UINT>(wparam); }
  HMENU hmenu() const { return reinterpret_cast<HMENU>(lparam); }
};

struct menuselect_params : public message_params {
  WORD item() const              { return LOWORD(wparam); }
  bool has_bitmap() const        { return (HIWORD(wparam) & MF_BITMAP) != 0; }
  bool is_checked() const        { return (HIWORD(wparam) & MF_CHECKED) != 0; }
  bool is_disabled() const       { return (HIWORD(wparam) & MF_DISABLED) != 0; }
  bool is_grayed() const         { return (HIWORD(wparam) & MF_GRAYED) != 0; }
  bool is_highlighted() const    { return (HIWORD(wparam) & MF_HILITE) != 0; }
  bool mouse_selected() const    { return (HIWORD(wparam) & MF_MOUSESELECT) != 0; }
  bool is_owner_draw() const     { return (HIWORD(wparam) & MF_OWNERDRAW) != 0; }
  bool opens_popup() const       { return (HIWORD(wparam) & MF_POPUP) != 0; }
  bool is_sysmenu() const        { return (HIWORD(wparam) & MF_SYSMENU) != 0; }
  bool system_has_closed() const { return HIWORD(wparam) == 0xFFFF && !lparam; }
  HMENU hmenu() const            { return (opens_popup() || is_sysmenu()) ? reinterpret_cast<HMENU>(lparam) : nullptr; }
};

struct mouseactivate_params : public message_params {
  short hit_test_code() const { return static_cast<short>(LOWORD(lparam)); }
  WORD  mouse_msg_id() const  { return HIWORD(lparam); }
};

using mouseleave_params = message_params;

struct mousewheel_params : public pos_params {
  short wheel_delta() const   { return GET_WHEEL_DELTA_WPARAM(wparam); }
  bool has_ctrl() const       { return (LOWORD(wparam) & MK_CONTROL) != 0; }
  bool has_left_btn() const   { return (LOWORD(wparam) & MK_LBUTTON) != 0; }
  bool has_middle_btn() const { return (LOWORD(wparam) & MK_MBUTTON) != 0; }
  bool has_right_btn() const  { return (LOWORD(wparam) & MK_RBUTTON) != 0; }
  bool has_shift() const      { return (LOWORD(wparam) & MK_SHIFT) != 0; }
  bool has_xbtn1() const      { return (LOWORD(wparam) & MK_XBUTTON1) != 0; }
  bool has_xbtn2() const      { return (LOWORD(wparam) & MK_XBUTTON2) != 0; }
};

using move_params = pos_params;

struct moving_params : public message_params {
  RECT& screen_coords() const { return *reinterpret_cast<RECT*>(lparam); }
};

struct ncactivate_params : public message_params {
  bool is_active() const { return wparam == TRUE; }
};

struct nccalcsize_params : public message_params {
  bool is_nccalcsize() const        { return wparam == TRUE; }
  bool is_rect() const              { return wparam == FALSE; }
  NCCALCSIZE_PARAMS& params() const { return *reinterpret_cast<NCCALCSIZE_PARAMS*>(lparam); }
  RECT& rect() const                { return *reinterpret_cast<RECT*>(lparam); }
};

struct nccreate_params : public message_params {
  auto createstruct() const { return reinterpret_cast<CREATESTRUCTW*>(lparam); }
};

using ncdestroy_params = message_params;
using nchittest_params = pos_params;

struct nclbuttondblclk_params : public pos_params {
  short hit_test_code() const { return static_cast<short>(wparam); }
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
  HRGN hrgn() const { return reinterpret_cast<HRGN>(wparam); }
};

struct nextdlgctl_params : public message_params {
  bool has_ctrl_receiving_focus() const { return LOWORD(lparam) != FALSE; }
  HWND ctrl_receiving_focus() const     { return LOWORD(lparam) ? reinterpret_cast<HWND>(wparam) : nullptr; }
  bool focus_next() const               { return wparam == 0; }
};

struct nextmenu_params : public message_params {
  BYTE vkey_code() const           { return static_cast<BYTE>(wparam); }
  MDINEXTMENU& mdinextmenu() const { return *reinterpret_cast<MDINEXTMENU*>(lparam); }
};

struct notifyformat_params : public message_params {
  HWND window_from() const           { return reinterpret_cast<HWND>(wparam); }
  bool is_query_from_control() const { return lparam == NF_QUERY; }
  bool is_requery_to_control() const { return lparam == NF_REQUERY; }
};

using paint_params = message_params;

struct paintclipboard_params : public message_params {
  HWND clipboard_viewer() const          { return reinterpret_cast<HWND>(wparam); }
  const PAINTSTRUCT& paintstruct() const { return *reinterpret_cast<const PAINTSTRUCT*>(lparam); }
};

struct palettechanged_params : public message_params {
  HWND window_origin() const { return reinterpret_cast<HWND>(wparam); }
};

using paletteischanging_params = palettechanged_params;

struct parentnotify_params : public pos_params {
  UINT event_message() const { return static_cast<UINT>(LOWORD(wparam)); }
  WORD child_id() const      { return HIWORD(wparam); }
  HWND child_hwnd() const    { return reinterpret_cast<HWND>(lparam); }
  bool is_xbutton1() const   { return HIWORD(wparam) == XBUTTON1; }
  WORD pointer_flag() const  { return HIWORD(wparam); }
};

struct powerbroadcast_params : public message_params {
  bool is_power_status_change() const           { return wparam == PBT_APMPOWERSTATUSCHANGE; }
  bool is_resuming() const                      { return wparam == PBT_APMRESUMEAUTOMATIC; }
  bool is_suspending() const                    { return wparam == PBT_APMSUSPEND; }
  bool is_power_setting_change() const          { return wparam == PBT_POWERSETTINGCHANGE; }
  POWERBROADCAST_SETTING& power_setting() const { return *reinterpret_cast<POWERBROADCAST_SETTING*>(lparam); }
};

struct print_params : public message_params {
  HDC hdc() const    { return reinterpret_cast<HDC>(wparam); }
  void set_hdc(HDC hdc)            { wparam = reinterpret_cast<WPARAM>(hdc); }
  UINT flags() const { return static_cast<UINT>(lparam); }
  void set_flags(UINT flags)       { lparam = static_cast<LPARAM>(flags); }
};

using printclient_params   = print_params;
using querydragicon_params = message_params;

struct queryendsession_params : public message_params {
  bool is_system_issue() const    { return (lparam & ENDSESSION_CLOSEAPP) != 0; }
  bool is_forced_critical() const { return (lparam & ENDSESSION_CRITICAL) != 0; }
  bool is_logoff() const          { return (lparam & ENDSESSION_LOGOFF) != 0; }
  bool is_shutdown() const        { return lparam == 0; }
};

using querynewpalette_params  = message_params;
using queryopen               = message_params;
using renderallformats_params = message_params;

struct renderformat_params : public message_params {
  WORD clipboard_format() const { return static_cast<WORD>(wparam); }
};

struct setcursor_params : public message_params {
  HWND cursor_owner() const   { return reinterpret_cast<HWND>(wparam); }
  short hit_test_code() const { return static_cast<short>(LOWORD(lparam)); }
  WORD mouse_msg_id() const   { return HIWORD(lparam); }
};

struct setfocus_params : public message_params {
  HWND unfocused_window() const { return reinterpret_cast<HWND>(wparam); }
};

struct setfont_params : public message_params {
  HFONT hfont() const        { return reinterpret_cast<HFONT>(wparam); }
  bool should_redraw() const { return LOWORD(lparam) != FALSE; }
  void set_should_redraw(bool f)           { lparam = MAKELONG(HIWORD(lparam), LOWORD(f ? TRUE : FALSE)); }
};

struct sethotkey_params : public message_params {
  BYTE vkey_code() const    { return static_cast<BYTE>(LOWORD(wparam)); }
  bool has_alt() const      { return (HIWORD(wparam) & 0x04) != 0; }
  bool has_ctrl() const     { return (HIWORD(wparam) & 0x02) != 0; }
  bool has_extended() const { return (HIWORD(wparam) & 0x08) != 0; }
  bool has_shift() const    { return (HIWORD(wparam) & 0x01) != 0; }
};

struct seticon_params : public message_params {
  bool is_small() const   { return wparam == ICON_SMALL; }
  HICON hicon() const     { return reinterpret_cast<HICON>(lparam); }
  bool is_removed() const { return hicon() == nullptr; }
};

struct setredraw_params : public message_params {
  bool can_redraw() const { return wparam != FALSE; }
};

struct settext_params : public message_params {
  const wchar_t* text() const { return reinterpret_cast<const wchar_t*>(lparam); }
};

struct settingchange_params : public message_params {
  const wchar_t* string_id() const { return reinterpret_cast<const wchar_t*>(lparam); }
  bool is_policy() const           { return 0 == lstrcmpW(string_id(), L"Policy"); }
  bool is_locale() const           { return 0 == lstrcmpW(string_id(), L"intl"); }
  bool is_environment_vars() const { return 0 == lstrcmpW(string_id(), L"Environment"); }
};

struct showwindow_params : public message_params {
  bool is_being_shown() const           { return wparam != FALSE; }
  bool is_other_away() const            { return lparam == SW_OTHERUNZOOM; }
  bool is_other_over() const            { return lparam == SW_OTHERZOOM; }
  bool is_owner_being_minimized() const { return lparam == SW_PARENTCLOSING; }
  bool is_owner_being_restored() const  { return lparam == SW_PARENTOPENING; }
};

struct size_params : public message_params {
  bool is_other_maximized() const { return wparam == 4; }
  bool is_maximized() const       { return wparam == 2; }
  bool is_other_restored() const  { return wparam == 3; }
  bool is_minimized() const       { return wparam == 1; }
  bool is_restored() const        { return wparam == 0; }
  SIZE sz() const                 { return {LOWORD(lparam), HIWORD(lparam)}; }
};

struct sizeclipboard_params : public message_params {
  HWND clipboard_viewer() const      { return reinterpret_cast<HWND>(wparam); }
  const RECT& clipboard_rect() const { return *reinterpret_cast<const RECT*>(lparam); }
};

struct sizing_params : public message_params {
  WORD edge() const           { return static_cast<WORD>(wparam); }
  RECT& screen_coords() const { return *reinterpret_cast<RECT*>(lparam); }
};

struct spoolerstatus_params : public message_params {
  UINT status_flag() const    { return static_cast<UINT>(wparam); }
  WORD remaining_jobs() const { return LOWORD(lparam); }
};

struct stylechanged_params : public message_params {
  bool is_style() const                  { return (wparam & GWL_STYLE) != 0; }
  bool is_ex_style() const               { return (wparam & GWL_EXSTYLE) != 0; }
  const STYLESTRUCT& stylestruct() const { return *reinterpret_cast<const STYLESTRUCT*>(lparam); }
};

using stylechanging_params = stylechanged_params;
using syschar_params       = char_params;

struct syscommand_params : public pos_params {
  WORD command_type() const { return static_cast<WORD>(wparam); }
};

using sysdeadchar_params = deadchar_params;
using syskeydown_params  = keydown_params;
using syskeyup_params    = keyup_params;

struct tcard_params : public message_params {
  UINT action_id() const   { return static_cast<UINT>(wparam); }
  long action_data() const { return static_cast<long>(lparam); }
};

using timechange_params = message_params;

struct timer_params : public message_params {
  UINT_PTR timer_id() const  { return static_cast<UINT_PTR>(wparam); }
  TIMERPROC callback() const { return reinterpret_cast<TIMERPROC>(lparam); }
};

struct uninitmenupopup_params : public message_params {
  HMENU hmenu() const  { return reinterpret_cast<HMENU>(wparam); }
  WORD menu_id() const { return HIWORD(lparam); }
};

using userchanged_params = message_params;

struct vkeytoitem_params : public message_params {
  BYTE vkey_code() const         { return static_cast<BYTE>(LOWORD(wparam)); }
  WORD current_caret_pos() const { return HIWORD(wparam); }
  HWND hlistbox() const          { return reinterpret_cast<HWND>(lparam); }
};

struct windowposchanged_params : public message_params {
  WINDOWPOS& windowpos() const { return *reinterpret_cast<WINDOWPOS*>(lparam); }
};

using windowposchanging_params = windowposchanged_params;

struct quit_params : public message_params {
  int exit_code() const { return static_cast<int>(wparam); }
};

struct dpichanged_params : public message_params {
  WORD dpi_x() const { return LOWORD(wparam); }
  WORD dpi_y() const { return HIWORD(wparam); }
  RECT* suggested_rect() const { return reinterpret_cast<RECT*>(lparam); }
};

using dpichangedbeforeparent_params = message_params;
using dpichangedafterparent_params = message_params;

}
