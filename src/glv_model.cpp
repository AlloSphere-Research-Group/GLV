#include "glv_model.h"
#include <stdio.h>	// sscanf, FILE
#include <cctype>	// isalnum, isblank
#include <cstring>	// strchr, strpbrk

//#ifndef WIN32
//#define	sprintf_s(buffer, buffer_size, stringbuffer, ...) (snprintf(buffer, buffer_size, stringbuffer, __VA_ARGS__))
//#endif

namespace glv{

// std::isblank not supported by some compilers!
bool isBlank(char c){
	return (c==' ') || (c=='\t');
}


bool isIdentifier(const std::string& v){
	if(isalpha(v[0]) || '_'==v[0]){
		unsigned i=1;
		for(; i<v.size(); ++i){
			if(!(std::isalnum(v[i]) || '_'==v[i])) break;
		}
		if(v.size()==i) return true;
	}
	return false;
}

template<> int toString<bool>(std::string& dst, const bool& src){
	dst = src ? "1" : "0";
	return 1;
}
template<> int toString<int>(std::string& dst, const int& src){//
	return toString(dst, src, "%i");
}
template<> int toString<float>(std::string& dst, const float& src){
	return toString(dst, src, "%g");
}
template<> int toString<double>(std::string& dst, const double& src){
	return toString(dst, src, "%g");
}
template<> int toString<std::string>(std::string& dst, const std::string& src){
	dst = src; return 1;
}
int toString(std::string& dst, const char * src){ return toString(dst, std::string(src)); }


template<>
int fromToken<bool>(bool& dst, const char * src){
	int v = dst;
	int r = sscanf(src, "%i", &v) > 0;
	if(r) dst = v;
	return r;
}
template<>
int fromToken<int>(int& dst, const char * src){
	return sscanf(src, "%i", &dst) > 0;
}
template<>
int fromToken<float>(float& dst, const char * src){
	return sscanf(src, "%g", &dst) > 0;
}
template<>
int fromToken<double>(double& dst, const char * src){
	return sscanf(src, "%lg", &dst) > 0; // l specifier only needed for scanf
}
template<>
int fromToken<std::string>(std::string& dst, const char * src){
	const char * beg = std::strchr(src, '\"');
	if(beg){
		++beg; // go to char past first "
		const char * end = std::strchr(beg, '\"');
		if(end){
			dst.assign(beg, end-beg);
			return 1;
		}
	}
	return 0;
}

template<> int fromToken<bool>(bool& dst, const std::string& src){
	return fromToken(dst, src.c_str());
}
template<> int fromToken<int>(int& dst, const std::string& src){
	return fromToken(dst, src.c_str());
}
template<> int fromToken<float>(float& dst, const std::string& src){
	return fromToken(dst, src.c_str());
}
template<> int fromToken<double>(double& dst, const std::string& src){
	return fromToken(dst, src.c_str());
}
template<> int fromToken<std::string>(std::string& dst, const std::string& src){
	return fromToken(dst, src.c_str());
}

template<class T>
static int fromToken(
	T * dst, int size, int stride, const std::string& src, const char * match){
	const char * s = src.c_str();
	int i=-1;
	while(++i<size && s){
		if(!(s = std::strpbrk(s, match))) break;
		fromToken(dst[i*stride], s);
		s = std::strpbrk(s, " ,");
	}
	return i;
}
template<> int fromToken<bool>(bool * dst, int size, int stride, const std::string& src){
	return fromToken(dst,size,stride, src, "01");
}
template<> int fromToken<int>(int * dst, int size, int stride, const std::string& src){
	return fromToken(dst,size,stride, src, "0123456789-+");
}
template<> int fromToken<float>(float * dst, int size, int stride, const std::string& src){
	return fromToken(dst,size,stride, src, "0123456789-+.");
}
template<> int fromToken<double>(double * dst, int size, int stride, const std::string& src){
	return fromToken(dst,size,stride, src, "0123456789-+.");
}
template<> int fromToken<std::string>(std::string * dst, int size, int stride, const std::string& src){
	const char * s = src.c_str();
	int i=-1;
	while(++i<size && s){
		if(!(s = std::strchr(s, '\"'))) break;
		++s;
		const char * e = std::strchr(s, '\"');
		if(!e) break;
		dst[i*stride].assign(s, e-s);
		s = std::strpbrk(e, " ,");
	}
	return i;
}

int toToken(std::string& dst, const char * src){ return toToken(dst, std::string(src)); }


int fromToken(Data& d, const std::string& s){
	switch(d.type()){
	case Data::BOOL:	return glv::fromToken(       d.elems<bool>(), d.size(), d.stride(), s);
	case Data::INT:		return glv::fromToken(        d.elems<int>(), d.size(), d.stride(), s);
	case Data::FLOAT:	return glv::fromToken(      d.elems<float>(), d.size(), d.stride(), s);
	case Data::DOUBLE:	return glv::fromToken(     d.elems<double>(), d.size(), d.stride(), s);
	case Data::STRING:	return glv::fromToken(d.elems<std::string>(), d.size(), d.stride(), s);
	default: return 0;
	}
}
int toString(std::string& s, const Data& d){
	if(!d.hasData()) return 0;
	switch(d.type()){
	case Data::BOOL:	return glv::toString(s,        d.elems<bool>(), d.size(), d.stride());
	case Data::INT:		return glv::toString(s,         d.elems<int>(), d.size(), d.stride());
	case Data::FLOAT:	return glv::toString(s,       d.elems<float>(), d.size(), d.stride());
	case Data::DOUBLE:	return glv::toString(s,      d.elems<double>(), d.size(), d.stride());
	case Data::STRING:	return glv::toString(s, d.elems<std::string>(), d.size(), d.stride());
	default: return 0;
	}
}
int toToken(std::string& s, const Data& d){
	if(!d.hasData()) return 0;
	switch(d.type()){
	case Data::BOOL:	return glv::toToken(s,        d.elems<bool>(), d.size(), d.stride());
	case Data::INT:		return glv::toToken(s,         d.elems<int>(), d.size(), d.stride());
	case Data::FLOAT:	return glv::toToken(s,       d.elems<float>(), d.size(), d.stride());
	case Data::DOUBLE:	return glv::toToken(s,      d.elems<double>(), d.size(), d.stride());
	case Data::STRING:	return glv::toToken(s, d.elems<std::string>(), d.size(), d.stride());
	default: return 0;
	}
}


Indexer::Indexer(int size1, int size2, int size3){
	int sizes[] = {size1,size2,size3};
	setSizes(sizes);
	setOffsets();
	reset();
}

Indexer::Indexer(const int * sizes, int * offsets){
	setSizes(sizes);
	setOffsets(offsets);
	reset();
}

Indexer& Indexer::reset(){
	mIndex[0] = mOffsets[0]-1;
	for(int i=1; i<N; ++i){ mIndex[i] = mOffsets[i]; }
	return *this;
}

Indexer& Indexer::shape(const int * sizes, int n){
	setSizes(sizes,n); return *this;
}

Indexer& Indexer::shape(int size1, int size2, int size3){
	int sizes[] = {size1, size2, size3};
	return shape(sizes, 3);
}

void Indexer::setSizes(const int * v, int n){
	for(int i=0;i<n;++i) mSizes[i]=v[i];
}

void Indexer::setOffsets(const int * v, int n){
	for(int i=0;i<n;++i) mOffsets[i] = v?v[i]:0;
}



// Get iteration count for element-wise operations
static inline int count(const Data& a, const Data& b){
	return a.size()<b.size() ? a.size() : b.size();
}

//#define PDEBUG printf("%p %s\t", this, __func__); print(); printf("\n")
#define PDEBUG
/*
	pointer mData;				// pointer to first element of source data
	pointer mElems;				// pointer to first element in this slice
	int mStride;				// stride factor
	int mSizes[DATA_MAXDIM];	// sizes of each dimension
	Type mType;	
*/

Data::Data()
:	mData(0), mElems(0), mStride(1), mType(Data::NONE)
{
	shapeAll(0);
	PDEBUG;
}

Data::Data(Data& v)
:	mData(0), mElems(0), mStride(1), mType(Data::NONE)
{ shapeAll(0); *this = v; PDEBUG; }

Data::Data(const Data& v)
:	mData(0), mElems(0), mStride(1), mType(Data::NONE)
{ shapeAll(0); *this = v; PDEBUG; }

Data::Data(Data::Type type, int n1, int n2, int n3, int n4)
:	mData(0), mElems(0), mStride(1), mType(type)
{
	int s[] = {n1,n2,n3,n4};
	shape(s,4);
	clone();
	if(isNumerical()) assignAll(0);
	PDEBUG;
}

Data::~Data(){
	clear();
}

Data& Data::operator= (const Data& v){
	if(&v != this){
		setRaw(v.mData, v.offset(), v.stride(), v.type());
		for(int i=0;i<maxDim();++i) mSizes[i]=v.mSizes[i];
	}
	return *this;
}

bool Data::operator==(const Data& v) const {
	if(hasData() && v.hasData()){
		// TODO: how do we compare numbers and strings?

		#define OP(t1, t2)\
			for(int i=0; i<count(*this,v); ++i){\
				if(elem<t1>(i) != v.elem<t2>(i)) return false;\
			} return true

		#define OPALL(t)\
			switch(v.type()){\
			case Data::BOOL:	OP(t, bool);\
			case Data::INT:		OP(t, int);\
			case Data::FLOAT:	OP(t, float);\
			case Data::DOUBLE:	OP(t, double);\
			default: return false;\
			}

		switch(type()){
		case Data::BOOL:	OPALL(bool);
		case Data::INT:		OPALL(int);
		case Data::FLOAT:	OPALL(float);
		case Data::DOUBLE:	OPALL(double);
		case Data::STRING:	OP(std::string, std::string);
		default:;
		}
	}
	return false;
	#undef OP
	#undef OPALL
}

Data& Data::operator+=(const Data& v){

	#define OP(t1, t2)\
	for(int i=0; i<count(*this,v); ++i){ elem<t1>(i) += v.elem<t2>(i); } break

	#define OPALL(t)\
		switch(v.type()){\
		case BOOL:		OP(t, bool);\
		case INT:		OP(t, int);\
		case FLOAT:		OP(t, float);\
		case DOUBLE:	OP(t, double);\
		case STRING:\
		default:;\
		} break

	switch(type()){
	case Data::BOOL:	OPALL(bool);
	case Data::INT:		OPALL(int);
	case Data::FLOAT:	OPALL(float);
	case Data::DOUBLE:	OPALL(double);
	case Data::STRING: //printf("copy strings: %p -> %p\n", &v, this);
		switch(v.type()){
//		case Data::BOOL:	v.toString(data<std::string>());
		case Data::STRING:	OP(std::string, std::string);
		default:;
		} break;
	default:;
	}
	return *this;
	#undef OP
	#undef OPALL
}

Data& Data::assign(const Data& v, int idx){
	if(hasData() && v.hasData()){
		int nd= size()-idx;				// number of destination elements to assign
		int n = nd < v.size() ? nd : v.size();

		#define OP(t1, t2)\
		for(int i=0; i<n; ++i){ elem<t1>(i+idx) = v.elem<t2>(i); } break

		#define OPALL(t)\
			switch(v.type()){\
			case BOOL:		OP(t, bool);\
			case INT:		OP(t, int);\
			case FLOAT:		OP(t, float);\
			case DOUBLE:	OP(t, double);\
			case STRING:\
			default:;\
			} break

		switch(type()){
		case Data::BOOL:	OPALL(bool);
		case Data::INT:		OPALL(int);
		case Data::FLOAT:	OPALL(float);
		case Data::DOUBLE:	OPALL(double);
		case Data::STRING: //printf("copy strings: %p -> %p\n", &v, this);
			switch(v.type()){
//			case Data::BOOL:	v.toString(data<std::string>());
//			case Data::FLOAT:	OP(t, float);
//			case Data::DOUBLE:	OP(t, double);
			case Data::STRING:	OP(std::string, std::string);
			default:;
			} break;
		default:;
		}
	}
	return *this;
	#undef OP
	#undef OPALL
}


int Data::indexOf(const Data& v) const {
	if(hasData() && v.hasData()){

		#define OP(t1, t2)\
			for(int i=0; i<size(); ++i){\
				if(elem<t1>(i) == v.elem<t2>(0)) return i;\
			} break

		#define OPALL(t)\
			switch(v.type()){\
			case Data::BOOL:	OP(t, bool);\
			case Data::INT:		OP(t, int);\
			case Data::FLOAT:	OP(t, float);\
			case Data::DOUBLE:	OP(t, double);\
			default:;\
			} break

		switch(type()){
		case Data::BOOL:	OPALL(bool);
		case Data::INT:		OPALL(int);
		case Data::FLOAT:	OPALL(float);
		case Data::DOUBLE:	OPALL(double);
		case Data::STRING:	OP(std::string, std::string);
		default:;
		}
	}
	return npos;
	#undef OP
	#undef OPALL
}


bool Data::isZero() const{
	if(isNumerical()){
		#define DO(TYPE, type)\
		case TYPE:\
		if(stride()==1){\
			return (elem<type>(0)==type(0))\
				&& (0==memcmp(mElems, mElems+sizeof(type), (size()-1)*sizeof(type)));\
		}else{\
			for(int i=0; i<size(); ++i){\
				if(elem<type>(i) != type(0)) return false;\
			}\
			return true;\
		}
		switch(type()){
			DO(BOOL,bool) DO(FLOAT,float) DO(DOUBLE,double) DO(INT,int)
			default:;
		}
		#undef DO
	}
	return false;
}


void Data::clear(){
PDEBUG;
	if(release(mData)){
		switch(type()){
		case Data::BOOL:	delete[] data<bool>(); break;
		case Data::INT:		delete[] data<int>(); break;
		case Data::FLOAT:	delete[] data<float>(); break;
		case Data::DOUBLE:	delete[] data<double>(); break;
		case Data::STRING:	delete[] data<std::string>(); break;
		default:;
		}
//		::free(mData);
	}
	mData=0;
	mElems=0;
	mStride=1;
	shapeAll(0);
//	mType=Data::NONE;	// LJP: leave type unchanged
PDEBUG;
}

void Data::clone(){
	if(hasData()){
		int cnt = references(mData);
		
		// cnt == 0		data points to external, unmanaged data
		// cnt  > 1		data points to another Data's cloned data
		// cnt == 1		data points to my own cloned data
		// allocate new memory if not sole owner of data
		if(cnt!=1){
			Data old(*this);
			realloc(type());
			assign(old);
		}
	}
	else{
		realloc(type());
	}
}

int Data::order() const {
	int r=0;
	for(int i=0; i<maxDim(); ++i){
		if(size(i)>0) ++r; 
	}
	return r;
}

// return highest dimension not equal to 1
static int highestDim(const Data& d){
	int i=d.maxDim()-1;
	for(; i>=0; --i){
		if(d.size(i) > (i?1:0)) break;
	}
	return i+1;
}

void Data::print() const{
	printf("%p + %d, (%d", mData, offset(), size(0));
	for(int i=1;i<highestDim(*this);++i){
		printf(",%d", size(i));
	}
	printf("):%+d, %s %s\n", stride(), typeToString(type()).c_str(), toToken().c_str());
}


// if sizes==0, then keep current shape
int Data::realloc(Data::Type t, const int * sizes, int n){
	Data old(*this); // REV0

	if(sizes){		// new shape requested
		clear();
		shape(sizes, n);
	}
	else{			// just changing type, leave shape unchanged
//		Data old(*this); // REV0
		clear();
		shape(old.mSizes, old.maxDim());
	}	

	if(size()){
		mType  = t;
		mStride= 1;
		switch(type()){
		case Data::BOOL:	mData = pointer(new bool[size()]); break;
		case Data::INT:		mData = pointer(new int[size()]); break;
		case Data::FLOAT:	mData = pointer(new float[size()]); break;
		case Data::DOUBLE:	mData = pointer(new double[size()]); break;
		case Data::STRING:	mData = pointer(new std::string[size()]); break;
		default:			goto end;
		}
		acquire(mData);
		offset(0);
//		if(hasData() && isNumerical()) assignAll(0); // REV0

		if(hasData() && isNumerical()){
			if(old.hasData()){
				assign(old);	// copy over as many old elements as possible
				if(size() > old.size()) slice(old.size()).assignAll(0);
			}
			else{
				assignAll(0);
			}
		}
	}

	end:
	return sizeBytes() - old.sizeBytes();
}


int Data::resize(Data::Type t, const int * sizes, int n){
PDEBUG;

	int numBytes = 0;

//	if(t!=NONE && (type()!=t || size()!=product(sizes,n))){
//		realloc(t, sizes,n);
//	}

	// only do allocation if valid type
	if(NONE != t){
	
		int newsize = product(sizes,n);

		if(size() == 0 && newsize == 0){		// no sizes, just change type
			mType = t;
		}
		else if((type() != t) || (size() != newsize)){	// different type or size triggers realloc
			numBytes = realloc(t, sizes,n);
		}
	
//		printf("resize: (%d %d) %d %d \t\t", t, type(), size(), product(sizes,n)); print();
//		if(type()!=t || size()!=product(sizes,n)){
//			printf("reallocing\n");
//			realloc(t, sizes,n);
//		}
//		else if(size()==0){
//			printf("size()==0\n");
//			mType = t;
//		}
//		else{
//			printf("else\n");
//		}
	}
	else{
		// should we free previous memory?
	}

PDEBUG;
	return numBytes;
}

void Data::setRaw(void * dt, int off, int stride, Type ty){
	// increment reference count first to avoid problems when dt == mData
	incrementCount(pointer(dt));
	clear();
	mData  = pointer(dt);
	mStride= stride;
	mType  = ty;
	offset(off);
}

// TODO: conform sizes to current size
Data& Data::shape(const int * sizes, int n){
	if(n){
		if(n <= maxDim()){
			for(int i=0;i<n;++i) mSizes[i]=sizes[i];
			for(int i=n;i<maxDim();++i) mSizes[i]=1;
		}
		else{
			for(int i=0;i<maxDim();++i) mSizes[i]=sizes[i];
		}
	}
	return *this; 
}

Data& Data::shapeAll(int n){
	for(int i=0;i<maxDim();++i) mSizes[i]=n;
	return *this;
}

int Data::sizeType() const {
	switch(type()){
	case Data::BOOL:	return sizeof(bool);
	case Data::INT:		return sizeof(int);
	case Data::FLOAT:	return sizeof(float);
	case Data::DOUBLE:	return sizeof(double);
	case Data::STRING:	return sizeof(std::string);
	default:			return 1; // non-zero to avoid divide by 0 exceptions
	}
}

Data Data::slice(int beg, int sz, int st) const {
	Data r(*this);
	int b = r.offset() + beg;
	
	// constrain offset and size to limits of parent data
	if(b < 0) b = 0;

	// TODO: constrain size
	int s = sz;
	
	r.offset(b);
	r.stride(st);
	r.shape(&s, 1);
	return r;
}

Data& Data::type(Data::Type ty){
	if(type()!=ty && size()){
		resize(ty, mSizes, maxDim());
	}
	else{
		mType=ty;
	}
	return *this;
}

std::string Data::typeToString(Type t){
	switch(t){
	case BOOL:	return "bool";
	case INT:	return "int";
	case FLOAT:	return "float";
	case DOUBLE:return "double";
	case STRING:return "std::string";
	default:	return "void";
	}
}



/*	ModelManager TODO list: 
		support for undo/redo
		incremental snapshots, sequence numbers
*/

// TODO:	need to delete allocated Models
//			need to be able to remove Data
//void ModelManager::add(const std::string& name, Data& v){
//	if(isIdentifier(name)) mNameVal[name] = new Model(v);
//}

void ModelManager::add(const std::string& name, Model& v){
	if(isIdentifier(name)) mState[name] = &v;
}
void ModelManager::add(const std::string& name, const Model& v){
	if(isIdentifier(name)) mConstState[name] = &v;
}

void ModelManager::clearModels(){
	mState.clear();
	mConstState.clear();
}

ModelManager& ModelManager::copyModels(const ModelManager& m){
	mState = m.mState;
	mConstState = m.mConstState;
	return *this;
}

void ModelManager::clearSnapshots(){
	mSnapshots.clear();
}

bool ModelManager::defaultFilePath(std::string& s) const {
	if(!mFileName.empty()){
		s = mFileDir + mFileName;
		return true;
	}
	else if(!name().empty()){
		s = mFileDir + name() + ".txt";
		return true;
	}
	return false;
}

ModelManager& ModelManager::filePath(const std::string& name, const std::string& dir){
	mFileName=name;
	return fileDir(dir);
}

ModelManager& ModelManager::fileDir(const std::string& dir){
	mFileDir = dir;
	
	char delim = '/';
	if(dir.size() && dir[dir.size()-1] != delim){
		mFileDir += delim;
	}

	return *this;
}

ModelManager& ModelManager::name(const std::string& v){
	if(isIdentifier(v)) mName=v;
	return *this;
}

void ModelManager::remove(const std::string& name){
	if(mState.count(name)){
//		Model * m = mNameVal[name];
		mState.erase(name);
//		if(allocated(m)){
//			delete m;
//		}
	}
	if(mConstState.count(name)){
		mConstState.erase(name);
	}
}


void ModelManager::printSnapshots() const {
	for(const auto& its : mSnapshots){
		printf("\"%s\"\n", its.first.c_str());			
		for(const auto& itd : its.second){
			printf("    \"%s\": ", itd.first.c_str());
			itd.second.print();
		}
	}
}


//struct TableWriter{
//
//	virtual void onKeyValue(const std::string& key, const std::string& val) = 0;
//
//	void operator()(std::string& dst, const std::string& modelName){
//		#define NEWLINE "\r\n"
//		if(modelName.size())	dst = "[\"" + modelName + "\"] = {"NEWLINE;
//		else					dst = "{"NEWLINE;
//
//		std::string t;
//		{
//			NamedModels::const_iterator i = mState.begin();
//			for(; i!=mState.end(); ++i){
//				dst += "\t" + namedDataToString(i->first, i->second->model());
//			}
//		}
//		{
//			NamedConstModels::const_iterator i = mConstState.begin();
//			for(; i!=mConstState.end(); ++i){
//				dst += "\t" + namedDataToString(i->first, i->second->model());
//			}
//		}
//		dst += "}"NEWLINE;
//		#undef NEWLINE
//	}
//
//};


int ModelManager::snapshotsToFile(const std::string& path_in) const {
	std::string s;
	if(!snapshotsToString(s)) return 0;

	std::string path = path_in;
	if(path.empty()){
		if(!defaultFilePath(path)) return 0;
	}

	int r=0;
	FILE * fp = fopen(path.c_str(), "w");
	if(fp){
		r = fwrite(s.c_str(), sizeof(std::string::value_type), s.size(), fp);
		fclose(fp);
	}
	return r;
}

int ModelManager::snapshotsFromFile(const std::string& path_in, bool add){
	std::string path = path_in;
	if(path.empty()){
		if(!defaultFilePath(path)) return 0;
	}

	int r=0;
	FILE * fp = fopen(path.c_str(), "r");
	if(fp){
		
		char buf[512];
		std::string s;
		while(!feof(fp)){
			int n = fread(buf, sizeof(buf[0]), sizeof(buf), fp);
			s.append(buf, n);
			r += n;
		}
		
		if(!add) clearSnapshots();
		snapshotsFromString(s);

		fclose(fp);
	}
	return r;
}

#define GLV_ENDL "\n"
//#define GLV_ENDL "\r\n"

static std::string namedDataToString(const std::string& s, const Data& d){
	std::string r;
	if(d.toToken(r)) r = s + " = " + r + "," GLV_ENDL;
	return r;
}

//bool ModelManager::stateToToken(std::string& dst, const std::string& modelName) const {
//	#define NEWLINE "\r\n"
//	if(modelName.size())	dst = "[\"" + modelName + "\"] = {"NEWLINE;
//	else					dst = "{"NEWLINE;
//
//	std::string t;
//	{
//		NamedModels::const_iterator i = mState.begin();
//		for(; i!=mState.end(); ++i){
//			dst += "\t" + namedDataToString(i->first, i->second->getData());
//		}
//	}
//	{
//		NamedConstModels::const_iterator i = mConstState.begin();
//		for(; i!=mConstState.end(); ++i){
//			dst += "\t" + namedDataToString(i->first, i->second->getData());
//		}
//	}
//	dst += "}"NEWLINE;
//	#undef NEWLINE
//	return true;
//}

int ModelManager::snapshotsToString(std::string& dst) const {
	if(mSnapshots.empty()) return 0;

	if(!name().empty())	dst = name() + " = ";
	else				dst = "";

	dst += "{" GLV_ENDL;
	for(const auto& its : mSnapshots){
		dst += "[\"" + its.first + "\"] = {" GLV_ENDL;
		for(const auto& itd : its.second){ // iterate over Data map
			if(itd.second.hasData()){
				dst += "\t" + namedDataToString(itd.first, itd.second);
			}
		}
		dst += "}," GLV_ENDL GLV_ENDL;
	}
	dst += "}" GLV_ENDL;
	return dst.size();
}


struct KeyValueParser{

