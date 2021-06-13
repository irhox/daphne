/*
 * Copyright 2021 The DAPHNE Consortium
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SRC_RUNTIME_LOCAL_KERNELS_MATMUL_H
#define SRC_RUNTIME_LOCAL_KERNELS_MATMUL_H

#include <runtime/local/datastructures/DataObjectFactory.h>
#include <runtime/local/datastructures/DenseMatrix.h>

#include <cblas.h>

#include <cassert>
#include <cstddef>

// ****************************************************************************
// Struct for partial template specialization
// ****************************************************************************

template<class DTRes, class DTLhs, class DTRhs>
struct MatMul {
    static void apply(DTRes *& res, const DTLhs * lhs, const DTRhs * rhs) = delete;
};

// ****************************************************************************
// Convenience function
// ****************************************************************************

template<class DTRes, class DTLhs, class DTRhs>
void matMul(DTRes *& res, const DTLhs * lhs, const DTRhs * rhs) {
    MatMul<DTRes, DTLhs, DTRhs>::apply(res, lhs, rhs);
}

// ****************************************************************************
// (Partial) template specializations for different data/value types
// ****************************************************************************

// ----------------------------------------------------------------------------
// DenseMatrix <- DenseMatrix, DenseMatrix
// ----------------------------------------------------------------------------

template<>
struct MatMul<DenseMatrix<float>, DenseMatrix<float>, DenseMatrix<float>> {
    static void apply(DenseMatrix<float> *& res, const DenseMatrix<float> * lhs, const DenseMatrix<float> * rhs) {
        const size_t nr1 = lhs->getNumRows();
        const size_t nc1 = lhs->getNumCols();
        const size_t nr2 = rhs->getNumRows();
        const size_t nc2 = rhs->getNumCols();
        assert((nc1 == nr2) && "#cols of lhs and #rows of rhs must be the same");

        if(res == nullptr)
            res = DataObjectFactory::create<DenseMatrix<float>>(nr1, nc2, false);

        if(nr1 == 1 && nc2 == 1) // Vector-Vector
            cblas_sdot(nc1, lhs->getValues(), rhs->getRowSkip(), rhs->getValues(), 1);
        else if(nc2 == 1)        // Matrix-Vector
            cblas_sgemv(CblasRowMajor, CblasNoTrans, nr1, nc1, 1, lhs->getValues(),
                lhs->getRowSkip(), rhs->getValues(), rhs->getRowSkip(), 0,
                res->getValues(), res->getRowSkip());
        else                     // Matrix-Matrix
            cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nr1, nc2, nc1,
                1, lhs->getValues(), lhs->getRowSkip(), rhs->getValues(),
                rhs->getRowSkip(), 0, res->getValues(), res->getRowSkip());
    }
};

template<>
struct MatMul<DenseMatrix<double>, DenseMatrix<double>, DenseMatrix<double>> {
    static void apply(DenseMatrix<double> *& res, const DenseMatrix<double> * lhs, const DenseMatrix<double> * rhs) {
        const size_t nr1 = lhs->getNumRows();
        const size_t nc1 = lhs->getNumCols();
        const size_t nr2 = rhs->getNumRows();
        const size_t nc2 = rhs->getNumCols();
        assert((nc1 == nr2) && "#cols of lhs and #rows of rhs must be the same");

        if(res == nullptr)
            res = DataObjectFactory::create<DenseMatrix<double>>(nr1, nc2, false);

        if(nr1 == 1 && nc2 == 1) // Vector-Vector
            cblas_ddot(nc1, lhs->getValues(), rhs->getRowSkip(), rhs->getValues(), 1);
        else if(nc2 == 1)        // Matrix-Vector
            cblas_dgemv(CblasRowMajor, CblasNoTrans, nr1, nc1, 1, lhs->getValues(),
                lhs->getRowSkip(), rhs->getValues(), rhs->getRowSkip(), 0,
                res->getValues(), res->getRowSkip());
        else                     // Matrix-Matrix
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, nr1, nc2, nc1,
                1, lhs->getValues(), lhs->getRowSkip(), rhs->getValues(),
                rhs->getRowSkip(), 0, res->getValues(), res->getRowSkip());
    }
};

#endif //SRC_RUNTIME_LOCAL_KERNELS_MATMUL_H
