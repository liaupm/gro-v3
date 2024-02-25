#ifndef UT_EVENT_HPP
#define UT_EVENT_HPP

#include "ut/defines.hpp"
#include "ut/Time.hpp"

namespace ut
{
	template< typename TElemBase>
	struct Event
	{
		using ElemBaseType = TElemBase;
		using TimeType = ut::Time::TimeType;

		enum EventMode
        {
            ELEM_CHANGE, INTERNAL
        };

	//---ctor, dtor
		inline Event( EventMode eventMode, const ElemBaseType* emitter, const ElemBaseType* element, TimeType time, int quantity = 0 ) : eventMode(eventMode), emitter(emitter), element(element), time(time), quantity(quantity) {;}
		virtual ~Event() = default;
		Event( const Event& rhs ) = default;
		Event( Event&& rhs ) noexcept = default;
		Event& operator=( const Event& rhs ) = default;
		Event& operator=( Event&& rhs ) noexcept = default;

	//---API
        bool compare( EventMode eventMode, const ElemBaseType* emitter, const ElemBaseType* element, TimeType time, int quantity = 0 ) const { return std::tie( eventMode, emitter, element, time, quantity ) == std::tie( this->eventMode, this->emitter, this->element, this->time, this->quantity ); }
	

        EventMode eventMode;
        const ElemBaseType* emitter;
        const ElemBaseType* element;
        TimeType time;
        int quantity;
	};
}

#endif //UT_EVENT_HPP