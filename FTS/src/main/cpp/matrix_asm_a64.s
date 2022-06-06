    .globl   fast_matrix_mul
    .p2align 2
    .type    fast_matrix_mul,%function

//
// fast_matrix_mul:
// Calculate 4x4 (matrix 0) * (matrix 1) and store to result 4x4 matrix.
//  matrix 0, matrix 1 and result pointers can be the same,
//  ie. my_matrix = my_matrix * my_matrix is possible.
//
// x0 = pointer to 4x4 result matrix, single precision floats, column major order
// x1 = pointer to 4x4 matrix 0, single precision floats, column major order
// x2 = pointer to 4x4 matrix 1, single precision floats, column major order
//

fast_matrix_mul:
    LD1  {V0.4S, V1.4S, V2.4S, V3.4S}, [x1]   // load all 16 elements of matrix 0 into V0-V3, four elements per register
    LD1  {V4.4S, V5.4S, V6.4S, V7.4S}, [x2]   // load all 16 elements of matrix 1 into V4-V7, four elements per register

    FMUL V8.4S, V0.4S, V4.S[0]                // rslt col0  = (mat0 col0) * (mat1 col0 elt0)
    FMUL V9.4S, V0.4S, V5.S[0]                // rslt col1  = (mat0 col0) * (mat1 col1 elt0)
    FMUL V10.4S, V0.4S, V6.S[0]               // rslt col2  = (mat0 col0) * (mat1 col2 elt0)
    FMUL V11.4S, V0.4S, V7.S[0]               // rslt col3  = (mat0 col0) * (mat1 col3 elt0)

    FMLA V8.4S, V1.4S, V4.S[1]                // rslt col0 += (mat0 col1) * (mat1 col0 elt1)
    FMLA V9.4S, V1.4S, V5.S[1]                // rslt col1 += (mat0 col1) * (mat1 col1 elt1)
    FMLA V10.4S, V1.4S, V6.S[1]               // rslt col2 += (mat0 col1) * (mat1 col2 elt1)
    FMLA V11.4S, V1.4S, V7.S[1]               // rslt col3 += (mat0 col1) * (mat1 col3 elt1)

    FMLA V8.4S, V2.4S,  V4.S[2]               // rslt col0 += (mat0 col2) * (mat1 col0 elt2)
    FMLA V9.4S, V2.4S,  V5.S[2]               // rslt col1 += (mat0 col2) * (mat1 col1 elt2)
    FMLA V10.4S, V2.4S, V6.S[2]               // rslt col2 += (mat0 col2) * (mat1 col2 elt2)
    FMLA V11.4S, V2.4S, V7.S[2]               // rslt col3 += (mat0 col2) * (mat1 col2 elt2)

    FMLA V8.4S, V3.4S, V4.S[3]                // rslt col0 += (mat0 col3) * (mat1 col0 elt3)
    FMLA V9.4S, V3.4S, V5.S[3]                // rslt col1 += (mat0 col3) * (mat1 col1 elt3)
    FMLA V10.4S, V3.4S, V6.S[3]               // rslt col2 += (mat0 col3) * (mat1 col2 elt3)
    FMLA V11.4S, V3.4S, V7.S[3]               // rslt col3 += (mat0 col3) * (mat1 col3 elt3)

    ST1  {V8.4S, V9.4S, V10.4S, V11.4S}, [x0] // store all 16 elements of result
    RET                                       // return to caller