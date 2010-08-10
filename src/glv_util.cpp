/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_util.h"

namespace glv{


template<> int fromString<bool>(bool& dst, const std::string& src){
	int v = dst;
	bool r = sscanf(src.c_str(), "%d", &v) > 0;
	dst = v;
	return r;
}
template<> int fromString<float>(float& dst, const std::string& src){
	return sscanf(src.c_str(), "%g", &dst) > 0;
}
template<> int fromString<double>(double& dst, const std::string& src){
	return sscanf(src.c_str(), "%lg", &dst) > 0;
}
template<> int fromString<std::string>(std::string& dst, const std::string& src){
	int b = src.find_first_not_of("\"");
	int e = src.find_last_not_of("\"");
	dst = src.substr(b, e-b+1);
	return 1;
}
template<class T>
static int fromString(T * dst, int size, const std::string& src, const char * match, const char * format){
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
template<> int fromString<bool>(bool * dst, int size, const std::string& src){
	return fromString(dst,size,src, "01","%d");
}
template<> int fromString<float>(float * dst, int size, const std::string& src){
	return fromString(dst,size,src, "0123456789-+.","%g");
}
template<> int fromString<double>(double * dst, int size, const std::string& src){
	return fromString(dst,size,src, "0123456789-+.","%lg");
}


template<> void toString<bool>(std::string& dst, const bool& src){
	dst = src ? "1" : "0";
}
template<> void toString<float>(std::string& dst, const float& src){
	char buf[32]; snprintf(buf, sizeof(buf), "%g", src); // "%.24g"
	dst = buf;
}
template<> void toString<double>(std::string& dst, const double& src){
	char buf[40]; snprintf(buf, sizeof(buf), "%lg", src); // "%.32lg"
	dst = buf;
}
template<> void toString<std::string>(std::string& dst, const std::string& src){
	dst = "\"" + src + "\"";
}
void toString(std::string& dst, const char * src){ toString(dst, std::string(src)); }

} // glv::
