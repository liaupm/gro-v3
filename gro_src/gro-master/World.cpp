/////////////////////////////////////////////////////////////////////////////////////////
//
// gro is protected by the UW OPEN SOURCE LICENSE, which is summaraized here.
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

#include "World.h"
#include "ut/SeedGenerator.hpp" //SeedGeneratorRnd groCollection arg
#include "cg/GeneticCollection.hpp" //geneticCollection, link to M genetics
#include "mg/MediumCollection.hpp" //mediumCollection, link to M genetics


void World::precompute( TReal stepSize, uint xSeed ) 
{
    seed = xSeed;
    time = ut::Time( stepSize );

    groCollection = std::make_shared<GroCollection>( std::make_shared<ut::SeedGeneratorRnd<RandomEngine2>>( seed ) );
    geneticCollection = std::make_shared<cg::GeneticCollection>( groCollection->getSeedGeneratorPtr() );
    mediumCollection = std::make_shared<mg::MediumCollection>();
}