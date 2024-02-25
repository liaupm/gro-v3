/////////////////////////////////////////////////////////////////////////////////////////
//
// gro is protected by the UW OPEN SOURCE LICENSE, which is summarized here.
// Please see the file LICENSE.txt for the complete license.
//
// THE SOFTWARE (AS DEFINED BELOW) AND HARDWARE DESIGNS (AS DEFINED BELOW) IS PROVIDED
// UNDER THE TERMS OF THIS OPEN SOURCE LICENSE (“LICENSE”).  THE SOFTWARE IS PROTECTED
// BY COPYRIGHT AND/OR OTHER APPLICABLE LAW.  ANY USE OF THIS SOFTWARE OTHER THAN AS
// AUTHORIZED UNDER THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
//
// BY EXERCISING ANY RIGHTS TO THE SOFTWARE AND/OR HARDWARE PROVIDED HERE, YOU ACCEPT AND
// AGREE TO BE BOUND BY THE TERMS OF THIS LICENSE.  TO THE EXTENT THIS LICENSE MAY BE
// CONSIDERED A CONTRACT, THE UNIVERSITY OF WASHINGTON (“UW”) GRANTS YOU THE RIGHTS
// CONTAINED HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF SUCH TERMS AND CONDITIONS.
//
// TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION
//
//

#ifndef CELL_H
#define CELL_H

#include "cg/Genome.hpp" //internal bio
#include "Defines.h"

//PRECOMPUTED
/*#include "CellEngine.h" //physics
#include "CESpace.h" //physics */


namespace cg { class CellType; } //ctor
class World; //ctor and member ptr
class Cell 
{
	public:
		static constexpr TReal DF_SCALE = 10.0; //pixel/um

	////////////////////////////////////////////////////////////////////////// BODY WRAPPER ///////////////////////////////////////////////////////////////////
		struct CellPosition
		{
			TReal x;
			TReal y;
			TReal rotation;

			CellPosition( TReal x, TReal y, TReal rotation = 0.0 ) : x(x), y(y), rotation(rotation) {;}
		};

		class BodyWrapper
		{
			public:
			  //CellEngine constants, in pixels
				static constexpr TReal CE_WIDTH = cg::Strain::DF_DIAMETER * DF_SCALE; 
				static constexpr TReal CE_HALF_WIDTH = CE_WIDTH / 2.0;
				static constexpr TReal CE_MIN_LENGTH = CE_WIDTH;
				static constexpr TReal CE_MAX_LENGTH = 5.0 * CE_WIDTH;
				static constexpr TReal DF_CONJ_DISTANCE = 1.5;

			//ctor, dtor
				BodyWrapper() : rawBody( nullptr ), bValidData( false ) {;} //null Body with not vlaid data, must be updated before use
				BodyWrapper( ceBody* rawBody, Cell* enclosingCell = nullptr ) : rawBody(rawBody) { bValidData = updateData( enclosingCell ); } //external assigment of resources
				BodyWrapper( World* world, Cell* enclosingCell, TReal length, TReal x, TReal y, TReal rotation ); //the novo creation of cells
				BodyWrapper( BodyWrapper& motherBody, Cell* enclosingCell, TReal dRotation, TReal lengthMother, TReal lengthDaughter ); //from cell division
				BodyWrapper( const BodyWrapper& original ) = delete;
				BodyWrapper& operator=( const BodyWrapper& original ) = delete;
				inline ~BodyWrapper() { destroy(); } 

			//---get
				inline ceBody* getRaw() const { return rawBody; }
				inline const ceVector2& getCenter() const { return rawBody->center; }
				inline TReal getCenterX() const { return rawBody->center.x; } //get x coord of center
				inline TReal getCenterY () const { return rawBody->center.y; } //get y coord of center
				inline TReal getSizeX() const { return rawBody->halfDimensionAABB.x; } //get length (half) in x
				inline TReal getSizeY() const { return rawBody->halfDimensionAABB.y; } //get length (half) in y
				inline TReal getRotation() const { return rawBody->rotation; }	
				inline TReal getLength() const { return rawBody->length; }

			//---set
				inline void setRaw( ceBody* xRawBody ) { rawBody = xRawBody; }
				inline void setCenterX( TReal x ) { rawBody->center.x = x; }
				inline void setCenterY( TReal y ) { rawBody->center.y = y; }
				inline void setRotation( TReal xRotation ) { rawBody->rotation = xRotation; }
				inline void setLength( TReal xLength ) { rawBody->length = xLength; }
				inline void setPosition( TReal x, TReal y, TReal xRotation ) { rawBody->center.x = x; rawBody->center.y = y; rawBody->rotation = xRotation; }

