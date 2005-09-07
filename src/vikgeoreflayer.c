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

#include "viking.h"
#include "vikgeoreflayer_pixmap.h"
#include <stdlib.h>
#include <string.h>

VikLayerParam georef_layer_params[] = {
  { "image", VIK_LAYER_PARAM_STRING, VIK_LAYER_NOT_IN_PROPERTIES },
  { "corner_easting", VIK_LAYER_PARAM_DOUBLE, VIK_LAYER_NOT_IN_PROPERTIES },
  { "corner_northing", VIK_LAYER_PARAM_DOUBLE, VIK_LAYER_NOT_IN_PROPERTIES },
  { "mpp_easting", VIK_LAYER_PARAM_DOUBLE, VIK_LAYER_NOT_IN_PROPERTIES },
  { "mpp_northing", VIK_LAYER_PARAM_DOUBLE, VIK_LAYER_NOT_IN_PROPERTIES },
};

enum { PARAM_IMAGE = 0, PARAM_CE, PARAM_CN, PARAM_ME, PARAM_MN, NUM_PARAMS };

static VikGeorefLayer *georef_layer_copy ( VikGeorefLayer *vgl, gpointer vp );
static gboolean georef_layer_set_param ( VikGeorefLayer *vgl, guint16 id, VikLayerParamData data, VikViewport *vp );
static VikLayerParamData georef_layer_get_param ( VikGeorefLayer *vgl, guint16 id );
VikGeorefLayer *georef_layer_new ( );
VikGeorefLayer *georef_layer_create ( VikViewport *vp );
static void georef_layer_free ( VikGeorefLayer *vgl );
gboolean georef_layer_properties ( VikGeorefLayer *vgl, gpointer vp );
static void georef_layer_draw ( VikGeorefLayer *vgl, gpointer data );
static void georef_layer_add_menu_items ( VikGeorefLayer *vgl, GtkMenu *menu, gpointer vlp );
static void georef_layer_set_image ( VikGeorefLayer *vgl, const gchar *image );
static gboolean georef_layer_dialog ( VikGeorefLayer **vgl, gpointer vp, GtkWindow *w );
static void georef_layer_load_image ( VikGeorefLayer *vgl );
static gboolean georef_layer_move_release ( VikGeorefLayer *vgl, GdkEventButton *event, VikViewport *vvp );
static gboolean georef_layer_move_press ( VikGeorefLayer *vgl, GdkEventButton *event, VikViewport *vvp );
static gboolean georef_layer_zoom_press ( VikGeorefLayer *vgl, GdkEventButton *event, VikViewport *vvp );

static VikToolInterface georef_tools[] = {
  { "Georef Move Map", (VikToolInterfaceFunc) georef_layer_move_press,  (VikToolInterfaceFunc) georef_layer_move_release },
  { "Georef Zoom Tool", (VikToolInterfaceFunc) georef_layer_zoom_press, NULL },
};

VikLayerInterface vik_georef_layer_interface = {
  "GeoRef Map",
  &georeflayer_pixbuf, /*icon */

  georef_tools,
  sizeof(georef_tools) / sizeof(VikToolInterface),

  georef_layer_params,
  NUM_PARAMS,
  NULL,
  0,

  (VikLayerFuncCreate)                  georef_layer_create,
  (VikLayerFuncRealize)                 NULL,
  (VikLayerFuncPostRead)                georef_layer_load_image,
  (VikLayerFuncFree)                    georef_layer_free,

  (VikLayerFuncProperties)              georef_layer_properties,
  (VikLayerFuncDraw)                    georef_layer_draw,
  (VikLayerFuncChangeCoordMode)         NULL,

  (VikLayerFuncAddMenuItems)            georef_layer_add_menu_items,
  (VikLayerFuncSublayerAddMenuItems)    NULL,

  (VikLayerFuncSublayerRenameRequest)   NULL,
  (VikLayerFuncSublayerToggleVisible)   NULL,

  (VikLayerFuncCopy)                    georef_layer_copy,

  (VikLayerFuncSetParam)                georef_layer_set_param,
  (VikLayerFuncGetParam)                georef_layer_get_param,

  (VikLayerFuncReadFileData)            NULL,
  (VikLayerFuncWriteFileData)           NULL,

  (VikLayerFuncCopyItem)                NULL,
  (VikLayerFuncPasteItem)               NULL,
  (VikLayerFuncFreeCopiedItem)          NULL,
};

