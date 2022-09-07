// TerraForm.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.

#include <iostream>
#include <thread>
#include <chrono>
#include <random>

//Genera un numero casuale tra min e max compresi gli estremi
int randint(int min, int max);

// Se densità_popolazione supera questo valore la popolazione va in guerra
const int LIMITE_SOVRAPPOPOLAZIONE = 1000;
const int POPOLAZIONE_DI_PARTENZA = 10;
const int LIMITE_VITA = 120;
const int SUPERFICIE = 10000; // 10 000 km^2

// Variabili globali
int popolazione = POPOLAZIONE_DI_PARTENZA;
int popolazione_max = popolazione;
std::string nomi_maschi[10] = {"Jose", "James", "Robert", "John", "Michael", "David", "William", "Daniel", "Joseph", "Thomas"};
std::string nomi_femmine[10] = {"Maria", "Mary", "Marie", "Ana", "Patricia", "Elizabeth", "Jessica", "Barbara", "Jennifer", "Sandra"};
std::string cognomi[10] = {"Smith", "Johnson", "Williams", "Jones", "Brown", "Davis", "Miller", "Wilson", "Moore", "Taylor"};
int anno_peste = randint(1000, 2000);
int anno_pandemia = randint(100, 200);
int anno = 1;
int ind_liberi[LIMITE_SOVRAPPOPOLAZIONE] = {};
// Fuzioni generali che verranno chiamate ogni anno
int update();
void ripruzione();
void aggiorna_eta();
void decessi();


// Prototipi delle funzioni che gestiscono gli eventi
bool guerra(bool comunica_risultato);
int pandemia(bool comunica_risultato, int popolazione);
bool peste(bool comunica_risultato);

void genera_popolazione_di_partenza(int numero_persone, bool comunica_risultato);
bool e_adatto(char sesso, int i);

