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
#include "thumbnails.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void a_dialog_msg ( GtkWindow *parent, gint type, const gchar *info, const gchar *extra )
{
  GtkWidget *msgbox = gtk_message_dialog_new ( parent, GTK_DIALOG_DESTROY_WITH_PARENT, type, GTK_BUTTONS_OK, info, extra );
  gtk_dialog_run ( GTK_DIALOG(msgbox) );
  gtk_widget_destroy ( msgbox );
}

gboolean a_dialog_goto_latlon ( GtkWindow *parent, struct LatLon *ll, const struct LatLon *old )
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Go to Lat/Lon",
                                                  parent,
                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  NULL);
  GtkWidget *latlabel, *lonlabel;
  GtkWidget *lat, *lon;
  gchar *tmp_lat, *tmp_lon;

  latlabel = gtk_label_new ("Latitude:");
  lat = gtk_entry_new ();
  tmp_lat = g_strdup_printf ( "%f", old->lat );
  gtk_entry_set_text ( GTK_ENTRY(lat), tmp_lat );
  g_free ( tmp_lat );

  lonlabel = gtk_label_new ("Longitude:");
  lon = gtk_entry_new ();
  tmp_lon = g_strdup_printf ( "%f", old->lon );
  gtk_entry_set_text ( GTK_ENTRY(lon), tmp_lon );
  g_free ( tmp_lon );

  gtk_widget_show ( latlabel );
  gtk_widget_show ( lonlabel );
  gtk_widget_show ( lat );
  gtk_widget_show ( lon );

  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), latlabel,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), lat, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), lonlabel,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), lon,  FALSE, FALSE, 0);

  if ( gtk_dialog_run ( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
  {
    ll->lat = atof ( gtk_entry_get_text ( GTK_ENTRY(lat) ) );
    ll->lon = atof ( gtk_entry_get_text ( GTK_ENTRY(lon) ) );
    gtk_widget_destroy ( dialog );
    return TRUE;
  }

  gtk_widget_destroy ( dialog );
  return FALSE;
}

gboolean a_dialog_goto_utm ( GtkWindow *parent, struct UTM *utm, const struct UTM *old )
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Go to Lat/Lon",
                                                  parent,
                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  NULL);
  GtkWidget *norlabel, *easlabel, *nor, *eas;
  GtkWidget *zonehbox, *zonespin, *letterentry;
  gchar *tmp_eas, *tmp_nor;
  gchar tmp_letter[2];

  norlabel = gtk_label_new ("Northing:");
  nor = gtk_entry_new ();
  tmp_nor = g_strdup_printf("%ld", (long) old->northing );
  gtk_entry_set_text ( GTK_ENTRY(nor), tmp_nor );
  g_free ( tmp_nor );

  easlabel = gtk_label_new ("Easting:");
  eas = gtk_entry_new ();
  tmp_eas = g_strdup_printf("%ld", (long) old->easting );
  gtk_entry_set_text ( GTK_ENTRY(eas), tmp_eas );
  g_free ( tmp_eas );

  zonehbox = gtk_hbox_new ( FALSE, 0 );
  gtk_box_pack_start ( GTK_BOX(zonehbox), gtk_label_new ( "Zone:" ), FALSE, FALSE, 5 );
  zonespin = gtk_spin_button_new ( (GtkAdjustment *) gtk_adjustment_new ( old->zone, 1, 60, 1, 5, 5 ), 1, 0 );
  gtk_box_pack_start ( GTK_BOX(zonehbox), zonespin, TRUE, TRUE, 5 );
  gtk_box_pack_start ( GTK_BOX(zonehbox), gtk_label_new ( "Letter:" ), FALSE, FALSE, 5 );
  letterentry = gtk_entry_new ();
  gtk_entry_set_max_length ( GTK_ENTRY(letterentry), 1 );
  gtk_entry_set_width_chars ( GTK_ENTRY(letterentry), 2 );
  tmp_letter[0] = old->letter;
  tmp_letter[1] = '\0';
  gtk_entry_set_text ( GTK_ENTRY(letterentry), tmp_letter );
  gtk_box_pack_start ( GTK_BOX(zonehbox), letterentry, FALSE, FALSE, 5 );

  gtk_widget_show ( norlabel );
  gtk_widget_show ( easlabel );
  gtk_widget_show ( nor );
  gtk_widget_show ( eas );

  gtk_widget_show_all ( zonehbox );

  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), norlabel, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), nor, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), easlabel,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), eas,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), zonehbox,  FALSE, FALSE, 0);

  if ( gtk_dialog_run ( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
  {
    const gchar *letter;
    utm->northing = atof ( gtk_entry_get_text ( GTK_ENTRY(nor) ) );
    utm->easting = atof ( gtk_entry_get_text ( GTK_ENTRY(eas) ) );
    utm->zone = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON(zonespin) );
    letter = gtk_entry_get_text ( GTK_ENTRY(letterentry) );
    if (*letter)
       utm->letter = toupper(*letter);
    gtk_widget_destroy ( dialog );
    return TRUE;
  }

  gtk_widget_destroy ( dialog );
  return FALSE;
}

