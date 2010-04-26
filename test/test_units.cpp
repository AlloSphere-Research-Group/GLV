/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv.h"
#include <assert.h>

using namespace glv;

void ntValue1(const Notification& n){
	bool& b = *(bool *)n.receiver();
	b = true;
}

void ntValue2(const Notification& n){
	bool& b = *(bool *)n.receiver();
	b = true;
}

void ntFocus(const Notification& n){
	bool& b = *(bool *)n.receiver();
	b = true;
}



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
	


	// Notifications	
	{
		bool bv1=false, bv2=false, bf=false;
		Notifier n;
		
		assert(n.numObservers(Update::Value) == 0);
		
		n.attach(ntValue1, Update::Value, &bv1);
		n.attach(ntValue2, Update::Value, &bv2);
		n.attach(ntFocus, Update::Focus, &bf);

		assert(n.numObservers(Update::Value) == 2);
		assert(n.numObservers(Update::Focus) == 1);

		n.notify(Update::Value);
		assert(bv1);
		assert(bv2);
		assert(!bf);

		n.notify(Update::Focus);
		assert(bf);
		
		bv1=bv2=false;
		n.detach(ntValue1, Update::Value, &bv1);
		n.notify(Update::Value);
		assert(!bv1);
		assert(bv2);

		bv1=bv2=false;
		n.detach(ntValue2, Update::Value, &bv2);
		n.notify(Update::Value);
		assert(!bv1);
		assert(!bv2);
	}
	
	{
		bool b=false;
		Button w;
		w.attach(ntValue1, Update::Value, &b);
		w.value(true);
		assert(b);
		assert(w.value() == true);
	}

	{
		bool b=false;
		Buttons w;
		w.attach(ntValue1, Update::Value, &b);
		w.value(true, 0);
		assert(b);
		assert(w.value(0) == true);
	}
	
//	{
//		bool b=false;
//		Slider w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(0.99);
//		assert(b);
//	}

	{
		bool b=false;
		Slider2D w;
		w.attach(ntValue1, Update::Value, &b);
		w.value(0.01f, 0);
		assert(b);
		assert(w.value(0) == 0.01f);
		
		b=false;
		w.value(0.01f, 0);
		assert(!b);
		
		b=false;
		w.value(0.00f, 0);
		w.value(0.01f, 1);
		assert(b);
		assert(w.value(1) == 0.01f);
		
		b=false;
		w.valueAdd(0.01f, 0);
		w.valueAdd(0.01f, 1);
		assert(b);
		assert(w.value(0) == 0.01f);
		assert(w.value(1) == 0.02f);
		
		b=false;
		w.valueMax();
		assert(b);
		assert(w.value(0) == 1.f);
		assert(w.value(1) == 1.f);
	}

	{
		bool b=false;
		Label w;
		w.attach(ntValue1, Update::Value, &b);
		w.value("hello");
		assert(b);
		assert(w.value() == "hello");
	}

	{
		bool b=false;
		NumberDialer w(1,2,1,0);
		w.attach(ntValue1, Update::Value, &b);
		w.value(0.99);
		assert(b);
		assert(w.value() == 0.99);
	}

	{
		bool b=false;
		TextView w;
		w.attach(ntValue1, Update::Value, &b);
		w.value("hello");
		assert(b);
		assert(w.value() == "hello");
	}
	
//	printf("%d\n", sizeof(glv::Notifier));
//	printf("%d\n", sizeof(glv::View));

	return 0;
}
