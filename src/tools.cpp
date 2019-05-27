#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits>

#include "tools.hpp"
//
//function: sort_by_cost
//	@param pair_1: pair de uint64_t
//	@param pair_2: pair de uint64_t
//	@returns boolean: retourne vrai si le deuxieme membre de p2 est superieur 
//		 au deuxieme membre de p1
bool sort_by_cost(const std::pair<uint64_t, uint64_t>& p1, const std::pair<uint64_t, uint64_t>&p2){
    return (p1.second < p2.second);
}
//
//function: compare_char
//	@param c1: caractere 
//	@param c2: caractere
//	@returns boolean: retourne vrai c1 et c2 sont egaux
//                ou si c1 et c2 en minuscule sont egaux
bool compare_char(const char& c1, const char& c2){
    if(c1 == c2){
        return true;
    }
    else if(std::tolower(c1) == std::tolower(c2)){
        return true;
    }
    return false;
}
//
//function: compare_string
//	@param s1: chaine de caracteres
//	@param s2: chaine de caracteres
//	@returns boolean: retourne vrai si s1 et s2 sont egales au sens de compare_char
//		 (insensible a la casse)
bool compare_string(const std::string& s1, const std::string& s2){
    return((s1.size() == s2.size()) && std::equal(s1.begin(),s1.end(), s2.begin(), &compare_char));
}
//
//function: remove_accent
//	@param s1: chaine de caracteres
//	@returns string: une chaine de caracteres sans accentuation
std::string remove_accent(const std::string& s1){
    std::string s2;
    std::unordered_map<std::string,char> dictionnary;//unordered_map associant les lettres accentuees a leur lettre non accentuee respective
    dictionnary["à"] = 'a';
    dictionnary["ä"] = 'a';
    dictionnary["â"] = 'a';
    dictionnary["é"] = 'e';
    dictionnary["è"] = 'e';
    dictionnary["ê"] = 'e';
    dictionnary["ë"] = 'e';
    dictionnary["î"] = 'i';
    dictionnary["ï"] = 'i';
    dictionnary["ô"] = 'o';
    dictionnary["ö"] = 'o';
    dictionnary["ù"] = 'u';
    dictionnary["ü"] = 'u';
    dictionnary["û"] = 'u';
    for(std::string::const_iterator it_string(s1.begin());it_string != s1.end();it_string++){
        bool in_dictionnary(false);
	for(std::unordered_map<std::string,char>::const_iterator it_dict(dictionnary.begin());it_dict!=dictionnary.end();it_dict++){
	    //On cherche si il y a des caracteres accentues dans s1
	    //un caractere accentue est sur 2 bytes de donnees (donc sur deux caracteres)
	    //On compare donc le caractere actuel et le suivant avec les cles de dictionnary
	    if(it_dict->first[0] == *it_string && it_dict->first[1] == *(it_string+1)){
	        s2.push_back(dictionnary[it_dict->first]);
                it_string++;
                in_dictionnary = true;
	    }
        }	
        //Si le caractere n'est pas dans le dictionnaire, c'est un caractere non accentue
        if(!in_dictionnary){
            s2.push_back(*it_string);
        }
    }
    return s2;
}
//
//function: levenshtein_distance
//	@param s1: chaine de caracteres 
//	@param s2: chaine de caracteres
//	@returns uint: le nombre minimal d'operations necessaires,
//	          au sens de levenshtein, pour passer de s1 a s2
unsigned int levenshtein_distance(const std::string& s1, const std::string& s2){
    unsigned int m(s1.size()+1);
    unsigned int n(s2.size()+1);
    if(m==0) return n;
    if(n==0) return m;
    int substitution_cost = 0;
    std::vector<unsigned int> matrix;
    for(unsigned int i = 0; i < m;i++){
        for(unsigned int j = 0; j < n;j++){
            matrix.push_back(0);
        }
    }
    for(unsigned int i = 0; i < m;i++){
        matrix[i*n] = i;
    }
    for(unsigned int j = 0; j < n; j++){
        matrix[j] = j;
    }
    for(unsigned int j = 1; j < n; j++){
        for(unsigned int i = 1; i < m; i++){
            if(s1[i-1]==s2[j-1]){
                substitution_cost = 0;
            }
            else{
                substitution_cost = 1;
            }
            matrix[i*n+j] = std::min(std::min(matrix[(i-1)*n+j]+1,matrix[i*n+j-1]+1),matrix[(i-1)*n+j-1]+substitution_cost);
        }
    }
    return matrix[(m-1)*n+n-1];
}
//
//function: sec_to_hhmmss
//	@param temps_sec: le temps en secondes
//	@returns nothing: affiche le temps au format heures:minutes:secondes (hh:mm:ss)
void sec_to_hhmmss(const uint64_t temps_sec){
    if (temps_sec < 60){
        std::cout<<temps_sec<<"s";
    }
    else if(temps_sec < 3600){
        if(temps_sec % 60 == 0){
            std::cout << temps_sec / 60 << "mn";
            std::cout << "00s";
        }
        else{
            std::cout << temps_sec / 60 << "mn";
            if(temps_sec % 60 < 10){
                std::cout << "0" << temps_sec % 60 << "s";
            }
            else {
                std::cout << temps_sec % 60 << "s";
            }
        }
    }
    else{
        if(temps_sec % 3600 == 0){
            std::cout << temps_sec / 3600 << "h";
            std::cout << "00mn";
            std::cout << "00s";
        }
        else{
            std::cout << temps_sec / 3600 << "h";
            int mmss = temps_sec%3600;//minutes:secondes
            if(mmss % 60 == 0){
                if (mmss / 60 < 10) {
                    std::cout << "0" << mmss / 60 << "mn";
                    }
                else{
                    std::cout << mmss / 60 << "mn";
                }
                std::cout << "00s";
            }
            else{
                if(mmss / 60 < 10){
                    std::cout << "0" << mmss / 60 << "mn";
                }
                else{
                    std::cout << mmss / 60 << "mn";
                }
                if(mmss % 60 < 10){
                    std::cout << "0" << mmss % 60 << "s";
                }
                else{
                    std::cout << mmss % 60 << "s";
                }
            }
        }
    }
}
//
//function: quit_program
//returns nothing: affiche un message de remerciement et quitte le programme
void quit_program(){
    std::cout<<std::endl<<"Merci d'avoir utiliser RATP itineraire. A bientot !"<<std::endl;
    exit(EXIT_SUCCESS);
}
//
//function: read_file
//	@param header: chaine de caracteres representant l'entete attendue du fichier que l'on va lire
//	@param type: chaine de caracteres representant quel type de fichier ou ouvre (stations ou connections par exemple)
//	@param filename: chaine de caracteres representant le nom du fichier
//	@returns input file: le stream de donnee
std::ifstream read_file(const std::string& _header, const std::string& _type, const std::string& _filename){
    std::ifstream data(_filename);
    //On verifie l'ouverture du fichier
    if(!data){
        std::cerr<<"Error opening " << _type << " file"<<std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string s;
    std::getline(data, s); //Recupere le fichier d'entete (header)
    if(s != _header){
        std::cerr<<"This is not the expected " << _type << " file"<<std::endl;
        std::exit(EXIT_FAILURE);
    }
    return data;
}
//
//function: clear_cin
//	@returns nothing: permet de reutiliser cin apres une mauvaise utilisation de l'utilisateur
void clear_cin(){
    std::cin.clear();//clear l'erreur sur cin pour que l'on puisse le reutiliser
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');//skip le nombre max de caracteres jusqu'à la prochaine ligne pour eviter une parse failure
}
//
//function: valid_int_input
//	@returns uint64_t: cette fonction permet de forcer l'utilisateur a fournir une entree de type entier valide
uint64_t valid_int_input(){
    uint64_t my_int;
    bool valid_input(false);
    while(!valid_input){
        std::cin>>my_int;
        if(std::cin.fail()){
            std::cout<<"/!\\ L'entree n'est pas valide /!\\"<<std::endl;
            std::cout<<"/!\\ Vous devez saisir un entier /!\\"<<std::endl;
            std::cout<<"Veuillez reessayer: ";
        }
        else{
            valid_input = true;
        }
        clear_cin();
    }
    return my_int;
}
