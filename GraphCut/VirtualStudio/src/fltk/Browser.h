//
// "$Id: Browser.h 5197 2006-06-14 07:43:46Z spitzak $"
//
// Copyright 2002 by Bill Spitzak and others.
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

#ifndef fltk_Browser_h
#define fltk_Browser_h

#include "Menu.h"
#include "Scrollbar.h"

namespace fltk {

class ItemGroup;
class Item;

class FL_API Browser : public Menu {
public:

  int handle(int);
  void layout();
  void draw();

  Browser(int X,int Y,int W,int H,const char*l=0);
  static NamedStyle* default_style;
  ~Browser();
  
  enum { //<! values for type()
    IS_MULTI = 1,
    NORMAL =  GROUP_TYPE,  //!< means single selection can be achieved by user 
    MULTI  =  GROUP_TYPE+1 //!< means multiple selection can be achieved by user
  };
  enum { // values selected_column
    NO_COLUMN_SELECTED = -1 //!< means that no column has been selected by user
  };
  enum { //!< predefined marks
    HERE = 0,	    //!< current item, the one moved by all the calls
    FOCUS,	    //!< what the user thinks is the curren item
    FIRST_VISIBLE,  //!< item at top of scroll region
    REDRAW_0,	    //!< item that needs to be redrawn
    REDRAW_1,	    //!< a second item that needs to be redrawn
    OPEN,	    //!< this and all parents are open
    TEMP,	    //!< scratch space reserved for fltk only
    TREE_TRAVERSAL, //!< this volatile mark is available for all tree traversal usage fltk+applications
    USER1,	    //!< all purpose scratch space 1, for applications only
    USER2,	    //!< all purpose scratch space 2, for applications only
    NUMMARKS	    //!< end/number of marks
  };
  enum NodeType { //!< values for tree node types
    GROUP= 0,	    //!< indicates that the node is a ItemGroup derived node in the tree
    LEAF = 1	    //!< indicates that the node is simple Item in the tree
  };
  enum linepos {  //!< Argument to make_item_visible()
      NOSCROLL,	    //!< no_scroll is made to make the current item visible
      TOP,	    //!< position current item to top when made visible
      MIDDLE,	    //!< position current item to middle when made visible
      BOTTOM	    //!< position current item to bottom when made visible
  };
  

  int width() const {return width_;}
  int height() const {return height_;}
  int box_width() const {return interior.w();}
  int box_height() const {return interior.h();}
  int xposition() const {return xposition_;}
  void xposition(int);
  int yposition() const {return yposition_;}
  void yposition(int);
  bool indented() const {return indented_;}
  void indented(bool v) {indented_ = v;}

  Scrollbar scrollbar;
  Scrollbar hscrollbar;

  Widget* goto_top();
  Widget* goto_focus() {return goto_mark(FOCUS);}
  Widget* goto_position(int y);
  Widget* goto_index(const int* indexes, int level);
  Widget* goto_index(int);
  Widget* goto_index(int,int,int=-1,int=-1,int=-1);
  Widget* next();
  Widget* next_visible();
  Widget* previous_visible();
  bool item_is_visible() const;
  bool item_is_parent() const;
  bool item_is_open() const;
  int item_h() const;

  bool set_focus();
  bool set_item_selected(bool value = true, int do_callback = 0);
  bool select_only_this(int do_callback = 0);
  bool deselect(int do_callback = 0);
  bool make_item_visible(linepos = NOSCROLL);
  void damage_item() {damage_item(HERE);}
  bool set_item_opened(bool);
  bool set_item_visible(bool);

  int current_level() const {return item_level[HERE];}
  const int* current_index() const {return item_index[HERE];}
  int current_position() const {return item_position[HERE];}

  int focus_level() const {return item_level[FOCUS];}
  const int* focus_index() const {return item_index[FOCUS];}
  int focus_position() const {return item_position[FOCUS];}
  void value(int v) {goto_index(v); set_focus();}
  int value() const {return focus_index()[0];}

  // Maybe these should be moved to base Menu class:
  const int *column_widths() const { return column_widths_p; }
  void column_widths(const int *pWidths);
  const char **column_labels() const { return column_labels_; }
  void column_labels(const char **pLabels);
  int selected_column() { return selected_column_; }
  int set_column_start(int column, int x);

