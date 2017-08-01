/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_notification.h"

namespace glv{

Notifier::Notifier()
:	mHandlers(0)
{}

// Non-virtual unless someone will delete a derived-class object via
// a Notifier pointer...
Notifier::~Notifier(){ delete[] mHandlers; }

void Notifier::attach(Callback cb, Update::t n, void * rcvr){
	handlers()[n].push_back(Handler(cb, rcvr));
}

void Notifier::detach(Callback cb, Update::t n, void * rcvr){

	if(hasHandlers()){
		for(unsigned int i=0; i<handlers()[n].size(); i++){
			Handler& h = handlers()[n][i];
			#ifdef GLV_USE_STD_FUNCTION
			if(h.handler.template target<Callback>() == cb.template target<Callback>() && h.receiver == rcvr)
			#else
			if(h.handler == cb && h.receiver == rcvr)
			#endif
				handlers()[n].erase(handlers()[n].begin() + i);
		}		
	}
}

void Notifier::notify(void * sender, Update::t n, void * data){

	if(!hasHandlers() || handlers()[n].empty()) return;

	// call handlers in FIFO order
	int i=handlers()[n].size();
	while(i--){
		Handler& h = handlers()[n][i];
		if(h.handler) h.handler(Notification(sender, h.receiver, data));
	}
}

int Notifier::numObservers(Update::t n) const {
	if(hasHandlers()) return handlers()[n].size();
	return 0;
}


} // glv::
