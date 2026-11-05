#include <algorithm> // for std::copy, std::swap_ranges, std::transform
#include <fstream> // for std::ifstream
#include <functional> // for std::plus
#include <iomanip> // for std::setw
#include <iostream> // for std::cout, std::cin, std::cerr
#include <sstream> // for std::istringstream
#include <stdexcept> // for std::invalid_argument, std::length_error, std::out_of_range
#include <string> // for std::string

namespace {
    // Represents a 2D matrix of doubles stored in a single row-major array
    class Matrix {
        int rows_; // number of rows
        int cols_; // number of columns
        double* data_; // pointer to a 1D array of size rows_ * cols_ storing the elements in row-major order

        void allocateData(); // allocate the 1D array (defined below the class)
        void deallocateData(); // deallocate the 1D array (defined below the class)

    public:
        // Constructor: creates a rows x cols matrix initialized to 0.0
        Matrix(int rows, int cols) : rows_(rows), cols_(cols), data_(nullptr) {
            if (rows < 0 || cols < 0) { // negative dimensions are invalid
                throw std::invalid_argument("Matrix dimensions cannot be negative"); // throw an exception if the dimensions are invalid
            }
            if (static_cast<long long>(rows) * cols > 10000000LL) { // limit the size to prevent excessive memory allocation
                throw std::length_error("Matrix is too large"); // throw an exception if the matrix is too large
            }
            allocateData(); // allocate the 1D array of size rows * cols
        }

        // Copy constructor: needed so a Matrix can be passed/returned by value
        Matrix(const Matrix& other) : rows_(other.rows_), cols_(other.cols_), data_(nullptr) {
            allocateData(); // allocate the 1D array of size rows * cols
            std::copy(other.data_, other.data_ + rows_ * cols_, data_); // copy the elements from the other matrix
        }

        // Copy assignment (rule of three)
        Matrix& operator=(const Matrix& other) {
            if (this != &other) { // self-assignment check
                deallocateData(); // free the old array
                rows_ = other.rows_; // copy the number of rows
                cols_ = other.cols_; // copy the number of columns
                allocateData(); // allocate a new array of the new size
                std::copy(other.data_, other.data_ + rows_ * cols_, data_); // copy the elements from the other matrix
            }
            return *this; // return a reference to this matrix
        }

        // Destructor: deallocates the 1D array
        ~Matrix() { deallocateData(); }

        // Element access: returns a reference so the element can be read or modified
        double& operator()(int row, int col) {
            return data_[row * cols_ + col]; // convert (row, col) to a position in the 1D array
        }

        // Element access for const matrices: read-only
        const double& operator()(int row, int col) const {
            return data_[row * cols_ + col]; // convert (row, col) to a position in the 1D array
        }

        int getRows() const { return rows_; } // return the number of rows in the matrix
        int getCols() const { return cols_; } // return the number of columns in the matrix

        // Reads rows_ * cols_ values from an open stream; returns false if any read fails
        bool readValues(std::istream& in) {
            for (int i = 0; i < rows_; ++i) { // loop through each row
                for (int j = 0; j < cols_; ++j) { // loop through each column
                    if (!(in >> (*this)(i, j))) { // read a value and store it in the matrix
                        return false; // non-numeric input or end of file
                    }
                }
            }
            return true; // all values were read successfully
        }

        // Operator overloading for addition: returns a new matrix, leaving both operands unchanged
        Matrix operator+(const Matrix& other) const {
            if (rows_ != other.rows_ || cols_ != other.cols_) { // dimensions must match
                throw std::invalid_argument("Matrix dimensions must match for addition");
            }
            Matrix result(rows_, cols_); // matrix to hold the sum
            std::transform(data_, data_ + rows_ * cols_, other.data_, result.data_, std::plus<double>()); // add the elements of the two matrices one by one
            return result; // return the sum
        }

        // Operator overloading for multiplication: returns a new matrix, leaving both operands unchanged
        Matrix operator*(const Matrix& other) const {
            if (cols_ != other.rows_) { // columns of the first must equal rows of the second
                throw std::invalid_argument("Invalid matrix dimensions for multiplication");
            }
            Matrix result(rows_, other.cols_); // matrix to hold the product
            for (int i = 0; i < rows_; ++i) { // each row of the first matrix
                for (int j = 0; j < other.cols_; ++j) { // each column of the second matrix
                    double sum = 0.0; // dot product of row i and column j
                    for (int k = 0; k < cols_; ++k) {
                        sum += (*this)(i, k) * other(k, j);
                    }
                    result(i, j) = sum; // store the dot product
                }
            }
            return result; // return the product
        }

        // Sums the main diagonal by default, or the secondary diagonal if secondary is true
        double sumDiagonal(bool secondary = false) const {
            double sum = 0.0; // running total
            for (int i = 0; i < std::min(rows_, cols_); ++i) { // one element per row along the diagonal
                sum += (*this)(i, secondary ? cols_ - 1 - i : i); // the secondary diagonal runs from the top-right corner
            }
            return sum; // return the diagonal sum
        }

        // Prints an optional title line, then the matrix with aligned columns
        void print(const std::string& title = "") const {
            if (!title.empty()) std::cout << title << '\n'; // print the title if there is one
            for (int i = 0; i < rows_; ++i) { // each row
                for (int j = 0; j < cols_; ++j) std::cout << std::setw(4) << (*this)(i, j); // width of 4 keeps the columns aligned
                std::cout << '\n'; // end the row
            }
        }
    };
}