			//---API
				inline bool updateData( Cell* enclosingCell ) { if( rawBody != nullptr && enclosingCell != nullptr ) { ceSetData( rawBody, enclosingCell ); return true; } return false; } //set a pointer to the owner Cell to the CellEngine raw body
				inline void grow( TReal dLength, TReal stepSize, bool bMustGrow = true ) { if( bMustGrow && ut::notZero( dLength ) ) ceGrowBody( rawBody, DF_SCALE * dLength * stepSize ); } //increase size
				void destroy(); //delete the raw CellEngine body and set this to a null invalid Body

				ceBody* rawBody;
				bool bValidData;
		};
		using Body = BodyWrapper;


////////////////////////////////////////////////////////////////////////// CELL ///////////////////////////////////////////////////////////////////

	//---ctor, dtor
		Cell( const cg::CellType* cellType, World* world, TReal x, TReal y, TReal rotation ); //de novo creation of a Cell
		Cell( Cell* motherCell ); //fake copy constructor for cell division
		Cell( const Cell& original ) = delete;
		Cell& operator=( const Cell& original ) = delete;
		virtual ~Cell() = default;

	//---get
	  //resources
		inline World* getWorld() const { return world; } 
		inline const Body& getBody() const { return body; } 
		inline Body& getBodyEdit() { return body; } 
		inline const cg::Genome& getGenome() const { return genome; }
		inline cg::Genome& getGenomeEdit() { return genome; }
	  //ids
		inline size_t getId() const { return id; }
		inline size_t getMotherId() const { return motherId; }
		inline size_t getSiblingId() const { return siblingId; }
		inline size_t getOriginId() const { return originId; }
		inline size_t getGeneration() const { return generation; }
		inline const std::string& getAncestry() const { return ancestry; }
	  //state
		inline bool checkDivision() { return genome.getBMustDivide(); }
		inline bool checkDeath() { return genome.getBMustDie() || bMarkedForDeath; } //either from internal bio or from external CellPlating
		inline bool getBMarkedForDeath() const { return bMarkedForDeath; }
		inline bool getIsMarkedForDeath() const { return bMarkedForDeath; }
		inline bool getBSelected() const { return bSelected; } //get wheter the cell is selected in the GUI
		inline bool getIsSelected() const { return bSelected; }
	  //body (CellEngine) wrapper
		inline TReal getX() const { return body.getCenterX(); }
		inline TReal getY() const { return body.getCenterY(); }
	  //genome (cg) wrapper
		inline TReal getGrowthRate() const { return genome.getGrowthRate(); }
		inline TReal getGenerationTime() const { return genome.getGenerationTime(); }
		inline TReal getTotalLength() const { return genome.getTotalLength(); }
		inline TReal getDLength() const { return genome.getDLength(); }
		inline TReal getVolume() const { return genome.getVolume(); }
		inline TReal getDVolume() const { return genome.getDVolume(); }

	//---set
		inline void markForDeath() { bMarkedForDeath = true; } 
		inline void select() { bSelected = true; } //select cell (GUI)
		inline void deselect() { bSelected = false; } //deselect cell (GUI)
		inline void updateIds( size_t xId, size_t xMotherId, size_t xSiblingId ) { id = xId; motherId = xMotherId; siblingId = xSiblingId; }

	//---API
		void initInternal( const std::vector<TReal>& mediumConcs ) { genome.initMetExternal( mediumConcs ); } //initialization of cg that requires external data but do not affect other Cells
		void updateInternal( const std::vector<TReal>& mediumConcs );//for calling every time step to update the agent's state. Update off cg that do not affect other Cells (expression, metabolism, qplasmids, colour)
		void calculateInteractions(); //calculate the interactions without applying them
		void updateInteractions(); //update of cg that does affect other cells (conjugation and growth)

		void updateDivided( size_t xId, size_t xMotherId, size_t xSiblingId ); //update the id and other ancestry trackers on cell division


	protected:
	  //resources
	 	World* world; //access to the world where the cell is contained
		cg::Genome genome; //link to CellGene
		Body body; //link to CellEngine. Contains position, dimension and orientation
	  //ids
		size_t id;
		size_t motherId;
		size_t siblingId;
		size_t originId; //ID of the original ancestor placed with CellPlacer
		size_t generation; //number of generation or divisions from the original cell placed with CellPlacer
		std::string ancestry; //track of all the divisions form the original cell

	  //state
		bool bMarkedForDeath; //death marker external to cg (from CellPlating) 
		bool bSelected; //wheter the cell is selected on the GUI
};

#endif // CELL_H
