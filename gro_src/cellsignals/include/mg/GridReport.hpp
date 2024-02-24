#ifndef _GRID_REPORT_H_
#define _GRID_REPORT_H_

#include "mg/defines.hpp"
#include "mg/Grid.hpp"
#include "mg/GridUnit.hpp"

#include <vector>
#include <algorithm> //std::swap


namespace mg
{
    class GridReport 
    {
        public:
            using ConcsDS = Grid::ConcsDS;
            using GridRow = Grid::GridRow;
            using GridMatrix = Grid::GridMatrix;
            using MatrixesDS = Grid::MatrixesDS;

        //---ctor, dtor
            GridReport( const Grid* grid )
            : grid( grid )
            , gridSize( grid->getIniGridSize() ), unitSize( grid->getUnitSize() )
            , matrixes( grid->getMatrixNum(), GridMatrix() ), currentGridIndex(0), tempGridIndex(1)
            { 
                recalculateCenter();
            }

            virtual ~GridReport() { clearGrids(); }
            GridReport( const GridReport& rhs ) = delete;
            GridReport& operator=( const GridReport& rhs ) = delete;

        //---get
            inline int getGridSize() const { return gridSize; }
            inline uint getUnitSize() const { return unitSize; }
            inline double getCenterCoord() const { return centerCoord; }

            inline GridMatrix& getMatrix( size_t idx ) { return matrixes[idx]; }
            inline GridMatrix& getCurrentMatrix() { return matrixes[currentGridIndex]; } 
            inline GridMatrix& getTempMatrix() { return matrixes[tempGridIndex]; } 
            
        //---set

        //---API
          //basic
            inline void init( size_t signalNum ) { fillGrids( signalNum ); }
            inline void update() { swapMatrixes(); } 
            inline void swapMatrixes() { std::swap( currentGridIndex, tempGridIndex ); } 

		//---unit access
          //init
            inline size_t getConcsSize() const { return getConcsAtGridUnit(0,0).size(); } //ÑAPA

            inline const GridUnit& getUnit( int row, int col) const { return matrixes[currentGridIndex][row][col]; }
            inline GridUnit& getUnitEdit( int row, int col) { return matrixes[currentGridIndex][row][col]; }
            inline const GridUnit& getUnit( int row, int col, uint index ) const { return matrixes[index][row][col]; }
            inline GridUnit& getUnitEdit( int row, int col, uint index ) { return matrixes[index][row][col]; }
			inline const GridUnit& getUnitAtSpace( double x, double y ) const { return getUnit( groCoord2gridUnit(y), groCoord2gridUnit(x) ); }
            inline GridUnit& getUnitAtSpaceEdit( double x, double y ) { return getUnitEdit( groCoord2gridUnit(y), groCoord2gridUnit(x) ); }
          //concs
            inline const ConcsDS& getConcsAtGridUnit( int row, int col ) const { return getUnit( row, col ).getConcs(); }
            inline TReal getConcAtGridUnit( size_t id, int row, int col ) const { return getUnit( row, col ).getConc(id); }
           	inline const ConcsDS& getConcsAtSpace( double x, double y ) const { return getConcsAtGridUnit( groCoord2gridUnit(y), groCoord2gridUnit(x) ); }
            inline TReal getConcAtSpace( size_t id, double x, double y ) const { return getConcAtGridUnit( id, groCoord2gridUnit(y), groCoord2gridUnit(x) ); }


            void setConcsRectangleUnit( const ConcsDS& concs, int row1, int row2, int col1, int col2 );
            void setConcRectangleUnit( size_t id, TReal conc, int row1, int row2, int col1, int col2 );
            inline void setConcsRectangleSpace( const ConcsDS& concs, double x1, double x2, double y1, double y2 ) { setConcsRectangleUnit( concs, groCoord2gridUnit( x1 ), groCoord2gridUnit( x2 ), groCoord2gridUnit( y1 ), groCoord2gridUnit( y2 ) ); }
            inline void setConcRectangleSpace( size_t id, TReal conc, double x1, double x2, double y1, double y2 ) { setConcRectangleUnit( id, conc, groCoord2gridUnit( x1 ), groCoord2gridUnit( x2 ), groCoord2gridUnit( y1 ), groCoord2gridUnit( y2 ) ); }

