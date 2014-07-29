#include <math.h>
#include <boost/make_shared.hpp>
#include <util/Logger.h>
#include "MarchingCubes.h"

logger::LogChannel marchingcubeslog("marchingcubeslog", "[MarchingCubes] ");

template <typename Volume>
const unsigned int MarchingCubes<Volume>::_edgeTable[256] = {
	0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
};

static const unsigned int None = -1;

template <typename Volume>
const unsigned int MarchingCubes<Volume>::_triTable[256][16] = {
	{None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{0, 8, 3, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{0, 1, 9, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{1, 8, 3, 9, 8, 1, None, None, None, None, None, None, None, None, None, None},
	{1, 2, 10, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{0, 8, 3, 1, 2, 10, None, None, None, None, None, None, None, None, None, None},
	{9, 2, 10, 0, 2, 9, None, None, None, None, None, None, None, None, None, None},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, None, None, None, None, None, None, None},
	{3, 11, 2, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{0, 11, 2, 8, 11, 0, None, None, None, None, None, None, None, None, None, None},
	{1, 9, 0, 2, 3, 11, None, None, None, None, None, None, None, None, None, None},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, None, None, None, None, None, None, None},
	{3, 10, 1, 11, 10, 3, None, None, None, None, None, None, None, None, None, None},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, None, None, None, None, None, None, None},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, None, None, None, None, None, None, None},
	{9, 8, 10, 10, 8, 11, None, None, None, None, None, None, None, None, None, None},
	{4, 7, 8, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{4, 3, 0, 7, 3, 4, None, None, None, None, None, None, None, None, None, None},
	{0, 1, 9, 8, 4, 7, None, None, None, None, None, None, None, None, None, None},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, None, None, None, None, None, None, None},
	{1, 2, 10, 8, 4, 7, None, None, None, None, None, None, None, None, None, None},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, None, None, None, None, None, None, None},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, None, None, None, None, None, None, None},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, None, None, None, None},
	{8, 4, 7, 3, 11, 2, None, None, None, None, None, None, None, None, None, None},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, None, None, None, None, None, None, None},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, None, None, None, None, None, None, None},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, None, None, None, None},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, None, None, None, None, None, None, None},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, None, None, None, None},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, None, None, None, None},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, None, None, None, None, None, None, None},
	{9, 5, 4, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{9, 5, 4, 0, 8, 3, None, None, None, None, None, None, None, None, None, None},
	{0, 5, 4, 1, 5, 0, None, None, None, None, None, None, None, None, None, None},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, None, None, None, None, None, None, None},
	{1, 2, 10, 9, 5, 4, None, None, None, None, None, None, None, None, None, None},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, None, None, None, None, None, None, None},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, None, None, None, None, None, None, None},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, None, None, None, None},
	{9, 5, 4, 2, 3, 11, None, None, None, None, None, None, None, None, None, None},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, None, None, None, None, None, None, None},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, None, None, None, None, None, None, None},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, None, None, None, None},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, None, None, None, None, None, None, None},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, None, None, None, None},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, None, None, None, None},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, None, None, None, None, None, None, None},
	{9, 7, 8, 5, 7, 9, None, None, None, None, None, None, None, None, None, None},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, None, None, None, None, None, None, None},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, None, None, None, None, None, None, None},
	{1, 5, 3, 3, 5, 7, None, None, None, None, None, None, None, None, None, None},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, None, None, None, None, None, None, None},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, None, None, None, None},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, None, None, None, None},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, None, None, None, None, None, None, None},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, None, None, None, None, None, None, None},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, None, None, None, None},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, None, None, None, None},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, None, None, None, None, None, None, None},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, None, None, None, None},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, None},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, None},
	{11, 10, 5, 7, 11, 5, None, None, None, None, None, None, None, None, None, None},
	{10, 6, 5, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{0, 8, 3, 5, 10, 6, None, None, None, None, None, None, None, None, None, None},
	{9, 0, 1, 5, 10, 6, None, None, None, None, None, None, None, None, None, None},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, None, None, None, None, None, None, None},
	{1, 6, 5, 2, 6, 1, None, None, None, None, None, None, None, None, None, None},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, None, None, None, None, None, None, None},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, None, None, None, None, None, None, None},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, None, None, None, None},
	{2, 3, 11, 10, 6, 5, None, None, None, None, None, None, None, None, None, None},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, None, None, None, None, None, None, None},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, None, None, None, None, None, None, None},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, None, None, None, None},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, None, None, None, None, None, None, None},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, None, None, None, None},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, None, None, None, None},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, None, None, None, None, None, None, None},
	{5, 10, 6, 4, 7, 8, None, None, None, None, None, None, None, None, None, None},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, None, None, None, None, None, None, None},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, None, None, None, None, None, None, None},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, None, None, None, None},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, None, None, None, None, None, None, None},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, None, None, None, None},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, None, None, None, None},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, None},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, None, None, None, None, None, None, None},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, None, None, None, None},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, None, None, None, None},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, None},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, None, None, None, None},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, None},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, None},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, None, None, None, None},
	{10, 4, 9, 6, 4, 10, None, None, None, None, None, None, None, None, None, None},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, None, None, None, None, None, None, None},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, None, None, None, None, None, None, None},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, None, None, None, None},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, None, None, None, None, None, None, None},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, None, None, None, None},
	{0, 2, 4, 4, 2, 6, None, None, None, None, None, None, None, None, None, None},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, None, None, None, None, None, None, None},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, None, None, None, None, None, None, None},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, None, None, None, None},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, None, None, None, None},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, None},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, None, None, None, None},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, None},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, None, None, None, None, None, None, None},
	{6, 4, 8, 11, 6, 8, None, None, None, None, None, None, None, None, None, None},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, None, None, None, None, None, None, None},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, None, None, None, None},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, None, None, None, None},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, None, None, None, None, None, None, None},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, None, None, None, None},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, None},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, None, None, None, None, None, None, None},
	{7, 3, 2, 6, 7, 2, None, None, None, None, None, None, None, None, None, None},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, None, None, None, None},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, None},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, None},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, None, None, None, None},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, None},
	{0, 9, 1, 11, 6, 7, None, None, None, None, None, None, None, None, None, None},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, None, None, None, None},
	{7, 11, 6, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{7, 6, 11, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{3, 0, 8, 11, 7, 6, None, None, None, None, None, None, None, None, None, None},
	{0, 1, 9, 11, 7, 6, None, None, None, None, None, None, None, None, None, None},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, None, None, None, None, None, None, None},
	{10, 1, 2, 6, 11, 7, None, None, None, None, None, None, None, None, None, None},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, None, None, None, None, None, None, None},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, None, None, None, None, None, None, None},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, None, None, None, None},
	{7, 2, 3, 6, 2, 7, None, None, None, None, None, None, None, None, None, None},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, None, None, None, None, None, None, None},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, None, None, None, None, None, None, None},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, None, None, None, None},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, None, None, None, None, None, None, None},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, None, None, None, None},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, None, None, None, None},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, None, None, None, None, None, None, None},
	{6, 8, 4, 11, 8, 6, None, None, None, None, None, None, None, None, None, None},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, None, None, None, None, None, None, None},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, None, None, None, None, None, None, None},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, None, None, None, None},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, None, None, None, None, None, None, None},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, None, None, None, None},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, None, None, None, None},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, None},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, None, None, None, None, None, None, None},
	{0, 4, 2, 4, 6, 2, None, None, None, None, None, None, None, None, None, None},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, None, None, None, None},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, None, None, None, None, None, None, None},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, None, None, None, None},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, None, None, None, None, None, None, None},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, None},
	{10, 9, 4, 6, 10, 4, None, None, None, None, None, None, None, None, None, None},
	{4, 9, 5, 7, 6, 11, None, None, None, None, None, None, None, None, None, None},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, None, None, None, None, None, None, None},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, None, None, None, None, None, None, None},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, None, None, None, None},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, None, None, None, None, None, None, None},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, None, None, None, None},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, None, None, None, None},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, None},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, None, None, None, None, None, None, None},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, None, None, None, None},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, None, None, None, None},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, None},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, None, None, None, None},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, None},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, None},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, None, None, None, None},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, None, None, None, None, None, None, None},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, None, None, None, None},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, None, None, None, None},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, None, None, None, None, None, None, None},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, None, None, None, None},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, None},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, None},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, None, None, None, None},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, None, None, None, None},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, None, None, None, None, None, None, None},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, None},
	{1, 5, 6, 2, 1, 6, None, None, None, None, None, None, None, None, None, None},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, None},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, None, None, None, None},
	{0, 3, 8, 5, 6, 10, None, None, None, None, None, None, None, None, None, None},
	{10, 5, 6, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{11, 5, 10, 7, 5, 11, None, None, None, None, None, None, None, None, None, None},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, None, None, None, None, None, None, None},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, None, None, None, None, None, None, None},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, None, None, None, None},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, None, None, None, None, None, None, None},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, None, None, None, None},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, None, None, None, None},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, None},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, None, None, None, None, None, None, None},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, None, None, None, None},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, None, None, None, None},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, None},
	{1, 3, 5, 3, 7, 5, None, None, None, None, None, None, None, None, None, None},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, None, None, None, None, None, None, None},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, None, None, None, None, None, None, None},
	{9, 8, 7, 5, 9, 7, None, None, None, None, None, None, None, None, None, None},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, None, None, None, None, None, None, None},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, None, None, None, None},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, None, None, None, None},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, None},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, None, None, None, None},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, None},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, None},
	{9, 4, 5, 2, 11, 3, None, None, None, None, None, None, None, None, None, None},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, None, None, None, None},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, None, None, None, None, None, None, None},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, None},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, None, None, None, None},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, None, None, None, None, None, None, None},
	{0, 4, 5, 1, 0, 5, None, None, None, None, None, None, None, None, None, None},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, None, None, None, None},
	{9, 4, 5, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, None, None, None, None, None, None, None},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, None, None, None, None},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, None, None, None, None},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, None},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, None, None, None, None},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, None},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, None, None, None, None, None, None, None},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, None, None, None, None},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, None, None, None, None},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, None},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, None},
	{1, 10, 2, 8, 7, 4, None, None, None, None, None, None, None, None, None, None},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, None, None, None, None, None, None, None},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, None, None, None, None},
	{4, 0, 3, 7, 4, 3, None, None, None, None, None, None, None, None, None, None},
	{4, 8, 7, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{9, 10, 8, 10, 11, 8, None, None, None, None, None, None, None, None, None, None},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, None, None, None, None, None, None, None},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, None, None, None, None, None, None, None},
	{3, 1, 10, 11, 3, 10, None, None, None, None, None, None, None, None, None, None},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, None, None, None, None, None, None, None},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, None, None, None, None},
	{0, 2, 11, 8, 0, 11, None, None, None, None, None, None, None, None, None, None},
	{3, 2, 11, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, None, None, None, None, None, None, None},
	{9, 10, 2, 0, 9, 2, None, None, None, None, None, None, None, None, None, None},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, None, None, None, None},
	{1, 10, 2, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{1, 3, 8, 9, 1, 8, None, None, None, None, None, None, None, None, None, None},
	{0, 9, 1, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{0, 3, 8, None, None, None, None, None, None, None, None, None, None, None, None, None},
	{None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None}
};

template <typename Volume>
MarchingCubes<Volume>::MarchingCubes()
{
	_cellSizeX = 0;
	_cellSizeY = 0;
	_cellSizeZ = 0;
	_nCellsX = 0;
	_nCellsY = 0;
	_nCellsZ = 0;
	_nTriangles = 0;
	_nNormals = 0;
	_nVertices = 0;
	_tIsoLevel = 0;
	_bValidSurface = false;
}

template <typename Volume>
MarchingCubes<Volume>::~MarchingCubes()
{
	deleteSurface();
}

template <typename Volume>
boost::shared_ptr<Mesh>
MarchingCubes<Volume>::generateSurface(
		const Volume& volume,
		value_type isoLevel,
		float cellSizeX,
		float cellSizeY,
		float cellSizeZ)
{
	if (_bValidSurface)
		deleteSurface();

	_mesh = boost::make_shared<Mesh>();

	float width  = volume.width();
	float height = volume.height();
	float depth  = volume.depth();

	_tIsoLevel = isoLevel;
	_nCellsX = ceil(width /cellSizeX) - 1;
	_nCellsY = ceil(height/cellSizeY) - 1;
	_nCellsZ = ceil(depth /cellSizeZ) - 1;
	_cellSizeX = cellSizeX;
	_cellSizeY = cellSizeY;
	_cellSizeZ = cellSizeZ;

	LOG_DEBUG(marchingcubeslog) << "number of cells: " << _nCellsX << ", " << _nCellsY << ", " << _nCellsZ << std::endl;

	// Generate isosurface.
	for (unsigned int z = 0; z < _nCellsZ; z++)
		for (unsigned int y = 0; y < _nCellsY; y++)
			for (unsigned int x = 0; x < _nCellsX; x++) {
				// Calculate table lookup index from those
				// vertices which are below the isolevel.
				unsigned int tableIndex = 0;
				if (getValue(volume, x, y, z) < _tIsoLevel)
					tableIndex |= 1;
				if (getValue(volume, x, y+1, z) < _tIsoLevel)
					tableIndex |= 2;
				if (getValue(volume, x+1, y+1, z) < _tIsoLevel)
					tableIndex |= 4;
				if (getValue(volume, x+1, y, z) < _tIsoLevel)
					tableIndex |= 8;
				if (getValue(volume, x, y, z+1) < _tIsoLevel)
					tableIndex |= 16;
				if (getValue(volume, x, y+1, z+1) < _tIsoLevel)
					tableIndex |= 32;
				if (getValue(volume, x+1, y+1, z+1) < _tIsoLevel)
					tableIndex |= 64;
				if (getValue(volume, x+1, y, z+1) < _tIsoLevel)
					tableIndex |= 128;

				// Now create a triangulation of the isosurface in this
				// cell.
				if (_edgeTable[tableIndex] != 0) {
					if (_edgeTable[tableIndex] & 8) {
						Point3dId pt = CalculateIntersection(volume, x, y, z, 3);
						unsigned int id = GetEdgeId(x, y, z, 3);
						_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
					}
					if (_edgeTable[tableIndex] & 1) {
						Point3dId pt = CalculateIntersection(volume, x, y, z, 0);
						unsigned int id = GetEdgeId(x, y, z, 0);
						_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
					}
					if (_edgeTable[tableIndex] & 256) {
						Point3dId pt = CalculateIntersection(volume, x, y, z, 8);
						unsigned int id = GetEdgeId(x, y, z, 8);
						_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
					}
					
					if (x == _nCellsX - 1) {
						if (_edgeTable[tableIndex] & 4) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 2);
							unsigned int id = GetEdgeId(x, y, z, 2);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
						if (_edgeTable[tableIndex] & 2048) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 11);
							unsigned int id = GetEdgeId(x, y, z, 11);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					}
					if (y == _nCellsY - 1) {
						if (_edgeTable[tableIndex] & 2) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 1);
							unsigned int id = GetEdgeId(x, y, z, 1);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
						if (_edgeTable[tableIndex] & 512) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 9);
							unsigned int id = GetEdgeId(x, y, z, 9);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					}
					if (z == _nCellsZ - 1) {
						if (_edgeTable[tableIndex] & 16) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 4);
							unsigned int id = GetEdgeId(x, y, z, 4);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
						if (_edgeTable[tableIndex] & 128) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 7);
							unsigned int id = GetEdgeId(x, y, z, 7);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					}
					if ((x==_nCellsX - 1) && (y==_nCellsY - 1))
						if (_edgeTable[tableIndex] & 1024) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 10);
							unsigned int id = GetEdgeId(x, y, z, 10);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					if ((x==_nCellsX - 1) && (z==_nCellsZ - 1))
						if (_edgeTable[tableIndex] & 64) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 6);
							unsigned int id = GetEdgeId(x, y, z, 6);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					if ((y==_nCellsY - 1) && (z==_nCellsZ - 1))
						if (_edgeTable[tableIndex] & 32) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 5);
							unsigned int id = GetEdgeId(x, y, z, 5);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					
					for (unsigned int i = 0; _triTable[tableIndex][i] != None; i += 3) {
						TriangleId triangle;
						unsigned int pointId0, pointId1, pointId2;
						pointId0 = GetEdgeId(x, y, z, _triTable[tableIndex][i]);
						pointId1 = GetEdgeId(x, y, z, _triTable[tableIndex][i+1]);
						pointId2 = GetEdgeId(x, y, z, _triTable[tableIndex][i+2]);
						triangle.pointId[0] = pointId0;
						triangle.pointId[1] = pointId1;
						triangle.pointId[2] = pointId2;
						_trivecTriangles.push_back(triangle);
					}
				}
			}
	
	RenameVerticesAndTriangles();
	CalculateNormals();
	_bValidSurface = true;

	return _mesh;
}

