#ifndef INC_GLV_MODEL_H
#define INC_GLV_MODEL_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <map>
#include <vector>
#include <string>

namespace glv {

class Data;

/// Convert object to a string. Returns number of elements converted.
template<class T>
int toString(std::string& dst, const T& src);
int toString(std::string& dst, const char * src);

/// Convert objects to a string. Returns number of elements converted.
template<class T>
int toString(std::string& dst, const T * src, int size);

/// Returns a stringification of an object
template <class T>
inline std::string toString(const T& obj){ std::string r; toString(r,obj); return r; }


/// Convert string token to object. Returns number of elements converted.
template<class T>
int fromToken(T& dst, const std::string& src);

/// Convert string token to objects. Returns number of elements converted.
template<class T>
int fromToken(T * dst, int size, const std::string& src);

/// Convert object to a string token. Returns number of elements converted.
template<class T>
int toToken(std::string& dst, const T& src){ return toString(dst,src); }
int toToken(std::string& dst, const char * src);

/// Convert objects to a string token. Returns number of elements converted.
template<class T>
int toToken(std::string& dst, const T * src, int size);

/// Returns string token of an object
template <class T>
inline std::string toToken(const T& obj){ std::string r; toToken(r,obj); return r; }


int toString(std::string& dst, const Data& src);
int toToken(std::string& dst, const Data& src);
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




/*
General array slicing can be implemented (whether or not built into the 
language) by referencing every array through a dope vector or descriptor — a 
record that contains the address of the first array element, and then the range 
of each index and the corresponding coefficient in the indexing formula. This 
technique also allows immediate array transposition, index reversal, 
subsampling, etc. For languages like C, where the indices always start at zero,
the dope vector of an array with d indices has at least 1 + 2d parameters.
*/

#ifndef DATA_MAXDIM
#define DATA_MAXDIM 4
#endif


/// Generic wrapper around single or arrayed primitive types

/// For operations between data with different types, standard type conversions
/// will be used when possible.
/// For binary operations between arrays, each array is treated as a 
/// one-dimensional array (or tensor with an order of 1). If the number of 
/// elements differ, then the maximum possible number of elements are used in
/// the comparison.
class Data : public ReferenceCounter {
public:
	enum Type{
		VOID=0, BOOL, INT, FLOAT, DOUBLE, STRING
	};
	static std::string typeToString(Type t);


	Data();
	
	/// @param[in] v		data to reference
	Data(Data& v);
	
	/// @param[in] v		data to be cloned
	Data(const Data& v);

	/// @param[in] type		type of allocated data
	/// @param[in] size1	size of dimension 1 of allocated data
	/// @param[in] size2	size of dimension 2 of allocated data
	/// @param[in] size3	size of dimension 3 of allocated data
	/// @param[in] size4	size of dimension 4 of allocated data
	Data(Data::Type type, int size1=1, int size2=1, int size3=1, int size4=1);

	template <class T>
	explicit Data(T& value){ set(&value, 1); }

	template <class T>
	explicit Data(const T& value){ set(&value, 1); }

	/// @param[in] data		pointer to external data to reference
	/// @param[in] size1	size of dimension 1 of external data
	/// @param[in] size2	size of dimension 2 of external data
	/// @param[in] size3	size of dimension 3 of external data
	/// @param[in] size4	size of dimension 4 of external data
	template <class T>
	Data(T * data, int size1, int size2=1, int size3=1, int size4=1){
		int sizes[] = {size1,size2,size3,size4};
		set(data, sizes, 4);
	}

	~Data();


	/// Returns whether elements from each array are equal
	bool operator==(const Data& v) const;
	
	/// Returns whether elements from each array are not equal
	bool operator!=(const Data& v) const { return !((*this)==v); }

	/// Get element at 1D index
	template <class T> const T at(int ind) const;

	/// Get element at 2D index
	template <class T> const T at(int i1, int i2) const;

	/// Get element at 3D index
	template <class T> const T at(int i1, int i2, int i3) const;

	/// Get element at 4D index
	template <class T> const T at(int i1, int i2, int i3, int i4) const;

	bool hasData() const { return mData!=0; }

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

	bool indexValid(int i1, int i2) const { return (i1>=0) && (i1<size(0)) && (i2>=0) && (i2<size(1)); }
	bool isNumerical() const { return type() != Data::STRING; }
	int offset() const { return mOffset; }
	int order() const;
	int size() const { return product(mSizes, maxDim()); }
	int size(int d) const { return mSizes[d]; }
	int size(int d1, int d2) const { return size(d1)*size(d2); }
	int size(int d1, int d2, int d3) const { return size(d1,d2)*size(d3); }
	int sizeType() const;
	Type type() const { return mType; }