// Allocates the 1D array of size rows_ * cols_
void Matrix::allocateData() {
    data_ = new double[rows_ * cols_](); // the trailing () initializes every element to 0.0
}

// Deallocates the 1D array
void Matrix::deallocateData() {
    delete[] data_; // free the memory
    data_ = nullptr; // avoid a dangling pointer
}

// Swaps two rows (default 0 and 1). The Matrix is passed by value, so the original is not modified.
// Throws std::out_of_range on a bad index.
static Matrix swapRows(Matrix m, int r1 = 0, int r2 = 1) {
    if (r1 < 0 || r2 < 0 || r1 >= m.getRows() || r2 >= m.getRows()) { // check that both row indices are valid
        throw std::out_of_range("Row index out of range");
    }
    // rows are contiguous in memory, so one library call swaps a whole row
    std::swap_ranges(&m(r1, 0), &m(r1, 0) + m.getCols(), &m(r2, 0));
    return m; // return the copy with the swapped rows
}

// Swaps two columns (default 0 and 1). The Matrix is passed by value, so the original is not modified.
// Throws std::out_of_range on a bad index.
static Matrix swapCols(Matrix m, int c1 = 0, int c2 = 1) {
    if (c1 < 0 || c2 < 0 || c1 >= m.getCols() || c2 >= m.getCols()) { // check that both column indices are valid
        throw std::out_of_range("Column index out of range");
    }
    for (int i = 0; i < m.getRows(); ++i) { // swap the two elements in every row
        std::swap(m(i, c1), m(i, c2));
    }
    return m; // return the copy with the swapped columns
}

// Updates one element (default row 0, column 0, value 100). The Matrix is passed by value, so the original is not modified.
// Throws std::out_of_range on a bad index.
static Matrix updateElement(Matrix m, int row = 0, int col = 0, double value = 100) {
    if (row < 0 || col < 0 || row >= m.getRows() || col >= m.getCols()) { // check that the row and column are valid
        throw std::out_of_range("Row or column index out of range");
    }
    m(row, col) = value; // set the new value
    return m; // return the copy with the updated element
}

// Removes leading/trailing whitespace and surrounding quotes from a filename
std::string cleanFilename(std::string f) {
    const std::string ws = " \t\r\n"; // whitespace characters
    f.erase(0, f.find_first_not_of(ws)); // remove leading whitespace
    f.erase(f.find_last_not_of(ws) + 1); // remove trailing whitespace
    if (f.size() >= 2 && (f.front() == '"' || f.front() == '\'') && f.back() == f.front()) { // matching quotes around the name
        f = f.substr(1, f.size() - 2); // remove the quotes
    }
    return f; // return the cleaned filename
}

// Reads two N x N matrices from a file and displays their sum, product, diagonal sums, and modified copies.
// Every input is checked up front (filename, N, matrix values, index range), so none of the
// exceptions thrown by Matrix or the swap/update functions can be triggered and no try/catch is needed.
int main() {
    std::string filename; // name of the input file
    std::cout << "Enter input filename: "; // prompt for the filename
    std::getline(std::cin, filename); // read the filename
    filename = cleanFilename(filename); // trim whitespace and quotes

    std::ifstream in(filename); // open the file (an empty filename fails to open too)
    if (!in) { // check that the file opened
        std::cerr << "\nError: could not open file '" << filename << "'\n";
        return 1;
    }

    std::string firstLine; // the first line must hold only N
    std::getline(in, firstLine); // read the first line
    std::istringstream header(firstLine); // parse it as a stream
    int n = 0; // matrix size (N x N)
    if (!(header >> n) || !(header >> std::ws).eof() || n <= 0 || n > 3162) { // N must be a single positive integer; 3162 keeps n * n under the 10,000,000 element limit
        std::cerr << "\nError: first line must be a single integer N from 1 to 3162\n";
        return 1;
    }

    Matrix A(n, n), B(n, n); // the two N x N matrices
    if (!A.readValues(in) || !B.readValues(in)) { // read both matrices from the file
        std::cerr << "\nError: file has missing or non-numeric matrix values\n";
        return 1;
    }

    A.print("Matrix A:"); // display matrix A
    B.print("\nMatrix B:"); // display matrix B
    (A + B).print("\nA + B:"); // display the sum using the overloaded + operator
    (A * B).print("\nA * B:"); // display the product using the overloaded * operator

    std::cout << "\nDiagonal sums for Matrix A:\n" // display the diagonal sums of A
              << "Main diagonal sum: " << A.sumDiagonal() << "\n"
              << "Secondary diagonal sum: " << A.sumDiagonal(true) << "\n";

    if (n < 3) { // the swaps and update below use index 2, so they only run when N is at least 3
        std::cout << "\nSwaps and update skipped: matrix is smaller than 3 x 3\n";
        return 0;
    }

    swapRows(A, 0, 2).print("\nProblem 5 - Rows 0 and 2 swapped:"); // swap rows 0 and 2 on a copy of A
    swapCols(A, 0, 2).print("\nProblem 6 - Columns 0 and 2 swapped:"); // swap columns 0 and 2 on a copy of A
    updateElement(A, 1, 2, 99).print("\nProblem 7 - Updated matrix:"); // set row 1, column 2 to 99 on a copy of A
    return 0;
}