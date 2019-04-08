#ifndef __GC_STITCH_DEFS_H__
#define __GC_STITCH_DEFS_H__

const int   Connectivity = 4;
const int   neighbourDX[Connectivity]   = { 0,     -1,     -1,      1};
const int   neighbourDY[Connectivity]   = {-1,      0,     -1,     -1};
const float neighbourDist[Connectivity] = { 1,      1, SQRT_2, SQRT_2};

//const int   Connectivity = 2;
//const int   neighbourDX[Connectivity]   = {  0,    -1};
//const int   neighbourDY[Connectivity]   = { -1,     0};
//const float neighbourDist[Connectivity] = {  1,     1};

const float UNDEFINED_DISPARITY = FLT_MAX;

const int HORIZONTAL_EDGE = 0;
const int VERTICAL_EDGE   = 1;
const int NEG_DIAG_EDGE   = 2;
const int POS_DIAG_EDGE   = 3;

const float DATA_COST_INF = 1e+10F;
const float EDGE_COST_INF = 1e+05F;

#endif
