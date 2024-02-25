#ifndef PLACINGHANDLER_H
#define PLACINGHANDLER_H

#include "Defines.h"
#include "GroHandler.h" //base


namespace ut { class Bitset; class MultiBitset; } //update() params
class CellPlacer; //model
class CellPlating; //model
class SignalPlating; //model
class GroCollection; //ctor
class PetriDish; //ctor

struct PlacingHReportIdx
{
    enum ReportTypeIdx : size_t { COUNT };
};

class PlacingHandler : public GroHandler<>
{
	REGISTER_CLASS( "PlacingHandler", "plaH", GroHandler<>::HandlerTypeIdx::H_PLACING )
	
	public:
    //---cotr, dtor
    	PlacingHandler( PetriDish* owner, const GroCollection* collection ) : GroHandler( owner, collection ) {;}
    	virtual ~PlacingHandler() = default;
      
    //---API
      //common
        bool init() override { return update( nullptr, nullptr, nullptr ); }
        bool update( ut::MultiBitset*, ut::Bitset*, const GroHandler<>::EventType* ) override;


    private :
        void updateCellPlacer( const CellPlacer* cellPlacer );
        void updateCellPlating( const CellPlating* cellPlating );
        void updateSignalPlacer( const SignalPlacer* signalPlacer );
};

#endif // PLACINGHANDLER_H
