#include <iostream>
#include <fstream>
#include <sstream>
#include <Eigen/Sparse> //Note: Eigen libraries are generally available as modules in supercomputers.
using namespace std;

int main() {
    // Open the Matrix Market file
    std::ifstream file("pds-50.mtx");
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    // Read the header of the Matrix Market file
    std::string line;
    std::getline(file, line);
    if (line != "%%MatrixMarket matrix coordinate real general") {
        std::cerr << "Invalid Matrix Market file format." << std::endl;
        return 1;
    }

    //Skip the comments in the mtx file 
    while (std::getline(file, line)) {
        if (line[0] != '%')
            break;  // Skip comments
    }

    // Parse the matrix dimensions
    int num_rows, num_cols, num_nonzeros;
    std::istringstream iss(line);    
    iss >> num_rows >> num_cols >> num_nonzeros;
    cout<<"num_rows: "<<num_rows<<" num_cols: "<<num_cols<<" num_nonzeros: "<<num_nonzeros<<endl;
      // Determine the square matrix size
    int matrix_size = std::max(num_rows, num_cols);
    // Create a sparse matrix
    cout<<"Matrix size: "<<matrix_size<<endl;
    Eigen::SparseMatrix<double> square_adjacency_matrix(matrix_size, matrix_size);
    // square_adjacency_matrix.reserve(num_nonzeros);
    // square_adjacency_matrix.reserve(num_nonzeros * 2);  // Reserve space for both (row, col) and (col, row)
    square_adjacency_matrix.reserve(num_nonzeros + matrix_size);  // Reserve space for both (row, col) and (col, row)
    int max_row_index =0;
    int min_row_index = INT_MAX;
    int max_col_index =0;
    int min_col_index = INT_MAX;
    // Read the matrix entries
    for (int i = 0; i < num_nonzeros; ++i) {
        // cout<<"Iteration: "<<i<<endl;
        int row, col;
        double value;
        file >> row >> col >> value;
        max_row_index = max_row_index < row ? row:max_row_index;
        min_row_index = min_row_index > row ? row:min_row_index;
        max_col_index = max_col_index < col ? col:max_col_index;
        min_col_index = min_col_index > col ? col:min_col_index;
   
        if (row < 0 || col < 0) {
            std::cerr << "Invalid row or column index at entry " << i + 1 << "." << std::endl;
            return 1;
        }
         // Adjust the matrix size if necessary
        int max_row = std::max(row, num_rows);
        int max_col = std::max(col, num_cols);
        square_adjacency_matrix.insert(row-1, col-1) = value;  // Adjust indices to 0-based. Only subtract if the number starts from 1 as indexing       
    }
    cout<<"max_row_index: "<<max_row_index<<" min_row_index: "<<min_row_index<<endl;
    cout<<"max_col_index: "<<max_row_index<<" min_col_index: "<<min_row_index<<endl;

      // Assign values to the diagonal entries
    int diagonal_value = 1;
    for (int i = 0; i < matrix_size; ++i) {
        // cout<<"Adding "<<i<<" diagonal entry!"<<endl;
        square_adjacency_matrix.coeffRef(i, i) = diagonal_value;  // Assign the desired value to the diagonal element
        // square_adjacency_matrix.insert(i, i) = diagonal_value;  // Assign the desired value to the diagonal element
    }
  // Generate CSR representation
    square_adjacency_matrix.makeCompressed();
    // Close the file
    file.close();
    // return 0;
    // Print the matrix size
    std::cout << "Matrix size: " << square_adjacency_matrix.rows()
              << " x " << square_adjacency_matrix.cols() << std::endl;

    // Print the number of non-zeros
    std::cout << "Number of non-zeros: " << square_adjacency_matrix.nonZeros() << std::endl;

  // Get the compressed CSR matrix data
const int* outer_indices = square_adjacency_matrix.outerIndexPtr();
const int* inner_indices = square_adjacency_matrix.innerIndexPtr();
const double* values = square_adjacency_matrix.valuePtr();
int nnz = square_adjacency_matrix.nonZeros();

// Print the compressed CSR matrix data
cout<<"Number of vertices in square matrix: "<<square_adjacency_matrix.rows()<<endl;
for (int row = 0; row < square_adjacency_matrix.rows(); ++row) {
    int start = outer_indices[row];
    int end = outer_indices[row + 1];
    for (int k = start; k < end; ++k) {
        int col = inner_indices[k];
        double value = values[k];
        // if (row == square_adjacency_matrix.rows() -1) 
        {
            std::cout << "Row: " << row << ", Column: " << col << ", Value: " << value << std::endl;
        }
    }
}

    return 0;
}