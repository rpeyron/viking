/*
 * viking -- GPS Data and Topo Analyzer, Explorer, and Manager
 *
 * Copyright (C) 2003-2005, Evan Battaglia <gtoevan@gmx.net>
 * Copyright (C) 2008, Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
 * Copyright (C) 2010, Rob Norris <rw_norris@hotmail.com>
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib/gi18n.h>

#include "globals.h"
#include "preferences.h"

gboolean vik_debug = FALSE;
gboolean vik_verbose = FALSE;
gboolean vik_version = FALSE;

static gchar * params_degree_formats[] = {"DDD", "DMM", "DMS", NULL};
static gchar * params_units_distance[] = {"Kilometres", "Miles", NULL};
static gchar * params_units_speed[] = {"km/h", "mph", "m/s", "knots", NULL};
static gchar * params_units_height[] = {"Metres", "Feet", NULL};
static VikLayerParamScale params_scales_lat[] = { {-90.0, 90.0, 0.05, 2} };
static VikLayerParamScale params_scales_long[] = { {-180.0, 180.0, 0.05, 2} };
 
static VikLayerParam prefs1[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_NAMESPACE "degree_format", VIK_LAYER_PARAM_UINT, VIK_LAYER_GROUP_NONE, N_("Degree format:"), VIK_LAYER_WIDGET_COMBOBOX, params_degree_formats, NULL, NULL },
};

static VikLayerParam prefs2[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_NAMESPACE "units_distance", VIK_LAYER_PARAM_UINT, VIK_LAYER_GROUP_NONE, N_("Distance units:"), VIK_LAYER_WIDGET_COMBOBOX, params_units_distance, NULL, NULL },
};

static VikLayerParam prefs3[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_NAMESPACE "units_speed", VIK_LAYER_PARAM_UINT, VIK_LAYER_GROUP_NONE, N_("Speed units:"), VIK_LAYER_WIDGET_COMBOBOX, params_units_speed, NULL, NULL },
};

static VikLayerParam prefs4[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_NAMESPACE "units_height", VIK_LAYER_PARAM_UINT, VIK_LAYER_GROUP_NONE, N_("Height units:"), VIK_LAYER_WIDGET_COMBOBOX, params_units_height, NULL, NULL },
};

static VikLayerParam prefs5[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_NAMESPACE "use_large_waypoint_icons", VIK_LAYER_PARAM_BOOLEAN, VIK_LAYER_GROUP_NONE, N_("Use large waypoint icons:"), VIK_LAYER_WIDGET_CHECKBUTTON, NULL, NULL, NULL },
};

static VikLayerParam prefs6[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_NAMESPACE "default_latitude", VIK_LAYER_PARAM_DOUBLE, VIK_LAYER_GROUP_NONE, N_("Default latitude:"),  VIK_LAYER_WIDGET_SPINBUTTON, params_scales_lat, NULL, NULL },
};
static VikLayerParam prefs7[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_NAMESPACE "default_longitude", VIK_LAYER_PARAM_DOUBLE, VIK_LAYER_GROUP_NONE, N_("Default longitude:"),  VIK_LAYER_WIDGET_SPINBUTTON, params_scales_long, NULL, NULL },
};

/* External/Export Options */

static gchar * params_kml_export_units[] = {"Metric", "Statute", "Nautical", NULL};
static gchar * params_gpx_export_trk_sort[] = {N_("Alphabetical"), N_("Time"), NULL};

static VikLayerParam io_prefs[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_IO_NAMESPACE "kml_export_units", VIK_LAYER_PARAM_UINT, VIK_LAYER_GROUP_NONE, N_("KML File Export Units:"), VIK_LAYER_WIDGET_COMBOBOX, params_kml_export_units, NULL, NULL },
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_IO_NAMESPACE "gpx_export_track_sort", VIK_LAYER_PARAM_UINT, VIK_LAYER_GROUP_NONE, N_("GPX Track Order:"), VIK_LAYER_WIDGET_COMBOBOX, params_gpx_export_trk_sort, NULL, NULL },
};

#ifndef WINDOWS
static VikLayerParam io_prefs_non_windows[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_IO_NAMESPACE "image_viewer", VIK_LAYER_PARAM_STRING, VIK_LAYER_GROUP_NONE, N_("Image Viewer:"), VIK_LAYER_WIDGET_FILEENTRY, NULL, NULL, NULL },
};
#endif

static VikLayerParam io_prefs_external_gpx[] = {
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_IO_NAMESPACE "external_gpx_1", VIK_LAYER_PARAM_STRING, VIK_LAYER_GROUP_NONE, N_("External GPX Program 1:"), VIK_LAYER_WIDGET_FILEENTRY, NULL, NULL, NULL },
  { VIK_LAYER_NUM_TYPES, VIKING_PREFERENCES_IO_NAMESPACE "external_gpx_2", VIK_LAYER_PARAM_STRING, VIK_LAYER_GROUP_NONE, N_("External GPX Program 2:"), VIK_LAYER_WIDGET_FILEENTRY, NULL, NULL, NULL },
};

/* End of Options static stuff */