void a_dialog_response_accept ( GtkDialog *dialog )
{
  gtk_dialog_response ( dialog, GTK_RESPONSE_ACCEPT );
}

/* todo: less on this side, like add track */
gboolean a_dialog_new_waypoint ( GtkWindow *parent, gchar **dest, VikWaypoint *wp, GHashTable *waypoints, VikCoordMode coord_mode )
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Create",
                                                   parent,
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_STOCK_CANCEL,
                                                   GTK_RESPONSE_REJECT,
                                                   GTK_STOCK_OK,
                                                   GTK_RESPONSE_ACCEPT,
                                                   NULL);
  struct LatLon ll;
  GtkWidget *latlabel, *lonlabel, *namelabel, *latentry, *lonentry, *altentry, *altlabel, *nameentry, *commentlabel, *commententry, *imagelabel, *imageentry;

  gchar *lat, *lon, *alt;

  vik_coord_to_latlon ( &(wp->coord), &ll );

  lat = g_strdup_printf ( "%f", ll.lat );
  lon = g_strdup_printf ( "%f", ll.lon );
  alt = g_strdup_printf ( "%f", wp->altitude );

  if ( dest != NULL )
  {
    namelabel = gtk_label_new ("Name:");
    nameentry = gtk_entry_new ();
    g_signal_connect_swapped ( nameentry, "activate", G_CALLBACK(a_dialog_response_accept), GTK_DIALOG(dialog) );
    gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), namelabel, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), nameentry, FALSE, FALSE, 0);
  }

  latlabel = gtk_label_new ("Latitude:");
  latentry = gtk_entry_new ();
  gtk_entry_set_text ( GTK_ENTRY(latentry), lat );
  g_free ( lat );

  lonlabel = gtk_label_new ("Longitude:");
  lonentry = gtk_entry_new ();
  gtk_entry_set_text ( GTK_ENTRY(lonentry), lon );
  g_free ( lon );

  altlabel = gtk_label_new ("Altitude:");
  altentry = gtk_entry_new ();
  gtk_entry_set_text ( GTK_ENTRY(altentry), alt );
  g_free ( alt );

  commentlabel = gtk_label_new ("Comment:");
  commententry = gtk_entry_new ();

  imagelabel = gtk_label_new ("Image:");
  imageentry = vik_file_entry_new ();

  if ( dest == NULL && wp->comment )
    gtk_entry_set_text ( GTK_ENTRY(commententry), wp->comment );

  if ( dest == NULL && wp->image )
    vik_file_entry_set_filename ( VIK_FILE_ENTRY(imageentry), wp->image );

  

  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), latlabel, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), latentry, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), lonlabel, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), lonentry, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), altlabel, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), altentry, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), commentlabel, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), commententry, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), imagelabel, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), imageentry, FALSE, FALSE, 0);

  gtk_widget_show_all ( GTK_DIALOG(dialog)->vbox );
	
  while ( gtk_dialog_run ( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
  {
    if ( dest )
    {
      const gchar *constname = gtk_entry_get_text ( GTK_ENTRY(nameentry) );
      if ( strlen(constname) == 0 ) /* TODO: other checks (isalpha or whatever ) */
        a_dialog_info_msg ( parent, "Please enter a name for the waypoint." );
      else {
        int i;
        gchar *name = g_strdup ( constname );

        for ( i = strlen ( name ) - 1; i >= 0; i-- )
          name[i] = toupper(name[i]); /* all caps for stardandization */

        if ( g_hash_table_lookup ( waypoints, name ) && !a_dialog_overwrite ( parent, "The waypoint \"%s\" exists, do you want to overwrite it?", name ) )
          g_free ( name );
        else
        {
          /* Do It */
          *dest = name;
          ll.lat = atof ( gtk_entry_get_text ( GTK_ENTRY(latentry) ) );
          ll.lon = atof ( gtk_entry_get_text ( GTK_ENTRY(lonentry) ) );
          vik_coord_load_from_latlon ( &(wp->coord), coord_mode, &ll );
          wp->altitude = atof ( gtk_entry_get_text ( GTK_ENTRY(altentry) ) );
          vik_waypoint_set_comment ( wp, gtk_entry_get_text ( GTK_ENTRY(commententry) ) );
          vik_waypoint_set_image ( wp, vik_file_entry_get_filename ( VIK_FILE_ENTRY(imageentry) ) );
          if ( wp->image && *(wp->image) && (!a_thumbnails_exists(wp->image)) )
            a_thumbnails_create ( wp->image );
          gtk_widget_destroy ( dialog );
          return TRUE;
        }
      } /* else (valid name) */
    }
    else
    {
      ll.lat = atof ( gtk_entry_get_text ( GTK_ENTRY(latentry) ) );
      ll.lon = atof ( gtk_entry_get_text ( GTK_ENTRY(lonentry) ) );
      vik_coord_load_from_latlon ( &(wp->coord), coord_mode, &ll );
      wp->altitude = atof ( gtk_entry_get_text ( GTK_ENTRY(altentry) ) );
      if ( (! wp->comment) || strcmp ( wp->comment, gtk_entry_get_text ( GTK_ENTRY(commententry) ) ) != 0 )
        vik_waypoint_set_comment ( wp, gtk_entry_get_text ( GTK_ENTRY(commententry) ) );
      if ( (! wp->image) || strcmp ( wp->image, vik_file_entry_get_filename ( VIK_FILE_ENTRY ( imageentry ) ) ) != 0 )
      {
        vik_waypoint_set_image ( wp, vik_file_entry_get_filename ( VIK_FILE_ENTRY(imageentry) ) );
        if ( wp->image && *(wp->image) && (!a_thumbnails_exists(wp->image)) )
          a_thumbnails_create ( wp->image );
      }

      gtk_widget_destroy ( dialog );

      return TRUE;
    }
  }
  gtk_widget_destroy ( dialog );
  return FALSE;
}

gchar *a_dialog_new_track ( GtkWindow *parent, GHashTable *tracks )
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Add Track",
                                                  parent,
                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  NULL);
  GtkWidget *label = gtk_label_new ( "Track Name:" );
  GtkWidget *entry = gtk_entry_new ();

  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), entry, FALSE, FALSE, 0);

  g_signal_connect_swapped ( entry, "activate", G_CALLBACK(a_dialog_response_accept), GTK_DIALOG(dialog) );

  gtk_widget_show ( label );
  gtk_widget_show ( entry );

  while ( gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT )
  {
    const gchar *constname = gtk_entry_get_text ( GTK_ENTRY(entry) );
    if ( *constname == '\0' )
      a_dialog_info_msg ( parent, "Please enter a name for the track." );
    else {
      gchar *name = g_strdup ( constname );
      gint i;

      for ( i = strlen ( name ) - 1; i >= 0; i-- )
        name[i] = toupper(name[i]); /* all caps for stardandization */

      if ( g_hash_table_lookup( tracks, name ) && !a_dialog_overwrite ( parent, "The track \"%s\" exists, do you want to overwrite it?", gtk_entry_get_text ( GTK_ENTRY(entry) ) ) )
      {
        g_free ( name );
      }
      else
      {
        gtk_widget_destroy ( dialog );
        return name;
      }
    }
  }
  gtk_widget_destroy ( dialog );
  return NULL;
}

