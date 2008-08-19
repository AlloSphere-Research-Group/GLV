#ifndef INC_GLV_UTIL_H
#define INC_GLV_UTIL_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


namespace glv {

/// Smart pointer functionality to avoid deleting references.

///	When a class contains pointers to other objects, sometimes it is not clear
///	if it is responsible for freeing the memory associated with those objects.
class SmartPointer{
public:

	/// @param[in] del	whether the object is deleted with smartDelete()
	SmartPointer(bool del=true): mDeletable(del){}

	/// Deletes object only if it is deletable, i.e. not a reference.
	void smartDelete(){
		//printf("%p %c\n", this, mDeletable ? 'y' : 'n');
		if(true == mDeletable) delete this;
	}
	
	/// Set whether the object can be deleted.
	void deletable(bool v){ mDeletable = v; }
	
	/// Returns whether the object can be deleted.
	bool deletable(){ return mDeletable; }
	
//	void operator delete (void * p){
//		
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




// Array
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




class NumberEntry{
public:
	#define BUFSIZE 16
	// 123456789012345678901234567890
	// 1234567890123456

	NumberEntry(){ reset(); }
	
	// interpret key press
	// returns true if successful, false otherwise
	bool read(char c){
	
		if(		mPos < BUFSIZE
			&&	(isdigit(c) || (c == '.' && !mHasPoint) || c == '-')
		){
			switch(c){
			case '-': mBuf[0] = mBuf[0] == '-' ? ' ' : '-'; break;
			case '.': mHasPoint = true;
			default : 
				if(mBuf[mPos] == '.') mHasPoint=false;
				mBuf[mPos++] = c;
				if(mPos>=mNum) mNum++;
			}
			//printf("%s\n", mBuf); printf("%f\n", value());
			return true;
		}
		return false;
	}

	
	double value(){		
		char * end;
		double v = strtod(mBuf, &end);
		if(end != mBuf) mVal = v;
		return mVal;
	}
	

	void value(double v){
		char t[BUFSIZE];
#ifdef WIN32
#define snprintf _snprintf
#endif
		int n = snprintf(t, BUFSIZE-1, "% g", v);	// %f is not precise enough!
#ifdef WIN32
#undef snprintf
#endif
		
		if(n>1){
			reset();

			// eliminate trailing zeros
			while(n-1){
				if(t[n-1] == '0' || t[n-1] == '.'){ t[n-1] = '\0'; --n; }
				else break;
			}
			
			if(n==1) read('0');
			else{
				for(int i=0; i<n; ++i){ read(t[i]); /*printf(" %d", t[i]);*/}
			}
		}
		printf("\n");
	}


	void reset(){
		mBuf[0] = ' ';
		memset(mBuf + 1, 0, (BUFSIZE-1));
		mPos = 1, mNum = 1;
		mHasPoint = false;
		mVal = 0;
	}

	
	void del(){
		if(pos() > 1){
			for(int i=pos(); i<=mNum; ++i){
				if(mBuf[i-1] == '.') mHasPoint = false;
				mBuf[i-1] = mBuf[i];
			}
			mBuf[mNum] = '\0';
			bwd1(); mNum--;
		}
	}

	
	void bwd1(){ if(pos() > 1) mPos--; }
	void fwd1(){ if(pos() < mNum) mPos++; }
	
	void addAtPos(int v){
		char c = mBuf[pos()];
		if(isdigit(c)){
			c += v;
			mBuf[pos()] = c > '9' ? '9' : c < '0' ? '0' : c;
		}
	}
	
	const char * buf(){ return mBuf; }
	int pos(){ return mPos; }
	
private:
	char mBuf[BUFSIZE];
	int mPos, mNum;
	double mVal;
	bool mHasPoint;
	#undef BUFSIZE
};



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

