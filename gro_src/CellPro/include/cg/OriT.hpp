#ifndef CG_ORIT_HPP
#define CG_ORIT_HPP

#include "ut/Metadata.hpp"
#include "ut/DistributionCombi.hpp" //plasmidSelectionDist

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor */


namespace cg
{
    class GeneticCollection;
 	class EventMarker; //donorEventMarker, recipientEventMarker
    class Pilus; //pilus
	class OriT : public GeneticElement
	/* DNA component of both BPlasmids and QPlasmids that allow transmission by conjugation, using a specific Pilus */
	{
		REGISTER_CLASS( "OriT", "oT", GeneticElement::ElemTypeIdx::ORI_T, true )

		public:
			struct Markers
			{
				const EventMarker* donorEventMarker; //on in the donor during the time step of a conjugation event transfering a plasmid from this OriT
				const EventMarker* recipientEventMarker; //same as donorEventMarker, but on the recipient side

				explicit Markers( const EventMarker* donorEventMarker = nullptr, const EventMarker* recipientEventMarker = nullptr )
				: donorEventMarker( donorEventMarker ), recipientEventMarker( recipientEventMarker ) {;}
			};

			inline static const GeneticElement* DF_RATE_W_FUN = nullptr;
			inline static const GeneticElement* DF_RATE_W_RND = nullptr;

			static constexpr TReal DF_RATE_W_MEAN = 1.0;
			static constexpr TReal DF_RATE_W_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_RATE_W_PARAMS = { DF_RATE_W_MEAN, DF_RATE_W_VAR };
			inline static const ParamDataType DF_RATE_W_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_RATE_W_PARAMS ); 

			static constexpr bool DF_BCOPY_DEPENDENT = false;
			static constexpr bool DF_B_ECLIPSE = false;
			inline static const std::vector<size_t> DF_MUTATION_KEYS = {1};
			

		//---ctor, dtor
			OriT( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const Pilus* pilus, const GeneticElement* donorGate, const GeneticElement* recipientGate, const ParamDataType& rateWData = DF_RATE_W_DATA, const Markers& markers = Markers()
			, bool bCopyDependent = DF_BCOPY_DEPENDENT, bool bEclipse = DF_B_ECLIPSE, const std::vector<size_t>& mutationKeys = DF_MUTATION_KEYS );

			virtual ~OriT() = default;

		//---get
		  //resources
			inline const Pilus* getPilus() const { return pilus; }
			inline const GeneticElement* getDonorGate() const { return donorGate; }
			inline const GeneticElement* getRecipientGate() const { return recipientGate; }
			inline const GeneticElement* getRateWFunction() const { return rateWFunction; }
			inline const GeneticElement* getRateWRandomness() const { return rateWRandomness; }
			inline const GeneticElement* getRateWMarker() const { return rateWMarker; }
		  //markers
			inline const EventMarker* getDonorEventMarker() const { return markers.donorEventMarker; }
			inline const EventMarker* getRecipientEventMarker() const { return markers.recipientEventMarker; }

		  //options
			inline bool getIsCopyDependent() const { return bCopyDependent; }
			inline bool getBEclipse() const { return bEclipse; }
			inline bool getHasEclipse() const { return bEclipse; }
			inline const std::vector<size_t>& getMutationKeys() const { return mutationKeys; }

		//---set
			void setAll( const GeneticElement* xDonorGate, const GeneticElement* xRecipientGate, const GeneticElement* xRateWFunction, const GeneticElement* xRateWRandomness );
			inline void setDonorGate( const GeneticElement* xDonorGate ) { donorGate = xDonorGate; }
			inline void setRecipientGate( const GeneticElement* xRecipientGate ) { recipientGate = xRecipientGate; }
			inline void setRateWFunction( const GeneticElement* xRateWFunction ) { rateWFunction = xRateWFunction; }
			void setRateWRandomness( const GeneticElement* xRateWRandomness );
			
		//---API
		  //precompute
			void loadDirectElemLink() override;
		  //sampling
			inline TReal sampleRateW( TReal rawRndVal ) const { return ut::fitL( rateWScaler->scale( rawRndVal ) ); } //positive
			inline TReal sampleRateW( TReal funVal, TReal rawRndVal ) const { return rateWFunction ? funVal : ut::fitL( rateWScaler->scale( rawRndVal ) ); } //positive
			inline size_t samplePlasmid( const ut::DiscreteDist::ParamsType& params ) const { return plasmidSelectionDist.sample( params ); } //sample concrete plasmid


		private:
		  //resources
			const Pilus* pilus; //must be active
			const GeneticElement* donorGate; //must be true for the donor cell
			const GeneticElement* recipientGate; //must be true for the recipient cell. To simulate tru entry exclusion (surface exclusion is at Pilus level)
			const GeneticElement* rateWFunction; //custom function for w
			const GeneticElement* rateWRandomness; //raw random values for rateWScaler
			const GeneticElement* rateWMarker;
			Markers markers; 
		  //options
			bool bCopyDependent; //whether the probability of selecting this OriT and a specific plasmid carrying is scaled by the copy number
			bool bEclipse; //whether there is an eclipse period (duration at QPlasmid level) if the transmitted plasmid is a QPlasmid
			std::vector<size_t> mutationKeys; //possible replication mutations 
		  //dists
			SP<ut::DistributionScaler> rateWScaler; //potentially stochastic weight (chance of selecting this oriT over others that use the same Pilus)
			ut::DistributionScaler::ParamsType rateWParams;
			mutable ut::DiscreteDistCombi< ut::DiscreteDist > plasmidSelectionDist; //to select the specific plasmid from those that carry the oriT
	};
}

#endif //CG_ORIT_HPP