template <typename Volume>
bool MarchingCubes<Volume>::isSurfaceValid()
{
	return _bValidSurface;
}

template <typename Volume>
void MarchingCubes<Volume>::deleteSurface()
{
	_cellSizeX = 0;
	_cellSizeY = 0;
	_cellSizeZ = 0;
	_nCellsX = 0;
	_nCellsY = 0;
	_nCellsZ = 0;
	_nTriangles = 0;
	_nNormals = 0;
	_nVertices = 0;
	_tIsoLevel = 0;
	_bValidSurface = false;
}

template <typename Volume>
int MarchingCubes<Volume>::getVolumeLengths(float& fVolLengthX, float& fVolLengthY, float& fVolLengthZ)
{
	if (isSurfaceValid()) {
		fVolLengthX = _cellSizeX*_nCellsX;
		fVolLengthY = _cellSizeY*_nCellsY;
		fVolLengthZ = _cellSizeZ*_nCellsZ;
		return 1;
	}
	else
		return -1;
}

template <typename Volume>
unsigned int MarchingCubes<Volume>::GetEdgeId(unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo)
{
	switch (nEdgeNo) {
	case 0:
		return GetVertexId(nX, nY, nZ) + 1;
	case 1:
		return GetVertexId(nX, nY + 1, nZ);
	case 2:
		return GetVertexId(nX + 1, nY, nZ) + 1;
	case 3:
		return GetVertexId(nX, nY, nZ);
	case 4:
		return GetVertexId(nX, nY, nZ + 1) + 1;
	case 5:
		return GetVertexId(nX, nY + 1, nZ + 1);
	case 6:
		return GetVertexId(nX + 1, nY, nZ + 1) + 1;
	case 7:
		return GetVertexId(nX, nY, nZ + 1);
	case 8:
		return GetVertexId(nX, nY, nZ) + 2;
	case 9:
		return GetVertexId(nX, nY + 1, nZ) + 2;
	case 10:
		return GetVertexId(nX + 1, nY + 1, nZ) + 2;
	case 11:
		return GetVertexId(nX + 1, nY, nZ) + 2;
	default:
		// Invalid edge no.
		return None;
	}
}

