#ifdef RUNGEKUTTA4_H
#include "rungekutta4.h"
#include <cmath>

RungeKutta4::RungeKutta4() {}

QVector2D RungeKutta4::RK4Step(double current_time){
    QVector2D rk[4], *result = new QVector2D;
    rk[0] = step(*result, current_time) * dt;
    rk[1] = step((*result) + 0.5 * rk[0], current_time + 0.5 * dt) * dt;
    rk[2] = step((*result) + 0.5 * rk[1], current_time + 0.5 * dt) * dt;
    rk[3] = step((*result) + rk[2], current_time + dt) * dt;
    *result += (rk[0] + rk[1] * 2 + rk[2] * 2 + rk[3]) / 6.0;
    return *result;
}

QVector2D RungeKutta4::step(QVector2D y, double t){
    QVector2D temp;
    temp.setX(y[0]);
    temp.setY(-damping * y[1] - std::sin(y[0]));
    return temp;
}

void RungeKutta4::setUp(double time_step, double damping_constant){
    dt = time_step;
    damping = damping_constant;
}
#endif
