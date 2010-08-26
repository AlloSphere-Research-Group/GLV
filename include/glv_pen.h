#ifndef INC_GLV_PEN_H
#define INC_GLV_PEN_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_draw.h"
#include <vector>

namespace glv {

class Pen;

class Pen{
public:

	void reset(){ mVertices2D.reset(); }

	void draw(int prim){
		draw::paint2D(prim, &mVertices2D[0], mVertices.fill());
	}

protected:

	template <class T>
	class Subvector : public std::vector<T>{
	public:
		Subvector(int n=8): mFill(0){ resize(n); }
	
		void reset(){ mFill=0; }
		void pushBack(const T& v){
			
			if(mFill >= size()){
				resize((mFill?mFill:1)*2);
			}
			(*this)[mFill] = v;
			++mFill;
		}

		int fill() const { return mFill; }

	protected:
		int mFill;
	};

	//int mPrim;
	Subvector<Point2D> mVertices2D;
	//Subvector<Point2D> mVertices2D;
};


} // glv::

#endif
