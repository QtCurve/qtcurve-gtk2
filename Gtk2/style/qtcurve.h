#ifndef __QTCURVE_H__
#define __QTCURVE_H__

/*
  QtCurve (C) Craig Drummond, 2003 - 2007 Craig.Drummond@lycos.co.uk

  ----

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
 */

#include <gtk/gtk.h>
#include "common.h"

typedef struct _QtCurveStyleClass QtCurveStyleClass;

extern GType qtcurve_type_style;

#define QTCURVE_TYPE_STYLE              qtcurve_type_style
/*#define QTCURVE_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), QTCURVE_TYPE_STYLE, QtCurveStyle))*/
#define QTCURVE_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), QTCURVE_TYPE_STYLE, QtCurveStyleClass))
#define QTCURVE_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), QTCURVE_TYPE_STYLE))
#define QTCURVE_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), QTCURVE_TYPE_STYLE))
#define QTCURVE_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), QTCURVE_TYPE_STYLE, QtCurveStyleClass))

typedef struct _QtCurveRcStyle QtCurveRcStyle;
typedef struct _QtCurveRcStyleClass QtCurveRcStyleClass;

extern GType qtcurve_type_rc_style;

#define QTCURVE_TYPE_RC_STYLE              qtcurve_type_rc_style
#define QTCURVE_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), QTCURVE_TYPE_RC_STYLE, QtCurveRcStyle))
#define QTCURVE_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), QTCURVE_TYPE_RC_STYLE, QtCurveRcStyleClass))
#define QTCURVE_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), QTCURVE_TYPE_RC_STYLE))
#define QTCURVE_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), QTCURVE_TYPE_RC_STYLE))
#define QTCURVE_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), QTCURVE_TYPE_RC_STYLE, QtCurveRcStyleClass))

struct _QtCurveRcStyle
{
    GtkRcStyle parent_instance;
};

struct _QtCurveRcStyleClass
{
    GtkRcStyleClass parent_class;
};

void qtcurve_rc_style_register_type (GTypeModule *module);

typedef struct 
{
    GtkStyle parent_instance;

    GdkColor background[TOTAL_SHADES+1],
             button[TOTAL_SHADES+1],
             slider[TOTAL_SHADES+1],
             defbtn[TOTAL_SHADES+1],
             mouseover[TOTAL_SHADES+1],
             menubar[TOTAL_SHADES+1],
             menuitem[TOTAL_SHADES+1],
             *check_radio;
    GdkGC    *background_gc[TOTAL_SHADES+1],
             *button_gc[TOTAL_SHADES+1],
             *slider_gc[TOTAL_SHADES+1],
             *defbtn_gc[TOTAL_SHADES+1],
             *mouseover_gc[TOTAL_SHADES+1],
             *menubar_gc[TOTAL_SHADES+1],
             *menuitem_gc[TOTAL_SHADES+1],
             *button_text_gc,
             *menutext_gc[2],
             *listview_alternate_gc,
             *temp_gc[2];
} QtCurveStyle;

struct _QtCurveStyleClass
{
    GtkStyleClass parent_class;
};

#endif
