#ifndef GLV_ICON_H_INC
#define GLV_ICON_H_INC

namespace glv{

/// Simple drawing routines for commonly used icons.
class Icon{
public:

	static float thickness;	///< thickness of lines

	static void none		(float l, float t, float w, float h){};

	static void rect		(float l, float t, float w, float h);

	static void triangleR	(float l, float t, float w, float h);
	static void triangleL	(float l, float t, float w, float h);
	static void triangleU	(float l, float t, float w, float h);
	static void triangleD	(float l, float t, float w, float h);
	
	static void x			(float l, float t, float w, float h);
	static void plus		(float l, float t, float w, float h);
	static void minus		(float l, float t, float w, float h);
	static void check		(float l, float t, float w, float h);
};

}

#endif

