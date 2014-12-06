/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "interface.h"
#include "support.h"

#include "network.h"
#include "beat.h"

int
main(int argc, char *argv[])
{
    GtkWidget      *iwc;

#ifdef ENABLE_NLS
    bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
    textdomain(PACKAGE);
#endif

    gnome_init("iwc", VERSION, argc, argv);

    beat_init();

    /*
     * The following code was added by Glade to create one of each component
     * (except popup menus), just so that you see something after building
     * the project. Delete any components that you don't want shown initially.
     */
    iwc = create_iwc();
    gtk_widget_show(iwc);

    gtk_main();
    return 0;
}
