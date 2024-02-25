#ifndef UT_MULTIBITSET_HPP
#define UT_MULTIBITSET_HPP

#include "ut/defines.hpp"
#include "ut/Bitset.hpp" //base

/*PRECOMPILED
#include <vector> //MasksDS
#include <array> //SeparatorDS */


namespace ut
{

///////////////////////////////////////////// MultiBitsetCommon //////////////////////////////////////////////////////
	  class MultiBitsetCommon
	  /* common features of several Multibitsets
	  All of them apply to the same collection of elements, so that they share the element numbers
	  Used to initialize those Multibitset, as separators and masks do not need to be calculated every time */
	  {
			public:
				  friend class MultiBitset;
				  using SeparatorDS = std::vector<size_t>;
				  using MaskType = Bitset;
				  using MasksDS = std::vector<MaskType>;


			//---ctor, dtor
				  inline explicit MultiBitsetCommon( size_t bitNum, const SeparatorDS& separators = {} ) 
				  : bitNum(bitNum), segmentNum( separators.size() > 0 ? separators.size() + 1 : 0 ), separators( separators ) { init(); }

				  template< size_t N >
				  inline explicit MultiBitsetCommon( size_t bitNum, const std::array< size_t, N >& separatorsArr ) 
				  : bitNum(bitNum), segmentNum( N > 0 ? separators.size() + 1 : 0 )
				  { separators.insert( separators.begin(), std::begin( separatorsArr ), std::end( separatorsArr ) ); init(); }

				  virtual ~MultiBitsetCommon() = default;

			//---get
				  inline size_t getSegmentNum() const { return segmentNum; }
				  inline size_t getStart( size_t segIdx ) const { return separators[ segIdx ]; }
				  inline size_t getEnd( size_t segIdx ) const { return separators[ segIdx + 1 ]; }
				  inline const MaskType& getMask( size_t segIdx ) const { return masks[ segIdx ]; }

			//---API
				  inline void init() { initSeparators(); initMasks(); }

				  inline size_t abs2relBit( size_t segIdx, size_t absIdx ) const { return absIdx - getStart( segIdx ); }
				  inline size_t rel2absBit( size_t segIdx, size_t relIdx ) const { return getStart( segIdx ) + relIdx; }


			private:
				  size_t bitNum; //total number of bits, potentially in several words
				  size_t segmentNum; //number of differenciated bit groups. Do not match the words
				  SeparatorDS separators; //separator idxs of those groups, including both bounds (n+1)
				  MasksDS masks; //masks for each group

				  inline void initSeparators() { /*separators.insert( separators.begin(), 0 );*/ separators.push_back( bitNum ); }
				  inline void initMasks() { masks.reserve( segmentNum ); for( size_t s = 0; s < segmentNum - 1; s++ ) masks.push_back( Bitset::rangeMask( bitNum, separators[s], separators[ s + 1 ] ) ); }
	  };



///////////////////////////////////////////// MultiBitset //////////////////////////////////////////////////////
	class MultiBitset : public Bitset
	  /* Bitset with divisions between groups of consecutive bits
	  To store the state of different types of elements in a single object, flat */
	{
		public:
				  using Parent = Bitset;
				  using MaskType = MultiBitsetCommon::MaskType;
				  using Bitset::all; using Bitset::none; using Bitset::any; using Bitset::reset;

				//static generic operators  
				  template< typename TOp >
				  static MultiBitset unaryOperator( const MultiBitset& obj ) { return MultiBitset( obj.common, Op::unaryOperator< TOp >( obj.getWords() ) ); }

				  template< typename TOp >
				  static MultiBitset binaryOperator( const MultiBitset& obj1, const MultiBitset& obj2 ) { return MultiBitset( obj1.common, Op::binaryOperator<TOp>( obj1.getWords(), obj2.getWords() ) ); }


			//---ctor, dtor
				  inline MultiBitset() : Bitset::Bitset(), common(nullptr) {;}
				  inline MultiBitset( const MultiBitsetCommon* common, const Bitset::UnderDS& words ) : Bitset::Bitset( words, common->bitNum ), common(common) { assert( common != nullptr ); }
				  inline MultiBitset( const MultiBitsetCommon* common ) : Bitset::Bitset( common->bitNum ), common(common) { assert( common != nullptr ); }

			virtual ~MultiBitset() = default;
			MultiBitset( const MultiBitset& rhs ) = default;
			MultiBitset( MultiBitset&& rhs ) noexcept = default;
			MultiBitset& operator=( const MultiBitset& rhs ) = default;
			MultiBitset& operator=( MultiBitset&& rhs ) noexcept = default;

		//---get
				  inline const MultiBitsetCommon* getCommon() const { return common; }


		//---API
			inline size_t abs2relBit( size_t segIdx, size_t absIdx ) const { return common->abs2relBit( segIdx, absIdx ); }
			inline size_t rel2absBit( size_t segIdx, size_t relIdx ) const { return common->rel2absBit( segIdx, relIdx ); }
	
				  inline size_t findFirstAbs( size_t segIdx, bool val = true ) const { return findNextAbs( segIdx, common->getStart( segIdx ), val); }
				  inline size_t findNextAbs( size_t segIdx, size_t start, bool val = true ) const { return this->next( start, common->getEnd( segIdx ), val ); }
			inline size_t findFirstRel( size_t segIdx, bool val = true ) const { return findNextRel( segIdx, 0, val); }
			size_t findNextRel( size_t segIdx, size_t start, bool val = true ) const;

			inline MaskType getMasked( size_t segIdx ) const { return (*this) & common->getMask( segIdx ); }
				  inline void setMasked( size_t segIdx, bool val = true ) { Bitset::setRange( common->getStart( segIdx ), common->getEnd( segIdx ), val ); }

				  inline bool all( size_t segIdx ) const { return allInRange( common->separators[segIdx], common->separators[segIdx + 1] ); } 
				  inline bool none( size_t segIdx ) const { return noneInRange( common->separators[segIdx], common->separators[segIdx + 1] ); } 
				  inline bool any( size_t segIdx ) const { return anyInRange( common->separators[segIdx], common->separators[segIdx + 1] ); } 
				  inline void reset( size_t segIdx ) { resetRange( common->separators[segIdx], common->separators[segIdx + 1] ); }

		//---friend operators (use static functions )
			inline friend MultiBitset operator~( const MultiBitset& rhs ) { return MultiBitset::unaryOperator< typename Op::BitNeg >( rhs ); }
				  inline friend MultiBitset operator&( const MultiBitset& obj1, const MultiBitset& obj2 ) { return MultiBitset::binaryOperator< typename Op::BitAnd >( obj1, obj2 ); }
				  inline friend MultiBitset operator|( const MultiBitset& obj1, const MultiBitset& obj2 ) { return MultiBitset::binaryOperator< typename Op::BitOr >( obj1, obj2 ); }
				  inline friend MultiBitset operator^( const MultiBitset& obj1, const MultiBitset& obj2 ) { return MultiBitset::binaryOperator< typename Op::BitXor >( obj1, obj2 ); }


		private:
				  const MultiBitsetCommon* common;
	};


//////////////////////////////// METHOD DEFINITIONS ////////////////////////////////////////////////////

	  inline size_t MultiBitset::findNextRel( size_t segIdx, size_t start, bool val ) const 
	  { 
			size_t absResult = this->next( rel2absBit( segIdx, start ), common->getEnd( segIdx ), val );
			return absResult == npos() ? absResult : abs2relBit( segIdx, absResult ); 
	  }
}

#endif //UT_MULTIBITSET_HPP