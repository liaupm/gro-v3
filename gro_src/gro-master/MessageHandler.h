#ifndef _MESSAGEHANDLER_H_
#define _MESSAGEHANDLER_H_

#include "Defines.h"

/*PRECOMPILED
#include <vector> //buffers
#include <list> //messageQueue
#include <string> //messages 

#include <QTextOption> //aligmentOption */

class GroPainter; //render()
class MessageHandler
{
	public:
		class MessageBuffer
		{
			public:
				friend MessageHandler;
				static constexpr int DF_INVERT_X = 1;
				static constexpr int DF_INVERT_Y = 1;
				static constexpr uint DF_OFFSET_X = 0;
				static constexpr uint DF_OFFSET_Y = 0;
				static constexpr uint DF_SIZE_X = 20;
				static constexpr uint DF_SIZE_Y = 0;
				static constexpr uint DF_BUFSIZE = 10;


			//---ctor. dtor
				MessageBuffer( int inverterX = DF_INVERT_X, int inverterY = DF_INVERT_Y, uint offsetX = DF_OFFSET_X, uint offsetY = DF_OFFSET_Y, uint sizeX = DF_SIZE_X, uint sizeY = DF_SIZE_Y
				, uint maxQueueSize = DF_BUFSIZE, const QTextOption& aligmentOption = QTextOption( Qt::AlignLeft ) )
                : inverterX( inverterX ), inverterY( inverterY ), offsetX(offsetX), offsetY( offsetY ), sizeX( sizeX ), sizeY( sizeY ), aligmentOption( aligmentOption ), maxQueueSize( maxQueueSize ) {;}

				~MessageBuffer() = default;

			//---API
				inline void addMessage( const std::string& newMessage ) { messageQueue.push_front( newMessage ); if( messageQueue.size() > maxQueueSize ) messageQueue.pop_back(); }
				inline void clearMessages() { messageQueue.clear(); }
				void render( GroPainter* painter, uint h, uint w );


			private:
				int inverterX;
				int inverterY;
				uint offsetX;
				uint offsetY;
				uint sizeX;
				uint sizeY;
				QTextOption aligmentOption;

				uint maxQueueSize;
				std::list<std::string> messageQueue;
		};



//////////////////////// MessageHandler //////////////////////////////////////
		
		static constexpr uint DF_BUFSIZE = MessageBuffer::DF_BUFSIZE;
		static constexpr uint DF_FONTSIZE = 16;

	//---ctor, dtor
		MessageHandler();
		~MessageHandler() = default;

	//---API
		inline void addMessage( uint bufferIndex, const std::string& newMessage ) { ASSERT ( bufferIndex < buffers.size() ); buffers[bufferIndex].addMessage( newMessage); }
		inline void clearMessages( uint bufferIndex ) { ASSERT ( bufferIndex < buffers.size() ); buffers[bufferIndex].clearMessages(); } // clears a quadrant
		inline void clear_messages_all() { for( uint b = 0; b < buffers.size(); b++ ) buffers[b].clearMessages(); } // clears all quadrants
		void render( GroPainter* painter );


	private:
		std::vector<MessageBuffer> buffers;
};

#endif
