#include "Generic_mapper.hpp"
#include "tools.hpp"
namespace travel{
    class Derived_mapper : public Generic_mapper{ 
	public :
	    Derived_mapper(const std::string& stations_filename, const std::string& connections_filename);
	    std::vector<std::pair<uint64_t, uint64_t> > compute_travel(uint64_t _start, uint64_t _end) override; 
	    std::vector<std::pair<uint64_t, uint64_t> > compute_and_display_travel(uint64_t _start, uint64_t _end) override;
	    std::vector<std::pair<uint64_t, uint64_t> > compute_travel(const std::string& _start, const std::string& _end) override;
	    std::vector<std::pair<uint64_t, uint64_t> > compute_and_display_travel(const std::string& _start, const std::string& _end) override;
	    uint64_t choose_station_id(const std::string& _type);
	    std::string choose_station_name(const std::string& _type);
	protected :
	    void read_stations(const std::string& _filename) override; 
	    void read_connections(const std::string& _filename) override; 
	private :
	    std::string best_station_match(const std::string& s);
    };
}
