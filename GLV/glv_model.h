#ifndef INC_GLV_MODEL_H
#define INC_GLV_MODEL_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <map>
#include <vector>
#include <string>
#include <stdio.h>

namespace glv {

class Data;

/// Determines whether string is a valid C identifier for a variable
bool isIdentifier(const std::string& s);

/// Convert object to a string. Returns number of elements converted.
template<class T>
int toString(std::string& dst, const T& src);
int toString(std::string& dst, const char * src);

/// Convert value to a string with user-defined formatting
template<class T>
int toString(std::string& dst, const T& src, const char * format);

/// Convert objects to a string. Returns number of elements converted.
template<class T>
int toString(std::string& dst, const T * src, int size, int stride=1);

/// Returns a stringification of an object
template <class T>
inline std::string toString(const T& obj){ std::string r; toString(r,obj); return r; }


/// Convert string token to object. Returns number of elements converted.
template<class T>
int fromToken(T& dst, const std::string& src);

/// Convert string token to objects. Returns number of elements converted.
template<class T>
int fromToken(T * dst, int size, int stride, const std::string& src);

/// Convert object to a string token. Returns number of elements converted.
template<class T>
int toToken(std::string& dst, const T& src);
int toToken(std::string& dst, const char * src);

/// Convert objects to a string token. Returns number of elements converted.
template<class T>
int toToken(std::string& dst, const T * src, int size, int stride=1);

/// Returns string token of an object
template <class T>
inline std::string toToken(const T& obj){ std::string r; toToken(r,obj); return r; }



/// Convert Data object to string
int toString(std::string& dst, const Data& src);

/// Convert Data object to token string
int toToken(std::string& dst, const Data& src);

/// Assign Data object elements from string
int fromToken(Data& dst, const std::string& src);



class ReferenceCounter {
public:

	/// Returns number of pointers to memory address being managed
	static int references(void * const m){ return counting(m) ? refCount()[m] : 0; }

protected:
	typedef std::map<void *, int> RefCount;

	// Begin counting reference or increment count if already counting
	static void acquire(void * m){
		if(!m) return; // do not count null pointers for obvious reasons
		counting(m) ? ++refCount()[m] : refCount()[m]=1;
	}
	
	// Increment count of reference if reference is being counted
	static void incrementCount(void * m){
		if(counting(m))	++refCount()[m];
	}

	// Returns true if the reference was being counted and can now be freed
	static bool release(void * m){
		if(counting(m)){
			int& c = refCount()[m];
			if(1==c){
				refCount().erase(m);
				return true;
			}
			--c;
		}
		return false;
	}

	static RefCount& refCount(){
		static RefCount * o = new RefCount;
		return *o;
	}
	
	// is reference being counted already?
	static bool counting(void * const m){ return refCount().count(m) != 0; }
//	static bool owner(void * const m){ return references(m) != 1; }
};




/// Iterates through multidmensional arrays
class Indexer{
public:

	/// @param[in] size1	size of dimension 1
	/// @param[in] size2	size of dimension 2
	/// @param[in] size3	size of dimension 3
	Indexer(int size1=1, int size2=1, int size3=1){
		reset();
		int sizes[] = {size1,size2,size3};
		setSizes(sizes);
	}

	/// @param[in] sizes	array of dimension sizes
	Indexer(const int * sizes){
		reset(); setSizes(sizes);
	}

	/// Perform one iteration returning whether more elements exist
	bool operator()() const {
		if(++mIndex[0] == mSizes[0]){
			if(++mIndex[1] == mSizes[1]){
				if(++mIndex[2] == mSizes[2]){
					return false;
				}
				mIndex[1] = 0;
			}
			mIndex[0] = 0;
		}
		return true;
	}

	/// Get current index within a dimension
	int operator[](int dim) const { return mIndex[dim]; }

	/// Get one-dimensional index into plane
	int indexFlat(int dim1, int dim2) const { return mIndex[dim2]*size(dim1) + mIndex[dim1]; }

	/// Get current position within a dimension in half-open unit interval [0, 1)
	double frac(int dim) const { return double(mIndex[dim])/mSizes[dim]; }
	
	/// Get current position within a dimension in closed unit interval [0, 1]
	double fracClosed(int dim) const { return double(mIndex[dim])/(mSizes[dim]-1); }

	/// Get size of a dimension
	int size(int dim) const { return mSizes[dim]; }

	/// Get product of sizes of two dimensions
	int size(int dim1, int dim2) const { return mSizes[dim1]*mSizes[dim2]; }
	