struct _VikGeorefLayer {
  VikLayer vl;
  gchar *image;
  GdkPixbuf *pixbuf;
  struct UTM corner;
  gdouble mpp_easting, mpp_northing;
  guint width, height;

  gint click_x, click_y;
};



GType vik_georef_layer_get_type ()
{
  static GType vgl_type = 0;

  if (!vgl_type)
  {
    static const GTypeInfo vgl_info =
    {
      sizeof (VikGeorefLayerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      NULL, /* class init */
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (VikGeorefLayer),
      0,
      NULL /* instance init */
    };
    vgl_type = g_type_register_static ( VIK_LAYER_TYPE, "VikGeorefLayer", &vgl_info, 0 );
  }

  return vgl_type;
}

static VikGeorefLayer *georef_layer_copy ( VikGeorefLayer *vgl, gpointer vp )
{
  VikGeorefLayer *rv = georef_layer_new ();
  rv->corner = vgl->corner;
  rv->mpp_easting = vgl->mpp_easting;
  rv->mpp_northing = vgl->mpp_northing;
  rv->width = vgl->width;
  rv->height = vgl->height;

  if ( vgl->image )
  {
    rv->image = g_strdup ( vgl->image );
    georef_layer_load_image ( rv );
  }
  return rv;
}

static gboolean georef_layer_set_param ( VikGeorefLayer *vgl, guint16 id, VikLayerParamData data, VikViewport *vp )
{
  switch ( id )
  {
    case PARAM_IMAGE: georef_layer_set_image ( vgl, data.s ); break;
    case PARAM_CN: vgl->corner.northing = data.d; break;
    case PARAM_CE: vgl->corner.easting = data.d; break;
    case PARAM_MN: vgl->mpp_northing = data.d; break;
    case PARAM_ME:  vgl->mpp_easting = data.d; break;
  }
  return TRUE;
}

static VikLayerParamData georef_layer_get_param ( VikGeorefLayer *vgl, guint16 id )
{
  VikLayerParamData rv;
  switch ( id )
  {
    case PARAM_IMAGE: rv.s = vgl->image ? vgl->image : ""; break;
    case PARAM_CN: rv.d = vgl->corner.northing; break;
    case PARAM_CE: rv.d = vgl->corner.easting; break;
    case PARAM_MN: rv.d = vgl->mpp_northing; break;
    case PARAM_ME: rv.d = vgl->mpp_easting; break;
  }
  return rv;
}

VikGeorefLayer *georef_layer_new ( )
{
  VikGeorefLayer *vgl = VIK_GEOREF_LAYER ( g_object_new ( VIK_GEOREF_LAYER_TYPE, NULL ) );
  vik_layer_init ( VIK_LAYER(vgl), VIK_LAYER_GEOREF );

  vgl->image = NULL;
  vgl->pixbuf = NULL;
  vgl->click_x = -1;
  vgl->click_y = -1;
  return vgl;
}

static void georef_layer_draw ( VikGeorefLayer *vgl, gpointer data )
{
/* bla, bla */
  if ( vgl->pixbuf )
  {
    VikViewport *vp = VIK_VIEWPORT(data);
    struct UTM utm_middle;
    gdouble xmpp = vik_viewport_get_xmpp(vp), ympp = vik_viewport_get_ympp(vp);
    vik_coord_to_utm ( vik_viewport_get_center ( vp ), &utm_middle );

    if ( xmpp == vgl->mpp_easting && ympp == vgl->mpp_northing )
    {
      guint width = vik_viewport_get_width(vp), height = vik_viewport_get_height(vp);
      gint32 x, y;
      vgl->corner.zone = utm_middle.zone;
      vgl->corner.letter = utm_middle.letter;
      VikCoord corner_coord;
      vik_coord_load_from_utm ( &corner_coord, vik_viewport_get_coord_mode(vp), &(vgl->corner) );
      vik_viewport_coord_to_screen ( vp, &corner_coord, &x, &y );
      if ( (x < 0 || x < width) && (y < 0 || y < height) && x+vgl->width > 0 && y+vgl->height > 0 )
        vik_viewport_draw_pixbuf ( vp, vgl->pixbuf, 0, 0, x, y, vgl->width, vgl->height ); /* todo: draw only what we need to. */
    }
  }
}

static void georef_layer_free ( VikGeorefLayer *vgl )
{
  if ( vgl->image != NULL )
    g_free ( vgl->image );
}

VikGeorefLayer *georef_layer_create ( VikViewport *vp )
{
  return georef_layer_new ();
}

gboolean georef_layer_properties ( VikGeorefLayer *vgl, gpointer vp )
{
  return georef_layer_dialog ( &vgl, vp, VIK_GTK_WINDOW_FROM_WIDGET(vp) );
}

static void georef_layer_load_image ( VikGeorefLayer *vgl )
{
  GError *gx = NULL;
  if ( vgl->image == NULL )
    return;

  if ( vgl->pixbuf )
    g_object_unref ( G_OBJECT(vgl->pixbuf) );

  vgl->pixbuf = gdk_pixbuf_new_from_file ( vgl->image, &gx );

  if (gx)
  {
    g_warning ( "Couldn't open image file: %s", gx->message );
    g_error_free ( gx );
  }
  else
  {
    vgl->width = gdk_pixbuf_get_width ( vgl->pixbuf );
    vgl->height = gdk_pixbuf_get_height ( vgl->pixbuf );
  }

  /* should find length and width here too */
}

static void georef_layer_set_image ( VikGeorefLayer *vgl, const gchar *image )
{
  if ( vgl->image )
    g_free ( vgl->image );
  if ( image == NULL )
    vgl->image = NULL;
  vgl->image = g_strdup ( image );
}

static gboolean world_file_read_line ( gchar *buffer, gint size, FILE *f, GtkWidget *widget, gboolean use_value )
{
  if (!fgets ( buffer, 1024, f ))
  {
    a_dialog_error_msg ( VIK_GTK_WINDOW_FROM_WIDGET(widget), "Unexpected end of file reading World file." );
    g_free ( buffer );
    fclose ( f );
    return FALSE;
  }
  if ( use_value )
  {
    gdouble val = strtod ( buffer, NULL );
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(widget), val > 0 ? val : -val );
  }
  return TRUE;
}

