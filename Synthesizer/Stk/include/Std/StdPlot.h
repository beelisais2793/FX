#pragma once 

#include "gnuplot_i.h"
#include <vector>

namespace Std
{
    struct GNUPlot
    {
        gnuplot_ctrl * gc;

        GNUPlot()
        {
            gc = gnuplot_init();
        }

        ~GNUPlot()
        {
            gnuplot_close(gc);
        }

        //void gnuplot_cmd(gnuplot_ctrl *  handle, char *  cmd, ...);
        
        //void gnuplot_setstyle(gnuplot_ctrl * h, char * plot_style);
        void setstyle(char * plot_style) { gnuplot_setstyle(gc, plot_style); }

        //void gnuplot_set_xlabel(gnuplot_ctrl * h, char * label);
        void set_xlabel(char * label) { gnuplot_set_xlabel(gc, label); }
        void set_ylabel(char * label) { gnuplot_set_ylabel(gc, label); }
        void reset() { gnuplot_resetplot(gc); }

        //void gnuplot_plot_x(gnuplot_ctrl * handle, double * d, int n, char * title);
        void plot_x(std::vector<double> & d, int n, char * title) { gnuplot_plot_x(gc,(double*)d.data(),n,(char*)title); }
        void plot_xy(std::vector<double> & x, std::vector<double> & y, int n, char * title) { gnuplot_plot_xy(gc,(double*)x.data(),y.data(),n,(char*)title); }
        void plot_once(char * title, char * style, char * label_x, char* label_y, std::vector<double> & x, std::vector<double> &y, int n)
        {
            gnuplot_plot_once((char*)title,(char*)style,(char*)label_x,(char*)label_y,(double*)x.data(),(double*)y.data(),n);        
        }
        void plot_slope(double a, double b, char * title) { gnuplot_plot_slope(gc, a, b, (char*)title); }    
        void plot_equation(char * equation, char * title) { gnuplot_plot_equation(gc,equation,title); }

        void write_x_csv(char * filename, std::vector<double> & d, int n, char * title) { gnuplot_write_x_csv(filename,d.data(),n,title); }
        void write_xy_csv(char * filename, std::vector<double> & x, std::vector<double> & y, int n, char * title) { gnuplot_write_xy_csv(filename, x.data(), y.data(), n, title); }

    };

}