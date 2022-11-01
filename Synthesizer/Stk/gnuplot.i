%module plot
%{
extern "C" {
#include "gnuplot_i.h"
}
using namespace std;
%}
%include <stdint.i>
%include <std_vector.i>

using namespace std;
%template(float_vector) vector<float>;
%template(double_vector) vector<double>;


%inline %{

struct Plot {
    gnuplot_ctrl * gc;

    
    Plot()
    {
        gc = gnuplot_init();
    }

    ~Plot()
    {
        gnuplot_close(gc);
    }
};

Plot plot;

void setstyle(char * plot_style) { gnuplot_setstyle(plot.gc, plot_style); }

//void gnuplot_set_xlabel(gnuplot_ctrl * h, char * label);
void set_xlabel(char * label) { gnuplot_set_xlabel(plot.gc, label); }
void set_ylabel(char * label) { gnuplot_set_ylabel(plot.gc, label); }

void reset() { gnuplot_resetplot(plot.gc); }

//void gnuplot_plot_x(gnuplot_ctrl * handle, double * d, int n, char * title);
void plot_x(const vector<double> & d, int n, const char * title) { gnuplot_plot_x(plot.gc,d.data(),n,title); }
void plot_xy(const vector<double> & x, const vector<double> & y, int n, const char * title) { gnuplot_plot_xy(plot.gc,x.data(),y.data(),n,title); }
void plot_once(const char * title, const char * style, const char * label_x, const char* label_y, const vector<double> & x, const vector<double> &y, int n)
{
    gnuplot_plot_once(title,style,label_x,label_y,x.data(),y.data(),n);        
}
void plot_slope(double a, double b, const char * title) { gnuplot_plot_slope(plot.gc, a, b, title); }    
void plot_equation(const char * equation, const char * title) { gnuplot_plot_equation(plot.gc,equation,title); }

void write_x_csv(const char * filename, const vector<double> & d, int n, const char * title) { gnuplot_write_x_csv(filename,d.data(),n,title); }
void write_xy_csv(const char * filename, const vector<double> & x, const vector<double> & y, int n, const char * title) { gnuplot_write_xy_csv(filename, x.data(), y.data(), n, title); }
%}