	/// Get product of sizes of all dimensions
	int size() const { int r=1; for(int i=0; i<N; ++i) r*=size(i); return r; }

	/// Reset position indices
	Indexer& reset(){ mIndex[0]=-1; for(int i=1; i<N; ++i){mIndex[i]=0;} return *this; }

	/// Set dimensions
	Indexer& shape(const int * sizes, int n){ setSizes(sizes,n); return *this; }

	/// Set dimensions
	Indexer& shape(int size1, int size2=1, int size3=1){
		int sizes[] = {size1, size2, size3};
		return shape(sizes, 3);
	}

protected:
	enum{N=3};				// max number of dimensions
	mutable int mIndex[N];	// indices of current position in array
	int mSizes[N];			// dimensions of array
	void setSizes(const int* v, int n=N){ for(int i=0;i<n;++i) mSizes[i]=v[i]; }
};




#ifndef DATA_MAXDIM
#define DATA_MAXDIM 4
#endif


/// Dynamically typed multidimensional array of primitive values

/// For operations between data with different types, standard type conversions
/// will be used when possible.
/// For binary operations between arrays, each array is treated as a 
/// one-dimensional array. If the number of elements differ, then the maximum 
/// possible number of elements are used in the comparison.
class Data : public ReferenceCounter {
public:

	/// Data types
	enum Type{
		BOOL=0,		/**< */
		INT,		/**< */
		FLOAT,		/**< */
		DOUBLE,		/**< */
		STRING,		/**< */
		NONE		/**< */
	};
	static std::string typeToString(Type t);

	
	template<class T> static Type getType();


	/// Value returned by various member functions when they fail
	static const int npos = static_cast<int>(-1);


	/// This sets the data type to void and does not allocate memory
	Data();
	
	/// @param[in] v		data to reference
	Data(Data& v);
	
	/// @param[in] v		data to make clone of
	Data(const Data& v);

	/// Constructor that sets internal attributes without allocating memory

	/// @param[in] type		type of data
	/// @param[in] size1	size of dimension 1 
	/// @param[in] size2	size of dimension 2
	/// @param[in] size3	size of dimension 3
	/// @param[in] size4	size of dimension 4
	Data(Data::Type type, int size1=1, int size2=1, int size3=1, int size4=1);

	/// @param[in] value	external value to reference
	template <class T>
	explicit Data(T& value){ init(); set(&value, 1); }

	/// @param[in] value	value to make clone of
	template <class T>
	explicit Data(const T& value){ init(); set(&value, 1); }

	/// @param[in] data		pointer to external data to reference
	/// @param[in] size1	size of dimension 1 of external data
	/// @param[in] size2	size of dimension 2 of external data
	/// @param[in] size3	size of dimension 3 of external data
	/// @param[in] size4	size of dimension 4 of external data
	template <class T>
	Data(T * data, int size1, int size2=1, int size3=1, int size4=1){
		init();
		int sizes[] = {size1,size2,size3,size4};
		set(data, sizes, 4);
	}

	~Data();


	/// Returns whether elements from each array are equal
	bool operator== (const Data& v) const;
	
	/// Returns whether elements from each array are not equal
	bool operator!= (const Data& v) const { return !((*this)==v); }
	
	Data& operator+= (const Data& v);

	/// Get element at 1D index, performing type conversion if necessary
	template <class T> T at(int idx) const;

	/// Get element at 2D index, performing type conversion if necessary
	template <class T> T at(int i1, int i2) const;

	/// Get element at 3D index, performing type conversion if necessary
	template <class T> T at(int i1, int i2, int i3) const;

	/// Get element at 4D index, performing type conversion if necessary
	template <class T> T at(int i1, int i2, int i3, int i4) const;

	/// Copy all elements into another array
	template <class T> void copyTo(T * dst) const { for(int i=0;i<size();++i) dst[i]=at<T>(i); }

	/// Get reference to element at 1D index using raw pointer casting
	template <class T>
	const T& elem(int i) const { return elems<T>()[i*stride()]; }

	/// Get reference to element at 2D index using raw pointer casting
	template <class T>
	const T& elem(int i1, int i2) const { return elems<T>()[indexFlat(i1,i2)*stride()]; }

	/// Get reference to element at 3D index using raw pointer casting
	template <class T>
	const T& elem(int i1, int i2, int i3) const { return elems<T>()[indexFlat(i1,i2,i3)*stride()]; }

