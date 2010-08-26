/*
 *  glv_model.cpp
 *  GLV
 *
 *  Created by Lance on 8/19/10.
 *  Copyright 2010 UCSB. All rights reserved.
 *
 */

#include "glv_model.h"
#include <cctype>	// isalnum, isblank
#include <stdio.h>	// sscanf
#include <string.h>	// strchr, strpbrk

namespace glv{

template<> int toString<bool>(std::string& dst, const bool& src){
	dst = src ? "1" : "0";
	return 1;
}
template<> int toString<int>(std::string& dst, const int& src){
	char buf[20]; snprintf(buf, sizeof(buf), "%i", src);
	dst = buf;
	return 1;
}
template<> int toString<float>(std::string& dst, const float& src){
	char buf[32]; snprintf(buf, sizeof(buf), "%g", src); // "%.24g"
	dst = buf;
	return 1;
}
template<> int toString<double>(std::string& dst, const double& src){
	char buf[40]; snprintf(buf, sizeof(buf), "%lg", src); // "%.32lg"
	dst = buf;
	return 1;
}
template<> int toString<std::string>(std::string& dst, const std::string& src){
	dst = src; return 1;
}
int toString(std::string& dst, const char * src){ return toString(dst, std::string(src)); }

template<> int fromToken<bool>(bool& dst, const std::string& src){
	int v = dst;
	bool r = sscanf(src.c_str(), "%hhi", &v) > 0;
	dst = v;
	return r;
}
template<> int fromToken<int>(int& dst, const std::string& src){
	return sscanf(src.c_str(), "%i", &dst) > 0;
}
template<> int fromToken<float>(float& dst, const std::string& src){
	return sscanf(src.c_str(), "%g", &dst) > 0;
}
template<> int fromToken<double>(double& dst, const std::string& src){
	return sscanf(src.c_str(), "%lg", &dst) > 0;
}
template<> int fromToken<std::string>(std::string& dst, const std::string& src){
	int b = src.find_first_not_of("\"");
	int e = src.find_last_not_of("\"");
	dst = src.substr(b, e-b+1);
	return 1;
}
template<class T>
static int fromToken(T * dst, int size, const std::string& src, const char * match, const char * format){
	const char * s = src.c_str();
	int i=-1;
	while(++i<size && s){
		if(!(s = strpbrk(s, match))) break;
		T v; sscanf(s, format, &v);
		dst[i] = v;
		s = strpbrk(s, " ,");
	}
	return i;
}
template<> int fromToken<bool>(bool * dst, int size, const std::string& src){
	return fromToken(dst,size,src, "01","%hhi");
}
template<> int fromToken<int>(int * dst, int size, const std::string& src){
	return fromToken(dst,size,src, "0123456789-+","%i");
}
template<> int fromToken<float>(float * dst, int size, const std::string& src){
	return fromToken(dst,size,src, "0123456789-+.","%g");
}
template<> int fromToken<double>(double * dst, int size, const std::string& src){
	return fromToken(dst,size,src, "0123456789-+.","%lg");
}
template<> int fromToken<std::string>(std::string * dst, int size, const std::string& src){
	const char * s = src.c_str();
	int i=-1;
	while(++i<size && s){
		if(!(s = strchr(s, '\"'))) break;
		++s;
		const char * e = strchr(s, '\"');
		if(!e) break;
		dst[i].assign(s, e-s);
		s = strpbrk(e, " ,");
	}
	return i;
}

template<> int toToken<std::string>(std::string& dst, const std::string& src){
	dst = "\"" + src + "\"";
	return 1;
}
int toToken(std::string& dst, const char * src){ return toToken(dst, std::string(src)); }




Data::Data(): mData(0), mSize1(0), mSize2(0), mType(Data::VOID){}

Data::Data(Data& v){ *this = v; }
Data::Data(const Data& v){ *this = v; }

Data::Data(Data::Type type, int n1, int n2)
:	mData(0), mSize1(n1), mSize2(n2), mType(type)
{}

Data::~Data(){
	free();
}

Data& Data::operator= (const Data& v){
	if(&v != this){
		setRaw(v.mData, v.size1(), v.size2(), v.type());
//		mData  = v.mData;
//		mSize1 = v.size1();
//		mSize2 = v.size2();
//		mType  = v.type();
//		incrementCount(mData);
	}
	return *this;
}

bool Data::operator==(const Data& v) const {
	if(hasData() && v.hasData()){
		// TODO: how do we compare different sizes?
		// TODO: how do we compare numbers and strings?

		//if(size() != v.size()) return false;

		int n = size() < v.size() ? size() : v.size();

		#define OP(t1, t2)\
			for(int i=0; i<n; ++i){\
				if(data<t1>()[i] != v.data<t2>()[i]) return false;\
			} return true

		#define OPALL(t)\
			switch(v.typeKind()){\
			case Data::BOOL:	OP(t, const bool);\
			case Data::INT:		OP(t, const int);\
			case Data::FLOAT:	OP(t, const float);\
			case Data::DOUBLE:	OP(t, const double);\
			default: return false;\
			}

		switch(typeKind()){
		case Data::BOOL:	OPALL(const bool);
		case Data::INT:		OPALL(const int);
		case Data::FLOAT:	OPALL(const float);
		case Data::DOUBLE:	OPALL(const double);
		case Data::STRING:	OP(const std::string, const std::string);
		default:;
		}
	}
	return false;
	#undef OP
	#undef OPALL
}

Data& Data::assign(const Data& v, int ind1, int ind2){

	if(hasData() && v.hasData()){
		int id= indexFlat(ind1,ind2);	// starting index of destination
		int nd= size()-id;				// number of destination elements to assign
		int n = nd < v.size() ? nd : v.size();
//printf("%p: %d %d %d\n", this, id,nd,n);
		#define OP(t1, t2)\
			for(int i=0; i<n; ++i){\
				data<t1>()[i+id] = v.data<t2>()[i];\
			} break

		#define OPALL(t)\
			switch(v.typeKind()){\
			case BOOL:			OP(t, const bool);\
			case INT:			OP(t, const int);\
			case FLOAT:			OP(t, const float);\
			case DOUBLE:		OP(t, const double);\
			default:;\
			} break

		switch(type()){
		case Data::BOOL:	OPALL(bool);
		case Data::INT:		OPALL(int);
		case Data::FLOAT:	OPALL(float);
		case Data::DOUBLE:	OPALL(double);
		case Data::STRING: //printf("copy strings: %p -> %p\n", &v, this);
			switch(v.typeKind()){
	//		case Data::BOOL:	v.toString(data<std::string>());
	//		case Data::FLOAT:	OP(t, float);
	//		case Data::DOUBLE:	OP(t, double);
			case Data::STRING:	OP(std::string, const std::string);
				//for(int i=0; i<n; ++i) printf("%s %s\n", data<std::string>()[i].c_str(), v.data<const std::string>()[i].c_str());
				
			default:;
			} break;
		default:;
		}
	}
	return *this;
	#undef OP
	#undef OPALL
}

void Data::clone(){
	if(hasData()){
		int cnt = references(mData);
		
		// cnt==0	data points to external data
		// cnt >1	data points to another Data's cloned data
		// cnt==1	data points to my own cloned data
//		if(cnt!=1){
			Data old(*this);
			realloc(typeKind(), size1(), size2());
			assign(old);
//		}
//		else{
//			realloc(typeKind(), size());
//		}
	}
	else{
		realloc(typeKind(), size1(), size2());
	}
}

void Data::free(){

	if(release(mData)){
		//printf("(%p) Data::free %p\n", this, mData);
		switch(type()){
		case Data::BOOL:	delete[] data<bool>(); break;
		case Data::INT:		delete[] data<int>(); break;
		case Data::FLOAT:	delete[] data<float>(); break;
		case Data::DOUBLE:	delete[] data<double>(); break;
		case Data::STRING:	delete[] data<std::string>(); break;
		default:;
		}
	}
	mData=0;
	mSize1=mSize2=0;
	mType=Data::VOID;
}

void Data::print() const{
	printf("data: %p\nsize: %d x %d\ntype: %s\n\n",
		mData, size1(), size2(), typeToString(type()).c_str());
}

void Data::realloc(Data::Type t, int n1, int n2){
//	if(type()!=t || size()!=sz){
		free();
		if(n1 && n2){
			mType = t;
			mSize1 = n1;
			mSize2 = n2;
			
			switch(type()){
			case Data::BOOL:	mData = new bool[size()]; break;
			case Data::INT:		mData = new int[size()]; break;
			case Data::FLOAT:	mData = new float[size()]; break;
			case Data::DOUBLE:	mData = new double[size()]; break;
			case Data::STRING:	mData = new std::string[size()]; break;
			default:			return;
			}
			acquire(mData);
		}
//	}
}

Data& Data::set(Data::Type t, int n1, int n2){
	if(type()!=t || size()!=(n1*n2)){
		if(hasData()){
			realloc(t, n1,n2);
		}
		else{
			mType=t;
			mSize1=n1;
			mSize2=n2;
		}
	}
	return *this;
}

void Data::setRaw(void * dt, int n1, int n2, Type ty){
	free();
	mData  = dt;
	mSize1 = n1;
	mSize2 = n2;
	mType  = ty;
	incrementCount(mData);
}

int Data::sizeType() const {
	switch(typeKind()){
	case Data::BOOL:	return sizeof(bool);
	case Data::INT:		return sizeof(int);
	case Data::FLOAT:	return sizeof(float);
	case Data::DOUBLE:	return sizeof(double);
	case Data::STRING:	return sizeof(std::string);
	default:			return 0;
	}
}

int toString(std::string& s, const Data& d){
	switch(d.typeKind()){
	case Data::BOOL:	return glv::toString(s,        d.data<const bool>(), d.size());
	case Data::INT:		return glv::toString(s,         d.data<const int>(), d.size());
	case Data::FLOAT:	return glv::toString(s,       d.data<const float>(), d.size());
	case Data::DOUBLE:	return glv::toString(s,      d.data<const double>(), d.size());
	case Data::STRING:	return glv::toString(s, d.data<const std::string>(), d.size());
	default: return 0;
	}
}
int fromToken(Data& d, const std::string& s){
	switch(d.type()){
	case Data::BOOL:	return glv::fromToken(       d.data<bool>(), d.size(), s);
	case Data::INT:		return glv::fromToken(        d.data<int>(), d.size(), s);
	case Data::FLOAT:	return glv::fromToken(      d.data<float>(), d.size(), s);
	case Data::DOUBLE:	return glv::fromToken(     d.data<double>(), d.size(), s);
	case Data::STRING:	return glv::fromToken(d.data<std::string>(), d.size(), s);
	default: return 0;
	}
}
int toToken(std::string& s, const Data& d){
	switch(d.typeKind()){
	case Data::BOOL:	return glv::toToken(s,        d.data<const bool>(), d.size());
	case Data::INT:		return glv::toToken(s,         d.data<const int>(), d.size());
	case Data::FLOAT:	return glv::toToken(s,       d.data<const float>(), d.size());
	case Data::DOUBLE:	return glv::toToken(s,      d.data<const double>(), d.size());
	case Data::STRING:	return glv::toToken(s, d.data<const std::string>(), d.size());
	default: return 0;
	}
}

//bool Data::toString(std::string& s) const {
//	switch(type()){
//	case Data::BOOL:   glv::toString(s,        data<bool>(), size()); return true;
//	case Data::FLOAT:  glv::toString(s,       data<float>(), size()); return true;
//	case Data::DOUBLE: glv::toString(s,      data<double>(), size()); return true;
//	case Data::STRING: glv::toString(s, data<std::string>(), size()); return true;
//	default: return false;
//	}
//}
//
//bool Data::fromToken(const std::string& s){
//	switch(type()){
//	case Data::BOOL:   return glv::fromToken(       data<bool>(), size(), s);
//	case Data::FLOAT:  return glv::fromToken(      data<float>(), size(), s);
//	case Data::DOUBLE: return glv::fromToken(     data<double>(), size(), s);
//	case Data::STRING: return glv::fromToken(data<std::string>(), size(), s);
//	default: return 0;
//	}
//}

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



void ModelManager::add(const std::string& name, Stringifiable& s){
	if(isIdentifier(name)){
		mNameObj[name] = &s;
	}
}

bool ModelManager::toToken(std::string& dst, const std::string& modelName){
	if(modelName.size())	dst = "[\"" + modelName + "\"] = {\r\n";
	else					dst = "{\r\n";

	NameObjMap::iterator i = mNameObj.begin();
	
	std::string t;
	for(; i!=mNameObj.end(); ++i){
		dst += "\t" + i->first + " = ";
		if(i->second->toToken(t)) dst += t+",\r\n";
	}
	
	dst += "}\r\n";
	return true;
}

int ModelManager::fromToken(const std::string& src){

	unsigned p = src.find_first_of("{");
	if(p == std::string::npos) return 0;		// no table found, so return
	
	std::string key, val;
	const char * b = &src[p+1];	// start 1 character after opening '{'

	while(*b && *b!='}'){
		if(isalpha(*b) || *b=='_'){	// is character valid start of identifier?
			const char * e = b+1;
			while(isalnum(*e) || *e=='_') ++e;	// go to end of key name
			key.assign(b, e-b);

			while(isblank(*e) || *e=='=') ++e;	// go to '='
			
			// find next valid token
			b=e=strpbrk(e, "\"{0123456789.-+");

			if(*b){
				// munch characters until end of token
				if(*e == '\"'){
					++e; while(*e!='\"') ++e; ++e;
				}
				else if(*e == '{'){
					while(*e!='}') ++e; ++e;
				}
				else{
					while((isdigit(*e) || *e=='.' || *e=='-' || *e=='+')) ++e;
				}

				val.assign(b,e-b);

				if(mNameObj.count(key)){
					//printf("%s = %s\n", key.c_str(), val.c_str());
					mNameObj[key]->fromToken(val);
				}
			}
			b=e;
		}
		++b;
	}
	//printf("%d\n", b-&v[0]);
	return b-&src[0];
}

bool ModelManager::isIdentifier(const std::string& s){
	if(isalpha(s[0]) || s[0]=='_'){
		unsigned i=1;
		for(; i<s.size(); ++i){
			if(!isalnum(s[i])) break;
		}
		if(s.size()==i) return true;
	}
	return false;
}



bool StateSpace::stateToString(std::string& dst, const std::string& name){
	if(name.size())	dst = "[\"" + name + "\"] = {\r\n";
	else			dst = "{\r\n";

	NameToIndex::iterator i = mNameToIndex.begin();
	
	std::string t;
	DataVector& v = hasState(name) ? mStates[name] : mState;

	for(; i!=mNameToIndex.end(); ++i){
		dst += "\t" + i->first + " = ";
		if(toString(t, v[i->second])) dst += t+",\r\n";
	}
	
	dst += "}\r\n";
	return true;
}

} // glv::
