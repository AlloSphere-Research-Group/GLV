#ifndef INC_GLV_MODEL_H
#define INC_GLV_MODEL_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <map>
#include <vector>
#include <string>

namespace glv {

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



struct Stringifiable{

	virtual ~Stringifiable(){}

	/// Convert from string. Returns true on success.
	virtual bool fromToken(const std::string& v){ return false; }

	/// Convert to string. Returns true on success.
	virtual bool toToken(std::string& v) const { return false; }
	
	std::string toToken(){ std::string r; toString(r); return r; }
};



template <class T>
class ReferenceCounted {
public:

	/// Returns number of pointers to memory address being managed
	static int references(T * const m){ return counting(m) ? refCount()[m] : 0; }

protected:
	typedef std::map<T *, int> RefCount;

	// Begin counting reference or increment count if already counting
	static void acquire(T * m){
		if(!m) return; // do not count null pointers for obvious reasons
		counting(m) ? ++refCount()[m] : refCount()[m]=1;
	}
	
	// Increment count of reference if reference is being counted
	static void incrementCount(T * m){
		if(counting(m))	++refCount()[m];
	}

	// Returns true if the reference was being counted and can now be freed
	static bool release(T * m){
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
	static bool counting(T* const m){ return refCount().count(m) != 0; }
//	static bool owner(T* const m){ return references(m) != 1; }
};



/// Generic wrapper around single or arrayed primitive types
class Data : public ReferenceCounted<void>{
public:
	enum Type{
//		CONST=(1<<31),
		VOID=0, BOOL, INT, FLOAT, DOUBLE, STRING,
//		CONST_VOID		= CONST|VOID,
//		CONST_BOOL		= CONST|BOOL,
//		CONST_INT		= CONST|INT,
//		CONST_FLOAT		= CONST|FLOAT,
//		CONST_DOUBLE	= CONST|DOUBLE,
//		CONST_STRING	= CONST|STRING
	};
	static std::string typeToString(Type t);


	Data();
	Data(Data& v);
	Data(const Data& v);
	Data(Data::Type type, int size1, int size2);

	template <class T>
	explicit Data(T& value){ set(&value, 1); }

	template <class T>
	explicit Data(const T& value){ set(&value, 1); }

	template <class T>
	Data(T * data, int size1, int size2=1){ set(data, size1, size2); }

	~Data();
	

//	/// Convert variable to human-readable string
//	virtual bool toString(std::string& dst) const;
//	
//	/// Convert human-readable string to variable
//	virtual bool fromString(const std::string& src);

	bool operator==(const Data& v) const;
	bool operator!=(const Data& v) const { return !((*this)==v); }

	template <class T>
	const T at(int ind1, int ind2=0) const;

	template <class T>
	const T * data() const { return static_cast<const T *>(mData); }

	bool hasData() const { return mData!=0; }
	int indexFlat(int i1, int i2) const { return i1 + i2*size1(); }
	void indexTo2(int i, int& i1, int& i2) const { i1=i%size1(); i2=i/size1(); }
	bool indexValid(int i1, int i2){ return (i1>=0) && (i1<size1()) && (i2>=0) && (i2<size2()); }
	bool isNumerical() const { return type() != Data::STRING; }
	int size() const { return size1()*size2(); }
	int size1() const { return mSize1; }
	int size2() const { return mSize2; }
	int sizeType() const;
	Type type() const { return mType; }
	Type typeKind() const { return (Type)(type() & maskType()); }


	void print() const;
	std::string toString() const { return glv::toString(*this); }

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

	// note: this is potentially dangerous call, therefore use with caution...
	template <class T>
	T * data(){ return static_cast<T *>(mData); }

	/// Set an element at specified index
	template <class T>
	Data& put(const T& v, int ind1=0, int ind2=0);
	
	//Data& resize(int size1, int size2=1);

	template <class T>
	Data& set(T& v){ return set(&v,1); }

	template <class T>
	Data& set(const T& v){ return set(&v,1); }

	Data& set(const char * v){ return set(std::string(v)); }

