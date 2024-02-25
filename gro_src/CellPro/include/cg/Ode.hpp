#ifndef CG_ODE_HPP
#define CG_ODE_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp" //iniValueDist, ut::ContinuousDistData
#include "ut/DistributionScaler.hpp"
#include "ut/Param.hpp" //ParamData

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class and delta elem

/*PRECOMPILED
#include <memory> //SP<> */


namespace cg
{
	class GeneticCollection; 
	class Ode : public GeneticElement
	/* Differential equation. Can be either ordinary, stochastic or delayed. It acts as the integral, so they are always non-negative */ 
	{
		REGISTER_CLASS( "Ode", "ode", GeneticElement::ElemTypeIdx::ODE, true )

		public:
            static constexpr TReal DF_INI_VALUE_MEAN = 0.0;
			static constexpr TReal DF_INI_VALUE_VAR = 0.0;
			static constexpr ut::ContinuousDist::DistributionType DF_INI_VALUE_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
			inline static const std::vector<TReal> DF_INI_VALUE_PARAMS = { DF_INI_VALUE_MEAN, DF_INI_VALUE_VAR };
			inline static const ut::ContinuousDistData DF_INI_VALUE_DIST = ut::ContinuousDistData( DF_INI_VALUE_DISTTYPE, DF_INI_VALUE_PARAMS );

            static constexpr TReal DF_SCALE = 1.0;
            static constexpr TReal DF_PARTITION_SCALE = 1.0;


    /////////////////////////////////////////////////////////// NESTED CLASSES (PARTITION) ///////////////////////////////////////////////////////////////////////////////
            class PartitionBase
            /* Virtual base class for Ode partitions. How the value of the Ode is split on cell division. They use the functions of DivisionSplit, as PartitionSystem does */
            {
            	public:
					static constexpr bool DF_B_VOLUME_SCALING = true;

					virtual ~PartitionBase() = default;

				//---get
					inline bool getBVolumeScaling() const { return bVolumeScaling; } 
					const GeneticElement* getParamFunction() const { return paramFunction; }
					const GeneticElement* getParamRandomness() const { return paramRandomness; }

				//---set
					inline void setParamFunction( const GeneticElement* xParamFunction ) { paramFunction = xParamFunction; }
					void setParamRandomness( const GeneticElement* xParamRandomness );

				//---API
					inline bool sampleDaughter() const { return daughterDist.sample(); }
					//virtual uint sample( TReal copyNum ) const = 0;
					virtual uint sample( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const = 0;
					virtual TReal sampleParam( TReal rawRndVal ) const = 0;


				protected:
					bool bVolumeScaling;

					const GeneticElement* paramFunction; //optional custom function for p param
		        	const GeneticElement* paramRandomness; //raw random values for pScaler
		       		SP<ut::DistributionScaler> paramScaler; //to get stochastic values for the p param of the binomial
					ut::DistributionScaler::ParamsType paramParams;

					mutable ut::ConcDistCombi< ut::DiscreteUniformDist > daughterDist;

					PartitionBase( const GeneticCollection* geneticCollection, const ParamDataType& paramData, bool bVolumeScaling = DF_B_VOLUME_SCALING );
            };


            class PartitionContinuous : public PartitionBase
            /* Split method for the Ode value on cell division that uses a custom continuous distribution */
            {
            	public: 
					static constexpr TReal DF_FRACTION_MEAN = 0.5;
					static constexpr TReal DF_FRACTION_VAR = 0.0;
					static constexpr ut::ContinuousDist::DistributionType DF_FRACTION_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
					inline static const ut::DistributionScaler::ParamsType DF_FRACTION_PARAMS = { DF_FRACTION_MEAN, DF_FRACTION_VAR };
					inline static const ParamDataType DF_PARAMS_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_FRACTION_PARAMS ); 
					

				//---ctor, dtor
					PartitionContinuous( const GeneticCollection* geneticCollection, const ParamDataType& paramData = DF_PARAMS_DATA, bool bVolumeScaling = DF_B_VOLUME_SCALING );
					virtual ~PartitionContinuous() = default;

				//---API
					uint sample( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const override; //scaled by the different volumes of the two daughter cells
					inline TReal sampleFraction( TReal rawRndVal ) const { return ut::fit( paramScaler->scale( rawRndVal ) ); } //as a fraction, in [0,1]
					TReal sampleParam( TReal rawRndVal ) const override { return sampleFraction( rawRndVal ); }  
            };


            class PartitionBinomial : public PartitionBase
            /* Split method for the Ode value on cell division that uses a binomial distribution */
            {
            	public: 
            		static constexpr TReal DF_P = 0.5;
            		inline static const ut::DistributionScaler::ParamsType DF_P_PARAMS = { DF_P, 0.0 };
            		inline static const ParamDataType DF_PARAMS_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_P_PARAMS ); 

            	//---ctor, dtor
            		PartitionBinomial( const GeneticCollection* geneticCollection, const ParamDataType& paramData = DF_PARAMS_DATA, bool bVolumeScaling = DF_B_VOLUME_SCALING );
					virtual ~PartitionBinomial() = default;

				//---API
					uint sample( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const override; //scaled by the different volumes of the two daughter cells
					inline TReal sampleP( TReal rawRndVal ) const { return ut::fit( paramScaler->scale( rawRndVal ) ); } //in [0,1]
					TReal sampleParam( TReal rawRndVal ) const override { return sampleP( rawRndVal ); }  


		        private:	
		            mutable ut::DiscreteDistCombi< ut::BinomialDist > binomialDist;
		    };
			
			
			


	/////////////////////////////////////////////////////////// ODE //////////////////////////////////////////////////////////////////////////////

            enum class PartitionType
            {
            	BINOMIAL, CONTINUOUS, COUNT
            };

            static constexpr PartitionType DF_PARTITION_TPYE = PartitionType::CONTINUOUS;
            static constexpr bool DF_B_BINOMIAL_SPLIT = false;
			inline static const ut::DistributionScaler::ParamsType DF_PARTITION_PARAMS = PartitionContinuous::DF_FRACTION_PARAMS;
			static constexpr bool DF_B_VOLUME_SCALING_PARTITION = PartitionBase::DF_B_VOLUME_SCALING;


		//---ctor, dtor
			Ode( const GeneticCollection* geneticCollection, size_t id, const std::string& name                                  
			, const GeneticElement* gate, const GeneticElement* deltaElem, const GeneticElement* iniValueFunction, const ut::ContinuousDistData& iniValueDist = DF_INI_VALUE_DIST, TReal scale = DF_SCALE
			, const ParamDataType& partitionParamData = PartitionContinuous::DF_PARAMS_DATA, PartitionType partitionType = DF_PARTITION_TPYE, TReal partitionScale = DF_PARTITION_SCALE, bool bVolumeScalingPartition = PartitionBase::DF_B_VOLUME_SCALING );

			virtual ~Ode() = default;

		//---get
			inline const GeneticElement* getGate() const { return gate; }
			inline const GeneticElement* getDeltaElem() const { return deltaElem; }
			inline const GeneticElement* getIniValueFunction() const { return iniValueFunction; }
			inline const GeneticElement* getPartitionFunction() const { return partition->getParamFunction(); }
			inline const GeneticElement* getPartitionRandomness() const { return partition->getParamRandomness(); }
			inline const GeneticElement* getSplitMarker() const { return splitMarker; }

			inline TReal getScale() const { return scale; }
			inline TReal getPartitionScale() const { return partitionScale; }

		//---set
			void setAll( const GeneticElement* xGate, const GeneticElement* xDeltaElem, const GeneticElement* xIniValueFunction, const GeneticElement* xPartitionFunction, const GeneticElement* xPartitionRandomness );
			inline void setGate( const GeneticElement* xGate ) { gate = xGate; }
			inline void setDeltaElem( const GeneticElement* xDeltaElem ) { deltaElem = xDeltaElem; }
			inline void setIniValueFunction( const GeneticElement* xIniValueFunction ) { iniValueFunction = xIniValueFunction; }
			inline void setPartitionFunction( const GeneticElement* xPartitionFunction ) { partition->setParamFunction( xPartitionFunction ); }
			inline void setPartitionRandomness( const GeneticElement* xPartitionRandomness ) { partition->setParamRandomness( xPartitionRandomness ); }
			
		//---API
		  //precompute
			void loadDirectElemLink() override { this->addLink( deltaElem, LinkDirection::BACKWARD ); }
		  //run
			inline TReal sampleIniValue() const { return ut::fitL( iniValueDist.sample() ); } //ensuring that it is not negative
			//inline uint samplePartition( uint copyNum ) const { return partition->sample( copyNum ); } //independent of cell volume
			inline uint samplePartition( uint copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const { return partition->sample( copyNum, volFraction, funVal, rawRndVal ); } //scaled by the relative volume of daughter cells
			inline TReal samplePartitionParam( TReal rawRndVal ) const { return partition->sampleParam( rawRndVal ); }


		private:
		  //resources
			const GeneticElement* gate; //condition for the Ode to be updated
			const GeneticElement* deltaElem; //derivative
			const GeneticElement* iniValueFunction; //initial value, in case a custom function is to be used. If null, a simple scalar from iniValueDist is used
			SP<PartitionBase> partition; //how the value is split on cell division
			const GeneticElement* splitMarker;
		  //options
			TReal scale; //multiplies deltaElem. Used for convenience to avoid creating a custom function just to scale
			TReal partitionScale; //conversion factor from the concentration units of the Ode to molecules/um3
		  //dists
			mutable ut::ContinuousDistCombi iniValueDist; //to sample the initial value of newly created cells. Daughter cells don't use this (they inherit the value using the partition)
	};
}

#endif //CG_ODE_HPP