template <typename Volume>
unsigned int MarchingCubes<Volume>::GetVertexId(unsigned int nX, unsigned int nY, unsigned int nZ)
{
	return 3*(nZ*(_nCellsY + 1)*(_nCellsX + 1) + nY*(_nCellsX + 1) + nX);
}

template <typename Volume>
Point3dId MarchingCubes<Volume>::CalculateIntersection(const Volume& volume, unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo)
{
	float x1, y1, z1, x2, y2, z2;
	unsigned int v1x = nX, v1y = nY, v1z = nZ;
	unsigned int v2x = nX, v2y = nY, v2z = nZ;
	
	switch (nEdgeNo)
	{
	case 0:
		v2y += 1;
		break;
	case 1:
		v1y += 1;
		v2x += 1;
		v2y += 1;
		break;
	case 2:
		v1x += 1;
		v1y += 1;
		v2x += 1;
		break;
	case 3:
		v1x += 1;
		break;
	case 4:
		v1z += 1;
		v2y += 1;
		v2z += 1;
		break;
	case 5:
		v1y += 1;
		v1z += 1;
		v2x += 1;
		v2y += 1;
		v2z += 1;
		break;
	case 6:
		v1x += 1;
		v1y += 1;
		v1z += 1;
		v2x += 1;
		v2z += 1;
		break;
	case 7:
		v1x += 1;
		v1z += 1;
		v2z += 1;
		break;
	case 8:
		v2z += 1;
		break;
	case 9:
		v1y += 1;
		v2y += 1;
		v2z += 1;
		break;
	case 10:
		v1x += 1;
		v1y += 1;
		v2x += 1;
		v2y += 1;
		v2z += 1;
		break;
	case 11:
		v1x += 1;
		v2x += 1;
		v2z += 1;
		break;
	}

	x1 = v1x*_cellSizeX;
	y1 = v1y*_cellSizeY;
	z1 = v1z*_cellSizeZ;
	x2 = v2x*_cellSizeX;
	y2 = v2y*_cellSizeY;
	z2 = v2z*_cellSizeZ;

	value_type val1 = getValue(volume, v1x, v1y, v1z);
	value_type val2 = getValue(volume, v2x, v2y, v2z);
	Point3dId intersection = Interpolate(x1, y1, z1, x2, y2, z2, val1, val2);
	
	return intersection;
}

