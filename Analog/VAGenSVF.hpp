// http://dafx.de/paper-archive/2020/proceedings/papers/DAFx2020_paper_70.pdf
#pragma once
#include <Eigen/Dense>
#include <cmath>

/**
 * Generalized State-Variable Filter structure,
 * as defined by Werner and McClellan (https://dafx2020.mdw.ac.at/proceedings/papers/DAFx2020_paper_70.pdf)
 * 
 * I've added some nonlinear elements of my own design.
 */
class GeneralSVF {
public:
    GeneralSVF();

    void reset();
    void calcCoefs(float r, float k, float wc);
    void setDrive (float newDrive);

    inline float process(float x) {
        Eigen::Matrix<float, 4, 1> v = A_tilde * v_n1 + B_tilde * x;
        float y = (C_tilde * v_n1)(0, 0) + D_tilde(0, 0) * x;
        nonlinearity(v);
        v_n1 = v;
        return y * makeup;
    }

    inline void nonlinearity(Eigen::Matrix<float, 4, 1>& v) {
        v(0,0) = std::tanh(v(0,0) * drive) * invDrive;
        v(2,0) = std::tanh(v(2,0) * drive) * invDrive;
        v(3,0) = std::tanh(v(3,0) * drive) * invDrive;
    }

private:
    Eigen::Matrix<float, 4, 4> A;
    Eigen::Matrix<float, 4, 1> B;
    Eigen::Matrix<float, 1, 4> C;

    Eigen::Matrix<float, 4, 4> A_tilde;
    Eigen::Matrix<float, 4, 1> B_tilde;
    Eigen::Matrix<float, 1, 4> C_tilde;
    Eigen::Matrix<float, 1, 1> D_tilde;

    float g;
    Eigen::Matrix<float, 4, 1> v_n1;

    float drive = 1.0f;
    float invDrive = 1.0f;
    float makeup = 1.0f;
};



GeneralSVF::GeneralSVF() {
    A << 0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
         0.0f,-1.0f, 0.0f, 1.0f,
         0.0f, 0.0f,-1.0f, 0.0f;

    B << 1.0f, 0.0f, 0.0f, 0.0f;
    C << 0.0f, 0.0f, 0.0f,-1.0f;
}

void GeneralSVF::reset() {
    v_n1 = Eigen::Matrix<float, 4, 1>::Zero();
}

void GeneralSVF::setDrive (float newDrive) {
    drive = newDrive;
    invDrive = 1.0f / drive;
    makeup = std::max(1.0f, (float) std::pow(drive, 0.75f));
}

void GeneralSVF::calcCoefs(float r, float k, float wc) {
    // cook parameters
    g = std::tan(wc);
    A(0, 0) = -2.0f * r;
    A(0, 3) = 4.0f * k * r * r;
    A(2, 2) = A(0, 0);

    // cook discrete state-space matrices
    Eigen::Matrix<float, 4, 4> I = Eigen::Matrix<float, 4, 4>::Identity();
    Eigen::Matrix<float, 4, 4> H = g * (I - g * A).inverse();
    A_tilde = 2 * H * A + I;
    B_tilde = 2 * H * B;
    C_tilde = C * (H * A + I);
    D_tilde = C * B;
}