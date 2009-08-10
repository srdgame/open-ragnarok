/* $Id$ */
#include "stdafx.h"

#include "ro/constants.h"

namespace RO {

	namespace EUC {
		char user_interface[] =  "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba";

		// BODY PARTS
		char body[] = "\xb8\xf6\xc5\xeb";
		char head[] = "\xb8\xd3\xb8\xae\xc5\xeb";
		char weapons[] = "\xb8\xf9\xc5\xa9";

		//CHARACTER TYPE
		char humans[] = "\xc0\xce\xb0\xa3\xc1\xb7"; // Human family
		char monsters[] = "\xb8\xf3\xbd\xba\xc5\xcd";

		char water[] = "\xbf\xf6\xc5\xcd";

		// === SEX
		char female[] = "\xbf\xa9";
		char male[] = "\xb3\xb2";
		char *sex[] = {
			male, female
		};

		// classes [kR105]
		char novice[] = "\xC3\xCA\xBA\xB8\xC0\xDA";
		char swordsman[] = "\xB0\xCB\xBB\xE7";
		char magician[] = "\xB8\xB6\xB9\xFD\xBB\xE7";
		char archer[] = "\xB1\xC3\xBC\xF6";
		char acolyte[] = "\xBC\xBA\xC1\xF7\x9F\xE0";
		char merchant[] = "\xBB\xF3\xC0\xCE";
		char thief[] = "\xB5\xB5\xB5\xCF";
		char knight[] = "\xB1\xE2\xBB\xE7";
		char priest[] = "\xC7\xC1\xB8\xAE\xBD\xBA\xC6\xAE";
		char wizard[] = "\xC0\xA7\xC0\xFA\xB5\xE5";
		char blacksmith[] = "\xC1\xA6\xC3\xB6";
		char hunter[] = "\xC7\xE5\xC5\xCD";
		char assassin[] = "\xBE\xEE\xBC\xBC\xBD\xC5";
		char knight_mounted[] = "";
		char crusader[] = "\xC5\xA9\xB7\xE7\xBC\xBC\xC0\xCC\xB4\xF5";
		char monk[] = "\xB8\xF9\xC5\xA9";
		char sage[] = "\xBC\xBC\xC0\xCC\xC1\xF6";
		char rogue[] = "\xB7\xCE\xB1\xD7";
		char alchemist[] = "\xBF\xAC\xB1\xDD\xBC\xFA\xBB\xE7";
		char bard[] = "\xB9\xD9\xB5\xE5";
		char dancer[] = "\xB9\xAB\xC8\xF1";
		char crusader_mounted[] = "";
		char gm[] = "\x9D\xC4\xBF\xB5\xC0\xDA";
		char mercenary[] = "";

		char *classname[] = {
			novice,
			swordsman,
			magician,
			archer,
			acolyte,
			merchant,
			thief,
			knight,
			priest,
			wizard,
			blacksmith,
			hunter,
			assassin,
			knight_mounted,
			crusader,
			monk,
			sage,
			rogue,
			alchemist,
			bard,
			dancer,
			crusader_mounted,
			gm,
			mercenary,
		};

		char *classname_en[] = {
			"Novice",
			"Swordsman",
			"Magician",
			"Archer",
			"Acolyte",
			"Merchant",
			"Thief",
			"Knight",
			"Priest",
			"Wizard",
			"Blacksmith",
			"Hunter",
			"Assassin",
			"Knight", //Mounted
			"Crusader",
			"Monk",
			"Sage",
			"Rogue",
			"Alchemist",
			"Bard",
			"Dancer",
			"Crusader", //Mounted
			"GM",
			"Mercenary",
		};
	}
}

