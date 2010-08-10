#ifndef INC_GLV_UTIL_H
#define INC_GLV_UTIL_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <cctype>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <list>

namespace glv {


template <class T>
inline T clip(const T& v, const T& mx=T(1), const T& mn=T(0)){ return v<mn ? mn : (v>mx ? mx : v); }

template <class T> inline T max(const T& a, const T& b){ return a<b ? b:a; }
template <class T> inline T min(const T& a, const T& b){ return a<b ? a:b; }
template <class T> inline T mean(const T& a, const T& b){ return (a+b)*T(0.5); }

/// Sort arguments so that a<b
template<class T> inline void sort(T& a, T& b){ if(a>b){ T t=a; a=b; b=t; }  }

/// Convert a string to a value type. Returns number of elements converted.
template<class T>
int fromString(T& dst, const std::string& src);

/// Convert string to a value array. Returns number of elements converted.
template<class T>
int fromString(T * dst, int size, const std::string& src);

/// Convert a value type to a string
template<class T>
void toString(std::string& dst, const T& src);

/// Convert a value array to a string
template<class T>
void toString(std::string& dst, const T * src, int size);


/// Smart pointer functionality to avoid deleting references.

///	When a class contains pointers to other objects, sometimes it is not clear
///	if it is responsible for freeing the memory associated with those objects.
class SmartPointer{
public:

	/// @param[in] del	whether the object is deleted with smartDelete()
	SmartPointer(bool del=true): mDeletable(del){}

	virtual ~SmartPointer(){}

	/// Deletes object only if it is deletable, i.e. not a reference.
	
	/// Returns true if the object destructor was called, false otherwise.
	///
	bool smartDelete(){
		//printf("%p %c\n", this, mDeletable ? 'y' : 'n');
		if(deletable()){ delete this; return true; }
		return false;
	}
	
	/// Set whether the object can be deleted.
	void deletable(bool v){ mDeletable = v; }
	
	/// Returns whether the object can be deleted.
	bool deletable(){ return mDeletable; }
	
//	void operator delete (void * p){
//		SmartPointer * pc = static_cast<SmartPointer *>(p);
//		if(pc && pc->mIsDeletable){
//			free(pc);
//			printf("freed\n");
//		}
//		else printf("denied\n");
//	}
	
private:
	bool mDeletable;
};



/// Mixin class for doing advanced memory management

/// The template parameter should be the base class in the derivation chain.
///
template <class BaseClass>
struct SmartObject {

	typedef std::list<void *> NewObjectList;

	SmartObject();

	void * operator new(size_t sz);
	void * operator new(size_t sz, int flag);
	void * operator new(size_t sz, void * m){ return m; }
	void operator delete(void * m){ free(m); }
	
	/// Returns true if the object was created dynamically with the new operator, false otherwise.
	bool dynamicAlloc() const { return mDynamicAlloc; }

private:
	bool mDynamicAlloc;
	
	// Returns true if this is within memory footprint of the base class
	bool withinFootPrint(void * m);
	
	// Temporarily list of 'new' objects
	static NewObjectList& newObjects(){
		static NewObjectList * l = new NewObjectList; return *l;
	}
};



/// Array
template <class T>
class Array{
public:

	Array(): mSizeX(0), mSizeY(0), mVal(0){}
	Array(int nx, int ny=1): mSizeX(0), mSizeY(0), mVal(0) { resize(nx, ny); }
	
	~Array(){ clear(); }

	// interface methods
	T& operator[](int i){ return mVal[i]; }
	const T& operator[](int i) const { return mVal[i]; }

	Array& operator= (const T& v){ for(int i=0; i<size(); ++i) (*this)[i]=v; return *this; }
	
	Array& data(){ return *this; }
	const Array& data() const { return *this; }
	
	void resize(int nx, int ny){
		if(nx*ny != size()){
			clear();
			mVal = new T[nx*ny];
			mSizeX = nx; mSizeY = ny;
		}
	}
	
	int size()  const { return sizeX() * sizeY(); }
	int sizeX() const { return mSizeX; }
	int sizeY() const { return mSizeY; }
	
	void zero(){ memset(mVal, 0, size()*sizeof(T)); }
	
protected:
	int mSizeX, mSizeY;
	T * mVal;
	void clear(){ if(mVal) delete[] mVal; mVal=0; }
};




/// One or more static values
template <class T, int Nx=1, int Ny=1>
class Values{
public:

	// interface methods
	T& operator[](int i){ return mVal[i]; }
	const T& operator[](int i) const { return mVal[i]; }
	
	Values& operator= (const T& v){ for(int i=0; i<size(); ++i) (*this)[i]=v; return *this; }
	