static void georef_layer_dialog_load ( GtkWidget *pass_along[4] )
{
  GtkWidget *file_selector = gtk_file_selection_new ("Choose World file");

  if ( gtk_dialog_run ( GTK_DIALOG ( file_selector ) ) == GTK_RESPONSE_OK )
  {
    FILE *f = fopen ( gtk_file_selection_get_filename ( GTK_FILE_SELECTION(file_selector) ), "r" );
    gtk_widget_destroy ( file_selector ); 
    if ( !f )
    {
      a_dialog_error_msg ( VIK_GTK_WINDOW_FROM_WIDGET(pass_along[0]), "The World file you requested could not be opened for reading." );
      return;
    }
    else
    {
      gchar *buffer = g_malloc ( 1024 * sizeof(gchar) );
      if ( world_file_read_line ( buffer, 1024, f, pass_along[0], TRUE ) && world_file_read_line ( buffer, 1024, f, pass_along[1], TRUE )
        && world_file_read_line ( buffer, 1024, f, pass_along[0], FALSE ) && world_file_read_line ( buffer, 1024, f, pass_along[0], FALSE )
        && world_file_read_line ( buffer, 1024, f, pass_along[2], TRUE ) && world_file_read_line ( buffer, 1024, f, pass_along[3], TRUE ) )
      {
        g_free ( buffer );
        fclose ( f );
      }
    }
  }
  else
    gtk_widget_destroy ( file_selector ); 
/* do your jazz
We need a
  file selection dialog
  file opener for reading, if NULL, send error_msg ( VIK_GTK_WINDOW_FROM_WIDGET(pass_along[0]) )
  does that catch directories too?
  read lines -- if not enough lines, give error.
  if anything outside, give error. define range with #define CONSTANTS
  put 'em in thar widgets, and that's it.
*/
}