int randint(int min, int max){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// Attende un certo numero di secondi prima di riprendere l'esecuzione del programma
void sleep(short seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

// Permette di sapere i dettagli di ogni persona
class Persona
{
    public:
        int id;
        std::string nome;
        std::string cognome;
        char sesso;
        int eta;
        int limite_vita;
        int tempo_da_ultimo_rapporto;
        bool si_e_riprodotto;
        char orientamento;//E = etero, O = omo
};
Persona persona;
Persona persone[LIMITE_SOVRAPPOPOLAZIONE + 100];

int main()
{
    int conta;

    std::cout << "TerraForm!\n\n";
    sleep(1);
    
    genera_popolazione_di_partenza(POPOLAZIONE_DI_PARTENZA, false);

    std::cout << "\n\nAnno " << anno;
    std::cout << "\nPopolazione:\t" << popolazione;
    while (popolazione > 0 || popolazione <= LIMITE_SOVRAPPOPOLAZIONE)
    {
        conta = 0;
        sleep(0);
        popolazione = update();
        for (int i = 0; i < LIMITE_SOVRAPPOPOLAZIONE; i++)
        {
            if (persone[i].id > 0)
            {
                conta++;
            }
        }
        popolazione = conta;
    }



    if (popolazione <=0)
    {
        std::cout << "\nNon resta più alcun individuo nel mondo";
        std::cout << "\nLA TUA POPOLAZIONE SI E' ESTINTA";
    }
    else if (popolazione == LIMITE_SOVRAPPOPOLAZIONE)
    {
        std::cout << "\n\nSOVRAPPOPOLAZIONE RAGGIUNTA";
        std::cout << "\nNon resta più abbastanza cibo per la popolazione, quindi il mondo e' andato in guerra per le scorte";
        std::cout << "\nLA TUA POPOLAZIONE SI E' ESTINTA";
    }
    std::cout << "\n";
    system("pause");
}

//Si occupa della gentione degli eventi ogni anno
int update()
{
    int scorte_di_terra = SUPERFICIE / popolazione;
    int densita_popolazione = popolazione / SUPERFICIE;
    int morti = 0;

    aggiorna_eta();
    decessi();
    
    ripruzione();

    anno++;
    std::cout << "\n\nAnno " << anno;

    peste(true);
    morti += pandemia(true, popolazione);
    guerra(true);

    popolazione = popolazione - morti;

    std::cout << "\nPopolazione:\t" << popolazione << "\n";

    if (popolazione > popolazione_max)
    {
        popolazione_max = popolazione;
    }
    return popolazione;
}

// Funzione che genera una popolazione di partenza di 10 individui dai 10 ai 20 anni
void genera_popolazione_di_partenza(int numero_persone, bool output)
{
    char sessi[2] = {'M', 'F'};
    
    for (int i = 0; i < numero_persone; i++)
    {
        persona.sesso = sessi[randint(0, 1)];
        if (persona.sesso == 'M')
        {
            persona.nome = nomi_maschi[randint(0, 9)];
        }
        else
        {
            persona.nome = nomi_femmine[randint(0, 9)];
        }
        char orientamento_s = randint(0, 9);
        if (orientamento_s > 33) {
            persona.orientamento = 'E';
        }
        else {
            persona.orientamento = 'O';
        }

        persona.id = i + 1;
        persona.cognome = cognomi[i];
        persona.eta = randint(10, 19);
        persona.limite_vita = LIMITE_VITA - (randint(0, 40));
        persona.tempo_da_ultimo_rapporto = 0;
        persona.si_e_riprodotto = false;

        persone[i] = persona;
    }
    if (output)
    {
        // comunica i dati della popolazione
        for (int i = 0; i < popolazione; i++)
        {
            std::cout << "ID:" << persone[i].id << "\t";
            std::cout << "Nome: " << persone[i].nome << "\t";
            std::cout << "Cognome: " << persone[i].cognome << "\t";
            std::cout << "Sesso: " << persone[i].sesso << "\t";
            std::cout << "Eta: " << persone[i].eta << "\t";
            std::cout << "Ultimo rapporto: " << persone[i].tempo_da_ultimo_rapporto << "\n";
        }
    }
}

//Ogni anno aggiorna l'età delle persone ancora vive
void aggiorna_eta() {
    int i = 0;
    while (i < popolazione_max || i < LIMITE_SOVRAPPOPOLAZIONE)
    {
        if (persone[i].id != 0)
        {
            persone[i].eta++;
        }
        i++;
    }
}

/*
    Le persone per riprodursi:
    - dovranno avere una età compresa tra 18 e 45 (F) o 20 50 (M)
    - dovranno essere passati almeno 2 anni dall'ultimo rapporto
    - ci si può accoppiare solo se ci sono almeno un maschio ed una femmina entrambi etero
 */
bool e_adatto(char sesso, int i) {

    if (sesso == 'M') {
        if (persone[i].id != 0 && (persone[i].eta > 20 && persone[i].eta < 50 && (!persone[i].si_e_riprodotto || persone[i].tempo_da_ultimo_rapporto >= 2)))
            return true;
        else
            return false;
    }
    else {
        if (persone[i].id != 0 && (persone[i].eta > 18 && persone[i].eta < 45 && (!persone[i].si_e_riprodotto || persone[i].tempo_da_ultimo_rapporto >= 2)))
            return true;
        else
            return false;
    }
}

//Genera un nuovo individuo nella posizione indicata
void genera_persona(int i, int pos_padre) {
    char sessi[2] = { 'M', 'F' };

    persona.sesso = sessi[randint(0, 1)];
    if (persona.sesso == 'M')
    {
        persona.nome = nomi_maschi[randint(0, 9)];
    }
    else
    {
        persona.nome = nomi_femmine[randint(0, 9)];
    }
    char orientamento_s = randint(0, 100);
    if (orientamento_s > 33) {
        persona.orientamento = 'E';
    }
    else {
        persona.orientamento = 'O';
    }

    persona.id = i + 1;
    persona.cognome = persone[pos_padre].cognome;
    persona.eta = 0;
    persona.limite_vita = LIMITE_VITA - (randint(0, 40));
    persona.tempo_da_ultimo_rapporto = 0;
    persona.si_e_riprodotto = false;

    persone[i] = persona;

    popolazione++;
    popolazione_max = popolazione;
}

//Gestisce l'aumento di popolazione nella simulazione
void ripruzione() {
    
    int limite = popolazione_max;
    for (int i = 0; i < limite; i++)
    {
        if (persone[i].si_e_riprodotto) {
            persone[i].tempo_da_ultimo_rapporto++;
        }
    }
    for (int i = 0; i < limite; i++)
    {
        //maschi
        if (e_adatto(persone[i].sesso, i))
        {
            bool trovato = false;
            int j = 0;
            int pos_padre;

            while (!trovato && j < limite)
            {
                if (e_adatto(persone[j].sesso, j) && persone[j].sesso != persone[i].sesso)
                {
                    //cambia tempo_da_ultimo_rapporto
                    //crea un nuovo individuo

                    persone[i].tempo_da_ultimo_rapporto = 0;
                    persone[j].tempo_da_ultimo_rapporto = 0;

                    persone[i].si_e_riprodotto = true;
                    persone[j].si_e_riprodotto = true;
                    trovato = true;

                    if (persone[i].sesso == 'M')
                    {
                        pos_padre = i;
                    }
                    else {
                        pos_padre = j;
                    }

                    for (int i = 0; i < limite; i++)
                    {
                        if (persone[i].id == 0)
                        {
                            genera_persona(i, pos_padre);
                            break;
                        }
                        else if (persone[i].id == popolazione_max) {
                            genera_persona(i + 1, pos_padre);
                            break;
                        }
                    }
                }
               j++;
            }
        }
    }
}

//Persone morte di vecchiaia
void decessi() {
    for (int i = 0; i < popolazione_max; i++)
    {
        if (persone[i].eta  >= persone[i].limite_vita && persone[i].id != 0)
        {
            persone[i].id = 0;
            persone[i].nome = "";
            persone[i].cognome = "";
            persone[i].sesso = ' ';
            persone[i].eta = 0;
            persone[i].limite_vita = persone[i].eta;

            popolazione--;
        }
    }
}

// Se è l'anno della peste allora 1/3 della popolazione sparisce
bool peste(bool output)
{
    
    if (anno == anno_peste)
    {
        int i = 0, ind = 0;
        int morti_di_peste = (int) (popolazione / 3);

        popolazione -= morti_di_peste;
        anno_peste += 1000;
        std::cout << "\n\nANNO DELLA PESTE!";

        // Cancella 1/5 delle persone morte, quelle con una età maggiore di 60 o minore di 10 da persone[]
        while (i < morti_di_peste)
        {
            if ((persone[ind].eta > 60 || persone[ind].eta < 10) && persone[ind].id > 0)
            {
                persone[ind].id = 0;
                persone[ind].nome = "";
                persone[ind].cognome = "";
                persone[ind].sesso = ' ';
                persone[ind].eta = 0;
                persone[ind].limite_vita = persone[ind].eta;
                i++;
            }
            ind++;
        }
        if (output)
        {
            std::cout << "\nLa popolazione e' diminuita di " << morti_di_peste << " persone per la peste!";
        }
        return true;
    }
    else
        return false;
}

//Se è l'anno della pandemia allora 1/20 della popolazione sparisce
int pandemia(bool output, int popolazione) {
    if (anno == anno_pandemia)
    {
        int i = 0, ind = 0;
        int morti_di_malattia = (int) (popolazione / 20);
        popolazione -= (int) (popolazione) / 20;
        anno_pandemia += 100;
        std::cout << "\n\nANNO DELLA PANDEMIA!";

        // Cancella 1/20 delle persone morte, quelle con una età maggiore di 60 o minore di 10 da persone[]
        while (i < morti_di_malattia)
        {
            if ((persone[ind].eta > 60 || persone[ind].eta < 10) && persone[ind].id > 0)
            {
                persone[ind].id = 0;
                persone[ind].nome = "";
                persone[ind].cognome = "";
                persone[ind].sesso = ' ';
                persone[ind].eta = 0;
                persone[ind].limite_vita = persone[ind].eta;

                i++;
            }
            ind++;
        }
        if (output)
        {
            std::cout << "\nLa popolazione e' diminuita di " << morti_di_malattia << " persone per la pandemia!";
        }
        return morti_di_malattia;
    }
    else {
        return 0;
    }
    
}

/* Viene "lanciato un dado": se il numero uscito è 0 allora viene scatenata guerra moniale,
   quindi le risorse diventeno limitate (alta moortalità e bassa natalità) */
bool guerra(bool output)
{
    // 1/10 della popolazione muore in battaglia.
    // La guerra non può partire se co sono meno di 100 persone
    // Le persone con età maggiore di 60 o minore di 18 non partecipano alla guerra
    int i = 0, ind = 0;
    int morti_guerra = (int) ((popolazione) / 10);
    int avvia_guerra = randint(0, 9) + 1;
    if (popolazione > 100 && avvia_guerra == 0)
    {
        std::cout << "\n\nANNO DELLA GUERRA!";
        for (i = 0; i < morti_guerra; i++)
        {
            if (persone[ind].eta >= 18 && persone[ind].eta < 60 && persone[ind].id > 0)
            {
                persone[ind].id = 0;
                persone[ind].nome = "";
                persone[ind].cognome = "";
                persone[ind].sesso = ' ';
                persone[ind].eta = 0;
                persone[ind].limite_vita = persone[ind].eta;

                popolazione--;
            }
        }
        if (output)
        {
            std::cout << "\nLa popolazione e' diminuita di " << morti_guerra << " persone per la guerra!";
        }
        return true;
    }
    else
    {
        return false;
    }
}

/*
    Questo serviva se si voleva rendere più complesso il programma applicando una mappa basata su piano cartesiano
    con coordinate x e y che indicano la posizione di diversi elementi sulla mappa.

    int coordinate_partenza[2] = { 50, 50 };

    int coordinate_montagne[20][2] = {
        {10, 10},{20, 10},{30, 10},{40, 10},{50, 10},{60, 10},{70, 10},{80, 10},{90, 10},
        {100, 10},{10, 100},{20, 100},{30, 100},{40, 100},{50, 100},{60, 100},{70, 100},{80, 100},{90, 100},{100, 100},
    };
    int coordinate_mare[20][2] = {
        {40,20},{70,20},{20,30},{30,30},{70,30},{80,30},{50,40},{100,40},{10,70},{20,70},
        {10,80},{20,80},{30,80},{70,80},{10,90},{20,90},{30,90},{40,90},{60,90},{100,90},
    };
    int coordinate_alberi[13][2] = {
        {60,20},{50,30},{10,50},{20,50},{30,50},{40,50},{50,50},
        {60,50},{70,50},{80,50},{90,50},{100,50},{90,70},
    };
    int coordinate_minerali[6][2] = {
        {20,20},{90,30},{70,70},{80,70},{80,80},{90,80},
    };
*/

// Per eseguire il programma: CTRL+F5 oppure Debug > Avvia senza eseguire debug
// Per eseguire il debug del programma: F5 oppure Debug > Avvia debug

// Suggerimenti per iniziare:
//   1. Usare la finestra Esplora soluzioni per aggiungere/gestire i file
//   2. Usare la finestra Team Explorer per connettersi al controllo del codice sorgente
//   3. Usare la finestra di output per visualizzare l'output di compilazione e altri messaggi
//   4. Usare la finestra Elenco errori per visualizzare gli errori
//   5. Passare a Progetto > Aggiungi nuovo elemento per creare nuovi file di codice oppure a Progetto > Aggiungi elemento esistente per aggiungere file di codice esistenti al progetto
//   6. Per aprire di nuovo questo progetto in futuro, passare a File > Apri > Progetto e selezionare il file con estensione sln
