// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <windows.h>
#include <commctrl.h>

namespace wndkit::details {

template<UINT Code>
struct notify_traits {
  using param_type = NMHDR; // Default to plain NMHDR
};

template<>
struct notify_traits<NM_CUSTOMDRAW> {
  using param_type = NMCUSTOMDRAW;
};

template<>
struct notify_traits<BCN_DROPDOWN> {
  using param_type = NMBCDROPDOWN;
};

template<>
struct notify_traits<BCN_HOTITEMCHANGE> {
  using param_type = NMBCHOTITEM;
};

template<>
struct notify_traits<CBEN_DELETEITEM> {
  using param_type = NMCOMBOBOXEXW;
};

template<>
struct notify_traits<CBEN_DRAGBEGIN> {
  using param_type = NMCBEDRAGBEGINW;
};

template<>
struct notify_traits<CBEN_ENDEDIT> {
  using param_type = NMCBEENDEDITW;
};

template<>
struct notify_traits<NM_SETCURSOR> {
  using param_type = NMMOUSE;
};

template<>
struct notify_traits<DTN_DATETIMECHANGE> {
  using param_type = NMDATETIMECHANGE;
};

template<>
struct notify_traits<DTN_FORMAT> {
  using param_type = NMDATETIMEFORMATW;
};

template<>
struct notify_traits<DTN_FORMATQUERY> {
  using param_type = NMDATETIMEFORMATQUERYW;
};

template<>
struct notify_traits<DTN_USERSTRING> {
  using param_type = NMDATETIMESTRINGW;
};

template<>
struct notify_traits<DTN_WMKEYDOWN> {
  using param_type = NMDATETIMEWMKEYDOWNW;
};

template<>
struct notify_traits<HDN_BEGINDRAG> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_BEGINFILTEREDIT> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_BEGINTRACK> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_DIVIDERDBLCLICK> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_DROPDOWN> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_ENDDRAG> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_ENDFILTEREDIT> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_ENDTRACK> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_FILTERBTNCLICK> {
  using param_type = NMHDFILTERBTNCLICK;
};

template<>
struct notify_traits<HDN_FILTERCHANGE> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_GETDISPINFO> {
  using param_type = NMHDDISPINFOW;
};

template<>
struct notify_traits<HDN_ITEMCHANGED> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_ITEMCHANGING> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_ITEMCLICK> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_ITEMDBLCLICK> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_ITEMKEYDOWN> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_ITEMSTATEICONCLICK> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_OVERFLOWCLICK> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<HDN_TRACK> {
  using param_type = NMHEADERW;
};

template<>
struct notify_traits<IPN_FIELDCHANGED> {
  using param_type = NMIPADDRESS;
};

template<>
struct notify_traits<DL_BEGINDRAG> {
  using param_type = DRAGLISTINFO;
};

template<>
struct notify_traits<DL_CANCELDRAG> {
  using param_type = DRAGLISTINFO;
};

template<>
struct notify_traits<DL_DRAGGING> {
  using param_type = DRAGLISTINFO;
};

template<>
struct notify_traits<DL_DROPPED> {
  using param_type = DRAGLISTINFO;
};

template<>
struct notify_traits<LVN_BEGINDRAG> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_BEGINLABELEDIT> {
  using param_type = NMLVDISPINFOW;
};

template<>
struct notify_traits<LVN_BEGINRDRAG> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_BEGINSCROLL> {
  using param_type = NMLVSCROLL;
};

template<>
struct notify_traits<LVN_COLUMNCLICK> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_COLUMNDROPDOWN> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_COLUMNOVERFLOWCLICK> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_DELETEALLITEMS> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_DELETEITEM> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_ENDLABELEDIT> {
  using param_type = NMLVDISPINFO;
};

template<>
struct notify_traits<LVN_ENDSCROLL> {
  using param_type = NMLVSCROLL;
};

template<>
struct notify_traits<LVN_GETDISPINFO> {
  using param_type = NMLVDISPINFOW;
};

template<>
struct notify_traits<LVN_GETEMPTYMARKUP> {
  using param_type = NMLVEMPTYMARKUP;
};

template<>
struct notify_traits<LVN_GETINFOTIP> {
  using param_type = NMLVGETINFOTIPW;
};

template<>
struct notify_traits<LVN_HOTTRACK> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_INCREMENTALSEARCH> {
  using param_type = NMLVFINDITEMW;
};

template<>
struct notify_traits<LVN_INSERTITEM> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_ITEMACTIVATE> {
#if (_WIN32_IE >= 0x0400)
  using param_type = NMITEMACTIVATE;
#else
  using param_type = NMHDR;
#endif
};

template<>
struct notify_traits<LVN_ITEMCHANGED> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_ITEMCHANGING> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_KEYDOWN> {
  using param_type = NMLVKEYDOWN;
};

template<>
struct notify_traits<LVN_LINKCLICK> {
  using param_type = NMLVLINK;
};

template<>
struct notify_traits<LVN_MARQUEEBEGIN> {
  using param_type = NMLISTVIEW;
};

template<>
struct notify_traits<LVN_ODCACHEHINT> {
  using param_type = NMLVCACHEHINT;
};

template<>
struct notify_traits<LVN_ODFINDITEM> {
  using param_type = NMLVFINDITEMW;
};

template<>
struct notify_traits<LVN_ODSTATECHANGED> {
  using param_type = NMLVODSTATECHANGE;
};

template<>
struct notify_traits<LVN_SETDISPINFO> {
  using param_type = NMLVDISPINFOW;
};

template<>
struct notify_traits<NM_CLICK> {
  using param_type = NMITEMACTIVATE;
};

template<>
struct notify_traits<NM_DBLCLK> {
  using param_type = NMITEMACTIVATE;
};

template<>
struct notify_traits<NM_RCLICK> {
  using param_type = NMITEMACTIVATE;
};

template<>
struct notify_traits<NM_RDBLCLK> {
  using param_type = NMITEMACTIVATE;
};

template<>
struct notify_traits<MCN_GETDAYSTATE> {
  using param_type = NMDAYSTATE;
};

template<>
struct notify_traits<MCN_SELCHANGE> {
  using param_type = NMSELCHANGE;
};

template<>
struct notify_traits<MCN_SELECT> {
  using param_type = NMSELCHANGE;
};

template<>
struct notify_traits<MCN_VIEWCHANGE> {
  using param_type = NMVIEWCHANGE;
};

template<>
struct notify_traits<PGN_CALCSIZE> {
  using param_type = NMPGCALCSIZE;
};

template<>
struct notify_traits<PGN_HOTITEMCHANGE> {
  using param_type = NMPGHOTITEM;
};

template<>
struct notify_traits<PGN_SCROLL> {
  using param_type = NMPGSCROLL;
};

template<>
struct notify_traits<PSN_APPLY> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_GETOBJECT> {
  using param_type = NMOBJECTNOTIFY;
};

template<>
struct notify_traits<PSN_HELP> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_KILLACTIVE> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_QUERYCANCEL> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_QUERYINITIALFOCUS> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_RESET> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_SETACTIVE> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_TRANSLATEACCELERATOR> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_WIZBACK> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_WIZFINISH> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<PSN_WIZNEXT> {
  using param_type = PSHNOTIFY;
};

template<>
struct notify_traits<TVN_SELCHANGED> {
  using param_type = NMTREEVIEWW;
};

}
