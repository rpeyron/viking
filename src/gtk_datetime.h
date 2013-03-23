/*
 * gtk_datetime.h - GtkDateTime GTK widget 
 * (c) 2013 - Rémi Peyronnet - LGPL
 * v1.0
 */

#ifndef GTK_DATETIME_H_
#define GTK_DATETIME_H_

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_DATETIME_TYPE            (gtk_datetime_get_type ())
#define GTK_DATETIME(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_DATETIME_TYPE, GtkDateTime))
#define GTK_DATETIME_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_DATETIME_TYPE, GtkDateTimeClass))
#define IS_GTK_DATETIME(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_DATETIME_TYPE))
#define IS_GTK_DATETIME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_DATETIME_TYPE))


typedef struct _GtkDateTime       GtkDateTime;
typedef struct _GtkDateTimeClass  GtkDateTimeClass;

struct _GtkDateTime
{
	GtkHBox hbox;
	
	GtkWidget * year;
	GtkWidget * month;
	GtkWidget * day;
	
	GtkWidget * hour;
	GtkWidget * minute;
	GtkWidget * second;
	
  /*
  GtkWidget *buttons[3][3];
	*/
};

struct _GtkDateTimeClass
{
  GtkHBoxClass parent_class;

  void (* gtk_datetime) (GtkDateTime *gdt);
};

GType 		gtk_datetime_get_type();
GtkWidget*  gtk_datetime_new();
time_t      gtk_datetime_get_timestamp(GtkDateTime *gdt);
void        gtk_datetime_set_timestamp(GtkDateTime *gdt, const time_t * timestamp);

G_END_DECLS


#endif /* GTK_DATETIME_H_ */
