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
		Focus=0,	/**< Focus update */
		Value,		/**< Value update */
		NumTypes
	};
}


/// Generic index/value struct for widget notifications

/// Since some widgets have multiple values, an index is used to specify which
/// element corresponds to the update. If a widget only has one value, an index
/// of zero is used.
template <class T>
class ChangedValue{
public:
	
	/// @param[in] v	value
	/// @param[in] i	index of value
	ChangedValue(const T& v, int i=0): mIndex(i), mValue(v){}
	
	T value() const { return mValue; }		///< Get value
	int index() const { return mIndex; }	///< Get index of value

private:
	int mIndex;
	T mValue;
};



/// Notification object

/// This is passed into notification callbacks
///
class Notification{
public:

	/// @param[in] sndr		Pointer to sending object (subject)
	/// @param[in] rcvr		Pointer to receiving object (observer)
	/// @param[in] data		Pointer to data object
	Notification(Notifier * sndr, void * rcvr=0, const void * data=0)
	:	mSender(sndr), mReceiver(rcvr), mData(data)
	{}

	Notifier * sender() const { return mSender; }	///< Get pointer to sending object
	void * receiver() const { return mReceiver; }	///< Get pointer to receiving object
	const void * data() const { return mData; }		///< Get pointer to data object

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