	/// Returns maximally sized slice at given offset
	Data slice(int offset=0) const { return slice(offset, size()-offset); }

	/// Returns slice of given size at offset
	Data slice(int offset, int size) const;

	int stride() const { return mStride; }

	void print() const;

	int fromToken(const std::string& v){ return glv::fromToken(*this, v); }
	std::string toString() const { return glv::toString(*this); }
	std::string toToken() const { return glv::toToken(*this); }
	int toToken(std::string& dst) const { return glv::toToken(dst, *this); }


	/// Perform shallow copy from another Data
	Data& operator= (const Data& v);

	/// Assign all elements to argument
	template <class T>
	Data& assign(const T& v){ return assign(&v,1); }

	/// Assign elements from an external array
	template <class T>
	Data& assign(const T * src, int size){ return assign(Data(const_cast<T*>(src), size)); }

	/// Assign elements to elements from argument Data
	Data& assign(const Data& v, int ind1=0, int ind2=0);

	template <class T>
	Data& assignAll(const T& v){ for(int i=0;i<size();++i){put(v,i);} return *this; }

	/// Allocate internal memory and copy over previous data
	void clone();

	/// Returns pointer to first element
	// note: this is potentially dangerous call, therefore use with caution...
	template <class T>
	T * elems(){ return ((T *)mData) + offset(); }

	template <class T>
	const T * elems() const { return (const T *)(((T *)mData) + offset()); }

	/// Set an element at specified index
	template <class T>
	Data& put(const T& v, int ind1=0, int ind2=0);
	
	/// Resize array
	Data& resize(const int * sizes, int numDims);

	template <class T>
	Data& set(T& v){ return set(&v,1); }

	template <class T>
	Data& set(const T& v){ return set(&v,1); }

	Data& set(const char * v){ return set(std::string(v)); }

	template <class T>
	Data& set(T * data, const int * sizes, int numDims);

	template <class T>
	Data& set(T * data, int size){ return set(data, &size,1); }

	Data& set(Data::Type type, const int * sizes, int numDims);

	Data& set(Data::Type type, int size){ return set(type, &size,1); }

	Data& shape(int n1, int n2=1, int n3=1, int n4=1){
		int n[]={n1,n2,n3,n4}; return shape(n,4);
	}
	
	Data& stride(int v){ mStride=v; return *this; }

protected:
	typedef char* pointer;

	pointer mData;				// pointer to first array element
	int mOffset;				// index offest into array
	int mStride;				// stride factor
	int mSizes[DATA_MAXDIM];	// sizes of each dimension
	Type mType;					// data type

	static int maxDim(){ return DATA_MAXDIM; }

	Data& offset(int i){ mOffset=i; return *this; }
	Data& shape(const int * sizes, int n);
	Data& shapeAll(int n);

	template <class T>
	const T * data() const { return (const T *)(mData); }

	template <class T>
	const T& elem(int i) const { return elems<T>()[i*stride()]; }

	template <class T>
	T& elem(int i){ return elems<T>()[i*stride()]; }

	void free();
	void realloc(Data::Type type, const int * sizes=0, int n=0);
	void setRaw(void * data, int offset, Type type);
	int sizeBytes() const { return size()*sizeType(); }
	
	static int product(const int * v, int n){
		if(n){ int r=v[0]; for(int i=1;i<n;++i) r*=v[i]; return r; }
		return 0;
	}
		
};


/// This is a Data object that notifies its base class on changes
class Model{
public:

	Model(){}
	Model(Data& d): mData(d){}

	virtual ~Model(){}

	const Data& model() const { return mData; }

	Data& model(){ return mData; }

	int modelFromToken(const std::string& v){
		Data d = model();
		d.clone();
		int r = d.fromToken(v);
		if(r) assignModel(d);
		return r;
	}

protected:
	Data mData;

	void assignModel(const Data& d, const int& ind=0){
		int i1=0,i2=0; model().indexDim(i1,i2, ind);
		assignModel(d, i1,i2);
	}

	// Assigns argument to elements at specified index.
	
	// onAssignModel() is called with the input data if the indices are valid.
	// onAssignModel() can be used to constrain the input data before it is
	// assigned.
	void assignModel(const Data& d, int ind1, int ind2){
		if(model().indexValid(ind1, ind2)){
			Data t=d; t.clone();
			if(onAssignModel(t, ind1, ind2)){
				//model().assign(t, ind1, ind2);
			}
		}
	}

	void setModel(const Data& d){
		mData = d;
	}
	
	virtual bool onAssignModel(Data& d, int ind1, int ind2){ return false; }
};



class StateSpace{
public:
	
