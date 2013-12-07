#pragma once

#include "ofConstants.h"

#include "Poco/PriorityEvent.h"
#include "Poco/PriorityDelegate.h"
#include "ofDelegate.h"

#include <stdio.h>
#include <stdlib.h>

namespace Poco {

	template <class TDelegate>
	class NotificationStrategy<void, TDelegate>
		/// The interface that all notification strategies must implement.
		/// 
		/// Note: Event is based on policy-driven design, so every strategy implementation
		/// must provide all the methods from this interface (otherwise: compile errors)
		/// but does not need to inherit from NotificationStrategy.
	{
	public:
		NotificationStrategy()
		{
		}

		virtual ~NotificationStrategy()
		{
		}

		virtual void notify(const void* sender) = 0;
			/// Sends a notification to all registered delegates.

		virtual void add(const TDelegate& delegate) = 0;
			/// Adds a delegate to the strategy.

		virtual void remove(const TDelegate& delegate) = 0;
			/// Removes a delegate from the strategy, if found.
			/// Does nothing if the delegate has not been added.

		virtual void clear() = 0;
			/// Removes all delegates from the strategy.

		virtual bool empty() const = 0;
			/// Returns false if the strategy contains at least one delegate.
	};

	template <class TDelegate> 
	class PriorityStrategy<void, TDelegate> : public NotificationStrategy<void, TDelegate>
		/// NotificationStrategy for PriorityEvent.
		///
		/// Delegates are kept in a std::vector<>, ordered
		/// by their priority.
	{
	public:
		typedef SharedPtr<TDelegate>         DelegatePtr;
		typedef std::vector<DelegatePtr>     Delegates;
		typedef typename Delegates::iterator Iterator;

	public:
		PriorityStrategy()
		{
		}

		PriorityStrategy(const PriorityStrategy& s):
			_delegates(s._delegates)
		{
		}

		~PriorityStrategy()
		{
		}

		void notify(const void* sender)
		{
			for (Iterator it = _delegates.begin(); it != _delegates.end(); ++it)
			{
				(*it)->notify(sender);
			}
		}

		void add(const TDelegate& delegate)
		{
			for (Iterator it = _delegates.begin(); it != _delegates.end(); ++it)
			{
				if ((*it)->priority() > delegate.priority())
				{
					_delegates.insert(it, DelegatePtr(static_cast<TDelegate*>(delegate.clone())));
					return;
				}
			}
			_delegates.push_back(DelegatePtr(static_cast<TDelegate*>(delegate.clone())));
		}

		void remove(const TDelegate& delegate)
		{
			for (Iterator it = _delegates.begin(); it != _delegates.end(); ++it)
			{
				if (delegate.equals(**it))
				{
					(*it)->disable();
					_delegates.erase(it);
					return;
				}
			}
		}

		PriorityStrategy& operator = (const PriorityStrategy& s)
		{
			if (this != &s)
			{
				_delegates = s._delegates;
			}
			return *this;
		}

		void clear()
		{
			for (Iterator it = _delegates.begin(); it != _delegates.end(); ++it)
			{
				(*it)->disable();
			}
			_delegates.clear();
		}

		bool empty() const
		{
			return _delegates.empty();
		}

	protected:
		Delegates _delegates;
	};

	template <class TStrategy, class TDelegate, class TMutex> 
	class AbstractEvent<void, TStrategy, TDelegate, TMutex>
	{
	public:
		typedef void Args;

		AbstractEvent(): 
			_executeAsync(this, &AbstractEvent::executeAsyncImpl),
			_enabled(true)
		{
		}

		AbstractEvent(const TStrategy& strat): 
			_executeAsync(this, &AbstractEvent::executeAsyncImpl),
			_strategy(strat),
			_enabled(true)
		{	
		}

		virtual ~AbstractEvent()
		{
		}

		void operator += (const TDelegate& aDelegate)
		{
			typename TMutex::ScopedLock lock(_mutex);
			_strategy.add(aDelegate);
		}
	
		void operator -= (const TDelegate& aDelegate)
		{
			typename TMutex::ScopedLock lock(_mutex);
			_strategy.remove(aDelegate);
		}
	
		void operator () (const void* pSender)
		{
			notify(pSender);
		}
	
