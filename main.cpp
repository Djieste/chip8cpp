// Tutorial: https://austinmorlan.com/posts/chip8_emulator/
// Test ROMs: https://github.com/dmatlack/chip8/tree/master/roms/games

#include <iostream>
#include <string>
#include <clocale>
#include <fstream>
#include <stack>
#include <unordered_map>

using namespace std;

class Processeur
{
public:
	using octet = uint8_t;
	using addresse = uint16_t;
	using instruction = uint16_t;
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

	unordered_map< uint16_t, void(Processeur::*)(instruction op) > instructions
	{
		{ 0x00E0u, &Processeur::OP_00E0 },
		{ 0x00EEu, &Processeur::OP_00EE },
		{ 0x1000u, &Processeur::OP_1nnn },
		{ 0x2000u, &Processeur::OP_2nnn },
		{ 0x3000u, &Processeur::OP_3xkk },
		{ 0x4000u, &Processeur::OP_4xkk },
		{ 0x5000u, &Processeur::OP_5xy0 },
		{ 0x6000u, &Processeur::OP_6xkk },
		{ 0x7000u, &Processeur::OP_7xkk },
		{ 0x8000u, &Processeur::OP_8xy0 },
		{ 0x8001u, &Processeur::OP_8xy1 },
		{ 0x8002u, &Processeur::OP_8xy2 },
		{ 0x8003u, &Processeur::OP_8xy3 },
		{ 0x8004u, &Processeur::OP_8xy4 },
		{ 0x8005u, &Processeur::OP_8xy5 },
		{ 0x8006u, &Processeur::OP_8xy6 },
		{ 0x8007u, &Processeur::OP_8xy7 },
		{ 0x800Eu, &Processeur::OP_8xyE },
		{ 0x9000u, &Processeur::OP_9xy0 },
		{ 0xA000u, &Processeur::OP_Annn },
		{ 0xB000u, &Processeur::OP_Bnnn },
		{ 0xC000u, &Processeur::OP_Cxkk },
		{ 0xD000u, &Processeur::OP_Dxyn },
		{ 0xE09Eu, &Processeur::OP_Ex9E },
		{ 0xE0A1u, &Processeur::OP_ExA1 },
		{ 0xF007u, &Processeur::OP_Fx07 },
		{ 0xF00Au, &Processeur::OP_Fx0A },
		{ 0xF015u, &Processeur::OP_Fx15 },
		{ 0xF018u, &Processeur::OP_Fx18 },
		{ 0xF01Eu, &Processeur::OP_Fx1E },
		{ 0xF029u, &Processeur::OP_Fx29 },
		{ 0xF033u, &Processeur::OP_Fx33 },
		{ 0xF055u, &Processeur::OP_Fx55 },
		{ 0xF065u, &Processeur::OP_Fx65 }
	};

public:

	void OP_00E0(instruction op)
	{
		memset(video, 0, sizeof(video));
	}

	void OP_00EE(instruction op)
	{
		pile.pop();
		compteurProgramme = pile.top();
	}

	void OP_1nnn(instruction op)
	{
		uint16_t address = op & 0x0FFFu;

		compteurProgramme = address;
	}

	void OP_2nnn(instruction op)
	{
		uint16_t address = op & 0x0FFFu;

		pile.push(compteurProgramme);
		compteurProgramme = address;
	}

	void OP_3xkk(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t byte = op & 0x00FFu;

		if (registres[Vx] == byte)
		{
			compteurProgramme += 2;
		}
	}

	void OP_4xkk(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t byte = op & 0x00FFu;

		if (registres[Vx] != byte)
		{
			compteurProgramme += 2;
		}
	}

	void OP_5xy0(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		if (registres[Vx] == registres[Vy])
		{
			compteurProgramme += 2;
		}
	}

	void OP_6xkk(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t byte = op & 0x00FFu;

		registres[Vx] = byte;
	}

	void OP_7xkk(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t byte = op & 0x00FFu;

		registres[Vx] += byte;
	}

	void OP_8xy0(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		registres[Vx] = registres[Vy];
	}

	void OP_8xy1(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		registres[Vx] |= registres[Vy];
	}

	void OP_8xy2(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		registres[Vx] &= registres[Vy];
	}

	void OP_8xy3(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		registres[Vx] ^= registres[Vy];
	}

	void OP_8xy4(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		uint16_t sum = registres[Vx] + registres[Vy];

		if (sum > 255U)
		{
			registres[0xF] = 1;
		}
		else
		{
			registres[0xF] = 0;
		}

		registres[Vx] = sum & 0xFFu;
	}

	void OP_8xy5(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		if (registres[Vx] > registres[Vy])
		{
			registres[0xF] = 1;
		}
		else
		{
			registres[0xF] = 0;
		}

		registres[Vx] -= registres[Vy];
	}

	void OP_8xy6(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		// Save LSB in VF
		registres[0xF] = (registres[Vx] & 0x1u);

		registres[Vx] >>= 1;
	}

	void OP_8xy7(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		if (registres[Vy] > registres[Vx])
		{
			registres[0xF] = 1;
		}
		else
		{
			registres[0xF] = 0;
		}

		registres[Vx] = registres[Vy] - registres[Vx];
	}

