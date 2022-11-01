#ifndef PHYZXMODEL_H
#define PHYZXMODEL_H

#include <cmath>
#include <QVector2D>
#include "rungekutta4.h"

#define PI 3.14159265359
#define G 9.8145 //not exact data, but good enough for Novosibirsk

enum COMPOUND{NONE = 0, BALL};

class Phyzxmodel
{
public:
    Phyzxmodel(bool is_enabled = true);
    void setUp();
    void updateData();
    double getKEnergy();
    double getPEnergy();
    double getPeriod();
    ~Phyzxmodel();

    //remove copy and move constructors
    Phyzxmodel(Phyzxmodel const &) = delete;
    void operator=(Phyzxmodel const &that) = delete;

    void setImp(float impulse);
    void setTht0(float t_z);
    void setRMass(double n_m);
    void setLen(double n_l);
    void setSpd(double n_s);
    void setComp(COMPOUND n_c);
    void setDamp(double n_d);
    void currentTime(double t_a);
    double getTheta();
    COMPOUND getComp();

private:
    bool is_enabled = true;
    void isReady();
    QVector2D RK4Step(QVector2D y);
    void RK4();
    QVector2D data; //[0] is theta, angle and [1] is omega, angular velocity
    //QVector<QPoint> graph;
    double rod_length{100}, rod_mass{10}, damping_factor{3000 / 100000}, m_coeff{G / 100},
    preserved_energy{}, kinetic_energy{}, swing_period{},
    natural_freq{}, current_time, omega, obj_rad{1};
    COMPOUND p_type{BALL};
    bool m_dirty = true, m_ready = false;
    double dt = 0.016;
};

#endif // PHYZXMODEL_H