		void operator () ()
		{
			notify(0);
		}

		void notify(const void* pSender)
		{
			Poco::ScopedLockWithUnlock<TMutex> lock(_mutex);
		
			if (!_enabled) return;
		
			TStrategy strategy(_strategy);
			lock.unlock();
			strategy.notify(pSender);
		}

		ActiveResult<void> notifyAsync(const void* pSender)
		{
			NotifyAsyncParams params(pSender);
			{
				typename TMutex::ScopedLock lock(_mutex);
				
				params.ptrStrat = SharedPtr<TStrategy>(new TStrategy(_strategy));
				params.enabled  = _enabled;
			}
			ActiveResult<void> result = _executeAsync(params);
			return result;
		}
	
		void enable()
		{
			typename TMutex::ScopedLock lock(_mutex);
			_enabled = true;
		}

		void disable()
		{
			typename TMutex::ScopedLock lock(_mutex);
			_enabled = false;
		}

		bool isEnabled() const
		{
			typename TMutex::ScopedLock lock(_mutex);
			return _enabled;
		}

		void clear()
		{
			typename TMutex::ScopedLock lock(_mutex);
			_strategy.clear();
		}
	
		bool empty() const
		{
			typename TMutex::ScopedLock lock(_mutex);
			return _strategy.empty();
		}

	protected:
		struct NotifyAsyncParams
		{
			SharedPtr<TStrategy> ptrStrat;
			const void* pSender;
			bool        enabled;
		
			NotifyAsyncParams(const void* pSend):ptrStrat(), pSender(pSend), enabled(true)
			{
			}
		};

		ActiveMethod<void, NotifyAsyncParams, AbstractEvent> _executeAsync;

		void executeAsyncImpl(const NotifyAsyncParams& par)
		{
			if (!par.enabled)
			{
				return par.args;
			}

			NotifyAsyncParams params = par;
			params.ptrStrat->notify(params.pSender);
		}

		TStrategy _strategy; /// The strategy used to notify observers.
		bool      _enabled;  /// Stores if an event is enabled. Notfies on disabled events have no effect
			             /// but it is possible to change the observers.
		mutable TMutex _mutex;

	private:
		AbstractEvent(const AbstractEvent& other);
		AbstractEvent& operator = (const AbstractEvent& other);
	};

	template <class TStrategy, class TDelegate> 
	class AbstractEvent<void, TStrategy, TDelegate>
	{
	public:
		typedef void Args;

		AbstractEvent(): 
			_executeAsync(this, &AbstractEvent::executeAsyncImpl),
			_enabled(true)
		{
		}

		AbstractEvent(const TStrategy& strat): 
			_executeAsync(this, &AbstractEvent::executeAsyncImpl),
			_strategy(strat),
			_enabled(true)
		{	
		}

		virtual ~AbstractEvent()
		{
		}

		void operator += (const TDelegate& aDelegate)
			/// Adds a delegate to the event. 
			///
			/// Exact behavior is determined by the TStrategy.
		{
			typename FastMutex::ScopedLock lock(_mutex);
			_strategy.add(aDelegate);
		}
	
		void operator -= (const TDelegate& aDelegate)
			/// Removes a delegate from the event.
			///
			/// If the delegate is not found, this function does nothing.
		{
			typename FastMutex::ScopedLock lock(_mutex);
			_strategy.remove(aDelegate);
		}
	
		void operator () (const void* pSender)
			/// Shortcut for notify(pSender, args);
		{
			notify(pSender);
		}
	
		void operator () ()
			/// Shortcut for notify(args).
		{
			notify(0);
		}

		void notify(const void* pSender)
			/// Sends a notification to all registered delegates. The order is 
			/// determined by the TStrategy. This method is blocking. While executing,
			/// the list of delegates may be modified. These changes don't
			/// influence the current active notifications but are activated with
			/// the next notify. If a delegate is removed during a notify(), the
			/// delegate will no longer be invoked (unless it has already been
			/// invoked prior to removal). If one of the delegates throws an exception, 
			/// the notify method is immediately aborted and the exception is propagated
			/// to the caller.
		{
			Poco::ScopedLockWithUnlock<FastMutex> lock(_mutex);
		
			if (!_enabled) return;
		
			// thread-safeness: 
			// copy should be faster and safer than blocking until
			// execution ends
			TStrategy strategy(_strategy);
			lock.unlock();
			strategy.notify(pSender);
		}

