#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class Matrix {
    public:
        Matrix(int rows, int cols) : rows_(rows), cols_(cols), data_(nullptr) {
            if (rows < 0 || cols < 0) {
                throw std::invalid_argument("Matrix dimensions cannot be negative");
            }
            if (static_cast<long long>(rows) * cols > 10000000LL) {
                throw std::length_error("Matrix is too large");
            }
            allocateData();
        }

        // Copy constructor: needed so a Matrix can be passed/returned by value
        Matrix(const Matrix& other) : rows_(other.rows_), cols_(other.cols_), data_(nullptr) {
            allocateData();
        }

        ~Matrix() { deallocateData(); }

        double& operator()(int row, int col) {
            return data_[row * cols_ + col];
        }

        const double& operator()(int row, int col) const {
            return data_[row * cols_ + col];
        }

        int getRows() const { return rows_; }
        int getCols() const { return cols_; }

        void addMatrix(const Matrix& other) {
            if (rows_ != other.rows_ || cols_ != other.cols_) {
                throw std::invalid_argument("Matrix dimensions must match for addition");
            }
            for (int i = 0; i < rows_; ++i) {
                for (int j = 0; j < cols_; ++j) {
                    (*this)(i, j) += other(i, j);
                }
            }
        }

        void multiplyMatrix(const Matrix& other) {
            if (cols_ != other.rows_) {
                throw std::invalid_argument("Invalid matrix dimensions for multiplication");
            }

            Matrix result(rows_, other.cols_);
            for (int i = 0; i < rows_; ++i) {
                for (int j = 0; j < other.cols_; ++j) {
                    double sum = 0.0;
                    for (int k = 0; k < cols_; ++k) {
                        sum += (*this)(i, k) * other(k, j);
                    }
                    result(i, j) = sum;
                }
            }
            deallocateData();
            cols_ = other.cols_;
            data_ = result.data_;
            result.data_ = nullptr;
        }

        double sumDiagonal() const {
            double sum = 0.0;
            for (int i = 0; i < std::min(rows_, cols_); ++i) {
                sum += (*this)(i, i);
            }
            return sum;
        }

        void print() const {
            if (data_ == nullptr) {
                std::cout << "Matrix is empty." << std::endl;
                return;
            }
            for (int i = 0; i < rows_; ++i) {
                for (int j = 0; j < cols_; ++j) {
                    std::cout << (*this)(i, j) << " ";
                }
                std::cout << std::endl;
            }
            }

    private:
        int rows_;
        int cols_;
        double* data_;

        void allocateData();
        void deallocateData();
};

void Matrix::allocateData() {
    data_ = new double[rows_ * cols_]();
}

void Matrix::deallocateData() {
    delete[] data_;
    data_ = nullptr;
}


int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