	void add(const std::string& name, const Data& d){
		if(!hasState(name)){
			mState.push_back(d);
			mNameToIndex[name] = mState.size()-1;
		}
	}
	
	void saveState(const std::string& name){
		DataVector& v = mStates[name];
		v.assign(mState.begin(), mState.end());
		
		for(unsigned i=0; i<v.size(); ++i){
			v[i].clone();
		}
	}
	
	void loadState(const std::string& name){
		if(hasState(name)){
			DataVector& v = mStates[name];
			for(unsigned i=0; i<mState.size(); ++i){
				mState[i].assign(v[i]);
				std::string t1, t2;
				toToken(t1, mState[i]);
				toToken(t2, v[i]);
				//printf("%s <- %s\n", t1.c_str(), t2.c_str());
			}
		}
	}
	
	bool stateToString(std::string& dst, const std::string& name="");

protected:
	typedef std::vector<Data> DataVector;
	typedef std::map<std::string, int> NameToIndex;
	
	DataVector mState;
	NameToIndex mNameToIndex;
	
	//std::map<std::string, Data *> mNameToData;
	std::map<std::string, DataVector> mStates;

	bool hasState(const std::string& name){ return mStates.count(name); }
};


class ModelManager{
public:
	
	/// Convert current model state to string
	bool toToken(std::string& dst, const std::string& modelName) const;
	
	/// Convert string to model state
	int fromToken(const std::string& src);

	/// Add new name-value pair
	
	/// The added Data should have the same lifecycle as 'this'. This is because
	/// pointers are stored internally.
	void add(const std::string& name, Data& v);
	void add(const std::string& name, Model& v);
	void add(const std::string& name, const Model& v);

	/// Remove named value
	void remove(const std::string& name);


	void saveState(const std::string& name){
	}

	void loadState(const std::string& name){
	}

protected:
	typedef std::map<std::string, Model *> NameValMap;
	typedef std::map<std::string, const Model *> NameConstValMap;

	NameValMap mNameVal;
	NameConstValMap mNameConstVal;
	
	static bool isIdentifier(const std::string& s);
};




// Implementation
//------------------------------------------------------------------------------

template<class T>
int toString(std::string& dst, const T * src, int size){
	if(1==size){ return toString(dst, *src); }
	
	else{
		dst = "";
		std::string temp;
		int count=0;
		for(int i=0; i<size; ++i){
			count += toString(temp, src[i]);
			dst += temp;
			if(i != (size-1)) dst += ", ";
		}
		return count;
	}
}

template<class T>
int toToken(std::string& dst, const T * src, int size){
	if(1==size){ return toToken(dst, *src); }
	
	else{
		dst = "{";
		std::string temp;
		int count = toString(temp, src, size);
		dst += (temp += "}");
		return count;
	}
}

template <class T>
const T Data::at(int i) const {
	switch(type()){
	case Data::BOOL:	return static_cast<const T>(elem<bool>(i));
	case Data::INT:		return static_cast<const T>(elem<int>(i));
	case Data::FLOAT:	return static_cast<const T>(elem<float>(i));
	case Data::DOUBLE:	return static_cast<const T>(elem<double>(i));
	case Data::STRING: {
		T v; if(glv::fromToken(v, elem<std::string>(i))){ return v; }
	}
	default: return T();
	}
}

template<>
inline const std::string Data::at<std::string>(int i) const {
	switch(type()){
	case Data::BOOL:	return glv::toString(elem<bool>(i));
	case Data::INT:		return glv::toString(elem<int>(i));
	case Data::FLOAT:	return glv::toString(elem<float>(i));
	case Data::DOUBLE:	return glv::toString(elem<double>(i));
	case Data::STRING:	return elem<std::string>(i);
	default: return "";
	}
}

template <class T>
inline const T Data::at(int i1, int i2) const { return at<T>(indexFlat(i1,i2)); }

template <class T>
inline const T Data::at(int i1, int i2, int i3) const { return at<T>(indexFlat(i1,i2,i3)); }

template <class T>
inline const T Data::at(int i1, int i2, int i3, int i4) const { return at<T>(indexFlat(i1,i2,i3,i4)); }

template <class T>
inline Data& Data::put(const T& v, int ind1, int ind2){ 
	Data d;
	d.setRaw(mData, offset() + indexFlat(ind1,ind2), type());
	d.shapeAll(1);
	d.assign(v);
	return *this;
}

#define DATA_SET(t, T)\
template<> inline Data& Data::set<t>(t * src, const int * sizes, int n){\
	free(); mData=(char*)(src); mOffset=0; shape(sizes,n); mType=Data::T; return *this;\
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
//DATA_SET(void, VOID)
#undef DATA_SET

} // glv::
#endif