		ActiveResult<void> notifyAsync(const void* pSender)
			/// Sends a notification to all registered delegates. The order is 
			/// determined by the TStrategy. This method is not blocking and will
			/// immediately return. The delegates are invoked in a seperate thread.
			/// Call activeResult.wait() to wait until the notification has ended.
			/// While executing, other objects can change the delegate list. These changes don't
			/// influence the current active notifications but are activated with
			/// the next notify. If a delegate is removed during a notify(), the
			/// delegate will no longer be invoked (unless it has already been
			/// invoked prior to removal). If one of the delegates throws an exception, 
			/// the execution is aborted and the exception is propagated to the caller.
		{
			NotifyAsyncParams params(pSender);
			{
				typename FastMutex::ScopedLock lock(_mutex);

				// thread-safeness: 
				// copy should be faster and safer than blocking until
				// execution ends
				// make a copy of the strategy here to guarantee that
				// between notifyAsync and the execution of the method no changes can occur
				
				params.ptrStrat = SharedPtr<TStrategy>(new TStrategy(_strategy));
				params.enabled  = _enabled;
			}
			ActiveResult<void> result = _executeAsync(params);
			return result;
		}
	
		void enable()
			/// Enables the event.
		{
			typename FastMutex::ScopedLock lock(_mutex);
			_enabled = true;
		}

		void disable()
			/// Disables the event. notify and notifyAsnyc will be ignored,
			/// but adding/removing delegates is still allowed.
		{
			typename FastMutex::ScopedLock lock(_mutex);
			_enabled = false;
		}

		bool isEnabled() const
		{
			typename FastMutex::ScopedLock lock(_mutex);
			return _enabled;
		}

		void clear()
			/// Removes all delegates.
		{
			typename FastMutex::ScopedLock lock(_mutex);
			_strategy.clear();
		}
	
		bool empty() const
			/// Checks if any delegates are registered at the delegate.
		{
			typename FastMutex::ScopedLock lock(_mutex);
			return _strategy.empty();
		}

	protected:
		struct NotifyAsyncParams
		{
			SharedPtr<TStrategy> ptrStrat;
			const void* pSender;
			bool        enabled;
		
			NotifyAsyncParams(const void* pSend):ptrStrat(), pSender(pSend), enabled(true)
				/// Default constructor reduces the need for TArgs to have an empty constructor, only copy constructor is needed.
			{
			}
		};

		ActiveMethod<void, NotifyAsyncParams, AbstractEvent> _executeAsync;

		void executeAsyncImpl(const NotifyAsyncParams& par)
		{
			if (!par.enabled)
			{
				return par.args;
			}

			NotifyAsyncParams params = par;
			params.ptrStrat->notify(params.pSender);
		}

		TStrategy _strategy; /// The strategy used to notify observers.
		bool      _enabled;  /// Stores if an event is enabled. Notfies on disabled events have no effect
			             /// but it is possible to change the observers.
		mutable FastMutex _mutex;

	private:
		AbstractEvent(const AbstractEvent& other);
		AbstractEvent& operator = (const AbstractEvent& other);
	};
}

//-----------------------------------------
// define ofEvent as a poco FIFOEvent
// to create your own events use:
// ofEvent<argType> myEvent

template <typename ArgumentsType>
class ofEvent: public Poco::PriorityEvent<ArgumentsType> {
public:

	ofEvent():Poco::PriorityEvent<ArgumentsType>(){

	}

	// allow copy of events, by copying everything except the mutex
	ofEvent(const ofEvent<ArgumentsType> & mom)
	:Poco::PriorityEvent<ArgumentsType>()
	{
		this->_enabled = mom._enabled;
	}

	ofEvent<ArgumentsType> & operator=(const ofEvent<ArgumentsType> & mom){
		if(&mom == this) return *this;
		this->_enabled = mom._enabled;
		return *this;
	}

};


enum ofEventOrder{
	OF_EVENT_ORDER_BEFORE_APP=0,
	OF_EVENT_ORDER_APP=100,
	OF_EVENT_ORDER_AFTER_APP=200
};