	Values& data(){ return *this; }
	const Values& data() const { return *this; }
	
	void resize(int nx, int ny){}
	
	int size()  const { return sizeX()*sizeY(); }
	int sizeX() const { return Nx; }
	int sizeY() const { return Ny; }
	
	void zero(){ memset(mVal, 0, size()*sizeof(T)); }

protected:
	T mVal[Nx*Ny];
};




// Implementation ______________________________________________________________
template<class T>
SmartObject<T>::SmartObject(): mDynamicAlloc(false){
	//printf("%x: SmartObject::SmartObject()\n", this);
	// Check new list to see if this was dynamically allocated.
	// If so, then remove from list and set internal flag.
	NewObjectList& l = newObjects();
	for(NewObjectList::iterator it=l.begin(); it!=l.end(); ++it){
		
		// check if we lie within the memory footprint of the base class
		if(withinFootPrint(*it)){
			mDynamicAlloc=true;
			l.erase(it);
			break;
		}
	}
}

template<class T>
void * SmartObject<T>::operator new(size_t sz){
	void * m = malloc(sz);	// this will point to the base class
	newObjects().push_back(m);
	//printf("%x: SmartObject::operator new(size_t)\n", m);
	return m;
}

template<class T>
void * SmartObject<T>::operator new(size_t sz, int flag){
	void * m = malloc(sz);	// this will point to the base class
	newObjects().push_back(m);
	return m;
}

template<class T>
bool SmartObject<T>::withinFootPrint(void * m){
//	T * b = (T *)m;
//	return (this >= b) && (this < (b + sizeof(T)));
	char * b = (char *)m;
	char * t = (char *)this;
	//printf("%x, %x, %x\n", t, b, b+sizeof(T));
	return (t >= b) && (t < (b + sizeof(T)));
}



template<class T> void toString(std::string& dst, const T * src, int size){
	dst = "{";
	std::string temp;
	for(int i=0; i<size; ++i){
		toString(temp, src[i]);
		dst += temp;
		if(i != (size-1)) dst += ", ";
	}
	dst += "}";
}





//
//// Triply-linked node
//
//// Getters must return pointers since it is possible that some linked nodes
//// are zero, i.e. nonexistant.
//template <class T>
//class Node3{
//public:
//
//	Node3(): mParent(0), mChild(0), mSibling(0){}
//
//	virtual ~Node3(){ remove(); }
//
//	void makeLastSibling();
//
//	void pushBack(Node3 * node);		///< Add node to back of my children
//	void pushBack(Node3 & node);		///< Add node to back of my children
//	
//	/// Remove myself from parent. Does not delete instance.
//	
//	/// This method should always be called from the instance's destructor.
//	///
//	void remove();
//
//	void child(Node3 * o){ mChild=o; }
//	void parent(Node3 * o){ mParent=o; }
//	void sibling(Node3 * o){ mSibling=o; }
//	void child(Node3& o){ mChild=&o; }
//	void parent(Node3& o){ mParent=&o; }
//	void sibling(Node3& o){ mSibling=&o; }
//
//	Node3 * child() const { return mChild; }
//	Node3 * parent() const { return mParent; }
//	Node3 * sibling() const { return mSibling; }
//
//	Node3 * lastSibling();
//	
//	bool firstChild() const { return parent() && (parent()->child() == this); }
//
//private:
//	Node3 * mParent;			// My parent view
//	Node3 * mChild;				// My first child (next to be drawn)
//	Node3 * mSibling;			// My next sibling view (drawn after all my children)
//};
//
//
//
//template <class T>
//void Node3<T>::pushBack(Node3 * node){
//	if(node) pushBack(*node);
//}
//
//template <class T>
//void Node3<T>::pushBack(Node3 & node){
//	node.remove();
//	node.parent(this);
//	
//	// I didn't have any children until now
//	if(!child())	child(node);
//	else			child()->lastSibling()->sibling(node);
//}
//
//template <class T>
//Node3<T> * Node3<T>::lastSibling(){
//	Node3 * t = this;
//	while(t->sibling()) t = t->sibling();
//	return t;
//}
//
//template <class T>
//void Node3<T>::makeLastSibling(){
//	if(parent() && sibling()){
//		parent()->pushBack(this); // this automatically removes node
//	}
//}
//
//template <class T>
//void Node3<T>::remove(){
//
//	if(parent() && parent()->child()){
//
//		// re-patch parent's child?
//		if(firstChild()){
//			// remove my reference, but keep the sibling list healthy
//			parent()->child(sibling());
//		}
//		
//		// re-patch the sibling chain?
//		else{
//			// I must be one of parent->child's siblings
//			Node3 * temp = parent()->child();
//			while(temp->sibling()){
//				if(temp->sibling() == this) {
//					// I'm temp's sibling
//					// - remove my reference, keep the sibling list healthy
//					temp->sibling(this->sibling()); 
//					break; 
//				}
//				temp = temp->sibling();
//			}
//		}
//		
//		parent(0); sibling(0); // no more parent or sibling, but child is still valid
//	}
//}

//class NumberEntry{
//public:
//	#define BUFSIZE 16
//	// 123456789012345678901234567890
//	// 1234567890123456
//
//	NumberEntry(){ reset(); }
//	
//	// interpret key press
//	// returns true if successful, false otherwise
//	bool read(char c){
//	
//		if(		mPos < BUFSIZE
//			&&	(isdigit(c) || (c == '.' && !mHasPoint) || c == '-')
//		){
//			switch(c){
//			case '-': mBuf[0] = mBuf[0] == '-' ? ' ' : '-'; break;
//			case '.': mHasPoint = true;
//			default : 
//				if(mBuf[mPos] == '.') mHasPoint=false;
//				mBuf[mPos++] = c;
//				if(mPos>=mNum) mNum++;
//			}
//			//printf("%s\n", mBuf); printf("%f\n", value());
//			return true;
//		}
//		return false;
//	}
//
//	
//	double value(){		
//		char * end;
//		double v = strtod(mBuf, &end);
//		if(end != mBuf) mVal = v;
//		return mVal;
//	}
//	
//
//	void value(double v){
//		char t[BUFSIZE];
//#ifdef WIN32
//#define snprintf _snprintf
//#endif
//		int n = snprintf(t, BUFSIZE-1, "% g", v);	// %f is not precise enough!
//#ifdef WIN32
//#undef snprintf
//#endif
//		
//		if(n>1){
//			reset();
//
//			// eliminate trailing zeros
//			while(n-1){
//				if(t[n-1] == '0' || t[n-1] == '.'){ t[n-1] = '\0'; --n; }
//				else break;
//			}
//			
//			if(n==1) read('0');
//			else{
//				for(int i=0; i<n; ++i){ read(t[i]); /*printf(" %d", t[i]);*/}
//			}
//		}
//		printf("\n");
//	}
//
//
//	void reset(){
//		mBuf[0] = ' ';
//		memset(mBuf + 1, 0, (BUFSIZE-1));
//		mPos = 1, mNum = 1;
//		mHasPoint = false;
//		mVal = 0;
//	}
//
//	
//	void del(){
//		if(pos() > 1){
//			for(int i=pos(); i<=mNum; ++i){
//				if(mBuf[i-1] == '.') mHasPoint = false;
//				mBuf[i-1] = mBuf[i];
//			}
//			mBuf[mNum] = '\0';
//			bwd1(); mNum--;
//		}
//	}
//
//	
//	void bwd1(){ if(pos() > 1) mPos--; }
//	void fwd1(){ if(pos() < mNum) mPos++; }
//	
//	void addAtPos(int v){
//		char c = mBuf[pos()];
//		if(isdigit(c)){
//			c += v;
//			mBuf[pos()] = c > '9' ? '9' : c < '0' ? '0' : c;
//		}
//	}
//	
//	const char * buf(){ return mBuf; }
//	int pos(){ return mPos; }
//	
//private:
//	char mBuf[BUFSIZE];
//	int mPos, mNum;
//	double mVal;
//	bool mHasPoint;
//	#undef BUFSIZE
//};

//template <class T, int Nx=1, int Ny=1>
//class RangedValues : public Values<T, Nx, Ny>{
//public:
//
//	T& operator[](int i){ return mVal[i]; }
//	const T& operator[](int i) const { return mVal[i]; }
//
//	RangedValues& range(T max, T min=0){ mMax=max; mMin=min; return *this; }
//
//	
//
//protected:
//	T mMin, mMax;
//	
//	void valueSet(T v){
//		v>mMax ? v=mMax : v<mMin ? v=mMin : 0;
//		value()[selected()] = v;
//	}
//	
//};

//template <class V>
//class ValuesRef{
//public:
//	ValuesRef(): mVal(0){}
//
//	V& data(){ return *mVal; }
//	const V& data() const { return *mVal; }
//
//	// interface methods
//	void resize(int nx, int ny){}
//	
//	void ref(V& r){ mVal = &r; }
//	
//protected:
//	V * mVal;
//};


// We something that wraps around more atomic types to give them all a common
// interface. However, if the atomic type has the interface methods already,
// they should be used. Otherwise, the wrapper's default methods should be called.


}	// glv::

#endif

