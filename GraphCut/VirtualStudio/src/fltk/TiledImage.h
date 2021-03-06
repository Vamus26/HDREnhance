//
// "$Id: TiledImage.h 5145 2006-05-22 06:11:17Z spitzak $"
//
// A tiled image completely fills the bounding box passed to it with
// replications of the internal Image passed to it.
//
// Copyright 1998-2006 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#ifndef fltk_TiledImage_h
#define fltk_TiledImage_h

#include "Symbol.h"

namespace fltk {

class FL_API TiledImage : public Symbol {
protected:
  const Symbol* image_;
public:
  TiledImage(Symbol *i) : Symbol(0), image_(i) {}
  // this constructor allow to load a sharedimage from an image name in one shot
  TiledImage(const char * name); 
  const Symbol* image() const {return image_;}
  void image(const Symbol* i) {image_ = i;}
  void _measure(int& w, int& h) const;
  void _draw(const Rectangle&) const;
  void inset(Rectangle&) const;
};

}
#endif

//
// End of "$Id: TiledImage.h 5145 2006-05-22 06:11:17Z spitzak $"
//