	/// Get reference to element at 4D index using raw pointer casting
	template <class T>
	const T& elem(int i1, int i2, int i3, int i4) const { return elems<T>()[indexFlat(i1,i2,i3,i4)*stride()]; }

	/// Returns whether there is valid data that can be accessed
	bool hasData() const { return mData!=0; }

	/// Returns whether 2D index is within bounds
	bool inBounds(int i1, int i2) const { return (i1>=0) && (i1<size(0)) && (i2>=0) && (i2<size(1)); }
	
	/// Returns whether 3D index is within bounds
	bool inBounds(int i1, int i2, int i3) const { return inBounds(i1,i2) && (i3>=0) && (i3<size(2)); }

	/// Returns whether 4D index is within bounds
	bool inBounds(int i1, int i2, int i3, int i4) const { return inBounds(i1,i2,i3) && (i4>=0) && (i4<size(3)); }

	/// Convert 2D index to 1D index
	int indexFlat(int i1, int i2) const {
		return i1 + size(0)*i2;
	}

	/// Convert 3D index to 1D index
	int indexFlat(int i1, int i2, int i3) const {
		return i1 + size(0)*(i2 + size(1)*i3);
	}

	/// Convert 4D index to 1D index
	int indexFlat(int i1, int i2, int i3, int i4) const {
		return i1 + size(0)*(i2 + size(1)*(i3 + size(2)*i4));
	}
	
	/// Convert 1D index to 2D index
	void indexDim(int& i1, int& i2, const int& i) const {
		i1=i%size(0); i2=i/size(0);
	}

	/// Convert 1D index to 3D index
	void indexDim(int& i1, int& i2, int& i3, const int& i) const {
		indexDim(i1,i2,i); i2%=size(1); i3=i/(size(0,1));
	}

	/// Convert 1D index to 4D index
	void indexDim(int& i1, int& i2, int& i3, int& i4, const int& i) const {
		indexDim(i1,i2,i3,i); i3%=size(2); i4=i/(size(0,1,2));
	}
	
	/// Get index of first element matching argument or Data::npos if not found
	int indexOf(const Data& v) const;

	/// Get index of first element matching argument or Data::npos if not found
	template <class T>
	int indexOf(const T& v) const { return indexOf(Data(v)); }
	
	/// Returns whether type is numerical, i.e., int, float, or double
	bool isNumerical() const { return type() < Data::STRING; }

	/// Returns true if all values are zero
	bool isZero() const;

	/// Get maximum allowable dimensions
	static int maxDim(){ return DATA_MAXDIM; }

	/// Get element offset into source array
	int offset() const { return (mElems-mData)/sizeType(); }
	
	/// Get number of dimensions sized larger than 1
	int order() const;
	
	/// Returns reversed slice
	Data reversed() const { return slice((size()-1)*stride(), size(), -stride()); }
	
	/// Get total number of elements
	int size() const { return product(mSizes, maxDim()); }
	
	int size(int d) const { return mSizes[d]; }
	int size(int d1, int d2) const { return size(d1)*size(d2); }
	int size(int d1, int d2, int d3) const { return size(d1,d2)*size(d3); }
	int size(int d1, int d2, int d3, int d4) const { return size(d1,d2,d3)*size(d4); }
	
	/// Get size array
	const int * shape() const { return mSizes; }
	
	/// Get size, in bytes, of element type
	int sizeType() const;

	/// Returns maximally sized 1D slice at given offset
	Data slice(int offset=0) const { return slice(offset, size()-offset); }

	/// Returns 1D slice with given offset, and size
	Data slice(int offset, int size) const { return slice(offset,size,stride()); }

	/// Returns 1D slice with given offset, size, and stride
	
	/// @param[in] offset	element offset from source data
	/// @param[in] size		number of elements in slice
	/// @param[in] stride	element stride through source data
	Data slice(int offset, int size, int stride) const;

	/// Get index stride
	int stride() const { return mStride; }

	/// Get element type
	const Type& type() const { return mType; }

	/// Print information to stdout
	void print() const;

	int fromToken(const std::string& v){ return glv::fromToken(*this, v); }
	std::string toString() const { return glv::toString(*this); }
	std::string toToken() const { return glv::toToken(*this); }
	int toToken(std::string& dst) const { return glv::toToken(dst, *this); }



	/// Perform shallow copy from another Data
	Data& operator= (const Data& v);

	/// Assign value to first element
	template <class T>
	Data& assign(const T& v){ return assignFromArray(&v,1); }

