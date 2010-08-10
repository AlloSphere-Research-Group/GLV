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

	{	//for(int i=0;i<4;++i) printf("%g\n", f4[i]);
		//printf("%s\n", s1.c_str());
		bool b1;
		float f1;
		double d1;
		std::string s1;
		
		bool b4[4];
		float f4[4];
		double d4[4];
		
		#define SET4(x, a,b,c,d) x[0]=a; x[1]=b; x[2]=c; x[3]=d
		#define EQ4(x, a,b,c,d) (x[0]==a && x[1]==b && x[2]==c && x[3]==d)
		
		fromString(f1, "1e-2");		assert(f1 == float(1e-2));
		fromString(f1, "1000");		assert(f1 == float(1000));
		fromString(d1, "1e-2");		assert(d1 == double(1e-2));
		fromString(d1, "1000");		assert(d1 == double(1000));
		fromString(b1, "0");		assert(b1 == 0);
		fromString(b1, "1");		assert(b1 == 1);
		fromString(s1, "\"test\"");	assert(s1 == "test");
		
		fromString(b4,4, "{1,0,1,1}");				assert(EQ4(b4, 1,0,1,1));
		fromString(b4,4, "{0,  1, 0,0}");			assert(EQ4(b4, 0,1,0,0));
		fromString(f4,4, "{1, -1.2, 1e10, +.1}");	assert(EQ4(f4, 1.f,-1.2f,1e10f,+.1f));
		fromString(d4,4, "{1, -1.2, 1e10, +.1}");	assert(EQ4(d4, 1,-1.2,1e10,+.1));

		toString(s1, 1000.f);		assert(s1 == "1000");
		toString(s1, 2000.0);		assert(s1 == "2000");
		//toString(rs, -2.1e100);		printf("%s\n", rs.c_str()); //assert(rs == "2e100");
		//toString(rs, (float)-2.1e38);		printf("%s\n", rs.c_str()); //assert(rs == "2e100");
		toString(s1, true);			assert(s1 == "1");
		toString(s1, false);		assert(s1 == "0");
		toString(s1, "test");		assert(s1 == "\"test\"");
		
		SET4(b4, 1,0,1,1); toString(s1, b4,4);		assert(s1 == "{1, 0, 1, 1}");
		SET4(f4,-1,0.1,3,1e10); toString(s1, f4,4); assert(s1 == "{-1, 0.1, 3, 1e+10}");
		SET4(d4,-1,0.1,3,1e10); toString(s1, d4,4); assert(s1 == "{-1, 0.1, 3, 1e+10}");

		#undef EQ4
		#undef SET4
	}


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

// Setting to current value should NOT send notification to avoid infinite loops
		b=false;
		w.value(w.value());
		assert(!b);

