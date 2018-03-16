//NETLIST Header
#ifndef NETLIST_H
#define NETLIST_H

#include <vector>

struct crs_matrix {
    std:: vector<unsigned int> row_ptr;
    std:: vector<unsigned int> col_ind;
    std::vector<unsigned int> values;
    int nnz;
    int num_rows;
    int num_cols;
    int offset;             //the number of the first row containing data
};

bool compare(const std::pair<int,int>& inp1, const std::pair<int,int>& inp2);
crs_matrix create(std::vector<std::pair<int,int> >& edgePair);

#endif




