/*
  QtCurve (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com

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

/*
    Stolen from Clearlooks...
 */

/* Yes, this is evil code. But many people seem to like hazardous things, so
 * it exists. Most of this was written by Kulyk Nazar.
 *
 * heavily modified by Benjamin Berg <benjamin@sipsolutions.net>.
 */

/* #define CHECK_ANIMATION_TIME 0.5 */

#include "animation.h"
#include <common/common.h>
#include "compatability.h"

#if GTK_CHECK_VERSION(2, 12, 0)
typedef struct {
    GTimer *timer;

    gdouble start_modifier;
    gdouble stop_time;
    GtkWidget *widget;
} AnimationInfo;

typedef struct {
    GtkWidget *widget;
    gulong handler_id;
} SignalInfo;

static GSList *connected_widgets = NULL;
static GHashTable *animated_widgets = NULL;
static int animation_timer_id = 0;

static gboolean qtcAnimationTimeoutHandler(gpointer data);

/* This forces a redraw on a widget */
static void
force_widget_redraw(GtkWidget *widget)
{
    if (GTK_IS_PROGRESS_BAR(widget)) {
        gtk_widget_queue_resize(widget);
    } else {
        gtk_widget_queue_draw(widget);
    }
}

/* ensures that the timer is running */
static void
qtcAnimationStartTimer()
{
    if(animation_timer_id == 0) {
        animation_timer_id = g_timeout_add(PROGRESS_ANIMATION,
                                           qtcAnimationTimeoutHandler, NULL);
    }
}

/* ensures that the timer is stopped */
static void
qtcAnimationStopTimer()
{
    if (animation_timer_id != 0) {
        g_source_remove(animation_timer_id);
        animation_timer_id = 0;
    }
}

/* destroys an AnimationInfo structure including the GTimer */
static void
qtcAnimationDestroyInfo(AnimationInfo *animation_info)
{
    g_timer_destroy(animation_info->timer);
    g_free(animation_info);
}

/* This function does not unref the weak reference, because the object
 * is beeing destroyed currently. */
static void
qtcAnimationOnWidgetDestruction(gpointer data, GObject *object)
{
    /* steal the animation info from the hash table(destroying it would
     * result in the weak reference to be unrefed, which does not work
     * as the widget is already destroyed. */
    g_hash_table_steal(animated_widgets, object);
    qtcAnimationDestroyInfo((AnimationInfo*)data);
}

/* This function also needs to unref the weak reference. */
static void
qtcAnimationDestroyInfoAndWeakUnref(gpointer data)
{
    AnimationInfo *animation_info = data;

    /* force a last redraw. This is so that if the animation is removed,
     * the widget is left in a sane state. */
    force_widget_redraw(animation_info->widget);

    g_object_weak_unref(G_OBJECT(animation_info->widget),
                        qtcAnimationOnWidgetDestruction, data);
    qtcAnimationDestroyInfo(animation_info);
}

/* Find and return a pointer to the data linked to this widget, if it exists */
static AnimationInfo*
qtcAnimationLookupInfo(const GtkWidget *widget)
{
    if(animated_widgets)
        return g_hash_table_lookup(animated_widgets, widget);
    return NULL;
}

/* Create all the relevant information for the animation,
 * and insert it into the hash table. */
static void
qtcAnimationAdd(const GtkWidget *widget, gdouble stop_time)
{
    AnimationInfo *value;

    /* object already in the list, do not add it twice */
    if(qtcAnimationLookupInfo(widget))
        return;

    if(animated_widgets == NULL)
        animated_widgets =
            g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                  NULL, qtcAnimationDestroyInfoAndWeakUnref);

    value = g_new(AnimationInfo, 1);

    value->widget =(GtkWidget*) widget;
    value->timer = g_timer_new();
    value->stop_time= stop_time;
    value->start_modifier = 0.0;

    g_object_weak_ref(G_OBJECT(widget), qtcAnimationOnWidgetDestruction, value);
    g_hash_table_insert(animated_widgets,(GtkWidget*) widget, value);

    qtcAnimationStartTimer();
}

/* update the animation information for each widget. This will also queue a redraw
 * and stop the animation if it is done. */
static gboolean
qtcAnimationUpdateInfo(gpointer key, gpointer value, gpointer user_data)
{
    QTC_UNUSED(user_data);
    AnimationInfo *animation_info = value;
    GtkWidget *widget = key;

    if ((widget == NULL) || (animation_info == NULL))
        g_assert_not_reached();

    /* remove the widget from the hash table if it is not drawable */
    if (!qtcWidgetDrawable(widget))
        return TRUE;

    if (GTK_IS_PROGRESS_BAR(widget)) {
        gfloat fraction =
            gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(widget));
        /* stop animation for filled/not filled progress bars */
        if (fraction <= 0.0 || fraction >= 1.0) {
            return TRUE;
        }
    } else if (GTK_IS_ENTRY(widget)) {
        gfloat fraction = gtk_entry_get_progress_fraction(GTK_ENTRY(widget));

        /* stop animation for filled/not filled progress bars */
        if(fraction <= 0.0 || fraction >= 1.0) {
            return TRUE;
        }
    }

    force_widget_redraw(widget);

    /* stop at stop_time */
    if(animation_info->stop_time != 0 &&
       g_timer_elapsed(animation_info->timer, NULL) > animation_info->stop_time)
        return TRUE;
    return FALSE;
}

