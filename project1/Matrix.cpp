#include "Gauss.h"
#include <iostream>
#include <gtk/gtk.h>

using std::cout;

#define POINT_COUNT 7
double points[POINT_COUNT][2];
int ptCount = 0;

static void destroy(GtkWidget *widget, gpointer data) {
  gtk_main_quit();
}

static GdkPixmap *pixmap = NULL;

static void reset(GtkWidget *widget) {
  gdk_draw_rectangle(pixmap, widget->style->white_gc, TRUE, 0, 0,
                     widget->allocation.width, widget->allocation.height);
  gtk_widget_queue_draw_area(widget, 0, 0,
                             widget->allocation.width,
                             widget->allocation.height);
}

static gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event) {
  if (pixmap)
    g_object_unref(pixmap);
  pixmap = gdk_pixmap_new (widget->window, widget->allocation.width,
                            widget->allocation.height, -1);
  gdk_draw_rectangle(pixmap, widget->style->white_gc, TRUE, 0, 0,
                     widget->allocation.width, widget->allocation.height);

  return true;
}

static gboolean expose_event(GtkWidget *widget, GdkEventExpose *event) {
  gdk_draw_drawable(widget->window,
                    widget->style->fg_gc[gtk_widget_get_state (widget)], pixmap,
                    event->area.x, event->area.y,
                    event->area.x, event->area.y,
                    event->area.width, event->area.height);

  return FALSE;
}

static void draw_point(GtkWidget *widget, double x, double y) {
  gdk_draw_rectangle(pixmap, widget->style->black_gc, true, x - 2, y - 2,4,4);
  gtk_widget_queue_draw_area(widget, x - 2, y - 2, 4, 4);
}

double bezier(double t, double control[4]) {
  return      (1 - t) * (1 - t) * (1 - t) * control[0] +
          3 * (1 - t) * (1 - t) * t       * control[1] + 
          3 * (1 - t) * t       * t       * control[2] + 
              t       * t       * t       * control[3];
}

static void draw_bezier(GtkWidget *widget, GdkGC *gc,
                        double xs[4], double ys[4]) {
  for (double t = 0; t <= 1; t += 1.0 / (1 << 10)) {
      double x = bezier(t, xs);
      double y = bezier(t, ys);
      gdk_draw_point(pixmap, gc, x, y);
  }

  for (int i = 0; i < 4; i++) {
    gdk_draw_rectangle(pixmap, widget->style->black_gc, true, xs[i] - 2, ys[i] - 2,4 ,4 );
  }
}

static void draw_interps(GtkWidget *widget, double pts[POINT_COUNT][2]) {
  GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state (widget)];
  const int N = POINT_COUNT - 1;
  double dxs[N + 3];
  double dys[N + 3];
  double *dx = dxs + 1;
  double *dy = dys + 1;

  bezier_points<POINT_COUNT>(pts, dxs, dys, NATURAL);

  // Line segments between control points
  GdkColor line_color = {0xffddccaa, 0xffff, 0xdddd, 0xcccc};
  gdk_gc_set_rgb_fg_color(gc, &line_color);
  for (int i = 1; i < N + 1; i++) {
    gdk_draw_line(pixmap, gc, dxs[i], dys[i], dxs[i + 1], dys[i + 1]);
  }
  for (int i = 1; i < N + 2; i++) {
    gdk_draw_rectangle(pixmap, gc, true, dxs[i] - 2, dys[i] - 2,4,4);
  }

  // First and last bezier curves
  double xc1[4] = {pts[0][0], dx[0], dx[0] / 2.0 + dx[1] / 2.0, pts[1][0]};
  double xcn[4] = {pts[N-1][0], dx[N-1] / 2.0 + dx[N] / 2.0, dx[N], pts[N][0]};
  double yc1[4] = {pts[0][1], dy[0], dy[0] / 2.0 + dy[1] / 2.0, pts[1][1]};
  double ycn[4] = {pts[N-1][1], dy[N-1] / 2.0 + dy[N] / 2.0, dy[N], pts[N][1]};

  GdkColor color = {0xccddffaa, 0xcccc, 0xdddd, 0xffff};
  gdk_gc_set_rgb_fg_color(gc, &color);
  draw_bezier(widget, gc, xc1, yc1);
  draw_bezier(widget, gc, xcn, ycn);
  GdkColor restColor = {0xccddffaa, 0x2222, 0xbbbb, 0x3333};
  gdk_gc_set_rgb_fg_color(gc, &restColor);
  // The rest of the curves
  for (int i = 2; i < N; i++) {
    double xc[4] = {pts[i-1][0], 2.0 * dx[i-1] / 3.0 + dx[i] / 3.0,
                    dx[i-1] / 3.0 + 2.0 * dx[i] / 3.0, pts[i][0]};

    double yc[4] = {pts[i-1][1], 2.0 * dy[i-1] / 3.0 + dy[i] / 3.0,
                    dy[i-1] / 3.0 + 2.0 * dy[i] / 3.0, pts[i][1]};
    draw_bezier(widget, gc, xc, yc);

  } 

  gtk_widget_queue_draw_area(widget, 0, 0,
                             widget->allocation.width,
                             widget->allocation.height);
}

static gboolean clicked(GtkWidget *widget, GdkEventMotion *event) {
    if (ptCount < POINT_COUNT) {
    int x, y;
    x = event->x;
    y = event->y;
    draw_point(widget, x, y);
    points[ptCount][0] = x;
    points[ptCount][1] = y;
    ptCount++;
    if (ptCount == POINT_COUNT) {
      draw_interps(widget, points);
    }
  } else  {
    ptCount = 0;
    reset(widget);
  }
  return true;
}

int main(int argc, char *argv[]) {

  // GTK stuff.
  GtkWidget *window, *drawing_area, *vbox;
  gtk_init(&argc, &argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name(window, "Bezier Demo");
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_widget_show(vbox);

  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(GTK_WIDGET(drawing_area), 800, 600);
  gtk_box_pack_start(GTK_BOX(vbox), drawing_area, true, true, 0);
  gtk_widget_show(drawing_area);


  // event signals
  g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
  g_signal_connect(drawing_area, "button_press_event",
                   G_CALLBACK(clicked), NULL) ;
  g_signal_connect(drawing_area, "expose_event",
                   G_CALLBACK(expose_event), NULL);
  g_signal_connect(drawing_area, "configure-event",
                   G_CALLBACK(configure_event), NULL);
  gtk_widget_set_events(drawing_area,
                        GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

  gtk_widget_show(window);
                                        
  gtk_main();

  return 0;
}
