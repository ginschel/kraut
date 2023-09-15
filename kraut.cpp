#include <fstream>
#include <string>
#include <string.h>
#include <map>
#include <iostream>
std::string reg(int c) {
	std::string vorgabe = "[ebp-";
	vorgabe+=std::to_string(c*4);
	vorgabe+="]";
	return vorgabe;
}
int main(int argc, char *argv[]) {
//Variableninitiallisierung
int varzaehler =0;int strzaehler=0;
std::string var, varwert;
bool gleichgefunden = 0; bool stringwert = 0; bool programmsektion = 0; bool asmblr = 0; bool anfuehrungsz =0;
std::map<std::string, int> vartabelle;
std::map<std::string, std::string> stringtabelle;
std::string ifi = argv[1], ofi = argv[2];
std::ifstream quelle(ifi); std::ofstream ausgabe(ofi);
ausgabe << "global _start\n_start:\nmov ebp,esp\n";
std::cout << "starte compiler\n";
//Hauptschleife
while(quelle.good()) {
std::string zeile; std::getline(quelle, zeile);
//std::cout << zeile << std::endl;
if(zeile == "Programmsektion") programmsektion = true;
if(!programmsektion) {
std::string s; for (int i = 0; i < zeile.length(); ++i) {
s+= zeile[i];
if(s == "Definiere") { for(int r = i+2; r < zeile.length(); ++r) {
if(!gleichgefunden) {
if (zeile[r] != '=' && zeile[r] != '.' && zeile[r] != ';' && zeile[r] != ',') var+=zeile[r];
else { gleichgefunden = true;
//falls ich mit nem Stringwert arbeiten will
//if(zeile[r+1] == '"') stringwert = true;
}
}
else {
if (!stringwert) {
if(zeile[r] != '=' && zeile[r] != '.' && zeile[r] != ';' && zeile[r] != ',') varwert += zeile[r];
}
}
}
}
}
//wieder in der Deklarationsschleife
//std::cout << var << std::endl;
//std::cout << varwert << std::endl;
if(varwert[0] != '"') {
std::cout << varwert[0] << std::endl;
vartabelle[var] = ++varzaehler;
ausgabe << "push "<< varwert<<"\n";
}
//prüft, ob der Wert ein String ist
else {
ausgabe << "section .data\n"<<var<<" db "<<varwert<<"\nsection .text\n";
varwert.pop_back(); varwert.erase(varwert.begin());
stringtabelle[var] = varwert;
}
//RESET
gleichgefunden = false;
var = ""; varwert="";
}
//Programmsektion
else {
//Schutz vor zu kurzen Zeilen...
if(!zeile.empty()); {
//Labels
if(*zeile.rbegin() == ':') {
ausgabe << zeile << "\n";
continue;
}
}
if(zeile == "Assemblerstart") {
asmblr = true;
continue;
}
if(zeile == "Assemblerende") {
asmblr = false;
continue;
}
if(asmblr) {
ausgabe << zeile << "\n";
}
else {
std::string s; for (int i = 0; i < zeile.length(); ++i) {
//Variablenmanipulation
if(vartabelle.count(s) && zeile[i] == '=') {
	std::string zahl;
	bool zuweisung = 0, plus = 0, minus = 0, mal = 0, geteilt = 0;
	for(int r = i+1; r < zeile.length(); ++r) {
		if(zeile[r] != '.' && (zeile[r] != '+'|| zeile[r-1] == '=') && zeile[r] != '/' && zeile[r] != '*' && (zeile[r] != '-' || zeile[r-1] == '=')) zahl+=zeile[r];
		else {
			if(vartabelle.count(zahl)){
				zahl = reg(vartabelle[zahl]);
			}
			if(zuweisung == 0) {
				ausgabe << "mov eax"<<","<<zahl<<"\n";
				zahl = "";
				zuweisung = 1;
			}
			else {


			//wenn flagge gesetzt ist
			if(plus) {
					ausgabe << "add eax"<<","<<zahl<<"\n";
					zahl = ""; plus = 0;
				}
			if(minus) {
					ausgabe << "sub eax"<<","<<zahl<<"\n";
					zahl = ""; minus = 0;
				}
			if(mal) {
					ausgabe << "imul eax,"<<zahl<<"\n";
					zahl = ""; mal = 0;
				}
			if(geteilt) {
					ausgabe << "mov ecx,"<<zahl<<"\n";
					ausgabe << "div dword "<<"ecx"<<"\n";
					zahl = ""; geteilt = 0;
				}
		}
			switch(zeile[r]) {
				case '+':
					plus = 1;
					break;
				case '-':
					minus = 1;
					break;
				case '*':
					mal = 1;
					break;
				case '/':
					geteilt = 1;
					break;
				default:
					ausgabe << "mov "<<reg(vartabelle[s])<<",eax\n"; //gibt der Variable ihren neuen Wert aus dem Akkumulator
					zahl = "";
					i = zeile.length();
					r = i;
			}






	}

}
}
//Hier ist der Befehlsscanner s
s+= zeile[i];
//goto
if(s == "Springe in") {
std::string sprungmarke;
for(int r = i+2; r < zeile.length(); ++r) {
if (zeile[r] != '.') sprungmarke+=zeile[r];
else ausgabe << "jmp " << sprungmarke << "\n";
}
}
//print
if(s == "Drucke") {s="";
std::string stringname ="";
for(int r = i+2; r < zeile.length(); ++r) {
if(zeile[r] == '"') {
if(anfuehrungsz) {
anfuehrungsz = 0;
}
else { anfuehrungsz = true; continue;
}
}
if (anfuehrungsz) s+=zeile[r];
else stringname+=zeile[r];
if(zeile[r] ==',') stringname="";
if (stringtabelle.count(stringname)) s+=stringtabelle[stringname];
if (zeile[r] == '.' && !anfuehrungsz){ ++strzaehler; ausgabe << "section .data\nmsg" << strzaehler<<" db "<< '"' << s << '"' << ",0x0a\nlen" << strzaehler<<" equ $- msg"<<strzaehler<<"\nsection .text\nmov eax,4\nmov ebx,1\nmov ecx, msg"<<strzaehler<<"\nmov edx,len"<<strzaehler<<"\nint 0x80\n";
}
//epilog
}
}
else if(s == "Setze Fehlernummer") {s="";
std::string zahl ="";
for(int r = i+2; r < zeile.length(); ++r) {
if(zeile[r] != '.') {
zahl+=zeile[r];
}
else {
if (vartabelle.count(zahl)) zahl = reg(vartabelle[zahl]);
ausgabe << "mov eax,1\nmov ebx,"<<zahl<<"\nint 0x80\n";
}

}

}
//Fehlernummer
}

}
}
}
//Ende der Hauptschleife
std::cout << "x hat den Wert " << vartabelle["x"] << std::endl;
std::string const kompilier = std::string("nasm -f elf32 ") + argv[2] + " -o programm.o";
std::cout << kompilier.c_str() << std::endl;
//system(kompilier.c_str());
//system("nasm -f elf32 kek -o programm.o");
//system("nasm -f elf32 kek -o programm.o");
//system("ld -m elf_i386 programm.o -o programm; ./programm; echo $?");
return 0;
}
