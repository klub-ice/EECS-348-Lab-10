class Matrix { 
    public:
        Matrix(int rows, int cols);
        ~Matrix();

        double& operator()(int row, int col);
        const double& operator()(int row, int col) const;

        int getRows() const;
        int getCols() const;

        void addMatrix(const Matrix& other);

        void multiplyMatrix(const Matrix& other);

        double sumDiagonal() const;

        void print() const;

    private:
        int rows_;
        int cols_;
        double* data_;

        void allocateData();
        void deallocateData();
};


int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