static void georef_layer_export_params ( gpointer *pass_along[2] )
{
  VikGeorefLayer *vgl = VIK_GEOREF_LAYER(pass_along[0]);
  GtkWidget *file_selector = gtk_file_selection_new ("Choose World file");

  if ( gtk_dialog_run ( GTK_DIALOG ( file_selector ) ) == GTK_RESPONSE_OK )
  {
    FILE *f = fopen ( gtk_file_selection_get_filename ( GTK_FILE_SELECTION(file_selector) ), "w" );
    gtk_widget_destroy ( file_selector ); 
    if ( !f )
    {
      a_dialog_error_msg ( VIK_GTK_WINDOW_FROM_WIDGET(pass_along[0]), "The file you requested could not be opened for writing." );
      return;
    }
    else
    {
      fprintf ( f, "%f\n%f\n%f\n%f\n%f\n%f\n", vgl->mpp_easting, vgl->mpp_northing, 0.0, 0.0, vgl->corner.easting, vgl->corner.northing );
      fclose ( f );
    }
  }
  else
   gtk_widget_destroy ( file_selector ); 
}

/* returns TRUE if OK was pressed. */
static gboolean georef_layer_dialog ( VikGeorefLayer **vgl, gpointer vp, GtkWindow *w )
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Layer Properties",
                                                  w,
                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  0 );
  GtkWidget *table, *wfp_hbox, *wfp_label, *wfp_button, *ce_label, *ce_spin, *cn_label, *cn_spin, *xlabel, *xspin, *ylabel, *yspin, *imagelabel, *imageentry;

  GtkWidget *pass_along[4];

  table = gtk_table_new ( 6, 2, FALSE );
  gtk_box_pack_start ( GTK_BOX(GTK_DIALOG(dialog)->vbox), table, TRUE, TRUE, 0 );

  wfp_hbox = gtk_hbox_new ( FALSE, 0 );
  wfp_label = gtk_label_new ( "World File Parameters:" );
  wfp_button = gtk_button_new_with_label ( "Load From File..." );

  gtk_box_pack_start ( GTK_BOX(wfp_hbox), wfp_label, TRUE, TRUE, 0 );
  gtk_box_pack_start ( GTK_BOX(wfp_hbox), wfp_button, FALSE, FALSE, 3 );

  ce_label = gtk_label_new ( "Corner pixel easting:" );
  ce_spin = gtk_spin_button_new ( (GtkAdjustment *) gtk_adjustment_new ( 4, 0.0, 1500000.0, 1, 5, 5 ), 1, 4 );

  cn_label = gtk_label_new ( "Corner pixel northing:" );
  cn_spin = gtk_spin_button_new ( (GtkAdjustment *) gtk_adjustment_new ( 4, 0.0, 9000000.0, 1, 5, 5 ), 1, 4 );

  xlabel = gtk_label_new ( "X (easting) scale (mpp): ");
  ylabel = gtk_label_new ( "Y (northing) scale (mpp): ");

  xspin = gtk_spin_button_new ( (GtkAdjustment *) gtk_adjustment_new ( 4, VIK_VIEWPORT_MIN_ZOOM, VIK_VIEWPORT_MAX_ZOOM, 1, 5, 5 ), 1, 8 );
  yspin = gtk_spin_button_new ( (GtkAdjustment *) gtk_adjustment_new ( 4, VIK_VIEWPORT_MIN_ZOOM, VIK_VIEWPORT_MAX_ZOOM, 1, 5, 5 ), 1, 8 );

  imagelabel = gtk_label_new ( "Map Image:" );
  imageentry = vik_file_entry_new ();

  if (*vgl)
  {
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(ce_spin), (*vgl)->corner.easting );
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(cn_spin), (*vgl)->corner.northing );
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(xspin), (*vgl)->mpp_easting );
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(yspin), (*vgl)->mpp_northing );
    if ( (*vgl)->image )
    vik_file_entry_set_filename ( VIK_FILE_ENTRY(imageentry), (*vgl)->image );
  }
  else
  {
    VikCoord corner_coord;
    struct UTM utm;
    vik_viewport_screen_to_coord ( VIK_VIEWPORT(vp), 0, 0, &corner_coord );
    vik_coord_to_utm ( &corner_coord, &utm );
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(ce_spin), utm.easting );
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(cn_spin), utm.northing );
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(xspin), vik_viewport_get_xmpp ( VIK_VIEWPORT(vp) ) );
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON(yspin), vik_viewport_get_ympp ( VIK_VIEWPORT(vp) ) );
  }

  gtk_table_attach_defaults ( GTK_TABLE(table), imagelabel, 0, 1, 0, 1 );
  gtk_table_attach_defaults ( GTK_TABLE(table), imageentry, 1, 2, 0, 1 );
  gtk_table_attach_defaults ( GTK_TABLE(table), wfp_hbox, 0, 2, 1, 2 );
  gtk_table_attach_defaults ( GTK_TABLE(table), xlabel, 0, 1, 2, 3 );
  gtk_table_attach_defaults ( GTK_TABLE(table), xspin, 1, 2, 2, 3 );
  gtk_table_attach_defaults ( GTK_TABLE(table), ylabel, 0, 1, 3, 4 );
  gtk_table_attach_defaults ( GTK_TABLE(table), yspin, 1, 2, 3, 4 );
  gtk_table_attach_defaults ( GTK_TABLE(table), ce_label, 0, 1, 4, 5 );
  gtk_table_attach_defaults ( GTK_TABLE(table), ce_spin, 1, 2, 4, 5 );
  gtk_table_attach_defaults ( GTK_TABLE(table), cn_label, 0, 1, 5, 6 );
  gtk_table_attach_defaults ( GTK_TABLE(table), cn_spin, 1, 2, 5, 6 );

  pass_along[0] = xspin;
  pass_along[1] = yspin;
  pass_along[2] = ce_spin;
  pass_along[3] = cn_spin;
  g_signal_connect_swapped ( G_OBJECT(wfp_button), "clicked", G_CALLBACK(georef_layer_dialog_load), pass_along );

  gtk_widget_show_all ( table );

  if ( gtk_dialog_run ( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
  {
    if (! *vgl)
    {
      *vgl = georef_layer_new ();
       vik_layer_rename ( VIK_LAYER(*vgl), vik_georef_layer_interface.name );
    }
    (*vgl)->corner.easting = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(ce_spin) );
    (*vgl)->corner.northing = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(cn_spin) );
    (*vgl)->mpp_easting = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(xspin) );
    (*vgl)->mpp_northing = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(yspin) );
    if ( (!(*vgl)->image) || strcmp( (*vgl)->image, vik_file_entry_get_filename(VIK_FILE_ENTRY(imageentry)) ) != 0 )
    {
      georef_layer_set_image ( *vgl, vik_file_entry_get_filename(VIK_FILE_ENTRY(imageentry)) );
      georef_layer_load_image ( *vgl );
    }

    gtk_widget_destroy ( GTK_WIDGET(dialog) );
    return TRUE;
  }
  gtk_widget_destroy ( GTK_WIDGET(dialog) );
  return FALSE;
}

