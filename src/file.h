/*
 * viking -- GPS Data and Topo Analyzer, Explorer, and Manager
 *
 * Copyright (C) 2003-2005, Evan Battaglia <gtoevan@gmx.net>
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

#ifndef _VIKING_FILE_H
#define _VIKING_FILE_H

#define FILE_TYPE_GPSPOINT 1
#define FILE_TYPE_GPSMAPPER 2

const gchar *a_file_basename ( const gchar *filename );

/* 0 on failure, 1 on success (vik file) 2 on success (other file) */
gshort a_file_load ( VikAggregateLayer *top, gpointer vp, const gchar *filename );
gboolean a_file_save ( VikAggregateLayer *top, gpointer vp, const gchar *filename );
gboolean a_file_export ( VikTrwLayer *vtl, const gchar *filename, gshort file_type );

#endif