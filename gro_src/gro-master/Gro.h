#ifndef GRO_H
#define GRO_H

#include "ccl.h" //as a connector to ccl module. Value and Scope in newElement()
#include "GroThread.h" //GroThread::currentThread in newElement()

template< typename T > 
Value* newElement( std::list<Value*>* args, Scope* )
{
    ASSERT ( args->size() == 1 );
    GroThread::currentThread->getParser()->newElement<T>( *args->begin() );
    return new Value ( Value::UNIT ); //not good design but necessary for compatibility with ccl
}

void registerGroFunctions();

#endif //GRO_H