	virtual ~KeyValueParser(){}

	virtual void onKeyValue(const std::string& key, const std::string& val) = 0;
	
	int operator()(const std::string& src){

		size_t p = src.find_first_of("{");
		if(std::string::npos == p) return 0;		// no table found, so return
		
		std::string key, val;
		const char * b = &src[p+1];	// start 1 character after opening '{'

		while(*b && *b!='}'){
			if(isalpha(*b) || *b=='_'){	// is character valid start of identifier?
				const char * e = b+1;
				while(std::isalnum(*e) || *e=='_') ++e;	// go to end of key name
				key.assign(b, e-b);

				while(isBlank(*e) || *e=='=') ++e;	// go to '='
				
				// find next valid value token
				b=e=std::strpbrk(e, "\"{0123456789.-+");

				if(!b) b=e=&src[src.size()];	// no more valid tokens, so go to end of string

				if(*b){
					// munch characters until end of token
					if(*e == '\"'){
						++e; while(*e!='\"') ++e; ++e;
					}
					else if(*e == '{'){
						while(*e!='}') ++e; ++e;
					}
					else{
						while((isdigit(*e) || *e=='.' || *e=='-' || *e=='+' || *e=='e')) ++e;
					}

					val.assign(b,e-b);

					onKeyValue(key, val);
				}
				b=e;
			}
			++b;
		}
		//printf("%d\n", b-&v[0]);
		
		if(*b == '}')	return b+1-&src[0];
		else			return b-&src[0];	
	}
};


//int ModelManager::stateFromToken(const std::string& src){
//
//	struct It : KeyValueParser {
//		It(NamedModels& v): m(v){}
//		void onKeyValue(const std::string& key, const std::string& val){
//			if(m.count(key)){
//				//printf("%s = %s\n", key.c_str(), val.c_str());
//				m[key]->modelFromString(val);
//			}
//		}
//		NamedModels& m;
//	} it(mState);
//	
//	return it(src);
//}



static bool goToNext(unsigned& p, char c, const std::string& str){
	size_t n = str.substr(p).find_first_of(c);
//	printf("%c: %d %d (%.5s ...)\n", c, p, n, str.substr(p).c_str());
	if(std::string::npos != n){
		p += n;
		return true;
	}
	return false;
}

static bool goToNextPrintablePast(unsigned& p, char c, const std::string& str){
	size_t n = str.substr(p).find_first_not_of(" \t\r\n");
//	printf("%c: %d %d (%.5s ...)\n", c, p, n, str.substr(p).c_str());
	if(std::string::npos != n){
		p += n;
//		printf("\t%c\n", str[p]);
		if(str[p] == c){ ++p; return true; }
	}
	return false;
}

//static int Data::interpretToken(Data::Type& type, int *& sizes, std::string& s){
//	unsigned p=0;
//	int level=-1;
//	int elems=0;
//	
//	for(int i=0;i<maxDim();++i) sizes[i]=0;
//
//	do{
//		switch(s[p]){
//		case '{': ++level; break;
//		case '}': --level; break;
//		case ',':
//		default:;
//		}
//	} while(++p < s.size());
//
////	if(s[p] == '{'){
////	
////	}
////	else{
////	
////	}
//}

int ModelManager::snapshotFromString(const std::string& src){
	//printf("%s\n", src.c_str());
	unsigned r = src.size();
	unsigned p=0, p2=0;

	// look for table name
	if(!goToNextPrintablePast(p, '[' , src)) return r;
	if(!goToNextPrintablePast(p, '\"', src)) return r;
	if(!goToNext(p2=p, '\"', src)) return r;

	std::string name = src.substr(p, p2-p);
	//printf("%s\n", name.c_str());
	p = p2+1;

	if(!goToNextPrintablePast(p, ']' , src)) return r;
	if(!goToNextPrintablePast(p, '=' , src)) return r;
	//printf("%s\n", src.substr(p).c_str());

	// retrieve key-value pairs
	struct It : KeyValueParser {
		It(Snapshot& vs, NamedModels& vm): s(vs), m(vm){}
		void onKeyValue(const std::string& key, const std::string& val){
			//printf("%s = %s\n", key.c_str(), val.c_str());

			// Only convert value string if main state contains key 
			// with same name.
			NamedModels::const_iterator it = m.find(key);
			if(it != m.end()){
				Data& ds = s[key]; //ds.print();

				// Set size of snapshot Data according to application Model				
				if(true){
					Data temp;
					const Data& dm = it->second->getData(temp);

					ds.resize(dm.type(), dm.shape(), dm.maxDim());
					ds.fromToken(val);
					//ds.print();
				}

				// Or, resize application model and snapshot data according to
				// number of elements counted in string.
				/*else {
					int N = numElemsToken(val);
					
					ds.resize(dm.type(), N);
					ds.fromToken(val);
				}*/
			}
		}
		Snapshot& s;
		NamedModels& m;
	} it(mSnapshots[name], mState);
	
	p += it(src.substr(p));
	return p;
}


int ModelManager::snapshotsFromString(const std::string& src){
	
	unsigned r = src.size();
	unsigned p=0;
	if(!goToNext(p, '{' , src)) return r;
	++p;
	
	do{
		p += snapshotFromString(src.substr(p));
		if(!goToNext(p, ',', src)) return r;
		++p;
	} while(p < src.size());

	return p;
}


void ModelManager::saveSnapshot(const std::string& name){
	//if(mSnapshots.count(name)){} // TODO: always overwrite existing?
	auto& snapshot = mSnapshots[name];

	// fetch read-write model values
	for(const auto& it : mState){
		Data temp;
		(snapshot[it.first] = it.second->getData(temp)).clone();
	}

	// fetch read-only model values
	for(const auto& it : mConstState){
		Data temp;
		(snapshot[it.first] = it.second->getData(temp)).clone();
	}
}


bool ModelManager::loadSnapshot(int i){
	for(auto& ss : mSnapshots){
		if(i-- == 0) return loadSnapshot(ss.first);
	}
	return false;
}


bool ModelManager::loadSnapshot(const std::string& name){
	Snapshots::const_iterator ssit = mSnapshots.find(name);
	if(mSnapshots.end() != ssit){
		const auto& snapshot = ssit->second;

		for(const auto& it : mState){
			Snapshot::const_iterator sit = snapshot.find(it.first);
			if(snapshot.end() != sit){
				it.second->setData(sit->second);
				/*printf("set state: \"%s\" = ", it->first.c_str());
				sit->second.print();
				Data temp;
				printf("\tstate is now "); it->second->getData(temp).print();*/
			}
		}
		return true;
	}
	return false;
}

bool ModelManager::loadSnapshot(const Snapshot& ss1, const Snapshot& ss2, double c1, double c2){
	auto it1 = ss1.begin();
	auto it2 = ss2.begin();
	
	while(ss1.end() != it1 && ss2.end() != it2){
		
		const auto& id1 = it1->first;
		const auto& id2 = it2->first;

		int cmp = id1.compare(id2);
		
		if(0 == cmp){		// ids match, attempt interpolation
			
			const auto itState = mState.find(id1);
			if(mState.end() != itState){	// found model with this id, lock and load!
				const Data& data1 = it1->second;
				const Data& data2 = it2->second;
				
				Data temp = data1;
				temp.clone();
				temp.mix(data1, data2, c1, c2);
				itState->second->setData(temp);
			}
			
			++it1;
			++it2;
		}
		else if(cmp < 0){	// first id less than second id
			++it1;			// let first iterator catch up with second
		}
		else{				// first id greater than second id
			++it2;			// let second iterator catch up with first
		}
	}
	return true;
}


bool ModelManager::loadSnapshot(
	const Snapshot& ss1, const Snapshot& ss2, const Snapshot& ss3, const Snapshot& ss4,
	double c1, double c2, double c3, double c4
){
	const int N = 4;
	Snapshot::const_iterator it[N] = {ss1.begin(), ss2.begin(), ss3.begin(), ss4.begin()};
	double cs[N] = {c1,c2,c3,c4};

	while(ss1.end() != it[0] && ss2.end() != it[1] && ss3.end() != it[2] && ss4.end() != it[3]){

		/*
		First, sort strings into A <= B <= C <= D
		
		if(A == B && B == C && C == D){
			interpolate
			 ++itA; ++itB; ++itC; ++itD;
		}
		else if(C != D) ++itA; ++itB; ++itC;
		else if(B != C) ++itA; ++itB;
		else if(A != B) ++itA;
		*/

		int sort[N]; // = 0, 1, 2, ..., N-1
		for(int i=0;i<N;++i) sort[i]=i;
		
		// do a shell sort
		int cmp13 = it[sort[0]]->first.compare(it[sort[2]]->first);
		int cmp24 = it[sort[1]]->first.compare(it[sort[3]]->first);
		
		if(cmp13 > 0) std::swap(sort[0], sort[2]);
		if(cmp24 > 0) std::swap(sort[1], sort[3]);

		int cmpAB = it[sort[0]]->first.compare(it[sort[1]]->first);
		int cmpCD = it[sort[2]]->first.compare(it[sort[3]]->first);

		if(cmpAB > 0) std::swap(sort[0], sort[1]);
		if(cmpCD > 0) std::swap(sort[2], sort[3]);

		int cmpBC = it[sort[1]]->first.compare(it[sort[2]]->first);

		if(cmpAB==0 && cmpBC==0 && cmpCD==0){
			const auto itState = mState.find(it[0]->first);
			if(mState.end() != itState){	// found model with this id, lock and load!
				const Data * D[N];
				for(int i=0; i<N; ++i) D[i] = &(it[i]->second);

				Data temp = *D[0];
				temp.clone();
				temp.mix<N>(D, cs);
				itState->second->setData(temp);
			}
			for(int i=0;i<N;++i) ++(it[i]);		
		}
		else if(cmpCD){ for(int i=0;i<N-1;++i) ++(it[i]); }
		else if(cmpBC){ for(int i=0;i<N-2;++i) ++(it[i]); }
		else if(cmpAB){ for(int i=0;i<N-3;++i) ++(it[i]); }
	}
	return true;
}


bool ModelManager::loadSnapshot(
	const std::string& name1, const std::string& name2,
	double c1, double c2
){
//	const std::string* names[] = {&name1, &name2}; 
//	const double cs[] = {c1,c2};
//	return loadSnapshot<2>(names, cs);
	const auto it1 = mSnapshots.find(name1);
	if(mSnapshots.end() == it1) return false;
	const auto it2 = mSnapshots.find(name2);
	if(mSnapshots.end() == it2) return false;
	return loadSnapshot(it1->second, it2->second, c1,c2);
}

bool ModelManager::loadSnapshot(
	const std::string& name1, const std::string& name2, const std::string& name3, const std::string& name4,
	double c1, double c2, double c3, double c4
){
//	const std::string* names[] = {&name1, &name2, &name3, &name4}; 
//	const double cs[] = {c1,c2,c3,c4};
//	return loadSnapshot<4>(names, cs);
	const auto it1 = mSnapshots.find(name1);
	if(mSnapshots.end() == it1) return false;
	const auto it2 = mSnapshots.find(name2);
	if(mSnapshots.end() == it2) return false;
	const auto it3 = mSnapshots.find(name3);
	if(mSnapshots.end() == it3) return false;
	const auto it4 = mSnapshots.find(name4);
	if(mSnapshots.end() == it4) return false;
	return loadSnapshot(it1->second, it2->second, it3->second, it4->second, c1,c2,c3,c4);
}


// to header...
//template <int N>
//bool ModelManager::loadSnapshot(const std::string ** names, const double * c){
//	const Snapshot * snapshots[N];
//	for(int i=0; i<N; ++i){
//		Snapshots::const_iterator it = mSnapshots.find(names[i]);
//		if(mSnapshots.end() == it) return false;
//		snapshots[i] = &it->second;	
//	}
//	return loadSnapshot<N>(snapshots, c);
//}

//bool ModelManager::loadSnapshot(const std::string& name1, const std::string& name2, double c1, double c2){
//	Snapshots::const_iterator it1 = mSnapshots.find(name1);
//	if(mSnapshots.end() == it1) return false;
//	Snapshots::const_iterator it2 = mSnapshots.find(name2);
//	if(mSnapshots.end() == it2) return false;
//	return loadSnapshot(it1->second, it2->second, c1,c2);
//}


// This runs through the current set of models and checks that snapshots have same
// parameters. If a snapshot is missing a parameter, it is copied in from the
// current model state.
void ModelManager::makeClosed(){

	// Run through each model in set
	for(const auto& itm : mState){
		const auto& paramName = itm.first;
		Data temp; 
		temp = itm.second->getData(temp);
	
		// Run through all the snapshots checking for param name
		for(auto& its : mSnapshots){
			const auto& ssName = its.first;
			Snapshot& ss = its.second;
			if(ss.find(paramName) == ss.end()){
				printf("In set \"%s\": Parameter \"%s\" not found in preset \"%s\"\n",
					name().c_str(), paramName.c_str(), ssName.c_str());
				(ss[paramName] = temp).clone();
			}
		}
	}
}

void ModelManager::zeroSmallValues(double eps){
	for(auto& its : snapshots()){ // (name, Snapshot)
		for(auto& itd : its.second){ // (name, Data)
			Data& d = itd.second;
			if(d.type() == Data::FLOAT || d.type() == Data::DOUBLE){
				for(int i=0; i<d.size(); ++i){
					double v = d.at<double>(i);
					if(-eps <= v && v <= eps){
						d.assign(0., i);
					}
				}
			}
		}
	}
}


} // glv::
