#include "Derived_mapper.hpp"
int main(int argc, char** argv){
    if(argc < 3){
	std::cerr << "Not enough arguments" << std::endl;
	return 1;
    }
    std::string data_stations(argv[1]);
    std::string data_connections(argv[2]);
    travel::Derived_mapper ratp(data_stations,data_connections);
    int user_choice;
    std::vector<std::pair<uint64_t,uint64_t> > shortest_path;
    std::cout<<"Bienvenue dans RATP itineraire !"<<std::endl;
    while(true){
	std::cout<<std::endl;
	std::cout<<"<Nouvelle recherche>"<<std::endl;
	std::cout<<"[1]Recherche d'itineraire par ID"<<std::endl;
	std::cout<<"[2]Recherche d'itineraire par NOM"<<std::endl;
	std::cout<<"[3]Quitter le programme"<<std::endl;
	std::cout<<"Saisir votre choix (1, 2 ou 3) et appuyer sur <ENTREE>: ";
	user_choice = valid_int_input();
	if(user_choice == 1){
	    uint64_t start(ratp.choose_station_id(std::string("depart")));
	    uint64_t end(ratp.choose_station_id(std::string("arrivee")));
	    shortest_path = ratp.compute_and_display_travel(start,end);
	}
	else if(user_choice == 2){
	    std::string start(ratp.choose_station_name(std::string("depart")));
	    std::string end(ratp.choose_station_name(std::string("arrivee")));
	    shortest_path= ratp.compute_and_display_travel(start,end);
	}
	else if(user_choice == 3){
	    quit_program();	
	}
	else{
	    std::cout<<std::endl<<"Désolé je n'ai pas compris !";
	}
    }
    return 0;
}
