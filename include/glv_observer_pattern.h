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


/// Generic index/value struct to pass for widget notifications
template <class T>
class ChangedValue{
public:
	
	ChangedValue(const T& v, int i=0): mIndex(i), mValue(v){}
	
	T value() const { return mValue; }
	int index() const { return mIndex; }

private:
	int mIndex;
	T mValue;
};



/// Notification object

/// This is passed into notification callbacks
///
class Notification{
public:

	Notification(Notifier * sndr, void * rcvr=0, const void * data=0)
	:	mSender(sndr), mReceiver(rcvr), mData(data)
	{}

	Notifier * sender() const { return mSender; }
	void * receiver() const { return mReceiver; }
	const void * data() const { return mData; }

protected:
	Notifier * mSender;
	void * mReceiver;
	const void * mData;
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
	void notify(Update::t n, void * data=0){
		if(mHandlers[n].empty()) return;
		
		// call handlers in FIFO order
		int i=mHandlers[n].size();
		while (i--){
			Handler& h = mHandlers[n][i];
			if(h.handler) h.handler(Notification(this, h.receiver, data));
		}		
	}
	
	/// Notify observers of a specific update type
	template <class T>
	void notify(Update::t n, const ChangedValue<T>& v){
		notify(n, (void *)&v);		
	}

	/// Returns number of observers for this update type
	int numObservers(Update::t n) const {
		return mHandlers[n].size();
	}

protected:

	struct Handler{
		Handler(callback c, void * r): handler(c), receiver(r){}
		callback handler;
		void * receiver;
	};

	std::vector<Handler> mHandlers[Update::NumTypes];
};

} // glv::
#endif
