/**
 *  Java Grinder
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2014-2016 by Michael Kohn
 *
 * CPC support by Carsten Dost - https://github.com/deringenieur71
 *
 */

#ifndef _API_CPC_H
#define _API_CPC_H

class API_CPC
{
public:
  // CPC
  virtual int cpc_beep() { return -1; }
  virtual int cpc_setPenColor_I() { return -1; }
  virtual int cpc_setPaperColor_I() { return -1; }
  virtual int cpc_setBorderColor_I() { return -1; }
  virtual int cpc_cls() { return -1; }
  virtual int cpc_screen_I() { return -1; }
  virtual int cpc_putChar_C() { return -1; }
  virtual int cpc_setCursor_II() { return -1; }
  virtual int cpc_plot_III() { return -1; }
  virtual int cpc_draw_III() { return -1; }
};

#endif

