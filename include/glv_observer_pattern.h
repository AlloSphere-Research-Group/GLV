#ifndef INC_GLV_OBSERVER_H
#define INC_GLV_OBSERVER_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <vector>

namespace glv {

class Notifier;


/// Notification update types
namespace Update{
	enum t{
		Focus=0,
		Value,
		NumTypes
	};
}


/// Notification object
///
/// This is passed into notification callbacks
class Notification{
public:

	Notification(Notifier * sndr, void * rcvr=0)
	:	mSender(sndr), mReceiver(rcvr)
	{}

	Notifier * sender() const { return mSender; }
	void * receiver() const { return mReceiver; }

protected:
	Notifier * mSender;
	void * mReceiver;
};




/// Notifies one or more observers of state change
class Notifier{
private:
	typedef void (* callback)(const Notification& n);

public:

	/// Attach a new notification callback, type, and receiver
	void attach(callback cb, Update::t n, void * rcvr=0){
		mHandlers[n].push_back(Handler(cb, rcvr));
	}

	
	/// Detach an existing notification callback, type, and receiver
	void detach(callback cb, Update::t n, void * rcvr=0){
		for(unsigned int i=0; i<mHandlers[n].size(); i++){
			Handler& h = mHandlers[n][i];
			if(h.handler == cb && h.receiver == rcvr) mHandlers[n].erase(mHandlers[n].begin() + i);
		}
	}


	/// Notify observers of a specific update type
	void notify(Update::t n){
		if(mHandlers[n].empty()) return;
		
		// call handlers in FIFO order
		int i=mHandlers[n].size();
		while (i--){
			Handler& h = mHandlers[n][i];
			if(h.handler) h.handler(Notification(this, h.receiver));
		}		
	}

protected:

	struct Handler{
		Handler(callback c, void * r): handler(c), receiver(r){}
		callback handler;
		void * receiver;
	};

	std::vector<Handler> mHandlers[Update::NumTypes];
};





//class Notification{
//
//protected:
//	Notifier * mSender;
//	void * mReceiver;
//};
//
//class ValueNotification : public Notification{
//
//protected:
//	int mWhich;
//	T mValue;
//}
//
//typedef Notification FocusNotification;


//void update(const ValueNotification<float>& n){
//
//}



//class Notifier;
//
//typedef void (*glvHandler)(Notifier * sender, void * userdata); ///< Handler type
//
///// Handler functor
//class HandlerFunctor{
//public:
//	HandlerFunctor(glvHandler a, void * v=0){ handler=a; udata=v; }
//
//	glvHandler handler;
//	void * udata;
//};
//
//
//
///// Notifier
//class Notifier{
//public:
//	void attachHandler(glvHandler a, void * userdata=0);
//	void detachHandler(glvHandler a, void * userdata=0);
//	void notify();
//	
//protected:
//	std::vector<HandlerFunctor> mHandlers;
//};
//
//
//
//// Implementation ______________________________________________________________
//
//inline
//void Notifier :: notify()
//{
//	if(mHandlers.empty()) return;
//	
//	int i=mHandlers.size(); // call handlers in FIFO order
//	while (i--)	
//	{
//		if (mHandlers[i].handler) mHandlers[i].handler(this, mHandlers[i].udata);
//	}
//}
//
//inline
//void Notifier :: attachHandler (glvHandler a, void * userdata) 
//{
//	mHandlers.push_back(HandlerFunctor(a, userdata));
//}
//
//inline
//void Notifier :: detachHandler (glvHandler a, void * userdata) 
//{
//	for(unsigned int i=0; i<mHandlers.size(); i++)
//	{
//		if (mHandlers[i].handler == a && mHandlers[i].udata == userdata) mHandlers.erase(mHandlers.begin() + i);
//	}
//}


} // glv::

#endif