static void georef_layer_zoom_to_fit ( gpointer vgl_vlp[2] )
{
  vik_viewport_set_xmpp ( vik_layers_panel_get_viewport(VIK_LAYERS_PANEL(vgl_vlp[1])), VIK_GEOREF_LAYER(vgl_vlp[0])->mpp_easting );
  vik_viewport_set_ympp ( vik_layers_panel_get_viewport(VIK_LAYERS_PANEL(vgl_vlp[1])), VIK_GEOREF_LAYER(vgl_vlp[0])->mpp_northing );
  vik_layers_panel_emit_update ( VIK_LAYERS_PANEL(vgl_vlp[1]) );
}

static void georef_layer_goto_center ( gpointer vgl_vlp[2] )
{
  VikGeorefLayer *vgl = VIK_GEOREF_LAYER ( vgl_vlp[0] );
  VikViewport *vp = vik_layers_panel_get_viewport(VIK_LAYERS_PANEL(vgl_vlp[1]));
  struct UTM utm;
  VikCoord coord;

  vik_coord_to_utm ( vik_viewport_get_center ( vp ), &utm );

  utm.easting = vgl->corner.easting + (vgl->width * vgl->mpp_easting / 2); /* only an approximation */
  utm.northing = vgl->corner.northing - (vgl->height * vgl->mpp_northing / 2);

  vik_coord_load_from_utm ( &coord, vik_viewport_get_coord_mode ( vp ), &utm );
  vik_viewport_set_center_coord ( vp, &coord );

  vik_layers_panel_emit_update ( VIK_LAYERS_PANEL(vgl_vlp[1]) );
}

