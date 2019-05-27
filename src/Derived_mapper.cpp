#include <fstream>
#include <algorithm>

#include "Derived_mapper.hpp"
namespace travel{
    //
    //public constructor: 
    //	@param stations_filename: le nom du fichier contenant les stations 
    //	@param connections_filename: le nom du fichier contenant les connections entre les stations
    Derived_mapper::Derived_mapper(const std::string& stations_filename, const std::string& connections_filename){
	//read_station et read_connections sont des methodes protegees
	//on ne peut pas les utiliser en dehors de la classe, c'est pour ca qu'on les utilise dans le constructeur
	this->read_stations(stations_filename);
	this->read_connections(connections_filename);
	this->stations_hashmap = this->get_stations_hashmap();
	this->connections_hashmap = this->get_connections_hashmap();
    }
    //
    //protected method: read_stations
    //	@param _filename: nom du fichier des stations
    //	returns nothing: construit l'unordered_map des stations
    void Derived_mapper::read_stations(const std::string& _filename){
	const std::string header("string_name_station,uint32_s_id,string_short_line,string_adress_station,string_desc_line");
	std::ifstream data(read_file(header,std::string("stations"),_filename));
	std::string name;
	std::string line_id;
	std::string address;
	std::string line_name;
	std::string id;
	Station s;
	while (std::getline(data, name, ',')) {
	    std::getline(data, id, ',');
	    std::getline(data, line_id, ',');
	    std::getline(data, address, ',');
	    std::getline(data, line_name);
	    s.address = address;
	    s.line_id = line_id;
	    s.line_name = line_name;
	    s.name = name;
	    this->stations_hashmap[std::strtoul(id.c_str(),NULL,10)] = s;
	}
    }
    //
    //protected method: read_connections
    //	@param _filename: nom du fichier des connections
    //	@returns nothing: construit l'unordered_map des connections
    void Derived_mapper::read_connections(const std::string& _filename){
	const std::string header("uint32_from_stop_id,uint32_to_stop_id,uint32_min_transfer_time");
	std::ifstream data(read_file(header,std::string("connections"),_filename));
	std::string start;
	std::string stop;
	std::string temps;
	while (std::getline(data, start, ',')) {
	    std::getline(data, stop, ',');
	    std::getline(data, temps);
	    this->connections_hashmap[std::strtoul(start.c_str(),NULL,10)][std::strtoul(stop.c_str(),NULL,10)] = std::strtoul(temps.c_str(),NULL,10);
	}
    }
    //
    //public method: compute_travel
    //	@param _start: id de la station de depart
    //	@param _end: id de la station d'arrivee
    //	@returns <vector<pair<uint64_t,uint64_t>>>: retourne le chemin le plus rapide calculé avec l'algorithme de Dijkstra 
    std::vector<std::pair<uint64_t, uint64_t> > Derived_mapper::compute_travel(uint64_t _start, uint64_t _end){
	std::vector<std::pair<uint64_t,uint64_t>> shortest_path;
	std::vector<std::pair<uint64_t, uint64_t>> open;
	std::vector<std::pair<uint64_t, uint64_t>> closed;
	std::unordered_map<uint64_t, uint64_t> parent_of;//map child ID to parent ID
	std::pair<uint64_t, uint64_t> curr_node(_start,0);
	std::pair<uint64_t, uint64_t> child_node;//child of current node
	open.push_back(curr_node);
	if(_start == _end){
	    return open;
	}
	while(!open.empty()){
	    curr_node = open.front();
	    closed.push_back(curr_node);
	    open.erase(open.begin());
	    if (curr_node.first == _end) {
		uint64_t parent_id(parent_of[curr_node.first]);
		shortest_path.insert(shortest_path.begin(), curr_node);
		//On parcourt closed à l'envers en ne gardant que les parents à partir du noeud d'arrivee jusqu'au noeud de depart 
		//On ajoute ces noeuds dans shortest_path pour creer le chemin
		for(std::vector<std::pair<uint64_t, uint64_t>>::const_reverse_iterator it(closed.rbegin()); it != closed.rend(); it++){
		    if(it->first == parent_id){
			child_node.first = it->first;
			child_node.second = it->second;
			parent_id = parent_of[it->first];
			shortest_path.insert(shortest_path.begin(), child_node);
		    }
		}
		break;
	    }
	    else{
		std::vector<uint64_t> child_nodes_id;//Le vecteur qui va contenir l'id des fils de la station courante (correspond aux connections a cette station)
		for (std::unordered_map<uint64_t, uint64_t>::const_iterator it(this->connections_hashmap[curr_node.first].begin());it != this->connections_hashmap[curr_node.first].end();it++){
		    child_nodes_id.push_back(it->first);
		}
		//On verifie si les stations connectees a la station courante ne sont pas deja visitees
		for(unsigned int i = 0; i < child_nodes_id.size(); i++){
		    bool in_closed(false);
		    child_node.first = child_nodes_id.at(i);
		    child_node.second = this->connections_hashmap[curr_node.first][child_nodes_id.at(i)] + curr_node.second;//Cout de developpement du noeud
		    for(std::vector<std::pair<uint64_t, uint64_t>>::const_iterator it(closed.begin()); it != closed.end(); it++){
			if(it->first == child_node.first){
			    in_closed = true;
			    break;
			}
		    }
		    //Si elle n'est pas visite on verifie si elle est dans open
		    if(!in_closed){
			bool in_open(false);
			for(std::vector<std::pair<uint64_t, uint64_t>>::const_iterator it(open.begin()); it != open.end(); it++){
			    if(it->first == child_node.first){
				in_open = true;
				break;
			    }
			}
			//Si elle n'est pas dans open, on la rajoute
			if(!in_open){
			    open.push_back(child_node);
			    parent_of[child_node.first] = curr_node.first;
			}
			//Sinon on cherche cette station dans open et on compare les couts de developpements des deux chemins pour cette station
			else{
			    for(std::vector<std::pair<uint64_t, uint64_t>>::iterator it(open.begin()); it != open.end(); it++){
				if(it->first == child_node.first){
				    //Si le cout de developpement de la station dans open est superieure a celle de
				    //la nouvelle connection: on met a jour le cout de developpement et on change 
				    //son pere
				    if(it->second > child_node.second){
					it->second = child_node.second;
					parent_of[it->first] = curr_node.first;
					break;
				    }
				}
			    }
			}
		    }
		}
	    }
	    std::sort(open.begin(), open.end(), sort_by_cost);//On range par ordre croissant du cout de developpement de chaque noeud
	}
	return shortest_path;
    }
    //
    //public method: compute_and_display_travel
    //	@param _start: id de la station de depart
    //	@param _end: id de la station d'arrivee
    //	@returns vector<pair<uint64_t, uint64_t>>>: retourne le chemin le plus rapide et l'affiche
    std::vector<std::pair<uint64_t, uint64_t> > Derived_mapper::compute_and_display_travel(uint64_t _start, uint64_t _end){
	std::vector<std::pair<uint64_t,uint64_t>>shortest_path(this->compute_travel(_start,_end));
	std::string curr_line_name(this->stations_hashmap[shortest_path.begin()->first].line_name);
	std::pair<uint64_t,uint64_t> same_line_station(shortest_path.at(0));//utilise pour garder la premiere station en memoire jusqu'a l'arrivee ou a un changement de ligne
	std::pair <uint64_t, uint64_t> curr_station(shortest_path.at(0));//utilise pour suivre la station actuelle
	std::cout<<std::endl;	
	std::cout<<"Le chemin le plus rapide de "<<this->stations_hashmap[_start].name<<" (ligne "<<this->stations_hashmap[_start].line_id<<") a ";
	std::cout<<this->stations_hashmap[_end].name<<" (ligne "<<this->stations_hashmap[_end].line_id<<") est:"<<std::endl;
	//Cette clause if permet de verifier que l'on ne va pas afficher simplement la ligne si elle est suivie d'un changement de direction (cout 0)
	//Si il y a un changement de direction, on affichera la ligne apres ce changement
	if(curr_line_name == this->stations_hashmap[(shortest_path.begin()+1)->first].line_name){
	    std::cout<<"\t<Prendre la ligne "<<this->stations_hashmap[same_line_station.first].line_id;
	    std::cout<<"> "<<curr_line_name<<std::endl;
	}
	for(std::vector<std::pair<uint64_t,uint64_t>>::const_iterator it(shortest_path.begin());it!=shortest_path.end();it++){
	    //Changement de ligne
	    if(curr_line_name != this->stations_hashmap[it->first].line_name){
		//On verifie qu'il y a eu un trajet sur la ligne avant le changement de ligne
		if(curr_station.second != same_line_station.second){
		    //Affiche le cout du trajet sur la ligne
		    std::cout<<"\t\tDe "<<this->stations_hashmap[same_line_station.first].name<<", a ";
		    std::cout<<this->stations_hashmap[curr_station.first].name<<" (";
		    sec_to_hhmmss(curr_station.second-same_line_station.second);
		    std::cout<<")"<<std::endl;
		}
		if(it->second != curr_station.second){
		    //On affiche le cout du changement
		    std::cout<<"\tMarcher jusqu'a "<<this->stations_hashmap[curr_station.first].name<<", ligne ";
		    std::cout<<this->stations_hashmap[it->first].line_id<<" (";
		    sec_to_hhmmss(it->second-curr_station.second);
		    std::cout<<")"<<std::endl;
		}
		curr_line_name = this->stations_hashmap[it->first].line_name;
		//Si on est pas arrive a destination, on affiche le changement de ligne
		if(this->stations_hashmap[it->first].name != this->stations_hashmap[_end].name){
		    if(this->stations_hashmap[it->first].line_name == this->stations_hashmap[(it+1)->first].line_name){
			std::cout<<"\t<Prendre la ligne "<<this->stations_hashmap[it->first].line_id<<"> ";
			std::cout<<curr_line_name<<std::endl;
		    }	
		}
		//On met a jour same_line_station avec la station de la nouvelle ligne
		same_line_station.first = it->first;
		same_line_station.second = it->second;
	    }
	    //On met a jour curr_station avec la station courante
	    curr_station.first = it->first;
	    curr_station.second = it->second;
	}
	if(curr_station.second != same_line_station.second){
	    std::cout<<"\t\tDe "<< this->stations_hashmap[same_line_station.first].name << " a ";
	    std::cout<<this->stations_hashmap[curr_station.first].name << " (";
	    sec_to_hhmmss(curr_station.second - same_line_station.second);
	    std::cout<<")"<<std::endl;
	}
	std::cout<<"Apres ";
	sec_to_hhmmss(curr_station.second);
	std::cout<<", vous etes arrive a destination !"<<std::endl;
	return shortest_path;
    }
    //
    //public method: compute_travel
    //	@param _start: nom de la station de depart
    //	@param _end: nom de la station d'arrivee
    //	@returns vector<pair<uint64_t,uint64_t>>: retourne le chemin le plus court entre _start et _end
    std::vector<std::pair<uint64_t, uint64_t> > Derived_mapper::compute_travel(const std::string& _start, const std::string& _end){
	std::vector<uint64_t> departure_stations;
	std::vector<uint64_t> arrival_stations;
	std::vector<std::pair<uint64_t, uint64_t> > shortest_path;
	uint64_t best_cost = 1000000;
	//On remplit les vecteurs start et arrival avec toutes les stations correspondant a _start, resp. _end
	for(std::unordered_map<uint64_t, Station>::const_iterator it(this->stations_hashmap.begin()); it != this->stations_hashmap.end(); it++){
	    if(compare_string(remove_accent(it->second.name), remove_accent(_start))){
		departure_stations.push_back(it->first);
	    }
	    if(compare_string(remove_accent(it->second.name), remove_accent(_end))){
		arrival_stations.push_back(it->first);
	    }
	}
	//On test tous les chemins pour les _start et _end present dans les vecteurs start et arrival
	//Et on conserve le plus rapide
	for (std::vector<uint64_t>::const_iterator it_depart(departure_stations.begin()); it_depart != departure_stations.end(); it_depart++) {
	    for (std::vector<uint64_t>::const_iterator it_arrivee(arrival_stations.begin()); it_arrivee != arrival_stations.end(); it_arrivee++) {
		std::vector<std::pair<uint64_t,uint64_t>>temp_path(this->compute_travel(*it_depart, *it_arrivee));
		if (temp_path.rbegin()->second < best_cost) {
		    best_cost = temp_path.rbegin()->second;
		    shortest_path = temp_path;
		}
	    }
	}
	return shortest_path;
    }
    //
    //public method: compute_and_display_travel
    //	@param _start: nom de la station de depart
    //	@param _end: nom de la station d'arrivee
    //	@returns vector<pair<uint64_t,uint64_t>>: retourne le chemin le plus court entre _start et _end et l'affiche dans la console
    std::vector<std::pair<uint64_t, uint64_t> > Derived_mapper::compute_and_display_travel(const std::string& _start, const std::string& _end) {
	std::vector<std::pair<uint64_t,uint64_t>>shortest_path(this->compute_travel(_start,_end));
	uint64_t departure_station(shortest_path.begin()->first);
	uint64_t arrival_station(shortest_path.rbegin()->first);
	shortest_path = compute_and_display_travel(departure_station, arrival_station);
	return shortest_path;
    }
    //
    //private method: best_station_match
    //	@param s: chaine de caractere
    //	@returns string: retourne le nom de la station qui ressemble le plus, au sens de Levenshtein,  a s
    std::string Derived_mapper::best_station_match(const std::string& s){
	unsigned int min_lvh_dist(40);
	std::string closest_str;
	for(std::unordered_map<uint64_t,travel::Station>::const_iterator it(this->stations_hashmap.begin());it != this->stations_hashmap.end();it++){
	    unsigned int temp_lvh_dist(levenshtein_distance(remove_accent(s),remove_accent(it->second.name)));
	    if(temp_lvh_dist < min_lvh_dist){
		min_lvh_dist = temp_lvh_dist;
		closest_str = it->second.name;
	    }
	}
	return closest_str;
    }
    //
    //public method: choose_station_id
    //	@param _type: arrivee ou depart
    //	@returns uint64_t: retourne l'id de la station choisie par l'utilisateur
    uint64_t Derived_mapper::choose_station_id(const std::string& _type){
	bool id_exist(false);
	std::cout<<"Saisir l'ID de la station de "<<_type<<" et appuyer sur <ENTREE>: ";
	uint64_t station_id = valid_int_input();
	//On verifie si l'id choisi par l'utilisateur correspond a une station
	for(std::unordered_map<uint64_t,Station>::const_iterator it(this->stations_hashmap.begin());it!=this->stations_hashmap.end();it++){
	    if(station_id == it->first){
		id_exist = true;
	    }
	}
	if(id_exist){
	    std::cout<<this->stations_hashmap[station_id]<<std::endl;
	    return station_id;
	}
	std::string user_answer;
	bool valid_input(false);
	//On propose a l'utilisateur d'entrer un id correct ou de quitter le programme
	while(!valid_input){
	    std::cout<<"Aucune station associee à cet ID (" << station_id << ") !"<<std::endl<<"Voulez vous reessayer? (y/n): ";	
	    std::cin>>user_answer;
	    if(compare_string(user_answer,"n")){
		quit_program();
	    }
	    else if(compare_string(user_answer,"y")){
		valid_input = true;
	    }
	    else{
		clear_cin();
	    }
	}
	return this->choose_station_id(_type);
    }