  // Convienence functions for flat browsers:
  bool select(int line, bool value = true);
  bool selected(int line);
  int topline() const {return item_index[FIRST_VISIBLE][0];}
  void topline(int line) {goto_index(line); make_item_visible(TOP);}
  void bottomline(int line) {goto_index(line); make_item_visible(BOTTOM);}
  void middleline(int line) {goto_index(line); make_item_visible();}
  bool displayed(int line);
  bool display(int line, bool value = true);

  Widget *header(int col) { if(col<0 || col>=nHeader) return 0; return header_[col]; }
  int nheader() const { return nHeader; }

  int load(const char *filename);
 
protected:
  void handle_callback(int doit); // defines how cb are handled in the browser

private:
  bool indented_;
  const int *column_widths_; // original column widths
  int *column_widths_i;      // original column widths after user interaction
  int *column_widths_p;      // actual column widths
  const char **column_labels_;
  int xposition_, yposition_;
  int width_, height_;
  int scrolldx, scrolldy;
  static void hscrollbar_cb(Widget*, void*);
  static void scrollbar_cb(Widget*, void*);
  void draw_item(int);
  void draw_clip(const Rectangle&);
  static void draw_clip_cb(void*,const Rectangle&);
  Rectangle interior; // inside box edges and scrollbars
  
  Widget **header_;
  int nHeader, nColumn, selected_column_;

  int multi() const {return type()&IS_MULTI;}

  // Marks serve as "indexes" into the hierarchial browser. We probably
  // need to make some sort of public interface but I have not figured
  // it out completely.
  Widget* goto_mark(int mark); // set HERE to mark
  Widget* goto_visible_focus(); // set HERE to focus if visible
  void set_mark(int dest, int mark); // set dest to mark
  int compare_marks(int mark1, int mark2); // returns >0 if mark1 after mark2
  bool at_mark(int mark) { return !compare_marks(HERE,mark); }
  void unset_mark(int mark);  // makes mark have illegal value
  bool is_set(int mark);  // false if unset_mark was called
  void damage_item(int mark); // make this item redraw
  int siblings; // # of children of parent of HERE item
  // For each mark:
  unsigned char item_level[NUMMARKS]; // depth in hierarchy of the item
  unsigned char open_level[NUMMARKS]; // depth of highest closed parent
  int item_position[NUMMARKS]; // distance in pixels from top of browser
  int* item_index[NUMMARKS]; // indexes at each level of hierarchy
  int levels; // maximum depth of all items encountered so far
  void set_level(int); // increases levels by reallocating the arrays

  static void column_click_cb_(Widget*, void*);

public:  
  //
  // tree construction high level API
  //   dramatically improves tree construction in an easy and elegant way
  // 
  void set_symbol(NodeType nodetype, 
      const Symbol* imgClosed=0, // default (and closed if open not null) img
      const Symbol* imgFocus=0,  // img when mouse comes on it
      const Symbol* imgOpen=0);  // img when node open (for group nodes only)
  const Symbol* get_symbol(NodeType nodetype, Flags f=fltk::NO_FLAGS) const;  
  ItemGroup* add_group(const char *label, Group* parent=0, int state=OPENED, 
      const Symbol* imgClosed=0, const Symbol* imgFocus=0, const Symbol* imgOpen=0);
  Item* add_leaf(const char *label, Group* parent=0,  
      const Symbol* img=0, const Symbol* imgFocus=0);
  
  // override item removal changes to have a notification
  void replace(Widget& old, Widget& o) {notify_remove(&old);Menu::replace(old,o);}
  void remove(Widget& o) {notify_remove(&o); Menu::remove(o);}
  void remove(Widget* o) {notify_remove(o); Menu::remove(o);}
  void remove(int index) {notify_remove(child(index)); Menu::remove(index);}
  //! update internals when an item is going to be removed, if null warn that more than one item is removed
  void notify_remove(Widget* o);
  // ! empty the browser from items and rest scrollbars
  void clear();
  

private:
  const Symbol *defGroupSymbol1, *defGroupSymbol2, *defGroupSymbol3;
  const Symbol *defLeafSymbol1,*defLeafSymbol2,*defLeafSymbol3;
  Widget* prev_item_;
};

}

#endif

//
// End of "$Id: Browser.h 5197 2006-06-14 07:43:46Z spitzak $".
//
