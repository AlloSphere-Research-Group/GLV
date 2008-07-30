#ifndef INC_GLV_OBSERVER_H
#define INC_GLV_OBSERVER_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <vector>

namespace glv {

class Notifier;

typedef void (*glvHandler)(Notifier * sender, void * userdata); ///< Handler type

//! Handler functor
class HandlerFunctor
{
public:

	HandlerFunctor(glvHandler a, void * v = 0) { handler = a; udata = v; }

	glvHandler		handler;
	void *			udata;
};

//! Notifier
class Notifier
{
public:

	void			notify();	
	
	void			attachHandler(glvHandler a, void * userdata = 0);
	void			detachHandler(glvHandler a, void * userdata = 0);
	
protected:

	std::vector<HandlerFunctor> mHandlers;
};



// Implementation ______________________________________________________________

inline
void Notifier :: notify()
{
	if(mHandlers.empty()) return;
	
	int i=mHandlers.size(); // call handlers in FIFO order
	while (i--)	
	{
		if (mHandlers[i].handler) mHandlers[i].handler(this, mHandlers[i].udata);
	}
}

inline
void Notifier :: attachHandler (glvHandler a, void * userdata) 
{
	mHandlers.push_back(HandlerFunctor(a, userdata));
}

inline
void Notifier :: detachHandler (glvHandler a, void * userdata) 
{
	for(unsigned int i=0; i<mHandlers.size(); i++)
	{
		if (mHandlers[i].handler == a && mHandlers[i].udata == userdata) mHandlers.erase(mHandlers.begin() + i);
	}
}


} // glv::

#endif

