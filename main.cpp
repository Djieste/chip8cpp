// Tutorial: https://austinmorlan.com/posts/chip8_emulator/
// Test ROMs: https://github.com/dmatlack/chip8/tree/master/roms/games

#include <iostream>
#include <string>
#include <clocale>
#include <fstream>
#include <stack>

using namespace std;

class Processeur
{
public:
	using octet = uint8_t;
	using addresse = uint16_t;
public:
	const unsigned int ADRESSE_DEBUT = 0x200;
public:
	const static int V0 = 0;
	const static int V1 = 1;
	const static int V2 = 2;
	const static int V3 = 3;
	const static int V4 = 4;
	const static int V5 = 5;
	const static int V6 = 6;
	const static int V7 = 7;
	const static int V8 = 8;
	const static int V9 = 9;
	const static int VA = 10;
	const static int VB = 11;
	const static int VC = 12;
	const static int VD = 13;
	const static int VE = 14;
	const static int VF = 15;
	octet registres[16] { 0 };
	octet memoire[4096] { 0 };
	addresse compteurProgramme{ ADRESSE_DEBUT };
	addresse registreIndex{ 0 };
	stack<addresse> pile;
	octet pointeurDePile;

public:
	bool executer(const string& fichierNomRom)
	{
		if (!charger(fichierNomRom))
			return false;
	}
		
	bool charger(const string& fichierNomRom)
	{
		std::ifstream fichier(fichierNomRom, std::ios::binary | std::ios::ate);
		if (!fichier.is_open())
			return false;

		// Get size of file and allocate a buffer to hold the contents
		std::streampos taille = fichier.tellg();

		// Go back to the beginning of the file and fill the buffer
		fichier.seekg(0, std::ios::beg);
		fichier.read((char*)memoire + ADRESSE_DEBUT, taille);
		fichier.close();
		return true;
	}
};

int main(int argc, char * argv[])
{
	setlocale(LC_ALL, "");

	if (argc != 2)
	{
		cout << "chip8cpp v1.0" << endl;
		cout << "SVP spécifier le fichier de ROM à lancer." << endl;
		cout << argv[0] << " FICHIER_ROM" << endl;
		return -1;
	}
	else
	{
		Processeur processeur;
		processeur.executer(argv[1]);
		return 0;
	}
}

