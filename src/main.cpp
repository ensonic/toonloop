#include <GL/glew.h>
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gtk/gtkgl.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

static void on_realize(GtkWidget *widget, gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
  
    GLUquadricObj *qobj;
    static GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};
    static GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
  
    /*** OpenGL BEGIN ***/
    if (! gdk_gl_drawable_gl_begin(gldrawable, glcontext))
        return;
  
    qobj = gluNewQuadric ();
    gluQuadricDrawStyle (qobj, GLU_FILL);
    glNewList (1, GL_COMPILE);
    gluSphere(qobj, 1.0, 20, 20);
    glEndList();
  
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glEnable (GL_DEPTH_TEST);
  
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glClearDepth (1.0);
  
    glViewport (0, 0,
                widget->allocation.width, widget->allocation.height);
  
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (40.0, 1.0, 1.0, 10.0);
  
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt (0.0, 0.0, 3.0,
               0.0, 0.0, 0.0,
               0.0, 1.0, 0.0);
    glTranslatef (0.0, 0.0, -3.0);
  
    gdk_gl_drawable_gl_end (gldrawable);
    /*** OpenGL END ***/
}

static gboolean on_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
    /*** OpenGL BEGIN ***/
    if (! gdk_gl_drawable_gl_begin(gldrawable, glcontext))
    {
        return FALSE;
    }
    glViewport (0, 0, widget->allocation.width, widget->allocation.height);
    gdk_gl_drawable_gl_end (gldrawable);
    /*** OpenGL END ***/
    return TRUE;
}

static gboolean on_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
    /*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
    {
      return FALSE;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glCallList(1);

    if (gdk_gl_drawable_is_double_buffered(gldrawable))
    {
        gdk_gl_drawable_swap_buffers(gldrawable);
    } else {
        glFlush();
    }
    gdk_gl_drawable_gl_end(gldrawable);
    /*** OpenGL END ***/
    return TRUE;
}

static void print_gl_config_attrib(GdkGLConfig *glconfig, const gchar *attrib_str, int attrib, gboolean is_boolean)
{
    int value;
    g_print ("%s = ", attrib_str);
    if (gdk_gl_config_get_attrib(glconfig, attrib, &value))
    {
        if (is_boolean)
        {
            g_print("%s\n", value == TRUE ? "TRUE" : "FALSE");
        } else {
            g_print("%d\n", value);
        } 
    } else {
        g_print("*** Cannot get %s attribute value\n", attrib_str);
    }
}