	/// Assign value to element at 1D index
	template <class T>
	Data& assign(const T& v, int idx){ slice(idx).assign(v); return *this; }

	/// Assign value to element at 2D index
	template <class T>
	Data& assign(const T& v, int i1, int i2){ return assign(v,indexFlat(i1,i2)); }

	/// Assign value to element at 3D index
	template <class T>
	Data& assign(const T& v, int i1, int i2, int i3){ return assign(v,indexFlat(i1,i2,i3)); }

	/// Assign value to element at 4D index
	template <class T>
	Data& assign(const T& v, int i1, int i2, int i3, int i4){ return assign(v,indexFlat(i1,i2,i3,i4)); }

	/// Assign elements to elements from argument Data
	Data& assign(const Data& v, int i=0);

	/// Assign elements to elements from argument Data
	Data& assign(const Data& v, int i1, int i2){ return assign(v, indexFlat(i1,i2)); }

	/// Assign elements to elements from argument Data
	Data& assign(const Data& v, int i1, int i2, int i3){ return assign(v, indexFlat(i1,i2,i3)); }

	/// Assign elements to elements from argument Data
	Data& assign(const Data& v, int i1, int i2, int i3, int i4){ return assign(v, indexFlat(i1,i2,i3,i4)); }

	/// Assign all elements to argument
	template <class T>
	Data& assignAll(const T& v){ for(int i=0;i<size();++i){assign(v,i);} return *this; }

	/// Assign elements from an external array
	
	/// @param[in] src		source array
	/// @param[in] size		size of source array
	/// @param[in] stride	amount to stride through source array
	/// @param[in] idx		destination starting index
	template <class T>
	Data& assignFromArray(const T * src, int size, int stride=1, int idx=0){
		return assign(Data(const_cast<T*>(src), size).stride(stride), idx);
	}

	/// Clear contents, leaving the size equal to 0
	void clear();

	/// Allocate internal memory and copy over previous data
	void clone();

	/// Get mutable reference to element at 1D index using raw pointer casting
	template <class T>
	T& elem(int i){ return elems<T>()[i*stride()]; }

	/// Get mutable reference to element at 2D index using raw pointer casting
	template <class T>
	T& elem(int i1, int i2){ return elems<T>()[indexFlat(i1,i2)*stride()]; }

	/// Get mutable reference to element at 3D index using raw pointer casting
	template <class T>
	T& elem(int i1, int i2, int i3){ return elems<T>()[indexFlat(i1,i2,i3)*stride()]; }

	/// Get mutable reference to element at 4D index using raw pointer casting
	template <class T>
	T& elem(int i1, int i2, int i3, int i4){ return elems<T>()[indexFlat(i1,i2,i3,i4)*stride()]; }

	/// Returns pointer to first element
	// note: this is potentially dangerous call, therefore use with caution...
	template <class T>
	T * elems(){ return (T *)mElems; }

	/// Get pointer to first element
	template <class T>
	const T * elems() const { return (const T *)mElems; }

	// TODO: insert elements
//	// src			source array
//	// count		number of source elements
//	// idx			destination start index
//	// stride		destination stride
//	template <class T>
//	Data& insert(const T* src, int count, int idx, int stride){
//		return *this;
//	}

	/// Assign contents from mixture
	void mix(const Data& d1, const Data& d2, double c1, double c2);
	void mix(const Data& d1, const Data& d2, const Data& d3, const Data& d4, double c1, double c2, double c3, double c4);
	template <int N> void mix(const Data* D[], double c[]);

	/// Resize array allocating new memory if necessary
	int resize(const int * sizes, int numDims){
		return resize(type(), sizes, numDims);
	}

	/// Resize array allocating new memory if necessary
	int resize(int size1, int size2=1, int size3=1, int size4=1){
		int s[]={size1,size2,size3,size4};
		return resize(s,4);
	}

	/// Resize array allocating new memory if necessary
	int resize(Data::Type type, const int * sizes, int numDims);

	/// Resize array allocating new memory if necessary
	int resize(Data::Type type, int size1=1, int size2=1, int size3=1, int size4=1){
		int s[]={size1,size2,size3,size4}; return resize(type, s,4);
	}

	/// Set data to reference a scalar
	template <class T>
	Data& set(T& v){ return set(&v,1); }

	/// Set new data from a scalar with specified value
	template <class T>
	Data& set(const T& v){ return set(&v,1); }

	/// Set new data from a character string
	Data& set(const char * v){ return set(std::string(v)); }

