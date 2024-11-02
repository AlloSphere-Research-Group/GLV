#ifndef INC_GLV_UTIL_H
#define INC_GLV_UTIL_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <cstdlib> // malloc
#include <cstring> // memset
#include <cmath>
#include <list>
#include <vector>

namespace glv {


/// Return value clipped to interval [min, max)
template <class T>
inline T clip(const T& v, const T& max=T(1), const T& min=T(0)){ return v<min? min:(v>max? max:v); }

template <class T> inline T floor(const T& v, const T& step){ return std::floor(v/step)*step; }

template <class T> inline T max(const T& a, const T& b){ return a<b ? b:a; }
template <class T> inline T min(const T& a, const T& b){ return a<b ? a:b; }
template <class T> inline T mean(const T& a, const T& b){ return (a+b)*T(0.5); }

/// Sort arguments so that a<b
template<class T> inline void sort(T& a, T& b){ if(a>b){ T t=a; a=b; b=t; }  }



/// Lazy initialization wrapper
template<class T>
class Lazy{
public:
	Lazy(): mValue(0){}
	~Lazy(){ clear(); }

	const T& operator()() const { return create(); }
	T& operator()(){ return create(); }

	bool created() const { return 0!=mValue; }
	void clear(){ delete mValue; mValue=0; }

protected:
	T& create() const { if(!mValue){ mValue = new T(); } return *mValue; }
	mutable T * mValue;
};


/// Smart pointer functionality to avoid deleting references.

///	When a class contains pointers to other objects, sometimes it is not clear
///	if it is responsible for freeing the memory associated with those objects.
class SmartPointer{
public:

	/// \param[in] del	whether the object is deleted with smartDelete()
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
	void operator delete(void * m){ std::free(m); }
	
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

	void zero(){ std::memset(mVal, 0, size()*sizeof(T)); }
	
protected:
	int mSizeX, mSizeY;
	T * mVal;
	void clear(){ if(mVal) delete[] mVal; mVal=0; }
};


/// Array optimized for dynamically changing its size
template <class T, class Alloc=std::allocator<T> >
class Buffer : protected Alloc{
public:

	/// \param[in] capacity		capacity of buffer
	explicit Buffer(int capacity=0): mSize(0), mElems(capacity){}

	~Buffer(){}

	/// Get mutable element at index
	T& operator[](int i){ return mElems[i]; }
	
	/// Get const element at index
	const T& operator[](int i) const { return mElems[i]; }

	/// Returns number of elements before needing to allocate more memory
	int capacity() const { return mElems.capacity(); }

	/// Returns number of elements
	int size() const { return mSize; }
	
	const T& last() const { return mElems[size()-1]; }

	/// Assign value to elements

	/// This function fills a Buffer with n copies of the given value. Note that
	/// the assignment completely changes the buffer and that the resulting size
	/// is the same as the number of elements assigned.  Old data may be lost.
	void assign(int n, const T& v){ mElems.assign(n,v); }

	/// Empty buffer
	void clear(){ mElems.clear(); reset(); }

	/// Set size of buffer back to 0
	void reset(){ setSize(0); }

	/// Resize buffer

	/// This will set both the size and capacity of the buffer to the requested 
	/// size. If the number is smaller than the current size the buffer is 
	/// truncated, otherwise the buffer is extended and new elements are
	/// default-constructed.
	void resize(int n){
		mElems.resize(n); setSize(n);
	}
	
	/// Set size of buffer avoiding memory reallocation when possible
	
	/// If the requested size is larger than the current capacity, then the 
	/// buffer will be resized.
	void size(int n){
		(capacity() >= n) ? setSize(n) : resize(n);
	}

	/// Appends element to end of buffer growing its size if necessary
	void append(const T &v, double growFactor=2){
		if(size() >= capacity()){
			T vcopy = v; // 'v' may become invalidated after resize
			mElems.resize(size() ? int(size()*growFactor) : 4);
			Alloc::construct(&mElems[0] + size(), vcopy);
		}
		else{
			Alloc::construct(&mElems[0] + size(), v);
		}
		++mSize;
	}

private:
	int mSize;
	std::vector<T, Alloc> mElems;
	void setSize(int n){ mSize=n; }
};


/// A closed interval [min, max]

/// An interval is a connected region of the real line. Geometrically, it
/// describes a 0-sphere. Order is strongly enforced so that the endpoints will
/// always satisfy min <= max.
template <class T=double>
class Interval{
public:

	Interval()
	:	mMin(0), mMax(1){}

	Interval(const T& max)
	{ endpoints(0,max); }

	Interval(const T& min, const T& max)
	{ endpoints(min,max); }

	T center() const { return (max()+min())/T(2); }	///< Returns center point

	/// Test is point is contained exclusively within interval
	bool contains(const T& v) const { return v>=min() && v<=max(); }