    //
    //public method: choose_station_name
    //	@param _type: arrivee ou depart
    //	@returns string: retourne le nom de la station choisie par l'utilisateur
    std::string Derived_mapper::choose_station_name(const std::string& _type){
	std::string station_name;
	std::cout<<"Saisir la station de " << _type << " et appuyer sur <ENTREE>: ";
	std::getline(std::cin>>std::ws,station_name);
	bool exist(false);
	//On verifie si le nom de la station choisie par l'utilisateur existe
	//Insensible a la casse et aux accents
	for (std::unordered_map<uint64_t, Station>::const_iterator it(this->stations_hashmap.begin()); it != this->stations_hashmap.end(); it++) {
	    if (compare_string(remove_accent(it->second.name), remove_accent(station_name))){
		exist = true;
	    }
	}
	if(exist){
		return station_name;
	}
	//Si la station n'existe pas
	std::string user_answer;	
	std::string closest_str(this->best_station_match(station_name));
	std::cout << "La station " << station_name << " est introuvable"<<std::endl;
	bool valid_answer(false);
	do{
	    std::cout<<"Vouliez-vous dire "<<closest_str<<"? (y/n): ";//On propose la station avec le nom qui ressemble le plus a l'entree de l'utilisateur
	    std::cin>>user_answer;
		if(compare_string(user_answer,"y")){	
		    return closest_str;
		}
		else if(compare_string(user_answer,"n")){//Si la station avec le nom le plus proche ne satisfait pas l'utilisateur
		    do{
			//On propose a l'utilisateur de saisir a nouveau une station
			std::cout<<"Voulez-vous reessayer? (y/n): ";
			std::cin>>user_answer;
			if(compare_string(user_answer,"y")){
			    valid_answer = true;
			}
			else if(compare_string(user_answer,"n")){
			    quit_program();//S'il ne veut pas, on quitte le programme
			}
			else{
			    clear_cin();
			}
		    }while(!valid_answer);	
		}
		else{
			clear_cin();
		}
	}while(!valid_answer);
	return this->choose_station_name(_type);
    }
}
