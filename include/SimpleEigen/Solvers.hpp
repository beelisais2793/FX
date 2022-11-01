#pragma once

#include "Eigen.h"
#include "Eigen/LU"
#include "Eigen/LLT"
#include "Eigen/LDLT"

namespace SimpleEigen {
    template<typename T>
    using EigenMatrix = Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;
    template<typename T>
    using Eigen2DArray = Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;

    template<typename T>
    struct LU {

    };

    template<typename T>
    SimpleEigen::RowVector<T> partial_piv_lu_solve(const SimpleEigen::Matrix<T> &m, const SimpleEigen::RowVector<T> &m)
    {        
        Eigen::PartialPivLU<Eigen::Ref<EigenMatrix>> lu(m.matrix);
        return SimpleEigen::RowVector<T>(lu.solve(b))
    }

    template<typename T>
    struct LLT {

    };
    template<typename T>
    SimpleEigen::Matrix<T> llt_matrixL(const SimpleEigen::Matrix<T> &m)
    {        
        Eigen::LLT<Eigen::Ref<EigenMatrix>> llt(m.matrix);
        return Matrix<T>(llt.matrixL());
    }

    template<typename T>
    SimpleEigen::Matrix<T> llt_matrixU(const SimpleEigen::Matrix<T> &m)
    {        
        Eigen::LLT<Eigen::Ref<EigenMatrix>> llt(m.matrix);
        return Matrix<T>(llt.matrixU());
    }

    template<typename T>
    struct LDLT {

    };
    template<typename T>
    SimpleEigen::Matrix<T> ldlt_matrixL(const SimpleEigen::Matrix<T> &m)
    {        
        Eigen::LDLT<Eigen::Ref<EigenMatrix>> ldlt(m.matrix);
        return Matrix<T>(llt.matrixL());
    }

    template<typename T>
    SimpleEigen::Matrix<T> ldlt_matrixU(const SimpleEigen::Matrix<T> &m)
    {        
        Eigen::LDLT<Eigen::Ref<EigenMatrix>> LLT(m.matrix);
        return Matrix<T>(llt.matrixU());
    }

    template<typename T>
    SimpleEigen::Matrix<T> ldlt_matrixLDLT(const SimpleEigen::Matrix<T> &m)
    {        
        Eigen::LDLT<Eigen::Ref<EigenMatrix>> LLT(m.matrix);
        return Matrix<T>(llt.matrixLDLT());
    }

}