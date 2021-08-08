#include "../include/grid_xz.h"

int grid_xz_get_vertex_count(GridXZ grid){
    return (grid.xCount + grid.zCount) * 2 + 4;
}