template <typename Volume>
Point3dId MarchingCubes<Volume>::Interpolate(float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, value_type tVal1, value_type tVal2)
{
	Point3dId interpolation;
	float mu;

	mu = float((_tIsoLevel - tVal1))/(tVal2 - tVal1);
	interpolation.x = fX1 + mu*(fX2 - fX1);
	interpolation.y = fY1 + mu*(fY2 - fY1);
	interpolation.z = fZ1 + mu*(fZ2 - fZ1);

	return interpolation;
}

template <typename Volume>
void MarchingCubes<Volume>::RenameVerticesAndTriangles()
{
	unsigned int nextId = 0;
	Id2Point3dId::iterator mapIterator = _i2pt3idVertices.begin();
	TriangleVector::iterator vecIterator = _trivecTriangles.begin();

	// Rename vertices.
	while (mapIterator != _i2pt3idVertices.end()) {
		(*mapIterator).second.newId = nextId;
		nextId++;
		mapIterator++;
	}

	// Now rename triangles.
	while (vecIterator != _trivecTriangles.end()) {
		for (unsigned int i = 0; i < 3; i++) {
			unsigned int newId = _i2pt3idVertices[(*vecIterator).pointId[i]].newId;
			(*vecIterator).pointId[i] = newId;
		}
		vecIterator++;
	}

	// Copy all the vertices and triangles into two arrays so that they
	// can be efficiently accessed.
	// Copy vertices.
	mapIterator = _i2pt3idVertices.begin();
	_nVertices = _i2pt3idVertices.size();
	_mesh->setNumVertices(_nVertices);

	LOG_DEBUG(marchingcubeslog) << "created a mesh with " << _nVertices << " vertices" << std::endl;

	for (unsigned int i = 0; i < _nVertices; i++, mapIterator++)
		_mesh->setVertex(i, mapIterator->second);

	// Copy vertex indices which make triangles.
	vecIterator = _trivecTriangles.begin();
	_nTriangles = _trivecTriangles.size();
	_mesh->setNumTriangles(_nTriangles);

	for (unsigned int i = 0; i < _nTriangles; i++, vecIterator++)
		_mesh->setTriangle(i,(*vecIterator).pointId[0], (*vecIterator).pointId[1], (*vecIterator).pointId[2]);

	_i2pt3idVertices.clear();
	_trivecTriangles.clear();
}

