/*
 * viking -- GPS Data and Topo Analyzer, Explorer, and Manager
 *
 * Copyright (C) 2007-2009, Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
 * Based on:
 * Copyright (C) 2003-2007, Leandro A. F. Pereira <leandro@linuxmag.com.br>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _VIKING_UTIL_H
#define _VIKING_UTIL_H

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

void open_url(GtkWindow *parent, const gchar * url);
void new_email(GtkWindow *parent, const gchar * address);

gchar *uri_escape(gchar *str);

GList * str_array_to_glist(gchar* data[]);

gboolean split_string_from_file_on_equals ( const gchar *buf, gchar **key, gchar **val );

G_END_DECLS

#endif