            inline void setConcsAtGridUnit( const ConcsDS& concs, int row, int col ) { for( size_t id = 0; id < concs.size(); id++ ) setConcAtGridUnit( id, concs[id], row, col ); }
            inline void setConcAtGridUnit( size_t id, TReal conc, int row, int col ) { getUnitEdit(row, col).setConc(id, conc); }
           	inline void setConcsAtSpace( const ConcsDS& concs, double x, double y) { setConcsAtGridUnit( concs, groCoord2gridUnit(y), groCoord2gridUnit(x) ); }
            inline void setConcAtSpace( size_t id, TReal conc, double x, double y ) { setConcAtGridUnit( id, conc, groCoord2gridUnit(y), groCoord2gridUnit(x) ); }

            inline void changeConcsAtGridUnit( const ConcsDS& changes, int row, int col ) { for( size_t id = 0; id < changes.size(); id++ ) changeConcAtGridUnit( id, changes[id], row, col ); } 
            inline void changeConcAtGridUnit( size_t id, TReal change, int row, int col ) { getUnitEdit(row, col).changeConc(id, change); } 
           	inline void changeConcsAtSpace( const ConcsDS& changes, double x, double y) { changeConcsAtGridUnit( changes, groCoord2gridUnit(y), groCoord2gridUnit(x) ); }
            inline void changeConcAtSpace( uint id, TReal change, double x, double y ) { changeConcAtGridUnit( id, change, groCoord2gridUnit(y), groCoord2gridUnit(x) ); }

          //coord conversion
            inline double groCoord2gridCoord( double groCoord ) const { return groCoord + centerCoord; }
            inline int gridCoord2gridUnit( double gridCoord ) const { return static_cast<int>( gridCoord / unitSize ); }
            inline int groCoord2gridUnit( double groCoord ) const { return gridCoord2gridUnit( groCoord2gridCoord( groCoord ) ); }
            inline double gridCoord2groCoord( double gridCoord ) const { return gridCoord - centerCoord; }

        //---misc
            inline bool checkUnitPhysical( int coord ) { return gridSize > 0 && coord > 0 && coord < (int)gridSize -1; }
            inline bool checkPhysicalLimitsUnit( int row, int col ) { return checkUnitPhysical( row ) && checkUnitPhysical( col ); }
            inline bool checkPhysicalLimitsSpace( double x, double y ) { return checkPhysicalLimitsUnit( groCoord2gridUnit( y ), groCoord2gridUnit( x ) ); }

            void resizePhysical();



        private:
            const Grid* grid;
          //params
            uint gridSize; //number of rows and columns (square)
            uint unitSize;
            double centerCoord;

          //matrixes
            MatrixesDS matrixes; //one the current grid; the other, temp grid used when updating. To avoid copying it, toggled every update cycle
            size_t currentGridIndex; //which of the two matrixes is the current one
            size_t tempGridIndex; //which of the two matrixes is used as temp for updating


        //---init and clear
            void fillGrids( size_t signalNum ) { clearGrids(); for( size_t idx = 0; idx < matrixes.size(); idx++ ) fillGrid( idx, signalNum ); }
            void fillGrid( size_t idx, size_t signalNum );
            void clearGrids() { /*for( size_t idx = 0; idx < matrixes.size(); idx++ ) clearGrid( idx );*/ }
            void clearGrid( size_t );

        //---aux
            inline void setGridSize( uint xGridSize ) { gridSize = xGridSize; recalculateCenter(); }
            inline void recalculateCenter() { centerCoord = ( gridSize * unitSize ) / 2.0;  }

            void createMatrix( GridMatrix& matrix, size_t signalNum, size_t rowNum, size_t colNum );
            GridMatrix createMatrix( size_t signalNum, size_t rowNum, size_t colNum );
            GridRow createRow( size_t signalNum, size_t colNum ); 
    };
}

#endif //_GRID_REPORT_H_