/* creates a vbox full of labels */
GtkWidget *a_dialog_create_label_vbox ( gchar **texts, int label_count )
{
  GtkWidget *vbox, *label;
  int i;
  vbox = gtk_vbox_new( TRUE, 3 );

  for ( i = 0; i < label_count; i++ )
  {
    label = gtk_label_new(NULL);
    gtk_label_set_markup ( GTK_LABEL(label), texts[i] );
    gtk_box_pack_start ( GTK_BOX(vbox), label, FALSE, TRUE, 5 );
  }
  return vbox;
}

gboolean a_dialog_overwrite ( GtkWindow *parent, const gchar *message, const gchar *extra )
{
  GtkWidget *dia;
  dia = gtk_message_dialog_new ( parent,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_QUESTION,
                                 GTK_BUTTONS_YES_NO,
                                 message, extra );

  if ( gtk_dialog_run ( GTK_DIALOG(dia) ) == GTK_RESPONSE_YES )
  {
    gtk_widget_destroy ( dia );
    return TRUE;
  }
  else
  {
    gtk_widget_destroy ( dia );
    return FALSE;
  }
}

static void zoom_spin_changed ( GtkSpinButton *spin, GtkWidget *pass_along[3] )
{
  if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(pass_along[2]) ) )
    gtk_spin_button_set_value ( 
        GTK_SPIN_BUTTON(pass_along[GTK_WIDGET(spin) == pass_along[0] ? 1 : 0]),
        gtk_spin_button_get_value ( spin ) );
}

