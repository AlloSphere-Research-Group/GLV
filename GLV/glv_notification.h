#ifndef INC_GLV_NOTIFICATION_H
#define INC_GLV_NOTIFICATION_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <vector>

namespace glv {

class Notifier;


/// Notification update types
namespace Update{
	enum t{
		Focus		=    0,	/**< Focus update */
		Value,				/**< Value update */
		Action,				/**< Perform an action based on current value */
		Selection,
		NumTypes,			/**< Number of predefined notification types */
		User		= 1000	/**< Start of user-defined notification types */
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
	
	const T& value() const { return mValue; }	///< Get value
	int index() const { return mIndex; }		///< Get index of value

private:
	int mIndex;
	T mValue;
};


/// Data passed with selection change notification
struct ChangedSelection{
	int oldIndex;
	int newIndex;
};


/// Notification object

/// This is passed into notification callbacks
///
class Notification{
public:

	/// @param[in] sndr		Pointer to sending object (subject)
	/// @param[in] rcvr		Pointer to receiving object (observer)
	/// @param[in] data		Pointer to data object
	Notification(void * sndr, void * rcvr=0, const void * data=0)
	:	mSender(sndr), mReceiver(rcvr), mData(data)
	{}

	/// Get pointer to sending object
	template <class T>
	T * sender() const { return static_cast<T *>(sender()); }

	/// Get raw pointer to sending object
	void * sender() const { return mSender; }

	/// Get pointer to receiving object
	template <class T>
	T * receiver() const { return static_cast<T *>(receiver()); }

	/// Get raw pointer to receiving object
	void * receiver() const { return mReceiver; }

	/// Get raw pointer to data object
	const void * data() const { return mData; }

	/// Get pointer to data object
	template <class T>
	const T * data() const { return static_cast<const T *>(data()); }

protected:
	void * mSender;
	void * mReceiver;
	const void * mData;
};




/// Notifies one or more observers of state change
class Notifier{
public:

	/// Notification callback
	typedef void (* Callback)(const Notification& n);
	
	Notifier();

	~Notifier();


	/// Attach a new notification callback, type, and receiver
	void attach(Callback cb, Update::t type, void * rcvr=0);
	
	/// Detach an existing notification callback, type, and receiver
	void detach(Callback cb, Update::t type, void * rcvr=0);


	/// Notify observers of a specific update type
	void notify(void * sender, Update::t type, void * data=0);

	/// Notify observers of a specific update type
	void notify(Update::t type, void * data=0){ notify(this, type, data); }
	// LJP: The above can lead to unexpected results when casting since the
	//		void * points to a Notifier.


	/// Notify observers of a specific update type
	template <class T>
	void notify(Update::t type, const ChangedValue<T>& v){
		notify(type, (void *)&v);		
	}

	/// Returns number of observers for this update type
	int numObservers(Update::t type) const;

protected:

	struct Handler{
		Handler(Callback c, void * r): handler(c), receiver(r){}
		Callback handler;
		void * receiver;
	};

	// Array of vectors for each notification type
	std::vector<Handler> * mHandlers;
	
	// Setter dynamically allocates new memory
	std::vector<Handler> * handlers(){
		if(0 == mHandlers){
			mHandlers = new std::vector<Handler>[Update::NumTypes];
		}
		return mHandlers;
	}

	// Getter just returns pointer (even if 0)
	const std::vector<Handler> * handlers() const {
		return mHandlers;
	}
	
	bool hasHandlers() const { return handlers() != 0;  }
};

} // glv::
#endif
