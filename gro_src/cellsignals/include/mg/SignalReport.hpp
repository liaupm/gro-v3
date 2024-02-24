#ifndef MG_SIGNALREPORT_HPP
#define MG_SIGNALREPORT_HPP

#include "mg/defines.hpp"
#include "mg/Signal.hpp"
#include "mg/DynamicBorder.hpp"


namespace mg
{
    class SignalReport
    {
        public: 
        //---ctor, dtor
            SignalReport( const Signal* signal ): signal(signal), border(), bUpdate(false) {;}
            virtual ~SignalReport() = default;
            SignalReport( const SignalReport& rhs ) = delete;
			SignalReport& operator=( const SignalReport& rhs ) = delete;

        //---get
            inline const Signal* getSignal() const { return signal; }
            inline const DynamicBorder& getBorder() const { return border; }
            inline DynamicBorder& getBorderEdit() { return border; }
            inline bool getBUpdate() const { return bUpdate; }

            inline bool getBPresent() const { return border.getBPresent(); }
        
        //---set
            inline void setBUpdate( bool xBUpdate ) { bUpdate = xBUpdate; }
        
        //---API
            bool checkUpdate( int gridSize );
            void update( GridReport* gridReport ) { border.update( gridReport, signal ); } 
            inline void offsetBorder( int offset ) { border.offset( offset ); }  
            inline void resizeTo( GridReport* gridReport, int row, int col, int pad ) { border.resizeTo( gridReport, row, col, pad ); }  


        private:
            const Signal* signal;
            DynamicBorder border;
            bool bUpdate;
    };
}

#endif //MG_SIGNALREPORT_HPP