gboolean a_dialog_custom_zoom ( GtkWindow *parent, gdouble *xmpp, gdouble *ympp )
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Zoom Factors...",
                                                  parent,
                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  NULL);
  GtkWidget *table, *label, *xlabel, *xspin, *ylabel, *yspin, *samecheck;
  GtkWidget *pass_along[3];

  table = gtk_table_new ( 4, 2, FALSE );
  gtk_box_pack_start ( GTK_BOX(GTK_DIALOG(dialog)->vbox), table, TRUE, TRUE, 0 );

  label = gtk_label_new ( "Zoom factor (in meters per pixel:" );
  xlabel = gtk_label_new ( "X (easting): ");
  ylabel = gtk_label_new ( "Y (northing): ");

  pass_along[0] = xspin = gtk_spin_button_new ( (GtkAdjustment *) gtk_adjustment_new ( *xmpp, VIK_VIEWPORT_MIN_ZOOM, VIK_VIEWPORT_MAX_ZOOM, 1, 5, 5 ), 1, 8 );
  pass_along[1] = yspin = gtk_spin_button_new ( (GtkAdjustment *) gtk_adjustment_new ( *ympp, VIK_VIEWPORT_MIN_ZOOM, VIK_VIEWPORT_MAX_ZOOM, 1, 5, 5 ), 1, 8 );

  pass_along[2] = samecheck = gtk_check_button_new_with_label ( "X and Y zoom factors must be equal" );
  /* TODO -- same factor */
  /*  samecheck = gtk_check_button_new_with_label ( "Same x/y zoom factor" ); */

  if ( *xmpp == *ympp )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON(samecheck), TRUE );

  gtk_table_attach_defaults ( GTK_TABLE(table), label, 0, 2, 0, 1 );
  gtk_table_attach_defaults ( GTK_TABLE(table), xlabel, 0, 1, 1, 2 );
  gtk_table_attach_defaults ( GTK_TABLE(table), xspin, 1, 2, 1, 2 );
  gtk_table_attach_defaults ( GTK_TABLE(table), ylabel, 0, 1, 2, 3 );
  gtk_table_attach_defaults ( GTK_TABLE(table), yspin, 1, 2, 2, 3 );
  gtk_table_attach_defaults ( GTK_TABLE(table), samecheck, 0, 2, 3, 4 );

  gtk_widget_show_all ( table );

  g_signal_connect ( G_OBJECT(xspin), "value-changed", G_CALLBACK(zoom_spin_changed), pass_along );
  g_signal_connect ( G_OBJECT(yspin), "value-changed", G_CALLBACK(zoom_spin_changed), pass_along );

  if ( gtk_dialog_run ( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
  {
    *xmpp = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(xspin) );
    *ympp = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(yspin) );
    gtk_widget_destroy ( dialog );
    return TRUE;
  }
  gtk_widget_destroy ( dialog );
  return FALSE;
}