void a_vik_preferences_init ()
{
  // Defaults for the options are setup here
  a_preferences_register_group ( VIKING_PREFERENCES_GROUP_KEY, _("General") );

  VikLayerParamData tmp;
  tmp.u = VIK_DEGREE_FORMAT_DMS;
  a_preferences_register(prefs1, tmp, VIKING_PREFERENCES_GROUP_KEY);

  tmp.u = VIK_UNITS_DISTANCE_KILOMETRES;
  a_preferences_register(prefs2, tmp, VIKING_PREFERENCES_GROUP_KEY);

  tmp.u = VIK_UNITS_SPEED_KILOMETRES_PER_HOUR;
  a_preferences_register(prefs3, tmp, VIKING_PREFERENCES_GROUP_KEY);

  tmp.u = VIK_UNITS_HEIGHT_METRES;
  a_preferences_register(prefs4, tmp, VIKING_PREFERENCES_GROUP_KEY);

  tmp.b = TRUE;
  a_preferences_register(prefs5, tmp, VIKING_PREFERENCES_GROUP_KEY);

  /* Maintain the default location to New York */
  tmp.d = 40.714490;
  a_preferences_register(prefs6, tmp, VIKING_PREFERENCES_GROUP_KEY);
  tmp.d = -74.007130;
  a_preferences_register(prefs7, tmp, VIKING_PREFERENCES_GROUP_KEY);

  // New Tab
  a_preferences_register_group ( VIKING_PREFERENCES_IO_GROUP_KEY, _("Export/External") );

  tmp.u = VIK_KML_EXPORT_UNITS_METRIC;
  a_preferences_register(&io_prefs[0], tmp, VIKING_PREFERENCES_IO_GROUP_KEY);

  tmp.u = VIK_GPX_EXPORT_TRK_SORT_TIME;
  a_preferences_register(&io_prefs[1], tmp, VIKING_PREFERENCES_IO_GROUP_KEY);

#ifndef WINDOWS
  tmp.s = "xdg-open";
  a_preferences_register(&io_prefs_non_windows[0], tmp, VIKING_PREFERENCES_IO_GROUP_KEY);
#endif

  // JOSM for OSM editing around a GPX track
  tmp.s = "josm";
  a_preferences_register(&io_prefs_external_gpx[0], tmp, VIKING_PREFERENCES_IO_GROUP_KEY);
  // Add a second external program - another OSM editor by default
  tmp.s = "merkaartor";
  a_preferences_register(&io_prefs_external_gpx[1], tmp, VIKING_PREFERENCES_IO_GROUP_KEY);
}

vik_degree_format_t a_vik_get_degree_format ( )
{
  vik_degree_format_t format;
  format = a_preferences_get(VIKING_PREFERENCES_NAMESPACE "degree_format")->u;
  return format;
}

vik_units_distance_t a_vik_get_units_distance ( )
{
  vik_units_distance_t units;
  units = a_preferences_get(VIKING_PREFERENCES_NAMESPACE "units_distance")->u;
  return units;
}

vik_units_speed_t a_vik_get_units_speed ( )
{
  vik_units_speed_t units;
  units = a_preferences_get(VIKING_PREFERENCES_NAMESPACE "units_speed")->u;
  return units;
}

vik_units_height_t a_vik_get_units_height ( )
{
  vik_units_height_t units;
  units = a_preferences_get(VIKING_PREFERENCES_NAMESPACE "units_height")->u;
  return units;
}

gboolean a_vik_get_use_large_waypoint_icons ( )
{
  gboolean use_large_waypoint_icons;
  use_large_waypoint_icons = a_preferences_get(VIKING_PREFERENCES_NAMESPACE "use_large_waypoint_icons")->b;
  return use_large_waypoint_icons;
}

gdouble a_vik_get_default_lat ( )
{
  gdouble data;
  data = a_preferences_get(VIKING_PREFERENCES_NAMESPACE "default_latitude")->d;
  return data;
}

gdouble a_vik_get_default_long ( )
{
  gdouble data;
  data = a_preferences_get(VIKING_PREFERENCES_NAMESPACE "default_longitude")->d;
  return data;
}

/* External/Export Options */

vik_kml_export_units_t a_vik_get_kml_export_units ( )
{
  vik_kml_export_units_t units;
  units = a_preferences_get(VIKING_PREFERENCES_IO_NAMESPACE "kml_export_units")->u;
  return units;
}

vik_gpx_export_trk_sort_t a_vik_get_gpx_export_trk_sort ( )
{
  vik_gpx_export_trk_sort_t sort;
  sort = a_preferences_get(VIKING_PREFERENCES_IO_NAMESPACE "gpx_export_track_sort")->u;
  return sort;
}

#ifndef WINDOWS
const gchar* a_vik_get_image_viewer ( )
{
  return a_preferences_get(VIKING_PREFERENCES_IO_NAMESPACE "image_viewer")->s;
}
#endif

const gchar* a_vik_get_external_gpx_program_1 ( )
{
  return a_preferences_get(VIKING_PREFERENCES_IO_NAMESPACE "external_gpx_1")->s;
}

const gchar* a_vik_get_external_gpx_program_2 ( )
{
  return a_preferences_get(VIKING_PREFERENCES_IO_NAMESPACE "external_gpx_2")->s;
}