/* This gets called by the glib main loop every once in a while. */
static gboolean
qtcAnimationTimeoutHandler(gpointer data)
{
    QTC_UNUSED(data);
    /*g_print("** TICK **\n");*/

    /* enter threads as qtcAnimationUpdateInfo will use gtk/gdk. */
    gdk_threads_enter();
    g_hash_table_foreach_remove(animated_widgets, qtcAnimationUpdateInfo, NULL);
    /* leave threads again */
    gdk_threads_leave();

    if (g_hash_table_size(animated_widgets) == 0) {
        qtcAnimationStopTimer();
        return FALSE;
    }
    return TRUE;
}

/*
static void
on_checkbox_toggle(GtkWidget *widget, gpointer data)
{
    AnimationInfo *animation_info = qtcAnimationLookupInfo(widget);

    if(animation_info != NULL)
    {
        gfloat elapsed = g_timer_elapsed(animation_info->timer, NULL);

        animation_info->start_modifier = elapsed - animation_info->start_modifier;
    }
    else
    {
        qtcAnimationAdd(widget, CHECK_ANIMATION_TIME);
    }
}
*/

static void
qtcAnimationOnConnectedWidgetDestruction(gpointer data, GObject *widget)
{
    QTC_UNUSED(widget);
    connected_widgets = g_slist_remove(connected_widgets, data);
    g_free(data);
}

static void
qtcAnimationDisconnect()
{
    GSList *item = connected_widgets;
    while (item != NULL) {
        SignalInfo *signal_info =(SignalInfo*) item->data;

        g_signal_handler_disconnect(signal_info->widget,
                                    signal_info->handler_id);
        g_object_weak_unref(G_OBJECT(signal_info->widget),
                            qtcAnimationOnConnectedWidgetDestruction,
                            signal_info);
        g_free(signal_info);

        item = g_slist_next(item);
    }

    g_slist_free(connected_widgets);
    connected_widgets = NULL;
}


#endif
/* external interface */

/* adds a progress bar */
void
qtcAnimationAddProgressBar(GtkWidget *progressbar, gboolean isEntry)
{
#if GTK_CHECK_VERSION(2, 12, 0)
    gdouble fraction = isEntry ?
        gtk_entry_get_progress_fraction(GTK_ENTRY(progressbar)) :
        gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progressbar));

    if (fraction < 1.0 && fraction > 0.0) {
        qtcAnimationAdd((GtkWidget*) progressbar, 0.0);
    }
#else
    QTC_UNUSED(progressbar);
    QTC_UNUSED(isEntry);
#endif
}

/* helper function for qtcAnimationConnectCheckbox */
/* static gint */
/* qtcAnimationFindSignalInfo(gconstpointer signal_info, gconstpointer widget) */
/* { */
/*     if (((SignalInfo*)signal_info)->widget == widget) { */
/*         return 0; */
/*     } else { */
/*         return 1; */
/*     } */
/* } */
/* hooks up the signals for check and radio buttons */
/*
static void qtcAnimationConnectCheckbox(GtkWidget *widget)
{
    if(GTK_IS_CHECK_BUTTON(widget))
    {
        if(!g_slist_find_custom(connected_widgets, widget, qtcAnimationFindSignalInfo))
        {
            SignalInfo * signal_info = g_new(SignalInfo, 1);

            signal_info->widget = widget;
            signal_info->handler_id = g_signal_connect((GObject*)widget, "toggled", G_CALLBACK(on_checkbox_toggle), NULL);

            connected_widgets = g_slist_append(connected_widgets, signal_info);
            g_object_weak_ref(G_OBJECT(widget), qtcAnimationOnConnectedWidgetDestruction, signal_info);
        }
    }
}
*/

/* returns TRUE if the widget is animated, and FALSE otherwise */
/* static gboolean */
/* qtcAnimationIsAnimated(GtkWidget *widget) */
/* { */
/* #if GTK_CHECK_VERSION(2, 12, 0) */
/*     return qtcAnimationLookupInfo(widget) != NULL ? TRUE : FALSE; */
/* #else */
/*     return FALSE; */
/* #endif */
/* } */

/* returns the elapsed time for the animation */
gdouble
qtcAnimationElapsed(gpointer data)
{
#if GTK_CHECK_VERSION(2, 12, 0)
    AnimationInfo *animation_info = qtcAnimationLookupInfo(data);

    if(animation_info)
        return (g_timer_elapsed(animation_info->timer, NULL) -
                animation_info->start_modifier);
#endif
    return 0.0;
}

/* cleans up all resources of the animation system */
void
qtcAnimationCleanup()
{
#if GTK_CHECK_VERSION(2, 12, 0)
    qtcAnimationDisconnect();

    if (animated_widgets != NULL) {
        g_hash_table_destroy(animated_widgets);
        animated_widgets = NULL;
    }
    qtcAnimationStopTimer();
#endif
}