	/// Set data to reference an external multidimensional array
	template <class T>
	Data& set(T * data, const int * sizes, int numDims);

	/// Set data to reference an external 1D array
	template <class T>
	Data& set(T * data, int size){ return set(data, &size,1); }

	/// Set data to reference an external 2D array
	template <class T>
	Data& set(T * data, int size1, int size2){
		int s[]={size1,size2}; return set(data, s,2); }

	/// Set data to reference an external 3D array
	template <class T>
	Data& set(T * data, int size1, int size2, int size3){
		int s[]={size1,size2,size3}; return set(data, s,3); }

	/// Set data to reference an external 4D array
	template <class T>
	Data& set(T * data, int size1, int size2, int size3, int size4){
		int s[]={size1,size2,size3,size4}; return set(data, s,4); }

	/// Set dimensions without reallocing new memory
	Data& shape(const int * sizes, int n);

	/// Set dimensions without reallocing new memory
	Data& shape(int n1, int n2=1, int n3=1, int n4=1){
		int n[]={n1,n2,n3,n4}; return shape(n,4);
	}
	
	/// Set index stride factor
	Data& stride(int v){ mStride=v; return *this; }
	
	/// Change type of data
	
	/// New memory will be allocated if the type differs from the current
	/// type and the number of elements in the array is non-zero.
	Data& type(Data::Type type);

//	static int interpretToken(Data::Type& type, int *& sizes, std::string& src);

protected:
	typedef char* pointer;		// pointer to memory address;
								// char vs. void to simplify pointer arithmetic
	pointer mData;				// pointer to first element of source data
	pointer mElems;				// pointer to first element in this slice
	int mStride;				// stride factor
	int mSizes[DATA_MAXDIM];	// sizes of each dimension
	Type mType;					// data type

	Data& offset(int i){ mElems=mData+i*sizeType(); return *this; }
	Data& shapeAll(int n);

	template <class T>
	const T * data() const { return (const T *)mData; }

	void init(){ // zeros all attributes (used in c'tor)
		mData=0; mElems=0; mStride=1; mType=NONE;
		shapeAll(0);
	}
	
	// Reallocate memory; returns size change, in bytes
	int realloc(Data::Type type, const int * sizes=0, int n=0);
	void setRaw(void * data, int offset, int stride, Type type);
	int sizeBytes() const { return size()*sizeType(); }
	
	static int product(const int * v, int n){
		if(n){ int r=v[0]; for(int i=1;i<n;++i) r*=v[i]; return r; }
		return 0;
	}

	template <int N, class T>
	static bool notEqual(const T* v){
		for(int i=1; i<N; ++i){ if(v[0] != v[i]) return true; }
		return false;
	}
};



/// An interface for accessing the model state of an object
class Model{
public:

	virtual ~Model(){}

	/// Get data associated with the model, if any
	
	/// @param[in] temp		Default constructed Data object passed in by callee.
	///						This can be used as a return value in situations
	///						where either the base class does not have its own 
	///						Data object or when some kind of conversion of the
	///						data in its Data object needs to occur.
	virtual const Data& getData(Data& temp) const { return temp; }

	/// Set data associated with the model, if any
	virtual void setData(const Data& d){}

	/// Set data from a string
	int setDataFromString(const std::string& v){
		Data d;
		(d = getData(d)).clone();
		int r = d.fromToken(v);
		if(r) setData(d);
		return r;
	}
	
	/// Get data as string
	std::string getDataAsString(){ Data d; return toString(getData(d)); }
};


/// Supplies a Data object with accessors
class DataObject{
public:
	DataObject(){}
	DataObject(Data& d): mData(d){}

	const Data& data() const { return mData; }
	Data& data(){ return mData; }

protected:
	Data mData;
};


/// A Model that explicitly contains Data
class DataModel : public Model, public DataObject{
public:

	DataModel(){}
	DataModel(Data& d): DataObject(d){}

	virtual ~DataModel(){}

