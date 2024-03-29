/*
 * viking -- GPS Data and Topo Analyzer, Explorer, and Manager
 *
 * Copyright (C) 2003-2005, Evan Battaglia <gtoevan@gmx.net>
 * Copyright (c) 2011, Rob Norris <rw_norris@hotmail.com>
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

#ifndef _VIKING_TRWLAYER_H
#define _VIKING_TRWLAYER_H

#include "viklayer.h"
#include "vikviewport.h"
#include "vikwaypoint.h"
#include "viktrack.h"
#include "viklayerspanel.h"

G_BEGIN_DECLS

#define VIK_TRW_LAYER_TYPE            (vik_trw_layer_get_type ())
#define VIK_TRW_LAYER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), VIK_TRW_LAYER_TYPE, VikTrwLayer))
#define VIK_TRW_LAYER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), VIK_TRW_LAYER_TYPE, VikTrwLayerClass))
#define IS_VIK_TRW_LAYER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VIK_TRW_LAYER_TYPE))
#define IS_VIK_TRW_LAYER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VIK_TRW_LAYER_TYPE))

enum {
  VIK_TRW_LAYER_SUBLAYER_TRACKS,
  VIK_TRW_LAYER_SUBLAYER_WAYPOINTS,
  VIK_TRW_LAYER_SUBLAYER_TRACK,
  VIK_TRW_LAYER_SUBLAYER_WAYPOINT,
  VIK_TRW_LAYER_SUBLAYER_ROUTES,
  VIK_TRW_LAYER_SUBLAYER_ROUTE
};

typedef struct _VikTrwLayerClass VikTrwLayerClass;
struct _VikTrwLayerClass
{
  VikLayerClass object_class;
};


GType vik_trw_layer_get_type ();

typedef struct _VikTrwLayer VikTrwLayer;

/* These are meant for use in file loaders (gpspoint.c, gpx.c, etc).
 * These copy the name, so you should free it if necessary. */
void vik_trw_layer_filein_add_waypoint ( VikTrwLayer *vtl, gchar *name, VikWaypoint *wp );
void vik_trw_layer_filein_add_track ( VikTrwLayer *vtl, gchar *name, VikTrack *tr );

gint vik_trw_layer_get_property_tracks_line_thickness ( VikTrwLayer *vtl );

void vik_trw_layer_add_waypoint ( VikTrwLayer *vtl, gchar *name, VikWaypoint *wp );
void vik_trw_layer_add_track ( VikTrwLayer *vtl, gchar *name, VikTrack *t );
void vik_trw_layer_add_route ( VikTrwLayer *vtl, gchar *name, VikTrack *t );

// Waypoint returned is the first one
VikWaypoint *vik_trw_layer_get_waypoint ( VikTrwLayer *vtl, const gchar *name );

// Track returned is the first one
VikTrack *vik_trw_layer_get_track ( VikTrwLayer *vtl, const gchar *name );
gboolean vik_trw_layer_delete_track ( VikTrwLayer *vtl, VikTrack *trk );
gboolean vik_trw_layer_delete_route ( VikTrwLayer *vtl, VikTrack *trk );

gboolean vik_trw_layer_auto_set_view ( VikTrwLayer *vtl, VikViewport *vvp );
gboolean vik_trw_layer_find_center ( VikTrwLayer *vtl, VikCoord *dest );
GHashTable *vik_trw_layer_get_tracks ( VikTrwLayer *l );
GHashTable *vik_trw_layer_get_routes ( VikTrwLayer *l );
GHashTable *vik_trw_layer_get_waypoints ( VikTrwLayer *l );
gboolean vik_trw_layer_is_empty ( VikTrwLayer *vtl );

gboolean vik_trw_layer_new_waypoint ( VikTrwLayer *vtl, GtkWindow *w, const VikCoord *def_coord );

VikCoordMode vik_trw_layer_get_coord_mode ( VikTrwLayer *vtl );

gboolean vik_trw_layer_uniquify ( VikTrwLayer *vtl, VikLayersPanel *vlp );

void vik_trw_layer_delete_all_waypoints ( VikTrwLayer *vtl );
void vik_trw_layer_delete_all_tracks ( VikTrwLayer *vtl );
void vik_trw_layer_delete_all_routes ( VikTrwLayer *vtl );
void trw_layer_cancel_tps_of_track ( VikTrwLayer *vtl, VikTrack *trk );

/* Exposed Layer Interface function definitions */
// Intended only for use by other trw_layer subwindows
void trw_layer_verify_thumbnails ( VikTrwLayer *vtl, GtkWidget *vp );
// Other functions only for use by other trw_layer subwindows
gchar *trw_layer_new_unique_sublayer_name ( VikTrwLayer *vtl, gint sublayer_type, const gchar *name );

void trw_layer_update_treeview ( VikTrwLayer *vtl, VikTrack *trk, gpointer *trk_id );

G_END_DECLS

#endif
