#include <fstream>
#include <string>
#include <string.h>
#include <map>
#include <iostream>
int main(int argc, char *argv[]) {
//Variableninitiallisierung
int varzaehler =0;int strzaehler=0;
std::string var, varwert;
bool gleichgefunden = 0; bool stringwert = 0; bool programmsektion = 0; bool asmblr = 0; bool anfuehrungsz =0;
std::map<std::string, int> vartabelle;
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
//wieder im Deklarationsschleife
//std::cout << var << std::endl;
//std::cout << varwert << std::endl;
vartabelle[var] = ++varzaehler;
ausgabe << "push "<< varwert<<"\n";
gleichgefunden = false;
var = ""; varwert="";
}
//Programmsektion
else {
if(zeile == "Assemblerstart") {
asmblr = true;
continue;
}
if(zeile == "Assemblerende") {
asmblr = false;
std::cout << "keck\n";
continue;
}
if(asmblr) {
std::cout << "Zeile " << zeile << std::endl;
ausgabe << zeile << "\n";
}
else {
std::string s; for (int i = 0; i < zeile.length(); ++i) {
s+= zeile[i];
if(s == "Drucke") {s="";
for(int r = i+2; r < zeile.length(); ++r) {
if(zeile[r] == '"') {
if(anfuehrungsz) {
anfuehrungsz = 0;
}
else { anfuehrungsz = true; continue;
}
}
if (anfuehrungsz) s+=zeile[r];
if (zeile[r] == '.' && !anfuehrungsz){ ++strzaehler; ausgabe << "section .data\nmsg" << strzaehler<<" db "<< '"' << s << '"' << ",0x0a\nlen" << strzaehler<<" equ $- msg"<<strzaehler<<"\nsection .text\nmov eax,4\nmov ebx,1\nmov ecx, msg"<<strzaehler<<"\nmov edx,len"<<strzaehler<<"\nint 0x80\n";
}
//epilog
}
}
}

}
}
}
//Ende der Hauptschleife
std::cout << "x hat den Wert " << vartabelle["x"] << std::endl;
std::cout << "kek hat den Wert " << vartabelle["kek"] << std::endl;
//ausgabe << "mov ebx,[ebp-"<<vartabelle["kek"]*4<< "]\nmov eax, 1\nint 0x80\n";
std::string const kompilier = std::string("nasm -f elf32 ") + argv[2] + " -o programm.o";
std::cout << kompilier.c_str() << std::endl;
//system(kompilier.c_str());
//system("nasm -f elf32 kek -o programm.o");
//system("nasm -f elf32 kek -o programm.o");
//system("ld -m elf_i386 programm.o -o programm; ./programm; echo $?");
return 0;
}