	template <class T>
	Data& set(T * data, int size1, int size2=1);

	Data& set(Data::Type type, int size1, int size2);

protected:
	void * mData;	// pointer to first array element
	int mSize1;		// number of elements in dimension 1
	int mSize2;		// number of elements in dimension 1
	Type mType;		// data type
	
	void free();
	void realloc(Data::Type type, int size1, int size2=1);
	
	void setRaw(void * data, int size1, int size2, Type type);

	static int maskType (){ return 0x7FFFFFFF; }
	static int maskConst(){ return 0x80000000; }
};

int toString(std::string& dst, const Data& src);
int toToken(std::string& dst, const Data& src);
int fromToken(Data& dst, const std::string& src);




class Model{
public:
	virtual ~Model(){}
	const Data& model() const { return mData; }

//	template <class T>
//	void put(const T& v, int ind1, int ind2){
//		
//	}

protected:
	Data mData;

	Data& model(){ return mData; }

	void assignModel(Data& d, int ind1, int ind2){
		if(model().indexValid(ind1, ind2)){
			onAssignModel(d, ind1, ind2);
			model().assign(d, ind1, ind2);
		}
	}

	void setModel(const Data& d){
		mData = d;
	}
	
	virtual void onAssignModel(Data& d, int ind1, int ind2){}
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
	bool toToken(std::string& dst, const std::string& modelName);
	
	/// Convert string to model state
	int fromToken(const std::string& src);

	/// Add new stringifiable object
	void add(const std::string& name, Stringifiable& s);

protected:
	typedef std::map<std::string, Stringifiable *> NameObjMap;

	NameObjMap mNameObj;
	std::map<std::string, std::string> mModelStrings;
	
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
const T Data::at(int ind1, int ind2) const {
	int i = indexFlat(ind1,ind2);
	switch(typeKind()){
	case Data::BOOL:	return static_cast<const T>(data<const bool>()[i]);
	case Data::INT:		return static_cast<const T>(data<const int>()[i]);
	case Data::FLOAT:	return static_cast<const T>(data<const float>()[i]);
	case Data::DOUBLE:	return static_cast<const T>(data<const double>()[i]);
	case Data::STRING: {
		T v; if(glv::fromToken(v, data<const std::string>()[i])){ return v; }
	}
	default: return T();
	}
}

template<>
inline const std::string Data::at<std::string>(int ind1, int ind2) const {
	int i = indexFlat(ind1,ind2);
	switch(typeKind()){
	case Data::BOOL:	return glv::toString(data<const bool>()[i]);
	case Data::INT:		return glv::toString(data<const int>()[i]);
	case Data::FLOAT:	return glv::toString(data<const float>()[i]);
	case Data::DOUBLE:	return glv::toString(data<const double>()[i]);
	case Data::STRING:	return data<const std::string>()[i];
	default: return "";
	}
}

template <class T>
Data& Data::put(const T& v, int ind1, int ind2){ 
	Data d;
	d.setRaw(
		static_cast<char *>(mData) + sizeType()*indexFlat(ind1,ind2),
		1,1, type()
	);
	d.assign(v);
	return *this;
}

#define DATA_SET(t, T)\
template<> inline Data& Data::set<t>(t * src, int n1, int n2){\
	free(); mData=src; mSize1=n1; mSize2=n2; mType=Data::T; return *this;\
}\
template<> inline Data& Data::set<const t>(const t * src, int n1, int n2){\
	if((n1*n2)!=size() || Data::T!=type()) realloc(Data::T, n1, n2);\
	for(int i=0; i<size(); ++i){ data<t>()[i] = src[i]; }\
	return *this;\
}
//template<> inline Data& Data::set<const t>(const t * data, int size){\
//	free(); mData=const_cast<t *>(data); mSize=size; mType=CONST_##T; return *this;\
//}

DATA_SET(bool, BOOL)
DATA_SET(int, INT)
DATA_SET(float, FLOAT)
DATA_SET(double, DOUBLE)
DATA_SET(std::string, STRING)
//DATA_SET(void, VOID)
#undef DATA_SET

} // glv::
#endif
