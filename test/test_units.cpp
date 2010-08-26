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



int main(){

	#define SET4(x, a,b,c,d) x[0]=a; x[1]=b; x[2]=c; x[3]=d
	#define EQ4(x, a,b,c,d) (x[0]==a && x[1]==b && x[2]==c && x[3]==d)

	{	//for(int i=0;i<4;++i) printf("%g\n", f4[i]);
		//printf("%s\n", s1.c_str());
		bool b1;
		float f1;
		double d1;
		std::string s1;
		
		bool b4[4];
		float f4[4];
		double d4[4];
		
		fromToken(f1, "1e-2");		assert(f1 == float(1e-2));
		fromToken(f1, "1000");		assert(f1 == float(1000));
		fromToken(d1, "1e-2");		assert(d1 == double(1e-2));
		fromToken(d1, "1000");		assert(d1 == double(1000));
		fromToken(b1, "0");			assert(b1 == 0);
		fromToken(b1, "1");			assert(b1 == 1);
		fromToken(s1, "\"test\"");	assert(s1 == "test");
		
		fromToken(b4,4, "{1,0,1,1}");				assert(EQ4(b4, 1,0,1,1));
		fromToken(b4,4, "{0,  1, 0,0}");			assert(EQ4(b4, 0,1,0,0));
		fromToken(f4,4, "{1, -1.2, 1e10, +.1}");	assert(EQ4(f4, 1.f,-1.2f,1e10f,+.1f));
		fromToken(d4,4, "{1, -1.2, 1e10, +.1}");	assert(EQ4(d4, 1,-1.2,1e10,+.1));

		toToken(s1, 1000.f);		assert(s1 == "1000");
		toToken(s1, 2000.0);		assert(s1 == "2000");
		//toToken(rs, -2.1e100);		printf("%s\n", rs.c_str()); //assert(rs == "2e100");
		//toToken(rs, (float)-2.1e38);		printf("%s\n", rs.c_str()); //assert(rs == "2e100");
		toToken(s1, true);			assert(s1 == "1");
		toToken(s1, false);			assert(s1 == "0");
		toToken(s1, "test");		assert(s1 == "\"test\"");
		
		SET4(b4, 1,0,1,1); toToken(s1, b4,4);		assert(s1 == "{1, 0, 1, 1}");
		SET4(f4,-1,0.1,3,1e10); toToken(s1, f4,4);	assert(s1 == "{-1, 0.1, 3, 1e+10}");
		SET4(d4,-1,0.1,3,1e10); toToken(s1, d4,4);	assert(s1 == "{-1, 0.1, 3, 1e+10}");
	}


//	// test View linked list implementation
//	{
//		View n00, n10, n11, n20;
//
//		n00.add(n10);
//		n00.add(n11);
//		n10.add(n20);
//		
//		// check all of our links
//		assert( n00.child == &n10);
//		assert(!n00.parent);
//		assert(!n00.sibling);
//		assert( n10.parent == &n00);
//		assert( n10.sibling == &n11);
//		assert( n10.child == &n20);
//		assert( n11.parent == &n00);
//		assert(!n11.child);
//		assert(!n11.sibling);
//		assert( n20.parent == &n10);
//		assert(!n20.child);
//		assert(!n20.sibling);
//		
//		n10.makeLastSibling();
//		assert(!n10.sibling);
//		assert(n11.sibling == &n10);
//		n11.makeLastSibling();
//
//		n10.remove();
//		assert(!n10.parent);
//		assert(!n10.sibling);
//		assert(!n00.child->sibling);
//		
//		n11.remove();
//		assert(!n00.child);
//
//		assert(n10.child == &n20);
//	}
//
//	
//	// test View memory management
//	{
//		View * v0d = new View;
//		View v1s;
//		View * v1d = new View;
//		
//		assert(v0d->dynamicAlloc());
//		assert(!v1s.dynamicAlloc());
//
//		*v0d << v1s << v1d;
//		
//		delete v0d;
//	}
//
//
//
//	// Notifications	
//	{
//		bool bv1=false, bv2=false, bf=false;
//		Notifier n;
//		
//		assert(n.numObservers(Update::Value) == 0);
//		
//		n.attach(ntValue1, Update::Value, &bv1);
//		n.attach(ntValue2, Update::Value, &bv2);
//		n.attach(ntFocus, Update::Focus, &bf);
//
//		assert(n.numObservers(Update::Value) == 2);
//		assert(n.numObservers(Update::Focus) == 1);
//
//		n.notify(Update::Value);
//		assert(bv1);
//		assert(bv2);
//		assert(!bf);
//
//		n.notify(Update::Focus);
//		assert(bf);
//		
//		bv1=bv2=false;
//		n.detach(ntValue1, Update::Value, &bv1);
//		n.notify(Update::Value);
//		assert(!bv1);
//		assert(bv2);
//
//		bv1=bv2=false;
//		n.detach(ntValue2, Update::Value, &bv2);
//		n.notify(Update::Value);
//		assert(!bv1);
//		assert(!bv2);
//	}
//	
//	{
//		bool b=false;
//		Button w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(true);
//		assert(b);
//		assert(w.value() == true);
//
//// Setting to current value should NOT send notification to avoid infinite loops
//		b=false;
//		w.value(w.value());
//		assert(!b);
//
//// Boolean model variable
//		bool v = false;
//		w.attachVariable(v);
//		w.value(true);		assert(v == true);
//		w.value(false);		assert(v == false);
//		
//		v = true;
//		w.onModelSync();	assert(w.value() == true);
//
//		std::string s;
//		w.value(true);
//		w.toString(s);			assert(s == "1");
//		w.value(false);
//		w.toString(s);			assert(s == "0");
//		w.fromString("0");		assert(w.value() == false);
//		w.fromString("1");		assert(w.value() == true);
//
//		assert(!w.fromString("invalid"));
//	}
//
//	{
//		bool b=false;
//		Buttons w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(true, 0);
//		assert(b);
//		assert(w.value(0) == true);
//		
//		w.resize(2,2);
//		
//		bool v1 = false;
//		bool v2 = false;
//		w.attachVariable(v1, 0);
//		w.attachVariable(v2, 2);
//		w.value(true, 0);		assert(v1 == true);
//		w.value(true, 2);		assert(v2 == true);
//		w.value(false, 0);		assert(v1 == false);
//		w.value(false, 2);		assert(v2 == false);
//
//		std::string s;
//		w.value(false, 0);
//		w.value(false, 1);
//		w.value(false, 2);
//		w.value(false, 3);
//		w.toString(s);		assert(s == "{0, 0, 0, 0}");
//		
//		v1 = v2 = false;
//		w.fromString("{1,1,1,1}");
//		assert(w.value(0) && w.value(1) && w.value(2) && w.value(3));
//		assert(v1 && v2);
//	}
//	
//	{
//		bool b=false;
//		Slider w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(0.99f);
//		assert(b);
//		assert(w.value() == 0.99f);
//
//// Setting to current value should NOT send notification to avoid infinite loops
//		b=false;
//		w.value(w.value());
//		assert(!b);
//
//		float v;
//		w.attachVariable(v);
//		w.value(0.99);
//		assert(v == 0.99f);
//		
//		v = 0;
//		w.onModelSync();
//		assert(w.value() == 0);
//
//		std::string s;
//		w.value(0.25);
//		w.toString(s);			assert(s == "0.25");
//		w.fromString("0.5");	assert(w.value() == 0.5);
//									assert(v == 0.5);
//
//		assert(!w.fromString("invalid"));
//		assert(w.value() == 0.5);
//	}
//
//	{
//		bool b=false;
//		Sliders w(Rect(1), 2,2);
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(0.99f, 3);
//		assert(b);
//		assert(w.value(3) == 0.99f);
//
//		float v1, v2;
//		w.attachVariable(v1, 0);
//		w.attachVariable(v2, 1);
//
//		w.value(0.1f, 0); assert(v1 == 0.1f);
//		w.value(0.2f, 0); assert(v1 == 0.2f);
//		
//		v1 = 0.8f; v2 = 0.9f;
//		w.onModelSync();
//		assert(w.value(0) == 0.8f);
//		assert(w.value(1) == 0.9f);
//		
//		std::string s;
//		w.value(0.1f, 0);
//		w.value(0.2f, 1);
//		w.value(0.3f, 2);
//		w.value(0.4f, 3);
//		w.toString(s);	assert(s == "{0.1, 0.2, 0.3, 0.4}");
//		
//		v1=v2=0;
//		w.fromString("{0.4,0.3,0.2,0.1}");
//		assert(w.value(0) == 0.4f);
//		assert(w.value(1) == 0.3f);
//		assert(w.value(2) == 0.2f);
//		assert(w.value(3) == 0.1f);
//		assert(v1 == w.value(0) && v2 == w.value(1));
//		
////		double vs[4] = {0.11, 0.22, 0.33, 0.44};
////		w.attachVariable(vs,4);
////		w.onModelSync();
////		for(int i=0; i<4; ++i) printf("%f\n", w.values()[i]);
////		
////		w.value(0.44, 0);
////		w.value(0.33, 0);
////		w.value(0.22, 0);
////		w.value(0.11, 0);
////		for(int i=0; i<4; ++i) printf("%f\n", vs[i]);
//	}
//
//	{
//		bool b=false;
//		Slider2D w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(0.01f, 0);
//		assert(b);
//		assert(w.value(0) == 0.01f);
//
//// Setting to current value should NOT send notification to avoid infinite loops
//		b=false;
//		w.value(0.01f, 0);
//		assert(!b);
//		
//		b=false;
//		w.value(0.00f, 0);
//		w.value(0.01f, 1);
//		assert(b);
//		assert(w.value(1) == 0.01f);
//		
////		b=false;
////		w.valueAdd(0.01f, 0);
////		w.valueAdd(0.01f, 1);
////		assert(b);
////		assert(w.value(0) == 0.01f);
////		assert(w.value(1) == 0.02f);
//		
//		b=false;
//		w.valueMax();
//		assert(b);
//		assert(w.value(0) == 1.f);
//		assert(w.value(1) == 1.f);
//		
//		float v1, v2;
//		w.attachVariable(v1, 0);
//		w.attachVariable(v2, 1);
//		
//		w.value(0.5f, 0);	assert(v1 == 0.5f);
//		w.value(0.6f, 1);	assert(v2 == 0.6f);
//		
//		v1 = 0.1;
//		v2 = 0.2;
//		
//		w.onModelSync();
//		assert(w.value(0) == v1);
//		assert(w.value(1) == v2);
//	
//		std::string s;
//		w.value(0.2, 0);
//		w.value(0.3, 1);
//		w.toString(s);		assert(s == "{0.2, 0.3}");
//		
//		v1 = v2 = 0;
//		w.fromString("{0.7, 0.8}");
//		assert(w.value(0)==0.7f && w.value(1)==0.8f);
//		assert(v1==w.value(0) && v2==w.value(1));
//	}
//
//	{
//		bool b=false;
//		SliderGrid<4> w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(0.01f, 0);
//		assert(b);
//		assert(w.value(0) == 0.01f);
//
//		float v1, v2;
//		w.attachVariable(v1, 0);
//		w.attachVariable(v2, 1);
//
//		w.value(0.1f, 0); assert(v1 == 0.1f);
//		w.value(0.2f, 0); assert(v1 == 0.2f);
//		
//		v1 = 0.8f; v2 = 0.9f;
//		w.onModelSync();
//		assert(w.value(0) == 0.8f);
//		assert(w.value(1) == 0.9f);
//		
//		std::string s;
//		w.value(0.1f, 0);
//		w.value(0.2f, 1);
//		w.value(0.3f, 2);
//		w.value(0.4f, 3);
//		w.toString(s);	assert(s == "{0.1, 0.2, 0.3, 0.4}");
//		
//		v1=v2=0;
//		w.fromString("{0.4,0.3,0.2,0.1}");
//		assert(w.value(0) == 0.4f);
//		assert(w.value(1) == 0.3f);
//		assert(w.value(2) == 0.2f);
//		assert(w.value(3) == 0.1f);
//		assert(v1 == w.value(0) && v2 == w.value(1));
//	}
//	
//	{
//		bool b=false;
//		SliderRange w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(0.01f, 0);
//		w.value(0.02f, 1);
//		assert(b);
//		assert(w.value(0) == 0.01f);
//		assert(w.value(1) == 0.02f);
//		assert(w.center() == 0.015f);
//		assert(w.range() == 0.01f);
//		
//		w.extrema(0,1);
//		assert(w.value(0) == 0);
//		assert(w.value(1) == 1);
//		
//		w.centerRange(0.5f, 0.2f);
//		assert(w.value(0) == 0.4f);
//		assert(w.value(1) == 0.6f);
//
//		float v1,v2;
//		w.attachVariable(v1, 0);
//		w.attachVariable(v2, 1);
//
//		std::string s;
//		w.value(0.2, 0);
//		w.value(0.3, 1);
//		w.toString(s);		assert(s == "{0.2, 0.3}");
//		
//		v1 = v2 = 0;
//		w.fromString("{0.7, 0.8}");
//		assert(w.value(0)==0.7f && w.value(1)==0.8f);
//		assert(v1==w.value(0) && v2==w.value(1));
//	}
//
//	{
//		bool b=false;
//		Label w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value("test");
//		assert(b);
//		assert(w.value() == "test");
//		
//		std::string s;
//		w.toString(s);		assert(s == "\"test\"");
//		
//		w.value("");
//		w.fromString("\"test\"");	assert(w.value() == "test");
//	}
//
//	{
//		bool b=false;
//		NumberDialer w(1,2,1,0);
//		w.attach(ntValue1, Update::Value, &b);
//		w.value(0.75);
//		assert(b);
//		assert(w.value() == 0.75);
//
//// Setting to current value should NOT send notification to avoid infinite loops
//		b=false;
//		w.value(w.value());
//		assert(!b);
//
//		double v=0;
//		w.attachVariable(v);
//		w.value(0.5);	assert(v == 0.5);
//		
//		v = 0.25;
//		w.onModelSync();
//		assert(w.value() == v);
//
//		std::string s;
//		w.value(0.2);
//		w.toString(s);		assert(s == "0.2");
//		
//		v = 0;
//		w.fromString("0.8");
//		assert(w.value()==0.8 && w.value()==v);
//	}
//
//	{
//		bool b=false;
//		TextView w;
//		w.attach(ntValue1, Update::Value, &b);
//		w.value("hello");
//		assert(b);
//		assert(w.value() == "hello");
//	}
//	
//	
//	{
//		bool b1;
//		float f1;
//		double d1;
//		std::string s1;
//		
//		bool b4[4];
//		float f4[4];
//		double d4[4];
//		
//		b1=0; f1=0;					assert(Data(b1) == Data(f1));
//		f1=1; Data(b1).copy(f1);	assert(b1==f1);
//		
//		SET4(b4, 0,1,0,1);
//		Data(f4,4).copy(b4,4);		assert(EQ4(f4, 0,1,0,1));
//		
//		{
//			Data d(f4, 4);
//			SET4(f4, 1,2,3,4);
//			d.clone();
//			d.data<float>()[0] = 4;
//			d.data<float>()[1] = 3;
//			d.data<float>()[2] = 2;
//			d.data<float>()[3] = 1;
//			
//			assert(EQ4(f4, 1,2,3,4));
//		}
//	}


	{
		const float cf = 10;
		float f = 10;
		std::string s = "hello";
		
		Data d(cf);

//		assert(d.data<float>() == &cf);
//		assert(dcf.data<float>()[0] = 10);	// TODO: how can we prevent this?
		assert(d.hasData());
		assert(d.size() == 1);
		assert(d.type() == Data::FLOAT);
		//assert(d.typeKind() == Data::FLOAT);
		assert(d.at<float>(0) == cf);
		assert(d.toString() == "10");
		assert(d.at<std::string>(0) == "10");
		
		d.assign(11.f);
		assert(d.at<float>(0) == 11.f);
		
		d.clone();
//		assert(d.data<float>() != &cf);
		assert(d.hasData());
		assert(d.size() == 1);
		assert(d.type() == Data::FLOAT);
//		assert(d.at<float>(0) == cf);

		d.assign(11.f);
		assert(d.at<float>(0) == 11.f);

		d.assign(100);
		assert(d.at<float>(0) == 100);
		
		d.set(f);
		assert(d.data<float>() == &f);
		assert(d.at<float>(0) == f);
		
//		d.set(s);
//		assert(d.type() == Data::STRING);
//		assert(d.size() == 1);
//		assert(d.data<std::string>() == &s);
//		assert(d.at<std::string>(0) == s);
//		
//		d.clone();
//		assert(d.data<std::string>() != &s);
//		assert(d.at<std::string>(0) == s);
//		d.data<std::string>()[0] = "test";
//		assert(s == "hello");
//		assert(d.at<std::string>(0) == "test");
		
		d.set("hello");
		assert(d.type() == Data::STRING);
		assert(d.at<std::string>(0) == "hello");
		//d.clone();

		{
			std::string s = "d2";
			Data d2(s);
			
			d.set("d");
			d2.assign(d);
			assert(d2.at<std::string>(0) == d.at<std::string>(0));
		}
	}

	// model to string conversion
	{
		Label l;
		TextView tv;
//		Button b;
//		Buttons bs(Rect(), 2, 2);
		Button b;
		Buttons bs(Rect(), 2, 2);
		Slider s;
		Sliders ss(Rect(), 1, 4);
		Slider2D s2D;
		NumberDialer nd(1,4,1,-1);
		std::string strings[] = {"test1", "test2", "test3"};
	
//		// Method #1: implicit
//		// Advantage:		simplicity- just name Views to be part of model
//		// Disadvantages:	cannot add arbitrary model data not contained in a View
//		//					all View's must have an extra name variable
		l.value("Hello Label!").name("ls");
		tv.value("Hello TextView!").name("tv");
		b.setValue(true).name("b");
		bs.setValue(true, 0,1).name("bs");
		s.value(0.5).name("s");
		ss.value(0.4, 1).name("ss");
		s2D.value(0.5, 0).value(0.1, 1).name("s2D");
		nd.value(-0.54941).name("nd");

		const View * views[] = {&l, &tv, &b, &bs, &s, &ss, &s2D, &nd};
		for(unsigned i=0; i<sizeof(views)/sizeof(View *); ++i){
			const Data& d = views[i]->model();
			printf("%p: %s\n", &d, d.toString().c_str());
			//views[i]->model().print();
		}

//		View top;
//		top << l << tv << b << bs << s << ss << s2D << nd;
//		
//		std::string str1, str2;
//		top.modelToString(str1, "test model");		
//		printf("size=%d\n%s\n", str1.size(), str1.c_str());
//		
//		top.modelFromString(str1);
//		top.modelToString(str2);
//		//printf("%s\n", str2.c_str());
//		//assert(str1 == str2);


//		// Method #2: explicit
//		// Advantage: can add arbitrary data to model
//		// Disadvantage: 
//		ModelManager mm;
//		
//		mm.add("l", l);
//		mm.add("tv", tv);
//		mm.add("b", b);
//		mm.add("bs", bs);
//		mm.add("s" , s);
//		mm.add("ss", ss);
//		mm.add("s2D",s2D);
//		mm.add("nd", nd);
//		mm.add("strings", *new Data(strings, 3));
//		
//		mm.toString(str1, "test");
//		printf("%s\n", str1.c_str());
//		
//		mm.fromString(str1);
//		mm.toString(str2, "test");
//		printf("%s\n", str2.c_str());

//		// How can we send notifications when values change?
//		StateSpace states;
//		states.add("strings", Data(strings, 3));
//		states.add("tv", Data(tv.value()));
//		states.add("l", Data(l.value()));
//		states.add("b", Data(b.values()[0]));
//		states.add("bs", Data(&bs.values()[0], bs.size()));
//		states.add("s", Data(s.values()[0]));
//		states.add("ss", Data(&ss.values()[0], ss.size()));
//		//states.add("nd", Data(nd.value()))
//		
//		states.saveState("state 1");
//		states.stateToString(str1, "state 1");
////		printf("%s\n", str1.c_str());
//		
//		bs.value(true, 0);
//		s.value(0.1);
//		ss.value(0.2, 0);
//		states.saveState("state 2");
//		
//		states.loadState("state 1");
//		states.stateToString(str1);
//		printf("%s\n", str1.c_str());
//
//		states.loadState("state 2");
//		states.stateToString(str1);
//		printf("%s\n", str1.c_str());

	}

//	{
//		std::string str( "a bc ab ca ab" );
//		std::string searchString("ab"); 
//		std::string replaceString("hello");
//
//		std::string::size_type pos = 0;
//		while((pos = str.find(searchString, pos)) != std::string::npos){
//			str.replace(pos++, searchString.size(), replaceString);
//		}
//		printf("%s\n", str.c_str());
//	}
//	printf("Notifier: %d\n", sizeof(Notifier));
//	printf("View: %d\n", sizeof(View));
//	printf("Data: %d\n", sizeof(Data));

	return 0;
}