	virtual const Data& getData(Data& temp) const { return data(); }
	virtual void setData(const Data& d){ data().assign(d); }
};


/*
class DataModel : public Model{
public:

	DataModel(){}
	DataModel(Data& d): mData(d){}

	virtual ~DataModel(){}

	virtual const Data& getData(Data& temp) const { return data(); }

	virtual void setData(const Data& d){ assignData(d); }

//	virtual std::string modelToString(){
//		return data().toToken();
//	}
//
	int modelFromString(const std::string& v){
		Data d = data();
		d.clone();
		int r = d.fromToken(v);
		if(r) assignData(d);
		return r;
	}

	const Data& data() const { return mData; }

	Data& data(){ return mData; }
	
	void assignData(const Data& d, const int& idx=0){
		int i1=0,i2=0; data().indexDim(i1,i2, idx);
		assignData(d, i1,i2);
	}

	// Assigns argument to elements at specified index.
	
	// assignData() is called with the input data if the indices are valid.
	// assignData() can be used to constrain the input data before it is
	// assigned.
	void assignData(const Data& d, int ind1, int ind2){
		if(data().inBounds(ind1, ind2)){
			Data t=d; t.clone();
			if(onAssignData(t, ind1, ind2)){
				//model().assign(t, ind1, ind2);
			}
		}
	}

protected:
	Data mData;
	
	virtual bool onAssignData(Data& d, int ind1, int ind2){
		data().assign(d, ind1, ind2);
		return true;
	}
};
*/


/*
memento		object that stores snapshot of another object
originator	object from which snapshot is taken
caretaker	keeps a list of mementos

undo mechanism will request memento object from originator

class ModelState{
public:
	virtual void toToken(std::string& dst);
	virtual void fromToken(const std::string& src);
};

{
["preset 1"] = {
	author = "John Doe",
	time = 10293339107,
	keywords = "fuzzy, buzzy",
	buttons = {0, 1, 0, 1},
	label = "hello",
},

["preset 2"] = {
	author = "Don Jo",
	time = 10293339108,
	keywords = "funny, bunny",
	buttons = {0, 0, 0, 1},
	label = "world",
},
}
*/

/// Manages the saving/restoring of model parameters to/from memory and disk.

/// A NamedModel is a string/Model pair associated with a "live" parameter of 
/// the application. A Snapshot is a recording of all the application's 
/// NamedModels as string/Data pairs.
/// When a snapshot is loaded, only data having an identifier having an 
/// identical identifier of an attached model is loaded. If an attached model 
/// variable exists, but does not have a corresponding variable in a snapshot,
/// then the model data will not be modified when the snapshot is loaded.
class ModelManager{
public:

	template <class Key, class Val>
	class Map : public std::map<Key, Val>{
	public:
		const Val& operator[](const Key& key) const {
			static const Val null;
			typename Map::const_iterator it = find(key);
			return (it!=this->end()) ? it->second : null;
		}
		Val& operator[](const Key& key){ return this->std::map<Key,Val>::operator[](key); }
	};

	typedef Map<std::string, Model*>		NamedModels;
	typedef Map<std::string, const Model*>	NamedConstModels;
	typedef Map<std::string, Data>			Snapshot;
	typedef Map<std::string, Snapshot>		Snapshots;

	/// Prints currently stored snapshots
	void printSnapshots() const;

	/// Get default file path
	std::string filePath() const { std::string s=""; defaultFilePath(s); return s; }

	/// Get identifier name
	const std::string& name() const { return mName; }
	
	/// Get snapshots
	const Snapshots& snapshots() const { return mSnapshots; }
	Snapshots& snapshots(){ return mSnapshots; }

	/// Save all snapshots to a file

	/// @param[in] path		path to file; if empty, then uses "<name>.txt"
	/// \returns			number of characters written
	int snapshotsToFile(const std::string& path="") const;

	/// Returns a string of all snapshots
	std::string snapshotsToString() const {
		std::string r; snapshotsToString(r); return r;
	}

	/// Convert all snapshots to a table string

	/// \returns number of characters written
	///
	int snapshotsToString(std::string& dst) const;

	/// Set snapshot from a table string. If the snapshot does not exist, a new one will be created.
	int snapshotFromString(const std::string& src);

	/// Load snapshots from a file

	/// @param[in] path				path to file; 
	///								if empty, then uses model name with ".txt" extension
	/// @param[in] addtoExisting	whether to add to or replace any existing snapshots
	/// \returns					number of characters read
	int snapshotsFromFile(const std::string& path="", bool addtoExisting=true);

	/// Set snapshots from a table string. If a snapshot does not exist, a new one will be created.
	int snapshotsFromString(const std::string& src);


	/// Add reference to readable-writable model
	
	/// The added Model should have the same lifecycle as 'this' since it is
	/// referenced through a pointer.
	void add(const std::string& name, Model& v);

	/// Add reference to read-only model
	
	/// The added Model should have the same lifecycle as 'this' since it is
	/// referenced through a pointer.
	void add(const std::string& name, const Model& v);