static void georef_layer_add_menu_items ( VikGeorefLayer *vgl, GtkMenu *menu, gpointer vlp )
{
  static gpointer pass_along[2];
  GtkWidget *item;
  pass_along[0] = vgl;
  pass_along[1] = vlp;

  item = gtk_menu_item_new();
  gtk_menu_shell_append ( GTK_MENU_SHELL(menu), item );
  gtk_widget_show ( item );

  item = gtk_menu_item_new_with_label ( "Zoom to Fit Map" );
  g_signal_connect_swapped ( G_OBJECT(item), "activate", G_CALLBACK(georef_layer_zoom_to_fit), pass_along );
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  gtk_widget_show ( item );

  item = gtk_menu_item_new_with_label ( "Goto Map Center" );
  g_signal_connect_swapped ( G_OBJECT(item), "activate", G_CALLBACK(georef_layer_goto_center), pass_along );
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  gtk_widget_show ( item );

  item = gtk_menu_item_new_with_label ( "Export to World File" );
  g_signal_connect_swapped ( G_OBJECT(item), "activate", G_CALLBACK(georef_layer_export_params), pass_along );
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  gtk_widget_show ( item );
}

static gboolean georef_layer_move_release ( VikGeorefLayer *vgl, GdkEventButton *event, VikViewport *vvp )
{
  if ( vgl->click_x != -1 )
  {
    vgl->corner.easting += (event->x - vgl->click_x) * vik_viewport_get_xmpp (vvp);
    vgl->corner.northing -= (event->y - vgl->click_y) * vik_viewport_get_ympp (vvp);
    vik_layer_emit_update ( VIK_LAYER(vgl) );
    return TRUE;
  }
  return FALSE; /* I didn't move anything on this layer! */
}

static gboolean georef_layer_zoom_press ( VikGeorefLayer *vgl, GdkEventButton *event, VikViewport *vvp )
{
  if ( event->button == 1 )
  {
    if ( vgl->mpp_easting < (VIK_VIEWPORT_MAX_ZOOM / 1.05) && vgl->mpp_northing < (VIK_VIEWPORT_MAX_ZOOM / 1.05) )
    {
      vgl->mpp_easting *= 1.01;
      vgl->mpp_northing *= 1.01;
    }
  }
  else
  {
    if ( vgl->mpp_easting > (VIK_VIEWPORT_MIN_ZOOM * 1.05) && vgl->mpp_northing > (VIK_VIEWPORT_MIN_ZOOM * 1.05) )
    {
      vgl->mpp_easting /= 1.01;
      vgl->mpp_northing /= 1.01;
    }
  }
  vik_viewport_set_xmpp ( vvp, vgl->mpp_easting );
  vik_viewport_set_ympp ( vvp, vgl->mpp_northing );
  vik_layer_emit_update ( VIK_LAYER(vgl) );
  return TRUE;
}

static gboolean georef_layer_move_press ( VikGeorefLayer *vgl, GdkEventButton *event, VikViewport *vvp )
{
  vgl->click_x = event->x;
  vgl->click_y = event->y;
  return TRUE;
}