	void OP_8xyE(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		// Save MSB in VF
		registres[0xF] = (registres[Vx] & 0x80u) >> 7u;

		registres[Vx] <<= 1;
	}

	void OP_9xy0(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;

		if (registres[Vx] != registres[Vy])
		{
			compteurProgramme += 2;
		}
	}

	void OP_Annn(instruction op)
	{
		uint16_t address = op & 0x0FFFu;

		registreIndex = address;
	}

	void OP_Bnnn(instruction op)
	{
		uint16_t address = op & 0x0FFFu;

		compteurProgramme = registres[0] + address;
	}

	void OP_Cxkk(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t byte = op & 0x00FFu;

		registres[Vx] = randByte(randGen) & byte;
	}

	void OP_Dxyn(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t Vy = (op & 0x00F0u) >> 4u;
		uint8_t height = op & 0x000Fu;

		// Wrap if going beyond screen boundaries
		uint8_t xPos = registres[Vx] % VIDEO_WIDTH;
		uint8_t yPos = registres[Vy] % VIDEO_HEIGHT;

		registres[0xF] = 0;

		for (unsigned int row = 0; row < height; ++row)
		{
			uint8_t pointeurDePileriteByte = memoire[registreIndex + row];

			for (unsigned int col = 0; col < 8; ++col)
			{
				uint8_t pointeurDePileritePixel = pointeurDePileriteByte & (0x80u >> col);
				uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

				// Sprite pixel is on
				if (pointeurDePileritePixel)
				{
					// Screen pixel also on - collision
					if (*screenPixel == 0xFFFFFFFF)
					{
						registres[0xF] = 1;
					}

					// Effectively XOR with the pointeurDePilerite pixel
					*screenPixel ^= 0xFFFFFFFF;
				}
			}
		}
	}

	void OP_Ex9E(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		uint8_t key = registres[Vx];

		if (keypad[key])
		{
			compteurProgramme += 2;
		}
	}

	void OP_ExA1(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		uint8_t key = registres[Vx];

		if (!keypad[key])
		{
			compteurProgramme += 2;
		}
	}

	void OP_Fx07(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		registres[Vx] = delayTimer;
	}

	void OP_Fx0A(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		if (keypad[0])
		{
			registres[Vx] = 0;
		}
		else if (keypad[1])
		{
			registres[Vx] = 1;
		}
		else if (keypad[2])
		{
			registres[Vx] = 2;
		}
		else if (keypad[3])
		{
			registres[Vx] = 3;
		}
		else if (keypad[4])
		{
			registres[Vx] = 4;
		}
		else if (keypad[5])
		{
			registres[Vx] = 5;
		}
		else if (keypad[6])
		{
			registres[Vx] = 6;
		}
		else if (keypad[7])
		{
			registres[Vx] = 7;
		}
		else if (keypad[8])
		{
			registres[Vx] = 8;
		}
		else if (keypad[9])
		{
			registres[Vx] = 9;
		}
		else if (keypad[10])
		{
			registres[Vx] = 10;
		}
		else if (keypad[11])
		{
			registres[Vx] = 11;
		}
		else if (keypad[12])
		{
			registres[Vx] = 12;
		}
		else if (keypad[13])
		{
			registres[Vx] = 13;
		}
		else if (keypad[14])
		{
			registres[Vx] = 14;
		}
		else if (keypad[15])
		{
			registres[Vx] = 15;
		}
		else
		{
			compteurProgramme -= 2;
		}
	}

	void OP_Fx15(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		delayTimer = registres[Vx];
	}

	void OP_Fx18(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		soundTimer = registres[Vx];
	}

	void OP_Fx1E(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		registreIndex += registres[Vx];
	}

	void OP_Fx29(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t digit = registres[Vx];

		registreIndex = FONTSET_START_ADDRESS + (5 * digit);
	}

	void OP_Fx33(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;
		uint8_t value = registres[Vx];

		// Ones-place
		memoire[registreIndex + 2] = value % 10;
		value /= 10;

		// Tens-place
		memoire[registreIndex + 1] = value % 10;
		value /= 10;

		// Hundreds-place
		memoire[registreIndex] = value % 10;
	}

	void OP_Fx55(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		for (uint8_t i = 0; i <= Vx; ++i)
		{
			memoire[registreIndex + i] = registres[i];
		}
	}

	void OP_Fx65(instruction op)
	{
		uint8_t Vx = (op & 0x0F00u) >> 8u;

		for (uint8_t i = 0; i <= Vx; ++i)
		{
			registres[i] = memoire[registreIndex + i];
		}
	}
	
	void faireUnCycle()
	{
		// Fetch
		instruction opcode = (memoire[compteurProgramme] << 8u) | memoire[compteurProgramme + 1];

		// Increment the PC before we execute anything
		compteurProgramme += 2;

		// Decode and Execute
		((*this).*(instructions[(opcode & 0xF000u) >> 12u]))();

		// Decrement the delay timer if it's been set
		if (delayTimer > 0)
		{
			--delayTimer;
		}

		// Decrement the sound timer if it's been set
		if (soundTimer > 0)
		{
			--soundTimer;
		}
	}

	bool executer(const string& fichierNomRom)
	{
		if (!charger(fichierNomRom))
			return false;
		while (true)
		{
			faireUnCycle();
		}
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