static void examine_gl_config_attrib(GdkGLConfig *glconfig)
{
  g_print("\nOpenGL visual configurations :\n\n");
  g_print("gdk_gl_config_is_rgba (glconfig) = %s\n", gdk_gl_config_is_rgba (glconfig) ? "TRUE" : "FALSE");
  g_print("gdk_gl_config_is_double_buffered (glconfig) = %s\n", gdk_gl_config_is_double_buffered (glconfig) ? "TRUE" : "FALSE");
  g_print("gdk_gl_config_is_stereo (glconfig) = %s\n", gdk_gl_config_is_stereo (glconfig) ? "TRUE" : "FALSE");
  g_print("gdk_gl_config_has_alpha (glconfig) = %s\n", gdk_gl_config_has_alpha (glconfig) ? "TRUE" : "FALSE");
  g_print("gdk_gl_config_has_depth_buffer (glconfig) = %s\n", gdk_gl_config_has_depth_buffer (glconfig) ? "TRUE" : "FALSE");
  g_print("gdk_gl_config_has_stencil_buffer (glconfig) = %s\n", gdk_gl_config_has_stencil_buffer (glconfig) ? "TRUE" : "FALSE");
  g_print("gdk_gl_config_has_accum_buffer (glconfig) = %s\n",
  gdk_gl_config_has_accum_buffer (glconfig) ? "TRUE" : "FALSE");
  g_print ("\n");
  print_gl_config_attrib(glconfig, "GDK_GL_USE_GL", GDK_GL_USE_GL, TRUE);
  print_gl_config_attrib(glconfig, "GDK_GL_BUFFER_SIZE", GDK_GL_BUFFER_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_LEVEL", GDK_GL_LEVEL, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_RGBA", GDK_GL_RGBA, TRUE);
  print_gl_config_attrib(glconfig, "GDK_GL_DOUBLEBUFFER", GDK_GL_DOUBLEBUFFER, TRUE);
  print_gl_config_attrib(glconfig, "GDK_GL_STEREO", GDK_GL_STEREO, TRUE);
  print_gl_config_attrib(glconfig, "GDK_GL_AUX_BUFFERS", GDK_GL_AUX_BUFFERS, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_RED_SIZE", GDK_GL_RED_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_GREEN_SIZE", GDK_GL_GREEN_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_BLUE_SIZE", GDK_GL_BLUE_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_ALPHA_SIZE", GDK_GL_ALPHA_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_DEPTH_SIZE", GDK_GL_DEPTH_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_STENCIL_SIZE", GDK_GL_STENCIL_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_ACCUM_RED_SIZE", GDK_GL_ACCUM_RED_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_ACCUM_GREEN_SIZE", GDK_GL_ACCUM_GREEN_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_ACCUM_BLUE_SIZE", GDK_GL_ACCUM_BLUE_SIZE, FALSE);
  print_gl_config_attrib(glconfig, "GDK_GL_ACCUM_ALPHA_SIZE", GDK_GL_ACCUM_ALPHA_SIZE, FALSE);
  g_print ("\n");
}

static void on_delete_event(GtkWidget* widget, GdkEvent* event, void* data)
{
    g_print("Close\n");
    gtk_main_quit();
}

gint main(gint argc, gchar* argv[])
{
    gtk_init(&argc, &argv);
    // Init GTK GL:
    gtk_gl_init(&argc, &argv);
    gint major; 
    gint minor;
    gdk_gl_query_version(&major, &minor);
    g_print("\nOpenGL extension version - %d.%d\n", major, minor);
    /* Try double-buffered visual */

    GdkGLConfig* glconfig;
    // the line above does not work in C++ if the cast is not there.
    glconfig = gdk_gl_config_new_by_mode(static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGB | GDK_GL_MODE_DOUBLE));
    if (glconfig == NULL)
    {
        g_print("*** Cannot find the double-buffered visual.\n");
        g_print("*** Trying single-buffered visual.\n");
        /* Try single-buffered visual */
        glconfig = gdk_gl_config_new_by_mode(static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGB));
        if (glconfig == NULL)
        {
            g_print ("*** No appropriate OpenGL-capable visual found.\n");
            exit(1);
        }
    }
    examine_gl_config_attrib(glconfig);

    // Main GTK window
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window, 640, 480);
    gtk_window_set_title(GTK_WINDOW (window), "Toonloop 1.3 experimental");
    GdkGeometry geometry;
    geometry.min_width = 1;
    geometry.min_height = 1;
    geometry.max_width = -1;
    geometry.max_height = -1;
    gtk_window_set_geometry_hints(GTK_WINDOW(window), window, &geometry, GDK_HINT_MIN_SIZE);
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(on_delete_event), NULL);

    //area where the video is drawn
    GtkWidget* drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);

    //avoid flickering when resizing or obscuring the main window
    //gtk_widget_realize(drawing_area);
    //gdk_window_set_back_pixmap(drawing_area->window, NULL, FALSE);
    //gtk_widget_set_app_paintable(drawing_area, TRUE);
    //gtk_widget_set_double_buffered(drawing_area, FALSE);

    /* Set OpenGL-capability to the widget. */
    gtk_widget_set_gl_capability(drawing_area, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);
  
    g_signal_connect_after(G_OBJECT(drawing_area), "realize", G_CALLBACK(on_realize), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "configure_event", G_CALLBACK(on_configure_event), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "expose_event", G_CALLBACK(on_expose_event), NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

