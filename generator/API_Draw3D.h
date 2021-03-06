/**
 *  Java Grinder
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2014-2018 by Michael Kohn
 *
 */

#ifndef _API_DRAW_3D_H
#define _API_DRAW_3D_H

#define DRAW3D_TYPE_POINT equ 0
#define DRAW3D_TYPE_LINE equ 1
#define DRAW3D_TYPE_LINE_STRIP equ 2
#define DRAW3D_TYPE_TRIANGLE equ 3
#define DRAW3D_TYPE_TRIANGLE_STRIP equ 4
#define DRAW3D_TYPE_TRIANGLE_FAN equ 5
#define DRAW3D_TYPE_SPRITE equ 6

class API_Draw3D
{
public:
  virtual int draw3d_Constructor_X(int type) { return -1; }
  virtual int draw3d_Constructor_I(int type) { return -1; }
  virtual int draw3d_rotate_III() { return -1; }
  virtual int draw3d_setPosition_III() { return -1; }
  virtual int draw3d_setPointPosition_IIII() { return -1; }
  virtual int draw3d_setPointColor_II() { return -1; }
  virtual int draw3d_draw() { return -1; }
};

#endif

