#ifndef MG_SIGNAL_H
#define MG_SIGNAL_H

#include "ut/RgbColour.hpp"
#include "mg/defines.hpp"
#include "mg/DiffusionMatrix.hpp"
#include "mg/MediumElement.hpp" //base class

#include <vector>
#include <array> //colour

namespace mg
{
	class MediumCollection;
    class Signal : public MediumElement
    {
        REGISTER_CLASS( "Signal", "s", MediumElement::ElemTypeIdx::SIGNAL );
        
        public:
            using Neighborhood = DiffusionMatrix::Neighborhood;

            static constexpr TReal DF_KDIFF = 0.1;
            static constexpr TReal DF_KDEG = 0.0;
            static constexpr TReal DF_CONC_THRESHOLD = 0.000001;
            static constexpr Neighborhood DF_NEIGHBORHOOD = Neighborhood::MOORE_8;
            inline static const DiffMatrixDS DF_MATRIX = {};

            inline static const ut::RgbColour DF_COLOUR = ut::RgbColour( 1.0, 0.0, 1.0 );
            static constexpr TReal DF_COLOUR_SAT = 1.0;


        //---ctor, dtor
            Signal( const MediumCollection* collection, size_t relativeId, const std::string& name 
            , const ut::RgbColour& rgbColour, TReal colourSaturationConc, TReal kdiff, TReal kdeg, TReal resizeThresholdConc = DF_CONC_THRESHOLD, Neighborhood neighborhood = DF_NEIGHBORHOOD, const DiffMatrixDS& matrix = DF_MATRIX ) 
            
            : MediumElement::MediumElement( relativeId, name, MediumElement::ElemTypeIdx::SIGNAL, false, collection )
            , rgbColour(rgbColour), colourSaturationConc( colourSaturationConc )
            , kdiff(kdiff), kdeg(kdeg), resizeThresholdConc(resizeThresholdConc), diffMatrix(neighborhood, matrix) {;}

            virtual ~Signal() = default;
            Signal( const Signal& rhs ) = delete;
            Signal& operator=( const Signal& rhs ) = delete;

        //---get 
            inline const ut::RgbColour& getColour() const { return rgbColour; }
            inline TReal getColourSaturationConc() const { return colourSaturationConc; }
            inline TReal getKdiff() const { return kdiff; }
            inline TReal getKdeg() const { return kdeg; }
            inline TReal getResizeThresholdConc() const { return resizeThresholdConc; }
            
            inline const DiffusionMatrix& getDiffMatrix() const { return diffMatrix; }
            inline Neighborhood getNeighborhood() const { return diffMatrix.neighborhood; }
            inline uint getMatrixSize() const { return diffMatrix.size(); }
            inline uint getMatrixHalfSize() const { return diffMatrix.halfSize; }
            inline uint getPaddingUnits() const { return diffMatrix.paddingUnits; }

        //---set

        //---API

        protected:
            ut::RgbColour rgbColour;
            //ut::RgbColourArr rgbColour;
            TReal colourSaturationConc;

            TReal kdiff;
            TReal kdeg;
            TReal resizeThresholdConc;
            DiffusionMatrix diffMatrix;
    };
}

#endif //MG_SIGNAL_H