static void split_spin_focused ( GtkSpinButton *spin, GtkWidget *pass_along[1] )
{
  gtk_toggle_button_set_active    (GTK_TOGGLE_BUTTON(pass_along[0]), 1);
}

gboolean a_dialog_time_threshold ( GtkWindow *parent, gchar *title_text, gchar *label_text, guint *thr )
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons (title_text, 
                                                  parent,
                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  NULL);
  GtkWidget *table, *t1, *t2, *t3, *t4, *spin, *label;
  GtkWidget *pass_along[1];

  table = gtk_table_new ( 4, 2, FALSE );
  gtk_box_pack_start ( GTK_BOX(GTK_DIALOG(dialog)->vbox), table, TRUE, TRUE, 0 );

  label = gtk_label_new (label_text);

  t1 = gtk_radio_button_new_with_label ( NULL, "1 min" );
  t2 = gtk_radio_button_new_with_label_from_widget ( GTK_RADIO_BUTTON(t1), "1 hour" );
  t3 = gtk_radio_button_new_with_label_from_widget ( GTK_RADIO_BUTTON(t2), "1 day" );
  t4 = gtk_radio_button_new_with_label_from_widget ( GTK_RADIO_BUTTON(t3), "Custom (in minutes):" );

  pass_along[0] = t4;

  spin = gtk_spin_button_new ( (GtkAdjustment *) gtk_adjustment_new ( *thr, 0, 65536, 1, 5, 5 ), 1, 0 );

  gtk_table_attach_defaults ( GTK_TABLE(table), label, 0, 2, 0, 1 );
  gtk_table_attach_defaults ( GTK_TABLE(table), t1, 0, 1, 1, 2 );
  gtk_table_attach_defaults ( GTK_TABLE(table), t2, 0, 1, 2, 3 );
  gtk_table_attach_defaults ( GTK_TABLE(table), t3, 0, 1, 3, 4 );
  gtk_table_attach_defaults ( GTK_TABLE(table), t4, 0, 1, 4, 5 );
  gtk_table_attach_defaults ( GTK_TABLE(table), spin, 1, 2, 4, 5 );

  gtk_widget_show_all ( table );

  g_signal_connect ( G_OBJECT(spin), "grab-focus", G_CALLBACK(split_spin_focused), pass_along );

  if ( gtk_dialog_run ( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
  {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(t1))) {
      *thr = 1;
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(t2))) {
      *thr = 60;
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(t3))) {
      *thr = 60 * 24;
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(t4))) {
      *thr = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(spin) );
    }
    gtk_widget_destroy ( dialog );
    return TRUE;
  }
  gtk_widget_destroy ( dialog );
  return FALSE;
}