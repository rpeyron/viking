/*
 * gtk_datetime.c - GtkDateTime GTK widget 
 * (c) 2013 - Rémi Peyronnet - LGPL
 * v1.0
 *
 * History :
 *  v1.0  (21/03/2013) - Initial release
 */

#include "gtk_datetime.h"
#include <gtk/gtksignal.h>
#include <time.h>

enum {
  GTK_DATETIME_CHANGED_SIGNAL,
  LAST_SIGNAL
};

static void gtk_datetime_class_init    (GtkDateTimeClass *klass);
static void gtk_datetime_init          (GtkDateTime      *gdt);
static void gtk_datetime_changed(GtkWidget * widget, GtkDateTime * gdt);

static guint gtk_datetime_signals[LAST_SIGNAL] = { 0 };

GType 
gtk_datetime_get_type ()
{
  static GType gdt_type = 0;

  if (!gdt_type)
	{
	  const GTypeInfo gdt_info = 
	  {
			sizeof (GtkDateTimeClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc) gtk_datetime_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (GtkDateTime),
			0,
			(GInstanceInitFunc) gtk_datetime_init,
	  };

	  gdt_type = g_type_register_static (GTK_TYPE_HBOX, "GtkDateTime", &gdt_info, 0);
	}

  return gdt_type;
}

static void
gtk_datetime_class_init (GtkDateTimeClass *klass)
{
  gtk_datetime_signals[GTK_DATETIME_CHANGED_SIGNAL] = g_signal_new ("gtk_datetime_changed",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET (GtkDateTimeClass, gtk_datetime),
			NULL, 
			NULL,                
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 0);
}



static void
gtk_datetime_init (GtkDateTime *gdt)
{

#define GTK_CONTAINER_ADD_AND_SHOW(container, widget)  {   \
		GtkWidget * gww = widget;	\
		gtk_container_add(GTK_CONTAINER(container), gww); \
		gtk_widget_show(gww); \
	}

#define GTK_CONTAINER_NEW_SPINNER_AND_SHOW(container, spinnername, min, max)   \
		spinnername = gtk_spin_button_new_with_range(min, max, 1);	\
		gtk_container_add(GTK_CONTAINER(container), spinnername); \
		g_signal_connect(spinnername, "value-changed", G_CALLBACK(gtk_datetime_changed), (gpointer) gdt); \
		gtk_widget_show(spinnername); 

	GTK_CONTAINER_NEW_SPINNER_AND_SHOW(gdt, gdt->day, 1, 31)
	GTK_CONTAINER_ADD_AND_SHOW(gdt, gtk_label_new(" / "));
	GTK_CONTAINER_NEW_SPINNER_AND_SHOW(gdt, gdt->month, 1, 31)
	GTK_CONTAINER_ADD_AND_SHOW(gdt, gtk_label_new(" / "));
	GTK_CONTAINER_NEW_SPINNER_AND_SHOW(gdt, gdt->year, 1970, 2038)
	GTK_CONTAINER_ADD_AND_SHOW(gdt, gtk_label_new("   "));
	GTK_CONTAINER_NEW_SPINNER_AND_SHOW(gdt, gdt->hour, 0, 23)
	GTK_CONTAINER_ADD_AND_SHOW(gdt, gtk_label_new(" : "));
	GTK_CONTAINER_NEW_SPINNER_AND_SHOW(gdt, gdt->minute, 0, 59)
	GTK_CONTAINER_ADD_AND_SHOW(gdt, gtk_label_new(" : "));
	GTK_CONTAINER_NEW_SPINNER_AND_SHOW(gdt, gdt->second, 0, 59)

#undef GTK_CONTAINER_ADD_AND_SHOW
#undef GTK_CONTAINER_NEW_SPINNER_AND_SHOW

}

GtkWidget*
gtk_datetime_new ()
{
  return GTK_WIDGET (g_object_new (gtk_datetime_get_type (), NULL));
}

void
gtk_datetime_set_timestamp(GtkDateTime *gdt, const time_t * timestamp)
{
	struct tm * timecomp;
	timecomp = localtime(timestamp);

#define SPIN_SET_VALUE_WITHOUT_SIGNAL(spin, value) \
	g_signal_handlers_block_matched  (spin, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, gdt); \
	gtk_spin_button_set_value ( spin, value ); \
	g_signal_handlers_unblock_matched  (spin, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, gdt);	
	
	if (timecomp != NULL)
	{
		SPIN_SET_VALUE_WITHOUT_SIGNAL ( GTK_SPIN_BUTTON ( gdt->day ), timecomp->tm_mday);
		SPIN_SET_VALUE_WITHOUT_SIGNAL ( GTK_SPIN_BUTTON ( gdt->month ), timecomp->tm_mon + 1);
		SPIN_SET_VALUE_WITHOUT_SIGNAL ( GTK_SPIN_BUTTON ( gdt->year ), timecomp->tm_year + 1900);
		SPIN_SET_VALUE_WITHOUT_SIGNAL ( GTK_SPIN_BUTTON ( gdt->hour ), timecomp->tm_hour);
		SPIN_SET_VALUE_WITHOUT_SIGNAL ( GTK_SPIN_BUTTON ( gdt->minute ), timecomp->tm_min);
		SPIN_SET_VALUE_WITHOUT_SIGNAL ( GTK_SPIN_BUTTON ( gdt->second ), timecomp->tm_sec);
	}
	
#undef SPIN_SET_VALUE_WITHOUT_SIGNAL
	
}

time_t 
gtk_datetime_get_timestamp(GtkDateTime *gdt)
{
	struct tm timecomp;
	timecomp.tm_mday = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( gdt->day ));
	timecomp.tm_mon = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( gdt->month )) - 1;
	timecomp.tm_year = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( gdt->year )) - 1900;
	timecomp.tm_hour = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( gdt->hour ));
	timecomp.tm_min = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( gdt->minute ));
	timecomp.tm_sec = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( gdt->second ));
	return mktime(&timecomp);
}

static void gtk_datetime_changed(GtkWidget * widget, GtkDateTime * gdt)
{
	  g_signal_emit (gdt, gtk_datetime_signals[GTK_DATETIME_CHANGED_SIGNAL], 0);
}

