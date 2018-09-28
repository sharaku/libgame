/*-
 *
 * MIT License
 * 
 * Copyright (c) 2005, 2018 Abe Takafumi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. *
 *
 */

#ifndef _OBJ_MAPS_HPP_
#define _OBJ_MAPS_HPP_

#ifdef __cplusplus
	#ifndef CPP_SRC
		#define CPP_SRC(x) x
	#endif
#else
	#ifndef CPP_SRC
		#define CPP_SRC(x)
	#endif
#endif

// ゲームのMAPを管理するためのコンテナ。
// 3次元MAPを管理する。
// 2次元MAPとして使用する場合はdepthを1とする。
// MAP領域のメモリは各自で確保する必要がある。

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

template<typename T>
struct obj_maps {

	obj_maps()
	{
		__width = 0;
		__hight = 0;
		__depth = 0;
		__objp = NULL;
	}

	obj_maps(int32_t w, int32_t h, int32_t d)
	{
		initialize(w, h, d);
	}

	int initialize(int32_t w, int32_t h, int32_t d)
	{
		__width = w;
		__hight = h;
		__depth = d;
		size_t size = sizeof(T) * w * h * d;
		__objp = (T*)malloc(size);
		if (__objp == NULL) {
			return -errno;
		}
		memset(__objp, 0, size);
		return 0;
	}


	int32_t get_width() { return __width; }
	int32_t get_hight() { return __hight; }
	int32_t get_depth() { return __depth; }
	int is_range(int32_t x, int32_t y, int32_t d) {
		if (x < 0 || x >= get_width() ||
		    y < 0 || y >= get_hight() ||
		    d < 0 || d >= get_depth()) {
			return 0;
		} else {
			return 1;
		}
	}
	T* get_object(int32_t x, int32_t y, int32_t d) {
		if (is_range(x, y, d)) {
			return &(__objp[(get_hight() * d) + (get_width() * y) + x]);
		} else {
			return NULL;
		}
	}

	struct position_2d {
		int	add_x;
		int	add_y;
	};

	int get_near4(int idx, int32_t *x, int32_t *y) {
		static struct position_2d ___vecter_pos[4] = {
			{ 0, -1},
			{ 0,  1},
			{-1,  0},
			{ 1,  0}
		};
		*x = ___vecter_pos[idx].add_x;
		*y = ___vecter_pos[idx].add_y;
		return idx;
	}
	int get_near8(int idx, int32_t *x, int32_t *y) {
		static struct position_2d ___vecter_pos[8] = {
			{-1,  0},
			{-1,  1},
			{ 0,  1},
			{ 1,  1},
			{ 1,  0},
			{ 1, -1},
			{ 0, -1},
			{-1, -1}
		};
		*x = ___vecter_pos[idx].add_x;
		*y = ___vecter_pos[idx].add_y;
		return idx;
	}
	int get_next_near4_idx(int idx, int32_t *x, int32_t *y,
			       int32_t cx, int32_t cy) {
		int32_t _x, _y, _add_x, _add_y;
		while (++idx < 4) {
			get_near4(idx, &_add_x, &_add_y);
			_x = cx + _add_x;
			_y = cy + _add_y;
			if (is_range(_x, _y, 0)) {
				*x = _x;
				*y = _y;
				break;
			}
		}
		return idx;
	}
	int get_next_near8_idx(int idx, int32_t *x, int32_t *y,
			       int32_t cx, int32_t cy) {
		int32_t _x, _y, _add_x, _add_y;
		while (++idx < 8) {
			get_near8(idx, &_add_x, &_add_y);
			_x = cx + _add_x;
			_y = cy + _add_y;
			if (is_range(_x, _y, 0)) {
				*x = _x;
				*y = _y;
				break;
			}
		}
		return idx;
	}

protected:
	uint16_t	__width;
	uint16_t	__hight;
	uint16_t	__depth;
	T		*__objp;
};

#define omap_for_each_depth(depth, omapp)		\
	for (depth = 0; depth < (omapp)->get_depth(); depth++)

#define omap_for_each_width(x, omapp)			\
	for (x = 0; x < (omapp)->get_width(); x++)

#define omap_for_each_hignt(y, omapp)			\
	for (y = 0; y < (omapp)->get_hight(); y++)

#define omap_for_each_depth_entry(pos, depth, x, y, omapp)	\
	for (depth = 0, (pos) = (omapp)->get_object(x, y, depth);	\
	     depth < (omapp)->get_depth();			\
	     depth++, (pos) = (omapp)->get_object(x, y, depth))

#define omap_for_each_width_entry(pos, x, y, depth, omapp)	\
	for (x = 0, (pos) = (omapp)->get_object(x, y, depth);	\
	     x < (omapp)->get_width();				\
	     x++, (pos) = (omapp)->get_object(x, y, depth))

#define omap_for_each_hignt_entry(pos, y, x, depth, omapp)	\
	for (y = 0, (pos) = (omapp)->get_object(x, y, depth);	\
	     y < (omapp)->get_hight();				\
	     y++, (pos) = (omapp)->get_object(x, y, depth))

#define omap_for_each_entry(pos, x, y, depth, omapp)		\
	omap_for_each_depth(depth, omapp)			\
		omap_for_each_width(x, omapp)			\
			omap_for_each_hignt_entry(pos, y, x, depth, omapp)

// 指定 x, y の上下左右の座標を呼び出す
#define omap_for_each_near4(x, y, i, cx, cy, omapp)		\
	for (i = (omapp)->get_next_near4_idx(-1, x, y, cx, cy);	\
	     i < 4;						\
	     i = (omapp)->get_next_near4_idx(i, x, y, cx, cy))

// 指定 x, yを中心とする8方向の座標を呼び出す
#define omap_for_each_near8(x, y, i, cx, cy, omapp)		\
	for (i = (omapp)->get_next_near8_idx(-1, x, y, cx, cy);	\
	     i < 8;						\
	     i = (omapp)->get_next_near8_idx(i, x, y, cx, cy))

// 指定 x, y の上下左右の座標を呼び出す(entryを変えす)
#define omap_for_each_near4_entry(pos, x, y, i, cx, cy, depth, omapp)	\
	for (i = (omapp)->get_next_near4_idx(-1, &x, &y, cx, cy),	\
	     (pos) = (omapp)->get_object(x, y, depth);			\
	     i < 4;							\
	     i = (omapp)->get_next_near4_idx(i, &x, &y, cx, cy),	\
	     (pos) = (omapp)->get_object(x, y, depth))

// 指定 x, y の上下左右の座標を呼び出す(entryを変えす)
#define omap_for_each_near8_entry(pos, x, y, i, cx, cy, depth, omapp)	\
	for (i = (omapp)->get_next_near8_idx(-1, &x, &y, cx, cy),	\
	     (pos) = (omapp)->get_object(x, y, depth);			\
	     i < 8;							\
	     i = (omapp)->get_next_near8_idx(i, &x, &y, cx, cy),	\
	     (pos) = (omapp)->get_object(x, y, depth))

#define omap_for_each_vecter4(x, y, i, omapp)				\
	for (i = (omapp)->get_near4(0, &x, &y);				\
	     i < 4;							\
	     i = (omapp)->get_near4(i + 1, &x, &y))

#define omap_for_each_vecter8(x, y, i, omapp)				\
	for (i = (omapp)->get_near8(0, &x, &y);				\
	     i < 8;							\
	     i = (omapp)->get_near8(i + 1, &x, &y))

#endif // _OBJ_MAPS_HPP_
