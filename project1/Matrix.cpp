#include "Gauss.h"
#include <iostream>
#include <gtk/gtk.h>

using std::cout;

#define POINT_COUNT 7
double points[2][POINT_COUNT];
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

static void draw_interps(GtkWidget *widget, double pts[2][POINT_COUNT]) {
  GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state (widget)];

  const int N = POINT_COUNT - 1;
  // Calculate matrices
  Matrix<N - 1,N - 1> mat;
  double blahx[N+3];
  double blahy[N+3];
  // Forgive me
  double *dx = blahx + 1;
  double *dy = blahy + 1;
  dx[-1] = pts[0][0];
  dx[N+1] = pts[0][N];
  dy[-1] = pts[1][0];
  dy[N+1] = pts[1][N];
  
  double xs[N-1];
  double ys[N-1];

  
  xs[0] = 6 * pts[0][1] - pts[0][0];
  ys[0] = 6 * pts[1][1] - pts[1][0];
  xs[N-2] = 6 * pts[0][N-1] - pts[0][N];
  ys[N-2] = 6 * pts[1][N-1] - pts[1][N];
  for (int i = 1; i < N-2; i++) {
    xs[i] = 6 * pts[0][i+1];
    ys[i] = 6 * pts[1][i+1];
    mat[i][i] = 4;
    mat[i-1][i] = 1;
    mat[i][i-1] = 1;
  }
  mat[0][0] = 4;
  mat[N-2][N-2] = 4;
  mat[N-3][N-2] = 1;
  mat[N-2][N-3] = 1;
  mat.luSolve(dx + 1, xs);
  mat.luSolve(dy + 1, ys);

  // Initial conditions
  dx[0] = 2.0 * pts[0][0] / 3.0 + dx[1] / 3.0;
  dx[N] = dx[N - 1] / 3.0 + 2.0 * pts[0][N] / 3.0;
  dy[0] = 2.0 * pts[1][0] / 3.0 + dy[1] / 3.0;
  dy[N] = dy[N - 1] / 3.0 + 2.0 * pts[1][N] / 3.0;
  

  // Line segments between control points
  //
  GdkColor line_color = {0xffddccaa, 0xffff, 0xdddd, 0xcccc};
  gdk_gc_set_rgb_fg_color(gc, &line_color);
  for (int i = 1; i < N + 1; i++) {
    gdk_draw_line(pixmap, gc, blahx[i], blahy[i], blahx[i + 1], blahy[i + 1]);
  }
  for (int i = 1; i < N + 2; i++) {
    gdk_draw_rectangle(pixmap, gc, true, blahx[i] - 2, blahy[i] - 2,4,4);
  }

  // First and last bezier curves
  double xc1[4] = {pts[0][0], dx[0], dx[0] / 2.0 + dx[1] / 2.0, pts[0][1]};
  double xcn[4] = {pts[0][N-1], dx[N-1] / 2.0 + dx[N] / 2.0, dx[N], pts[0][N]};
  double yc1[4] = {pts[1][0], dy[0], dy[0] / 2.0 + dy[1] / 2.0, pts[1][1]};
  double ycn[4] = {pts[1][N-1], dy[N-1] / 2.0 + dy[N] / 2.0, dy[N], pts[1][N]};

  GdkColor color = {0xccddffaa, 0xcccc, 0xdddd, 0xffff};
  gdk_gc_set_rgb_fg_color(gc, &color);
  draw_bezier(widget, gc, xc1, yc1);
  draw_bezier(widget, gc, xcn, ycn);
  GdkColor restColor = {0xccddffaa, 0x2222, 0xbbbb, 0x3333};
  gdk_gc_set_rgb_fg_color(gc, &restColor);
  // The rest of the curves
  for (int i = 2; i < N; i++) {
    double xc[4] = {pts[0][i-1], 2.0 * dx[i-1] / 3.0 + dx[i] / 3.0,
                    dx[i-1] / 3.0 + 2.0 * dx[i] / 3.0, pts[0][i]};

    double yc[4] = {pts[1][i-1], 2.0 * dy[i-1] / 3.0 + dy[i] / 3.0,
                    dy[i-1] / 3.0 + 2.0 * dy[i] / 3.0, pts[1][i]};
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
    points[0][ptCount] = x;
    points[1][ptCount] = y;
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

  /*double tr[3][3] = {{3.5,1,0},{1,4,1},{0,1,3.5}};
  Matrix<3,3> mat(tr);
  double target[3] = {20, 30, 40};
  double res[3];
  double answer[3] = {4.6429, 3.75,10.3571};
  mat.luSolve(res, target);
  (mat.id() * res).printMatrix();
  (mat * res).printMatrix();
  (mat * answer).printMatrix();
  */
  /*
    cout << "Got back:\n";
  (id * res).printMatrix();
  cout << "\nResult:\n";
  Matrix<4,1> m2 = mat * res;
  m2.printMatrix();

  Matrix<10,10> tridiag(tridiagonal);

  cout << "\nNow to test tridiagonal solving. Here's the GaussElim solution\n";
  tridiag.printMatrix();
  double resTen[10];
  double targetTen[10] = {10,9,8,7,6,5,4,3,2,1};

  tridiag.gaussElim();
  tridiag.backSubstitute(resTen, targetTen);
  cout << "Got back:\n";
  (tridiag.id() * resTen).printMatrix();
  cout << "\nResult:\n";
  (tridiag * resTen).printMatrix();


  // And using the tridiagonal solution...
  cout << "\nAnd the tridiagonal solution\n";
  tridiag = Matrix<10,10>(tridiagonal);
  tridiag.luSolve(resTen, targetTen);
  cout << "Got back:\n";
  (tridiag.id() * resTen).printMatrix();
  cout << "\nResult:\n";
  (tridiag * resTen).printMatrix();
  */

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