// Boolean model variable
		bool v = false;
		w.attachVariable(v);
		w.value(true);		assert(v == true);
		w.value(false);		assert(v == false);
		
		v = true;
		w.onModelSync();	assert(w.value() == true);

		std::string s;
		w.value(true);
		w.valueToString(s);			assert(s == "1");
		w.value(false);
		w.valueToString(s);			assert(s == "0");
		w.valueFromString("0");		assert(w.value() == false);
		w.valueFromString("1");		assert(w.value() == true);

		assert(!w.valueFromString("invalid"));
	}

	{
		bool b=false;
		Buttons w;
		w.attach(ntValue1, Update::Value, &b);
		w.value(true, 0);
		assert(b);
		assert(w.value(0) == true);
		
		w.resize(2,2);
		
		bool v1 = false;
		bool v2 = false;
		w.attachVariable(v1, 0);
		w.attachVariable(v2, 2);
		w.value(true, 0);		assert(v1 == true);
		w.value(true, 2);		assert(v2 == true);
		w.value(false, 0);		assert(v1 == false);
		w.value(false, 2);		assert(v2 == false);

		std::string s;
		w.value(false, 0);
		w.value(false, 1);
		w.value(false, 2);
		w.value(false, 3);
		w.valueToString(s);		assert(s == "{0, 0, 0, 0}");
		
		v1 = v2 = false;
		w.valueFromString("{1,1,1,1}");
		assert(w.value(0) && w.value(1) && w.value(2) && w.value(3));
		assert(v1 && v2);
	}
	
	{
		bool b=false;
		Slider w;
		w.attach(ntValue1, Update::Value, &b);
		w.value(0.99f);
		assert(b);
		assert(w.value() == 0.99f);

// Setting to current value should NOT send notification to avoid infinite loops
		b=false;
		w.value(w.value());
		assert(!b);

		float v;
		w.attachVariable(v);
		w.value(0.99);
		assert(v == 0.99f);
		
		v = 0;
		w.onModelSync();
		assert(w.value() == 0);

		std::string s;
		w.value(0.25);
		w.valueToString(s);			assert(s == "0.25");
		w.valueFromString("0.5");	assert(w.value() == 0.5);
									assert(v == 0.5);

		assert(!w.valueFromString("invalid"));
		assert(w.value() == 0.5);
	}

	{
		bool b=false;
		Sliders w(Rect(1), 4,4);
		w.attach(ntValue1, Update::Value, &b);
		w.value(0.99f, 13);
		assert(b);
		assert(w.value(13) == 0.99f);
	}

	{
		bool b=false;
		Slider2D w;
		w.attach(ntValue1, Update::Value, &b);
		w.value(0.01f, 0);
		assert(b);
		assert(w.value(0) == 0.01f);

// Setting to current value should NOT send notification to avoid infinite loops
		b=false;
		w.value(0.01f, 0);
		assert(!b);
		
		b=false;
		w.value(0.00f, 0);
		w.value(0.01f, 1);
		assert(b);
		assert(w.value(1) == 0.01f);
		
//		b=false;
//		w.valueAdd(0.01f, 0);
//		w.valueAdd(0.01f, 1);
//		assert(b);
//		assert(w.value(0) == 0.01f);
//		assert(w.value(1) == 0.02f);
		
		b=false;
		w.valueMax();
		assert(b);
		assert(w.value(0) == 1.f);
		assert(w.value(1) == 1.f);
		
		float v1, v2;
		w.attachVariable(v1, 0);
		w.attachVariable(v2, 1);
		
		w.value(0.5f, 0);	assert(v1 == 0.5f);
		w.value(0.6f, 1);	assert(v2 == 0.6f);
		
		v1 = 0.1;
		v2 = 0.2;
		
		w.onModelSync();
		assert(w.value(0) == v1);
		assert(w.value(1) == v2);
	
		std::string s;
		w.value(0.2, 0);
		w.value(0.3, 1);
		w.valueToString(s);		assert(s == "{0.2, 0.3}");
		
		v1 = v2 = 0;
		w.valueFromString("{0.7, 0.8}");
		assert(w.value(0)==0.7f && w.value(1)==0.8f);
		assert(v1==w.value(0) && v2==w.value(1));
	}

	{
		bool b=false;
		SliderGrid<4> w;
		w.attach(ntValue1, Update::Value, &b);
		w.value(0.01f, 0);
		assert(b);
		assert(w.value(0) == 0.01f);
	}
	
	{
		bool b=false;
		SliderRange w;
		w.attach(ntValue1, Update::Value, &b);
		w.value(0.01f, 0);
		w.value(0.02f, 1);
		assert(b);
		assert(w.value(0) == 0.01f);
		assert(w.value(1) == 0.02f);
		assert(w.center() == 0.015f);
		assert(w.range() == 0.01f);
		
		w.extrema(0,1);
		assert(w.value(0) == 0);
		assert(w.value(1) == 1);
		
		w.centerRange(0.5f, 0.2f);
		assert(w.value(0) == 0.4f);
		assert(w.value(1) == 0.6f);

		float v1,v2;
		w.attachVariable(v1, 0);
		w.attachVariable(v2, 1);

		std::string s;
		w.value(0.2, 0);
		w.value(0.3, 1);
		w.valueToString(s);		assert(s == "{0.2, 0.3}");
		
		v1 = v2 = 0;
		w.valueFromString("{0.7, 0.8}");
		assert(w.value(0)==0.7f && w.value(1)==0.8f);
		assert(v1==w.value(0) && v2==w.value(1));
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
		w.value(0.75);
		assert(b);
		assert(w.value() == 0.75);

// Setting to current value should NOT send notification to avoid infinite loops
		b=false;
		w.value(w.value());
		assert(!b);

		double v=0;
		w.attachVariable(v);
		w.value(0.5);	assert(v == 0.5);
		
		v = 0.25;
		w.onModelSync();
		assert(w.value() == v);

		std::string s;
		w.value(0.2);
		w.valueToString(s);		assert(s == "0.2");
		
		v = 0;
		w.valueFromString("0.8");
		assert(w.value()==0.8 && w.value()==v);
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