	/// Remove all models
	void clearModels();

	/// Copy over models from another manager; replaces existing models
	ModelManager& copyModels(const ModelManager& m);

	/// Clear all currently stored snapshots
	void clearSnapshots();
	
	/// Set the default file directory and name
	
	/// @param[in] name		file name without directory; 
	///						if empty, then uses model name with ".txt" extension
	/// @param[in] dir		file directory
	ModelManager& filePath(const std::string& name, const std::string& dir="");

	/// Set the default file directory

	/// @param[in] dir		file directory
	///
	ModelManager& fileDir(const std::string& dir);

	/// Set identifier name
	ModelManager& name(const std::string& v);

	/// Remove model data with given name
	void remove(const std::string& name);

	/// Save current model state as a snapshot with given name
	
	/// If a snapshot with the given name exists, then it will be overwritten.
	///
	void saveSnapshot(const std::string& name);

	/// Load snapshot with given name into current model state
	
	/// \returns whether snapshot was loaded successfully
	///
	bool loadSnapshot(const std::string& name);

	/// Load linear mixture of two snapshots
	
	/// \returns whether snapshot mixture was loaded successfully
	///
	bool loadSnapshot(const std::string& name1, const std::string& name2, double c1, double c2);
	bool loadSnapshot(const Snapshot& ss1, const Snapshot& ss2, double c1, double c2);

	bool loadSnapshot(
		const std::string& name1, const std::string& name2, const std::string& name3, const std::string& name4,
		double c1, double c2, double c3, double c4);

	bool loadSnapshot(
		const Snapshot& ss1, const Snapshot& ss2, const Snapshot& ss3, const Snapshot& ss4,
		double c1, double c2, double c3, double c4);

	void makeClosed();

protected:
	std::string mName;				// name identifier
	std::string mFileDir, mFileName;// directory and name of file
	NamedModels mState;				// pointers to active readable/writable models
	NamedConstModels mConstState;	// pointers to active read-only models
	Snapshots mSnapshots;			// repository of saved model data

//	// Convert current model state to string
//	bool stateToToken(std::string& dst, const std::string& modelName) const;
	
//	// Convert string to model state
//	int stateFromToken(const std::string& src);

	bool defaultFilePath(std::string& s) const;

//	template <int N> bool loadSnapshot(const std::string ** names, const double * c);
//	template <int N> bool loadSnapshot(const Snapshot ** snapshots, const double * c);

