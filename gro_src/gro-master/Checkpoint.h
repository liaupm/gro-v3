#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include "ut/Metadata.hpp" //REGISTER_CLASS

#include "Defines.h"
#include "GroElement.h" //base

//PRECOMPILED
//#include <string> //name, message


class Timer; //timer
class GroCollection; //ctor
class Checkpoint : public GroElement
{
    REGISTER_CLASS( "Checkpoint", "chp", ElemTypeIdx::CHECKPOINT )

	public:
		inline static const std::string DF_MESSAGE = "";
		static constexpr bool DF_B_SOUND = false;

	//---ctor, dtor 
	    Checkpoint( GroCollection* groCollection, size_t id, const std::string& name
	    , const Timer* timer, const std::string& message = DF_MESSAGE, bool bSound = DF_B_SOUND );

	    virtual ~Checkpoint() = default;

	//---get
	    inline const Timer* getTimer() const { return timer; }
	    inline const std::string& getMessage() const { return message; }
        inline bool getBSound() const { return bSound; }

	//---API
	  //precompute
	    void loadDirectElemLink();


	private:
		const Timer* timer;
		std::string message; //shown when the checkpoint is triggered, only in non-batch mode
		bool bSound;
};

#endif // CHECKPOINT_H
