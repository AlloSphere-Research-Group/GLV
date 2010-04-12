/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv.h"
#include <assert.h>

using namespace glv;

int main(int argc, char ** argv){

	// test View linked list implementation
	{
		View n00, n10, n11, n20;
		
		n00.add(n10);
		n00.add(n11);
		n10.add(n20);
		
		// check all of our links
		assert( n00.child == &n10);
		assert(!n00.parent);
		assert(!n00.sibling);
		assert( n10.parent == &n00);
		assert( n10.sibling == &n11);
		assert( n10.child == &n20);
		assert( n11.parent == &n00);
		assert(!n11.child);
		assert(!n11.sibling);
		assert( n20.parent == &n10);
		assert(!n20.child);
		assert(!n20.sibling);
		
		n10.makeLastSibling();
		assert(!n10.sibling);
		assert(n11.sibling == &n10);
		n11.makeLastSibling();

		n10.remove();
		assert(!n10.parent);
		assert(!n10.sibling);
		assert(!n00.child->sibling);
		
		n11.remove();
		assert(!n00.child);

		assert(n10.child == &n20);
	}
	
	
	// test View memory management
	{
		View * v0d = new View;
		View v1s;
		View * v1d = new View;
		
		assert(v0d->dynamicAlloc());
		assert(!v1s.dynamicAlloc());

		*v0d << v1s << v1d;
		
		delete v0d;
	}
	
	return 0;
}
