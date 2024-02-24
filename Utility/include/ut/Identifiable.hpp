#ifndef UT_IDENTIFIABLE_HPP
#define UT_IDENTIFIABLE_HPP

#include "ut/defines.hpp"
#include "ut/Bitset.hpp" //masks

/*PRECOMPILED
#include <string> //name */


namespace ut
{
    class Identifiable
    {
        public:
            using Mask = Bitset;

            inline static constexpr uint DF_MASK_SIZE = 64;

        //---ctor, dtor
            virtual ~Identifiable() = default;

            Identifiable( const Identifiable& rhs ) = default;
            Identifiable( Identifiable&& rhs ) noexcept = default;
            Identifiable& operator=( const Identifiable& rhs ) = default;
            Identifiable& operator=( Identifiable&& rhs ) noexcept = default;

		//---get
            inline size_t getRelativeId() const { return relativeId; }
            inline size_t getAbsoluteId() const { return absoluteId; }
			inline size_t getId() const { return getRelativeId(); }

            //inline const Mask& getRelativeMask() const { return relativeMask; }
            //inline const Mask& getAbsoluteMask() const { return absoluteMask; }
            //inline const Mask& getMask() const { return getRelativeMask(); }

			inline const std::string& getName() const { return name; }

        //---set
            inline void setRelativeId( size_t xRelativeId ) { relativeId = xRelativeId; }
            inline void setAbsoluteId( size_t xAbsoluteId ) { absoluteId = xAbsoluteId; }
            inline void updateAbsoluteId( size_t startingId ) { absoluteId = startingId + relativeId; }

            //inline void setAbsoluteMask( const Mask& xAbsoluteMask ) { absoluteMask = xAbsoluteMask; }
            //inline void updateAbsoluteMask( uint maskSize, size_t startingId ) { absoluteMask = Mask( maskSize, startingId + relativeId ); }


        protected:
			size_t relativeId;
            size_t absoluteId;

            //Mask relativeMask;
            //Mask absoluteMask;

			std::string name;

            //as derived objects are constructed only during parsing spec, efficience is not critical --> no perfect forwarding here
            //Identifiable( size_t relativeId, const std::string& name ) : relativeId(relativeId), absoluteId(relativeId), relativeMask( DF_MASK_SIZE, relativeId ), absoluteMask(DF_MASK_SIZE), name(name) {;} 
            Identifiable( size_t relativeId, const std::string& name ) : relativeId(relativeId), absoluteId(relativeId), name(name) {;} 
    };
}

#endif //UT_IDENTIFIABLE_HPP
