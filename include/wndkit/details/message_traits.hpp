// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <windows.h>
#include <type_traits>
#include "wndkit/message_params.hpp"

namespace wndkit::details {

template<typename T>
concept message_params_compatible =
  std::is_base_of_v<message_params, T> &&
  std::is_standard_layout_v<T> &&
  sizeof(T) == sizeof(message_params);

template<UINT Msg>
struct message_traits {
  using param_type = message_params;
};

template<>
struct message_traits<WM_COMMAND> {
  using param_type = command_params;
};

template<>
struct message_traits<WM_NOTIFY> {
  using param_type = notify_params;
};

template<>
struct message_traits<WM_ACTIVATE> {
  using param_type = activate_params;
};

template<>
struct message_traits<WM_ACTIVATEAPP> {
  using param_type = activateapp_params;
};

template<>
struct message_traits<WM_ASKCBFORMATNAME> {
  using param_type = askcbformatname_params;
};

template<>
struct message_traits<WM_CAPTURECHANGED> {
  using param_type = capturechanged_params;
};

template<>
struct message_traits<WM_CHANGECBCHAIN> {
  using param_type = changecbchain_params;
};

template<>
struct message_traits<WM_CHAR> {
  using param_type = char_params;
};

template<>
struct message_traits<WM_CHARTOITEM> {
  using param_type = chartoitem_params;
};

template<>
struct message_traits<WM_COMPACTING> {
  using param_type = compacting_params;
};

template<>
struct message_traits<WM_COMPAREITEM> {
  using param_type = compareitem_params;
};

template<>
struct message_traits<WM_CONTEXTMENU> {
  using param_type = contextmenu_params;
};

template<>
struct message_traits<WM_COPYDATA> {
  using param_type = copydata_params;
};

template<>
struct message_traits<WM_CREATE> {
  using param_type = create_params;
};

template<>
struct message_traits<WM_CTLCOLORBTN> {
  using param_type = ctlcolorbtn_params;
};

template<>
struct message_traits<WM_CTLCOLORDLG> {
  using param_type = ctlcolordlg_params;
};

template<>
struct message_traits<WM_CTLCOLOREDIT> {
  using param_type = ctlcoloredit_params;
};

template<>
struct message_traits<WM_CTLCOLORLISTBOX> {
  using param_type = ctlcolorlistbox_params;
};

template<>
struct message_traits<WM_CTLCOLORSCROLLBAR> {
  using param_type = ctlcolorscrollbar_params;
};

template<>
struct message_traits<WM_CTLCOLORSTATIC> {
  using param_type = ctlcolorstatic_params;
};

template<>
struct message_traits<WM_DEADCHAR> {
  using param_type = deadchar_params;
};

template<>
struct message_traits<WM_DELETEITEM> {
  using param_type = deleteitem_params;
};

template<>
struct message_traits<WM_DEVMODECHANGE> {
  using param_type = devmodechange_params;
};

template<>
struct message_traits<WM_DISPLAYCHANGE> {
  using param_type = displaychange_params;
};

template<>
struct message_traits<WM_DRAWITEM> {
  using param_type = drawitem_params;
};

template<>
struct message_traits<WM_DROPFILES> {
  using param_type = dropfiles_params;
};

template<>
struct message_traits<WM_ENABLE> {
  using param_type = enable_params;
};

template<>
struct message_traits<WM_ENDSESSION> {
  using param_type = endsession_params;
};

template<>
struct message_traits<WM_ENTERIDLE> {
  using param_type = enteridle_params;
};

template<>
struct message_traits<WM_ENTERMENULOOP> {
  using param_type = entermenuloop_params;
};

template<>
struct message_traits<WM_ERASEBKGND> {
  using param_type = erasebkgnd_params;
};

template<>
struct message_traits<WM_EXITMENULOOP> {
  using param_type = exitmenuloop_params;
};

template<>
struct message_traits<WM_GETDLGCODE> {
  using param_type = getdlgcode_params;
};

template<>
struct message_traits<WM_GETICON> {
  using param_type = geticon_params;
};

template<>
struct message_traits<WM_GETMINMAXINFO> {
  using param_type = getminmaxinfo_params;
};

template<>
struct message_traits<WM_GETTEXT> {
  using param_type = gettext_params;
};

template<>
struct message_traits<WM_HELP> {
  using param_type = help_params;
};

template<>
struct message_traits<WM_HOTKEY> {
  using param_type = hotkey_params;
};

template<>
struct message_traits<WM_HSCROLL> {
  using param_type = hscroll_params;
};

template<>
struct message_traits<WM_VSCROLL> {
  using param_type = vscroll_params;
};

template<>
struct message_traits<WM_HSCROLLCLIPBOARD> {
  using param_type = hscrollclipboard_params;
};

template<>
struct message_traits<WM_VSCROLLCLIPBOARD> {
  using param_type = vscrollclipboard_params;
};

template<>
struct message_traits<WM_ICONERASEBKGND> {
  using param_type = iconerasebkgnd_params;
};

template<>
struct message_traits<WM_INITDIALOG> {
  using param_type = initdialog_params;
};

template<>
struct message_traits<WM_INITMENU> {
  using param_type = initmenu_params;
};

template<>
struct message_traits<WM_INITMENUPOPUP> {
  using param_type = initmenupopup_params;
};

template<>
struct message_traits<WM_INPUTLANGCHANGE> {
  using param_type = inputlangchange_params;
};

template<>
struct message_traits<WM_INPUTLANGCHANGEREQUEST> {
  using param_type = inputlangchangerequest_params;
};

template<>
struct message_traits<WM_KEYDOWN> {
  using param_type = keydown_params;
};

template<>
struct message_traits<WM_KEYUP> {
  using param_type = keyup_params;
};

template<>
struct message_traits<WM_KILLFOCUS> {
  using param_type = killfocus_params;
};

template<>
struct message_traits<WM_LBUTTONDBLCLK> {
  using param_type = lbuttondblclk_params;
};

template<>
struct message_traits<WM_LBUTTONDOWN> {
  using param_type = lbuttondown_params;
};

template<>
struct message_traits<WM_LBUTTONUP> {
  using param_type = lbuttonup_params;
};

template<>
struct message_traits<WM_MBUTTONDBLCLK> {
  using param_type = mbuttondblclk_params;
};

template<>
struct message_traits<WM_MBUTTONDOWN> {
  using param_type = mbuttondown_params;
};

template<>
struct message_traits<WM_MBUTTONUP> {
  using param_type = mbuttonup_params;
};

template<>
struct message_traits<WM_MOUSEHOVER> {
  using param_type = mousehover_params;
};

template<>
struct message_traits<WM_MOUSEMOVE> {
  using param_type = mousemove_params;
};

template<>
struct message_traits<WM_RBUTTONDBLCLK> {
  using param_type = rbuttondblclk_params;
};

template<>
struct message_traits<WM_RBUTTONDOWN> {
  using param_type = rbuttondown_params;
};

template<>
struct message_traits<WM_RBUTTONUP> {
  using param_type = rbuttonup_params;
};

template<>
struct message_traits<WM_MDIACTIVATE> {
  using param_type = mdiactivate_params;
};

template<>
struct message_traits<WM_MEASUREITEM> {
  using param_type = measureitem_params;
};

template<>
struct message_traits<WM_MENUCHAR> {
  using param_type = menuchar_params;
};

template<>
struct message_traits<WM_MENUDRAG> {
  using param_type = menudrag_params;
};

template<>
struct message_traits<WM_MENUGETOBJECT> {
  using param_type = menugetobject_params;
};

template<>
struct message_traits<WM_MENURBUTTONUP> {
  using param_type = menurbuttonup_params;
};

template<>
struct message_traits<WM_MENUSELECT> {
  using param_type = menuselect_params;
};

template<>
struct message_traits<WM_MOUSEACTIVATE> {
  using param_type = mouseactivate_params;
};

template<>
struct message_traits<WM_MOUSEWHEEL> {
  using param_type = mousewheel_params;
};

template<>
struct message_traits<WM_MOVE> {
  using param_type = move_params;
};

template<>
struct message_traits<WM_MOVING> {
  using param_type = moving_params;
};

template<>
struct message_traits<WM_NCACTIVATE> {
  using param_type = ncactivate_params;
};

template<>
struct message_traits<WM_NCCALCSIZE> {
  using param_type = nccalcsize_params;
};

template<>
struct message_traits<WM_NCCREATE> {
  using param_type = nccreate_params;
};

template<>
struct message_traits<WM_NCHITTEST> {
  using param_type = nchittest_params;
};

template<>
struct message_traits<WM_NCLBUTTONDBLCLK> {
  using param_type = nclbuttondblclk_params;
};

template<>
struct message_traits<WM_NCLBUTTONDOWN> {
  using param_type = nclbuttondown_params;
};

template<>
struct message_traits<WM_NCLBUTTONUP> {
  using param_type = nclbuttonup_params;
};

template<>
struct message_traits<WM_NCMBUTTONDBLCLK> {
  using param_type = ncmbuttondblclk_params;
};

template<>
struct message_traits<WM_NCMBUTTONDOWN> {
  using param_type = ncmbuttondown_params;
};

template<>
struct message_traits<WM_NCMBUTTONUP> {
  using param_type = ncmbuttonup_params;
};

template<>
struct message_traits<WM_NCMOUSEMOVE> {
  using param_type = ncmousemove_params;
};

template<>
struct message_traits<WM_NCRBUTTONDBLCLK> {
  using param_type = ncrbuttondblclk_params;
};

template<>
struct message_traits<WM_NCRBUTTONDOWN> {
  using param_type = ncrbuttondown_params;
};

template<>
struct message_traits<WM_NCRBUTTONUP> {
  using param_type = ncrbuttonup_params;
};

template<>
struct message_traits<WM_PAINT> {
  using param_type = ncpaint_params;
};

template<>
struct message_traits<WM_NEXTDLGCTL> {
  using param_type = nextdlgctl_params;
};

template<>
struct message_traits<WM_NEXTMENU> {
  using param_type = nextmenu_params;
};

template<>
struct message_traits<WM_NOTIFYFORMAT> {
  using param_type = notifyformat_params;
};

template<>
struct message_traits<WM_PAINTCLIPBOARD> {
  using param_type = paintclipboard_params;
};

template<>
struct message_traits<WM_PALETTECHANGED> {
  using param_type = palettechanged_params;
};

template<>
struct message_traits<WM_PALETTEISCHANGING> {
  using param_type = paletteischanging_params;
};

template<>
struct message_traits<WM_PARENTNOTIFY> {
  using param_type = parentnotify_params;
};

template<>
struct message_traits<WM_POWERBROADCAST> {
  using param_type = powerbroadcast_params;
};

template<>
struct message_traits<WM_PRINT> {
  using param_type = print_params;
};

template<>
struct message_traits<WM_PRINTCLIENT> {
  using param_type = printclient_params;
};

template<>
struct message_traits<WM_QUERYENDSESSION> {
  using param_type = queryendsession_params;
};

template<>
struct message_traits<WM_RENDERFORMAT> {
  using param_type = renderformat_params;
};

template<>
struct message_traits<WM_SETCURSOR> {
  using param_type = setcursor_params;
};

template<>
struct message_traits<WM_SETFOCUS> {
  using param_type = setfocus_params;
};

template<>
struct message_traits<WM_SETFONT> {
  using param_type = setfont_params;
};

template<>
struct message_traits<WM_SETHOTKEY> {
  using param_type = sethotkey_params;
};

template<>
struct message_traits<WM_SETICON> {
  using param_type = seticon_params;
};

template<>
struct message_traits<WM_SETREDRAW> {
  using param_type = setredraw_params;
};

template<>
struct message_traits<WM_SETTEXT> {
  using param_type = settext_params;
};

template<>
struct message_traits<WM_SETTINGCHANGE> {
  using param_type = settingchange_params;
};

template<>
struct message_traits<WM_SHOWWINDOW> {
  using param_type = showwindow_params;
};

template<>
struct message_traits<WM_SIZE> {
  using param_type = size_params;
};

template<>
struct message_traits<WM_SIZECLIPBOARD> {
  using param_type = sizeclipboard_params;
};

template<>
struct message_traits<WM_SIZING> {
  using param_type = sizing_params;
};

template<>
struct message_traits<WM_SPOOLERSTATUS> {
  using param_type = spoolerstatus_params;
};

template<>
struct message_traits<WM_STYLECHANGED> {
  using param_type = stylechanged_params;
};

template<>
struct message_traits<WM_STYLECHANGING> {
  using param_type = stylechanging_params;
};

template<>
struct message_traits<WM_SYSCHAR> {
  using param_type = syschar_params;
};

template<>
struct message_traits<WM_SYSCOMMAND> {
  using param_type = syscommand_params;
};

template<>
struct message_traits<WM_SYSDEADCHAR> {
  using param_type = sysdeadchar_params;
};

template<>
struct message_traits<WM_SYSKEYDOWN> {
  using param_type = syskeydown_params;
};

template<>
struct message_traits<WM_SYSKEYUP> {
  using param_type = syskeyup_params;
};

template<>
struct message_traits<WM_TCARD> {
  using param_type = tcard_params;
};

template<>
struct message_traits<WM_TIMER> {
  using param_type = timer_params;
};

template<>
struct message_traits<WM_UNINITMENUPOPUP> {
  using param_type = uninitmenupopup_params;
};

template<>
struct message_traits<WM_VKEYTOITEM> {
  using param_type = vkeytoitem_params;
};

template<>
struct message_traits<WM_WINDOWPOSCHANGED> {
  using param_type = windowposchanged_params;
};

template<>
struct message_traits<WM_WINDOWPOSCHANGING> {
  using param_type = windowposchanging_params;
};

template<>
struct message_traits<WM_DPICHANGED> {
  using param_type = dpichanged_params;
};

}
