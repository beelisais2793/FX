#include<iostream>
void rotate(double* x, double* y, double angle);
double getAngle(double x, double y);
void rotaByTheTha(double* x, double* y, int position, bool ccw);
double thetha[14] = {
45.00000000,
26.56505118,
14.03624347,
7.12501635,
3.57633437,
1.78991061,
0.89517371,
0.44761417,
0.22381050,
0.11190568,
0.05595289,
0.02797645,
0.01398823,
0.00699411,
};

double cosine[14] = {
0.70710678,
0.89442719,
0.97014250,
0.99227788,
0.99805258,
0.99951208,
0.99987795,
0.99996948,
0.99999237,
0.99999809,
0.99999952,
0.99999988,
0.99999997,
0.99999999,
};

int main()
{
    //set point
    double x = 5;
    double y = 12;
    //get current degree
    std::cout << "angle: "<< getAngle(x, y) << std::endl;
    //totate
    rotate(&x, &y, 10);
    std::cout << "angle: " << getAngle(x, y) << std::endl;
}

void rotate(double* x, double* y, double angle) {
    if (angle < 99.87597172 && angle > -99.87597172 && angle != 0.0) {
        bool ccw = true;
        if (angle < 0) {
            ccw = false;
        }
        double turned = 0;
        //cordic 14 iter
        for (size_t i = 0; i < 14; i++)
        {
            rotaByTheTha(x, y, i, ccw);
            turned += thetha[i] * (ccw ? 1 : -1);
            if (turned == angle) {
                break;
            }
            else if (turned > angle) {
                ccw = false;
            }else if (turned < angle) {
                ccw = true;
            }
        }
    }
}
double getAngle(double x, double y) {
    if (x >= 0 && y >= 0) {

        bool ccw = false;
        double angle = 0;

        for (int i = 0; i < 14; i++)
        {
            rotaByTheTha(&x, &y, i, ccw);
            angle += thetha[i] * (ccw ? -1 : 1);
            //if (y == 0) {
            //    angle += thetha[i] * -1;
            //}
            //else {
            //    angle += thetha[i] * 1;
            //}
            if (y == 0) {
                break;
            }
            else if (y > 0) {
                ccw = false;
            }
            else if (y < 0) {
                ccw = true;
            }
        }
        return angle;
    }
    else {
        return 0.0;
    }

}
void rotaByTheTha(double* x, double* y, int position, bool ccw) {
    double x_old = *x;
   
    if (ccw) {
        *x = *x * cosine[position] - *y * cosine[position] * pow(2, -position);
        *y = x_old * cosine[position] * pow(2, -position) + *y * cosine[position];
    }
    else {
        *x = *x * cosine[position] + *y * cosine[position] * pow(2, -position);
        *y = -x_old * cosine[position] * pow(2, -position) + *y * cosine[position];
    }
}