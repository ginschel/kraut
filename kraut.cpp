#include <fstream>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <iostream>
void test(std::string c) {
	std::cout << "t" << c <<"t"<< std::endl;
}
std::string reg(int c) {
	std::string vorgabe = "[ebp-";
	vorgabe+=std::to_string(c*4);
	vorgabe+="]";
	return vorgabe;
}
int main(int argc, char *argv[]) {
//Variableninitiallisierung
int varzaehler =0;int strzaehler=0; int wennzaehler = 0; int schleifenzaehler = 0; int variablenausgaben = 0;
int schreibzaehler = 0;
std::string var, varwert;
bool gleichgefunden = 0; bool stringwert = 0; bool programmsektion = 0; bool asmblr = 0; bool anfuehrungsz =0;
std::map<std::string, int> vartabelle;
std::map<std::string, std::string> stringtabelle;
std::vector<int> wennschlange,wennoderschleife;
std::vector<std::string> schleifenschlange;
std::string ifi = argv[1], ofi = argv[2];
std::ifstream quelle(ifi); std::ofstream ausgabe(ofi);
ausgabe << "global _start\n_start:\nmov ebp,esp\nsection .data\n_nar db \"000000000000000000000000000000000\"\nsection .text\n";
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
if(varwert[0] != '"') {
if(zeile[r] != '=' && zeile[r] != '.' && zeile[r] != ';' && zeile[r] != ',') varwert += zeile[r];
}
else {
if(zeile[r] != ';' && r!=zeile.length()-1 ) varwert += zeile[r];
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
ausgabe << "section .data\n"<<var<<" db "<<varwert<<"\n"<<var<<"len equ $- "<<var<<"\nsection .text\n";
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
else if(stringtabelle.count(s) && zeile[i]=='[') {
	std::string welcheselement,ersatz;
	bool elementgef = 0;
	for(int r = i+1; r < zeile.length(); ++r) {
		if(zeile[r] != ']' && !elementgef) welcheselement+=zeile[r];
		if(zeile[r]==']') {
			elementgef = 1; r+=2;
		}
		if (elementgef && zeile[r] != '.') ersatz += zeile[r];
		if (zeile[r] == '.') {
			ausgabe <<"mov [" <<s<<"+"<<welcheselement<<"],byte "<<ersatz << "\n";
			std::string temp = stringtabelle[s];int we = std::stoi(welcheselement);
			if(we < temp.length()) {
				temp[we] = ersatz[1];
				stringtabelle[s] = temp; }
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
//WENN
if(s == "Wenn") {
std::string s1, s2,vergleicher;
bool s1gef = 0, opgef = 0;
bool grals = 0, klals = 0, gleich = 0, ungleich = 0, grgleichals = 0, klgleichals = 0;
for(int r = i+2; r < zeile.length(); ++r) {
if(s1gef == 0 && zeile[r] != ' ') s1+=zeile[r];
if(zeile[r] == ' ' && s1gef == 0){ s1gef = true; continue;
}
if(opgef == 0 && s1gef == 1 && zeile[r] != ' ') vergleicher+=zeile[r];
//jetzt kommen die vergleichsoperationen
if(!opgef && zeile[r] == ' ') {
if(vergleicher  == "größer" || vergleicher  == "groesser") {
opgef =1; grals = 1; r+=1;
}
if(vergleicher  == "kleiner als" || vergleicher == "kleiner") {
opgef =1; klals = 1; r+=1;
}
if(vergleicher  == "gleich") {
opgef =1; gleich = 1; r+=1;
}
if(vergleicher  == "ungleich") {
opgef =1; ungleich = 1; r+=1;
}
if(vergleicher  == "größer/gleich" || vergleicher == "groesser/gleich") {
opgef =1; grgleichals = 1; r+=1;
}
if(vergleicher  == "kleiner/gleich") {
opgef =1; klgleichals = 1; r+=1;
}
}
if(opgef && zeile[r] != ' ' && zeile[r] != '.') s2+=zeile[r];
if(zeile[r]=='.') {
//schaut, ob s1 s2 Variablen sind und substituiert die Variablen mit ihren Zeigern
if(vartabelle.count(s1)) s1=reg(vartabelle[s1]);
if(vartabelle.count(s2)) s2=reg(vartabelle[s2]);
//Standardfall
ausgabe << "mov eax, "<<s1<<"\nmov ebx,"<<s2<<"\n";
ausgabe << "cmp eax,ebx\n";
//Durchgang durch alle Fälle, die es gibt
if(grals) ausgabe << "jle Wennmarke"<<wennzaehler<<"\n";
if(klals) ausgabe << "jge Wennmarke"<<wennzaehler<<"\n";
if(gleich) ausgabe << "jne Wennmarke"<<wennzaehler<<"\n";
if(ungleich) ausgabe << "je Wennmarke"<<wennzaehler<<"\n";
if(grgleichals) ausgabe << "jl Wennmarke"<<wennzaehler<<"\n";
if(klgleichals) ausgabe << "jg Wennmarke"<<wennzaehler<<"\n";
wennschlange.push_back(wennzaehler);
wennoderschleife.push_back(0);
wennzaehler++;
}
//Ende des Parsers
}
}
//WÄHREND
if(s == "Während" || s=="Waehrend") {
std::string s1, s2,vergleicher;
bool s1gef = 0, opgef = 0;
bool grals = 0, klals = 0, gleich = 0, ungleich = 0, grgleichals = 0, klgleichals = 0;
for(int r = i+2; r < zeile.length(); ++r) {
if(s1gef == 0 && zeile[r] != ' ') s1+=zeile[r];
if(zeile[r] == ' ' && s1gef == 0){ s1gef = true; continue;
}
if(opgef == 0 && s1gef == 1 && zeile[r] != ' ') vergleicher+=zeile[r];
//jetzt kommen die vergleichsoperationen
if(!opgef && zeile[r] == ' ') {
if(vergleicher  == "größer" || vergleicher  == "groesser") {
opgef =1; grals = 1; r+=1;
}
if(vergleicher  == "kleiner als" || vergleicher == "kleiner") {
opgef =1; klals = 1; r+=1;
}
if(vergleicher  == "gleich") {
opgef =1; gleich = 1; r+=1;
}
if(vergleicher  == "ungleich") {
opgef =1; ungleich = 1; r+=1;
}
if(vergleicher  == "größer/gleich" || vergleicher == "groesser/gleich") {
opgef =1; grgleichals = 1; r+=1;
}
if(vergleicher  == "kleiner/gleich") {
opgef =1; klgleichals = 1; r+=1;
}
}
if(opgef && zeile[r] != ' ' && zeile[r] != '.') s2+=zeile[r];
if(zeile[r]=='.') {
//schaut, ob s1 s2 Variablen sind und substituiert die Variablen mit ihren Zeigern
if(vartabelle.count(s1)) s1=reg(vartabelle[s1]);
if(vartabelle.count(s2)) s2=reg(vartabelle[s2]);
//Standardfall
std::string aus = "mov eax, "+s1+"\nmov ebx,"+s2+"\ncmp eax,ebx\n";
//Durchgang durch alle Fälle, die es gibt
if(grals) aus+= "jg Schleifenmarke";
if(klals) aus+= "jl Schleifenmarke";
if(gleich) aus+=  "je Schleifenmarke";
if(ungleich) aus+= "jne Schleifenmarke";
if(grgleichals) aus+= "jge Schleifenmarke";
if(klgleichals) aus+= "jle Schleifenmarke";
aus+=std::to_string(schleifenzaehler)+"\n";
schleifenschlange.push_back(aus);
ausgabe << "Schleifenmarke"<<schleifenzaehler<<":\n";
wennoderschleife.push_back(1);
schleifenzaehler++;
}
//Ende des Parsers
}
}
if(s=="Mach weiter.") {
//Wenn ein Wenn aufm Stack ist
if(!*wennoderschleife.rbegin()) {
ausgabe << "Wennmarke"<<*wennschlange.rbegin()<<":\n";
wennschlange.pop_back();
}
//wenn eine Schleife aufm Stack ist
else {
ausgabe << *schleifenschlange.rbegin();
schleifenschlange.pop_back();
}
wennoderschleife.pop_back();
}
//schreibe
if(s == "Schreibe") { s="";
std::string nachr="",datei=""; bool in = 0,zeichenktt=0;
for (int r=i+2; r < zeile.length();++r) {
	test(nachr);
	if(zeile[r] == '"' && zeichenktt==0 && !in) {zeichenktt=1; nachr+=zeile[r];continue;}
	if(!in) {
		if(!zeichenktt) {
		if(zeile[r] != ' ') nachr+=zeile[r];
		else {
			r+=2; in = 1;
			test("in");
		}
		}
		else {
			nachr+=zeile[r];
			test(nachr);
			if(zeile[r] == '"') {
				r+=4; in = 1;
			}
		}
	}
	else {
		if (zeile[r] == '.' && zeile[r-1] == '"') {
			//filtert nach schon bestehenden Strings
			if(nachr[0] == '"' && *nachr.rbegin() == '"') {
				ausgabe << "section .data\n_pfad"<<schreibzaehler<<" dd "<<datei<<"\n_nachr"<<schreibzaehler<<" dd "<<nachr<<",0x0a\n_nachr"<<schreibzaehler<<"len equ $- _nachr"<<schreibzaehler<<"\nsection .text\nmov eax,5\nmov ebx,_pfad"<<schreibzaehler<<"\nmov ecx,101o\nmov edx,700o\nint 0x80\nmov ebx,eax\nmov eax, 4\nmov ecx,_nachr"<<schreibzaehler<<"\nmov edx,"<<"_nachr"<<schreibzaehler<<"len\nint 0x80\n";
			}
			else ausgabe << "section .data\n_pfad"<<schreibzaehler<<" dd "<<datei<<"\nsection .text\nmov eax,5\nmov ebx,_pfad"<<schreibzaehler<<"\nmov ecx,101o\nmov edx,700o\nint 0x80\nmov ebx,eax\nmov eax, 4\nmov ecx,"<<nachr<<"\nmov edx,"<<nachr<<"len\nint 0x80\n";
			//erstellt Datei
			test(nachr);test(datei);
			schreibzaehler++;
		}
		else datei+=zeile[r];
	}
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
if(s == "Gebe aus") {s="";
std::string stringlaenge =""; int anfuehrungseichen =0; bool klammer = 0, variable = 0;
for(int r = i+2; r < zeile.length(); ++r) {
if (!variable) {
if(zeile[r] != '.') {
if(zeile[r] == '(') klammer = 1;
if (klammer == 0) s+=zeile[r];
if(vartabelle.count(s)&&zeile[r+1]=='.') variable = 1;
if(zeile[r] == ')') klammer = 0;
if (klammer && zeile[r]!='(') stringlaenge+=zeile[r];
}
else {
if(stringlaenge=="") ausgabe << "mov eax,4\nmov ebx,1\nmov ecx,"<<s<<"\nmov edx,"<<s<<"len\nint 0x80\n";
else ausgabe << "mov eax,4\nmov ebx,1\nmov ecx,"<<s<<"\nmov edx,"<<stringlaenge<<"\nint 0x80\n";
}
}
//Variablenausgabealgorithmus
else {
ausgabe<< "mov ecx,0\nmov eax,"<<reg(vartabelle[s])<<"\nmov ebx,10\n_a"<<variablenausgaben<<":\nxor edx, edx\ndiv ebx\npush edx\ninc ecx\ncmp eax, 0\n jg _a"<<variablenausgaben<<"\nmov edx,ecx\n_b"<<variablenausgaben<<":\nmov ebx,edx\npop eax\nadd eax,48\nsub ebx,ecx\nmov [_nar+ebx],eax\ndec ecx\ncmp ecx,0\njg _b"<<variablenausgaben<<"\nmov [_nar+edx],byte 0x0a\ninc edx\nmov eax,4\nmov ebx,1\nmov ecx, _nar\nint 0x80\n";
variablenausgaben++;
}
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
