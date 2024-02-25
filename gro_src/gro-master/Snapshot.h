#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "ut/Metadata.hpp" //REGISTER_CLASS

#include "Defines.h"
#include "GroElement.h" //base


class Timer; //timer
class GroCollection; //ctor
class Snapshot : public GroElement
{
    REGISTER_CLASS( "Snapshot", "sn", ElemTypeIdx::SNAPSHOT )

	public:
		inline static const std::string DF_BASE_PATH = "C:";
        inline static const std::string DF_FOLDER_NAME = "gro_snapshots";
		inline static const std::string DF_FILE_NAME = "";
		inline static const std::string DF_FORMAT = ".png";


	//---ctor, dtor
		Snapshot( GroCollection* groCollection, size_t id, const std::string& name
		, const Timer* timer, const std::string& path, const std::string& fileName = DF_FILE_NAME, const std::string& imgFormat = DF_FORMAT );

		virtual ~Snapshot() = default;

	//---get
		inline const Timer* getTimer() const { return timer; }
		inline const std::string& getPath() const { return path; }
		inline const std::string& getFileName() const { return fileName; }
		inline const std::string& getImgFormat() const { return imgFormat; }

	//---API
	  //precompute
		void loadDirectElemLink() override;


	private:
	  //resources
		const Timer* timer;
	  //files
		std::string path;
		std::string fileName;
		std::string imgFormat;
};

#endif // SNAPSHOT_H