template <typename Volume>
void MarchingCubes<Volume>::CalculateNormals()
{
	_nNormals = _nVertices;
	
	// Set all normals to 0.
	for (unsigned int i = 0; i < _nNormals; i++)
		_mesh->setNormal(i, Vector3d(0, 0, 0));

	// Calculate normals.
	for (unsigned int i = 0; i < _nTriangles; i++) {
		Vector3d vec1, vec2, normal;
		unsigned int id0, id1, id2;
		id0 = _mesh->getTriangle(i).v0;
		id1 = _mesh->getTriangle(i).v1;
		id2 = _mesh->getTriangle(i).v2;
		vec1.x = _mesh->getVertex(id1).x - _mesh->getVertex(id0).x;
		vec1.y = _mesh->getVertex(id1).y - _mesh->getVertex(id0).y;
		vec1.z = _mesh->getVertex(id1).z - _mesh->getVertex(id0).z;
		vec2.x = _mesh->getVertex(id2).x - _mesh->getVertex(id0).x;
		vec2.y = _mesh->getVertex(id2).y - _mesh->getVertex(id0).y;
		vec2.z = _mesh->getVertex(id2).z - _mesh->getVertex(id0).z;
		normal.x = vec1.z*vec2.y - vec1.y*vec2.z;
		normal.y = vec1.x*vec2.z - vec1.z*vec2.x;
		normal.z = vec1.y*vec2.x - vec1.x*vec2.y;
		_mesh->getNormal(id0) += normal;
		_mesh->getNormal(id1) += normal;
		_mesh->getNormal(id2) += normal;
	}

	// Normalize normals.
	for (unsigned int i = 0; i < _nNormals; i++) {
		float length = sqrt(
				_mesh->getNormal(i).x*_mesh->getNormal(i).x +
				_mesh->getNormal(i).y*_mesh->getNormal(i).y +
				_mesh->getNormal(i).z*_mesh->getNormal(i).z);
		_mesh->getNormal(i).x /= length;
		_mesh->getNormal(i).y /= length;
		_mesh->getNormal(i).z /= length;
	}
}

// template instantiation for image stack volume adaptors
#include <imageprocessing/ImageStackVolumeAdaptor.h>
template class MarchingCubes<ImageStackVolumeAdaptor>;