	bool degenerate() const { return min()==max(); }///< Returns true if diameter is zero
	T diameter() const { return max()-min(); }		///< Returns absolute difference of endpoints
	const T& max() const { return mMax; }			///< Get maximum endpoint
	const T& min() const { return mMin; }			///< Get minimum endpoint
	bool proper() const { return min()!=max(); }	///< Returns true if diameter is non-zero
	T radius() const { return diameter()/T(2); }	///< Returns one-half the diameter

	/// Linearly map point in unit interval to point in interval
	T fromUnit(const T& v) const { return v*diameter() + min(); }

	/// Linearly map point in interval to point in the unit interval
	T toUnit(const T& v) const { return (v-min())/diameter(); }
	
	/// Linearly map point from interval to argument interval
	template <class U>
	T to(const T& v, const Interval<U>& i){ return i.fromUnit(toUnit(v)); }
	
	/// Returns true if both endpoints are equal
	template <class U>
	bool operator == (const Interval<U>& v){ return min()==v.min() && max()==v.max(); }

	/// Returns true if endpoints are not equal
	template <class U>
	bool operator != (const Interval<U>& v){ return !(*this == v); }
	
	template <class U>
	Interval& operator +=(const Interval<U>& v){ endpoints(min()+v.min(), max()+v.max()); return *this; }

	template <class U>
	Interval& operator -=(const Interval<U>& v){ endpoints(min()-v.max(), max()-v.min()); return *this; }
	
	template <class U>
	Interval& operator *=(const Interval<U>& v){
		T a=min()*v.min(), b=min()*v.max(), c=max()*v.min(), d=max()*v.max();
		mMin = min(min(a,b),min(c,d));
		mMax = max(max(a,b),max(c,d));
		return *this;
	}

	template <class U>
	Interval& operator /=(const Interval<U>& v){
		T a=min()/v.min(), b=min()/v.max(), c=max()/v.min(), d=max()/v.max();
		mMin = min(min(a,b),min(c,d));
		mMax = max(max(a,b),max(c,d));
		return *this;
	}

	/// Set center point preserving diameter
	Interval& center(const T& v){ return centerDiameter(v, diameter()); }

	/// Set diameter (width) preserving center
	Interval& diameter(const T& v){ return centerDiameter(center(), v); }

	/// Set center and diameter
	Interval& centerDiameter(const T& c, const T& d){
		mMin = c - d*T(0.5);
		mMax = mMin + d;
		return *this;
	}

	/// Set the endpoints
	Interval& endpoints(const T& min, const T& max){
		mMax=max; mMin=min;
		if(mMin > mMax){ T t=mMin; mMin=mMax; mMax=t; }
		return *this;
	}


	/// Scale interval
	Interval& scale(const T& v){
		mMin *= v; mMax *= v;
		return *this;
	}

	/// Scale interval
	
	/// \param[in] v			scale amount
	/// \param[in] centerFrac	scaling center as fraction between endpoints 
	Interval& scale(const T& v, const T& centerFrac){
		T trans = fromUnit(centerFrac);
		translate(-trans);
		mMin *= v; mMax *= v;
		translate( trans);
		return *this;
	}

	/// Translate interval by fixed amount
	Interval& translate(const T& v){
		mMin += v; mMax += v;
		return *this;
	}


	/// Set maximum endpoint
	Interval& max(const T& v){ return endpoints(min(), v); }
	
	/// Set minimum endpoint
	Interval& min(const T& v){ return endpoints(v, max()); }

private:
	T mMin, mMax;

	const T& min(const T& a, const T& b){ return a<b?a:b; }
	const T& max(const T& a, const T& b){ return a>b?a:b; }
};



/// Maps characters (i.e., keyboard keys) to integers
template <int KeyMax=128, int KeyMin=0>
class CharMap{
public:
	CharMap(const char * keySeq="", char begin=0, char unmappedVal=-128)
	:	mUnmapped(unmappedVal)
	{
		reset();
		set(keySeq, begin);
	};

	static int size(){ return N; }

	char operator[](int key) const { return mMap[key-KeyMin]; }
	char& operator[](int key){ return mMap[key-KeyMin]; }

	bool mapped(int key) const { return (*this)[key] != mUnmapped; }

	CharMap& set(const char * keySeq, char begin=0){
		int c=0;
		while(keySeq[c]){
			(*this)[int(keySeq[c])] = begin;
			++c; ++begin;
		}
		//for(int i=0; i<128; ++i) printf("%3d %2d\n", i, mMap[i]);
		return *this;
	}

	CharMap& set(char index, char value){
		(*this)[int(index)] = value; return *this;
	}

	void reset(){ for(int i=0; i<size(); ++i) mMap[i] = mUnmapped; }

private:
	static const int N = KeyMax-KeyMin+1;
	char mMap[N];
	char mUnmapped;
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
	void * m = std::malloc(sz);	// this will point to the base class
	newObjects().push_back(m);
	//printf("%x: SmartObject::operator new(size_t)\n", m);
	return m;
}

template<class T>
void * SmartObject<T>::operator new(size_t sz, int flag){
	void * m = std::malloc(sz);	// this will point to the base class
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

}	// glv::

#endif