//----------------------------------------------------
// register any method of any class to an event.
// the method must provide one of the following
// signatures:
//     void method(ArgumentsType & args)
//     void method(const void * sender, ArgumentsType &args)
// ie:
//     ofAddListener(addon.newIntEvent, this, &Class::method)

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofAddListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= Poco::priorityDelegate(listener, listenerMethod, prio);
    event += Poco::priorityDelegate(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofAddListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= Poco::priorityDelegate(listener, listenerMethod, prio);
    event += Poco::priorityDelegate(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofAddListener(ofEvent<void> & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= Poco::priorityDelegate(listener, listenerMethod, prio);
    event += Poco::priorityDelegate(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofAddListener(ofEvent<void> & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= Poco::priorityDelegate(listener, listenerMethod, prio);
    event += Poco::priorityDelegate(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofAddListener(EventType & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= ofDelegate<ListenerClass,ArgumentsType,true>(listener, listenerMethod, prio);
    event += ofDelegate<ListenerClass,ArgumentsType,true>(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofAddListener(EventType & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= ofDelegate<ListenerClass,ArgumentsType,false>(listener, listenerMethod, prio);
    event += ofDelegate<ListenerClass,ArgumentsType,false>(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofAddListener(ofEvent<void> & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= ofDelegate<ListenerClass,void,true>(listener, listenerMethod, prio);
    event += ofDelegate<ListenerClass,void,true>(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofAddListener(ofEvent<void> & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= ofDelegate<ListenerClass,void,false>(listener, listenerMethod, prio);
    event += ofDelegate<ListenerClass,void,false>(listener, listenerMethod, prio);
}
//----------------------------------------------------
// unregister any method of any class to an event.
// the method must provide one the following
// signatures:
//     void method(ArgumentsType & args)
//     void method(const void * sender, ArgumentsType &args)
// ie:
//     ofAddListener(addon.newIntEvent, this, &Class::method)

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofRemoveListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= Poco::priorityDelegate(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofRemoveListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= Poco::priorityDelegate(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofRemoveListener(ofEvent<void> & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= Poco::priorityDelegate(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofRemoveListener(ofEvent<void> & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= Poco::priorityDelegate(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofRemoveListener(EventType & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= ofDelegate<ListenerClass,ArgumentsType,true>(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofRemoveListener(EventType & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= ofDelegate<ListenerClass,ArgumentsType,false>(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofRemoveListener(ofEvent<void> & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= ofDelegate<ListenerClass,void,true>(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofRemoveListener(ofEvent<void> & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event -= ofDelegate<ListenerClass,void,false>(listener, listenerMethod, prio);
}
//----------------------------------------------------
// notifies an event so all the registered listeners
// get called
// ie:
//	ofNotifyEvent(addon.newIntEvent, intArgument, this)
//
// or in case there's no sender:
//	ofNotifyEvent(addon.newIntEvent, intArgument)

template <class EventType,typename ArgumentsType, typename SenderType>
void ofNotifyEvent(EventType & event, ArgumentsType & args, SenderType * sender){
	try{
		event.notify(sender,args);
	}catch(ofEventAttendedException & e){

	}
}

template <class EventType,typename ArgumentsType>
void ofNotifyEvent(EventType & event, ArgumentsType & args){
	try{
		event.notify(NULL,args);
	}catch(ofEventAttendedException & e){

	}
}

template <class EventType, typename ArgumentsType, typename SenderType>
void ofNotifyEvent(EventType & event, const ArgumentsType & args, SenderType * sender){
	try{
		event.notify(sender,args);
	}catch(ofEventAttendedException & e){

	}
}

template <class EventType,typename ArgumentsType>
void ofNotifyEvent(EventType & event, const ArgumentsType & args){
	try{
		event.notify(NULL,args);
	}catch(ofEventAttendedException & e){

	}
}

template <typename SenderType>
void ofNotifyEvent(ofEvent<void> & event, SenderType * sender){
	try{
		event.notify(sender);
	}catch(ofEventAttendedException & e){

	}
}

inline void ofNotifyEvent(ofEvent<void> & event){
	try{
		event.notify(NULL);
	}catch(ofEventAttendedException & e){

	}
}