	static std::string namedDataToString(const std::string& s, const Data& d){
		std::string r;
		if(d.toToken(r)) r = s + " = " + r + ",\r\n";
		return r;
	}
};



// Implementation ______________________________________________________________

template<class T>
int toString(std::string& dst, const T& src, const char * format){
	#ifdef WIN32
		#define TO_STRING_FUNC _snprintf
	#else
		#define TO_STRING_FUNC snprintf
	#endif
	char buf[32]; 
	TO_STRING_FUNC(buf, sizeof(buf), format, src);
	dst = buf;
	#undef TO_STRING_FUNC
	return 1;
}

template<class T>
inline int stringifyArray(
	std::string& dst, const T * src, int size, int stride,
	int (*stringifyElem)(std::string&, const T&)
){
	if(1==size){ return stringifyElem(dst, *src); }
	
	else{
		dst = "";
		std::string temp;
		int count=0;
		for(int i=0; i<size; ++i){
			count += stringifyElem(temp, src[i*stride]);
			dst += temp;
			if(i != (size-1)) dst += ", ";
		}
		return count;
	}
}

template<class T>
int toString(std::string& dst, const T * src, int size, int stride){
	return stringifyArray<T>(dst,src,size,stride, toString);
}

template<class T>
inline int toToken(std::string& dst, const T& src){ return toString(dst,src); }

template<>
inline int toToken<std::string>(std::string& dst, const std::string& src){
	dst = "\"" + src + "\""; return 1;
}

template<class T>
int toToken(std::string& dst, const T * src, int size, int stride){
	int res = stringifyArray<T>(dst,src,size,stride, toToken);
	if(1!=size) dst = "{" + dst + "}";
	return res;
}



// Data ________________________________________________________________________

template<> inline Data::Type Data::getType<bool>(){ return Data::BOOL; }
template<> inline Data::Type Data::getType<int>(){ return Data::INT; }
template<> inline Data::Type Data::getType<float>(){ return Data::FLOAT; }
template<> inline Data::Type Data::getType<double>(){ return Data::DOUBLE; }
template<> inline Data::Type Data::getType<std::string>(){ return Data::STRING; }

template <class T>
T Data::at(int i) const {
	#define CS(TY,t) case Data::TY: return static_cast<const T>(elem<t>(i));
	switch(type()){
	CS(BOOL,bool) CS(INT,int) CS(FLOAT,float) CS(DOUBLE,double)
	case Data::STRING: {
		T v; if(glv::fromToken(v, elem<std::string>(i))){ return v; }
	}
	default: return T();
	}
	#undef CS
}

template<>
inline std::string Data::at<std::string>(int i) const {
	#define CS(T,t) case Data::T: return glv::toString(elem<t>(i));
	switch(type()){
	CS(BOOL,bool) CS(INT,int) CS(FLOAT,float) CS(DOUBLE,double)
	CS(STRING,std::string)
	default: return "";
	}
	#undef CS
}

template <class T>
inline T Data::at(int i1, int i2) const { return at<T>(indexFlat(i1,i2)); }

template <class T>
inline T Data::at(int i1, int i2, int i3) const { return at<T>(indexFlat(i1,i2,i3)); }

template <class T>
inline T Data::at(int i1, int i2, int i3, int i4) const { return at<T>(indexFlat(i1,i2,i3,i4)); }

#define DATA_SET(t, T)\
template<> inline Data& Data::set<t>(t * src, const int * sizes, int n){\
	setRaw(src,0,1,Data::T); shape(sizes,n); return *this;\
}\
template<> inline Data& Data::set<const t>(const t * src, const int * sizes, int n){\
	if(product(sizes,n)!=size() || Data::T!=type()) realloc(Data::T, sizes,n);\
	for(int i=0; i<size(); ++i){ elems<t>()[i] = src[i]; }\
	return *this;\
}

DATA_SET(bool, BOOL)
DATA_SET(int, INT)
DATA_SET(float, FLOAT)
DATA_SET(double, DOUBLE)
DATA_SET(std::string, STRING)
//template<> inline Data& Data::set<char *>(char* * src, const int * sizes, int n){
//	setRaw(src,0,1,Data::STRING); shape(sizes,n); return *this;
//}
#undef DATA_SET

inline void Data::mix(const Data& d1, const Data& d2, double c1, double c2){
	const Data* d[]={&d1,&d2};
	double c[]={c1,c2};
	mix<2>(d,c);
}

inline void Data::mix(
	const Data& d1, const Data& d2, const Data& d3, const Data& d4,
	double c1, double c2, double c3, double c4
){
	const Data* d[]={&d1,&d2,&d3,&d4};
	double c[]={c1,c2,c3,c4};
	mix<4>(d,c);
}

template <int M>
void Data::mix(const Data* D[], double c[]){
	int N = size();

	//bool isNum = isNumerical();
	bool isNum = true;

	for(int i=0; i<M; ++i){
		const Data& d = *D[i];
		int sz = d.size();
		if(sz < N) N = sz;
		isNum	&= d.isNumerical()
				&& (d.type() != Data::BOOL);
	}

	// TODO: make this more efficient...
	if(isNum){
		double v[M];
	
		for(int i=0; i<N; ++i){
		
			for(int m=0; m<M; ++m) v[m] = D[m]->at<double>(i);

			if(notEqual<M>(v)){	// avoid numerical error
				double res = v[0]*c[0];
				for(int m=1; m<M; ++m) res += v[m]*c[m];
			
				// for booleans, we truncate first
				//if(type() == Data::BOOL)	assign(int(res), i);
				//else						assign(res, i);
				assign(res, i);
			}
			else{
				assign(v[0], i);
			}
		}
	}
	else{	// for strings and bools copy the lower index middle element
		assign(*D[M/2-1]);
	}
}


/// This assumes that all snapshots are members of a closed set
//template <int N>
//bool ModelManager::loadSnapshot(const Snapshot ** S, double * c){
//	
//	Snapshot::const_iterator it[N];
//	for(int i=0; i<N; ++i) it[i] = S[i]->begin();
//	
//	while(S[0]->end() != it[0]){
//	
//		const Data * D[N];
//		for(int i=0; i<N; ++i) D[i] = &(it[i]->second);
//
//		Data temp = data1;
//		temp.clone();
//		temp.mix<N>(D, c);
//		itState->second->setData(temp);
//		
//		for(int i=0; i<N; ++i) ++(it[i]);
//	}
//	return true;
//}


} // glv::
#endif
