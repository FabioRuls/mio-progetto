#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

// Strutture dati
struct Studente {
    string matricola;
    string cognome;
    string nome;
};

struct Materia {
    string codice;
    string descrizione;
};

struct Corso {
    string codice;
    string nome;
    set<string> materie;  // Codici delle materie
    set<string> studenti; // Matricole studenti
};

class GestioneCorsi {
private:
    map<string, Corso> corsi;
    map<string, Materia> materie;
    map<string, Studente> studenti;
    map<string, set<string>> studenteCorsi;
    vector<vector<string>> esami; // Per mantenere tutti i record originali

    // Funzione per pulire stringhe
    string pulisciStringa(const string& s) {
        string result = s;
        result.erase(remove_if(result.begin(), result.end(), ::isspace), result.end());
        return result;
    }

public:
    // 2. Lettura del file CSV
    void leggiCSV(const string& nomeFile) {
        ifstream file(nomeFile);
        if (!file.is_open()) {
            cerr << "Errore nell'apertura del file" << endl;
            return;
        }

        string linea;
        getline(file, linea); // Salta l'header

        while (getline(file, linea)) {
            vector<string> record;
            stringstream ss(linea);
            string campo;

            while (getline(ss, campo, ',')) {
                record.push_back(campo);
            }

            if (record.size() != 7) continue;

            // Aggiungi ai dati strutturati
            string codiceCorso = pulisciStringa(record[0]);
            string nomeCorso = record[1];
            string codiceMateria = pulisciStringa(record[2]);
            string descrizioneMateria = record[3];
            string matricola = pulisciStringa(record[4]);
            string cognome = record[5];
            string nome = record[6];

            // Aggiorna strutture
            corsi[codiceCorso].codice = codiceCorso;
            corsi[codiceCorso].nome = nomeCorso;
            corsi[codiceCorso].materie.insert(codiceMateria);
            corsi[codiceCorso].studenti.insert(matricola);

            materie[codiceMateria] = {codiceMateria, descrizioneMateria};
            studenti[matricola] = {matricola, cognome, nome};
            studenteCorsi[matricola].insert(codiceCorso);

            // Mantieni una copia del record originale
            esami.push_back(record);
        }
        file.close();
    }

    // 3.1 Ricerca corsi per matricola
    void ricercaPerMatricola() {
        string matricola;
        cout << "Inserisci matricola: ";
        cin >> matricola;
        matricola = pulisciStringa(matricola);

        if (studenteCorsi.count(matricola)) {
            cout << "\nCorsi frequentati da " << studenti[matricola].nome << " "
                 << studenti[matricola].cognome << " (" << matricola << "):\n";
            for (const auto& codice : studenteCorsi[matricola]) {
                cout << "- " << corsi[codice].nome << " (" << codice << ")\n";
            }
        } else {
            cout << "Matricola non trovata.\n";
        }
    }

    // 3.2 Ricerca corsi per cognome
    void ricercaPerCognome() {
        string cognome;
        cout << "Inserisci cognome: ";
        cin >> cognome;

        bool trovato = false;
        for (const auto& [matricola, s] : studenti) {
            if (s.cognome == cognome) {
                trovato = true;
                cout << "\nStudente: " << s.nome << " " << s.cognome << " (" << matricola << ")\n";
                cout << "Corsi frequentati:\n";
                for (const auto& codice : studenteCorsi[matricola]) {
                    cout << "- " << corsi[codice].nome << endl;
                }
                cout << endl;
            }
        }

        if (!trovato) {
            cout << "Nessuno studente trovato con questo cognome.\n";
        }
    }

    // 4. Lista studenti per corso
    void studentiPerCorso() {
        string codiceCorso;
        cout << "Inserisci codice corso: ";
        cin >> codiceCorso;
        codiceCorso = pulisciStringa(codiceCorso);

        if (corsi.count(codiceCorso)) {
            cout << "\nStudenti iscritti a " << corsi[codiceCorso].nome << ":\n";
            for (const auto& matricola : corsi[codiceCorso].studenti) {
                cout << "- " << matricola << ": " << studenti[matricola].nome
                     << " " << studenti[matricola].cognome << endl;
            }
            cout << "Totale: " << corsi[codiceCorso].studenti.size() << " studenti\n";
        } else {
            cout << "Corso non trovato.\n";
        }
    }

    // 5. Stampa esami di un corso
    void stampaEsamiCorso() {
        string codiceCorso;
        cout << "Inserisci codice corso: ";
        cin >> codiceCorso;
        codiceCorso = pulisciStringa(codiceCorso);

        if (!corsi.count(codiceCorso)) {
            cout << "Corso non trovato.\n";
            return;
        }

        cout << "\nEsami per " << corsi[codiceCorso].nome << ":\n";
        cout << "--------------------------------------------------\n";
        for (const auto& record : esami) {
            if (pulisciStringa(record[0]) == codiceCorso) {
                cout << "Materia: " << record[3] << " (" << record[2] << ")\n";
                cout << "Studente: " << record[6] << " " << record[5] << " (" << record[4] << ")\n";
                cout << "--------------------------------------------------\n";
            }
        }
    }

    // 6. Numero studenti per corso
    void stampaNumeroStudenti() {
        cout << "\nNumero studenti per corso:\n";
        for (const auto& [codice, corso] : corsi) {
            cout << "- " << corso.nome << ": " << corso.studenti.size() << " studenti\n";
        }
    }

    // 7. Numero materie per corso
    void stampaNumeroMaterie() {
        cout << "\nNumero materie per corso:\n";
        for (const auto& [codice, corso] : corsi) {
            cout << "- " << corso.nome << ": " << corso.materie.size() << " materie\n";
        }
    }

    // 8. Ricerca materie per descrizione
    void ricercaMaterie() {
        string query;
        cout << "Inserisci termine di ricerca: ";
        cin.ignore();
        getline(cin, query);

        transform(query.begin(), query.end(), query.begin(), ::tolower);
        set<string> materieTrovate;

        for (const auto& [codice, materia] : materie) {
            string descLower = materia.descrizione;
            transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);

            if (descLower.find(query) != string::npos) {
                materieTrovate.insert(materia.descrizione);
            }
        }

        if (!materieTrovate.empty()) {
            cout << "\nMaterie trovate:\n";
            for (const auto& m : materieTrovate) {
                cout << "- " << m << endl;
            }
        } else {
            cout << "Nessuna materia trovata.\n";
        }
    }

    // 9. Inserimento nuovo studente
    void inserisciStudente() {
        string matricola, nome, cognome, codiceMateria;

        cout << "\nInserisci matricola: ";
        cin >> matricola;
        matricola = pulisciStringa(matricola);

        if (studenti.count(matricola)) {
            cout << "Matricola già esistente.\n";
            return;
        }

        cout << "Inserisci nome: ";
        cin >> nome;
        cout << "Inserisci cognome: ";
        cin >> cognome;
        cout << "Inserisci codice materia: ";
        cin >> codiceMateria;
        codiceMateria = pulisciStringa(codiceMateria);

        if (!materie.count(codiceMateria)) {
            cout << "Materia non trovata.\n";
            return;
        }

        // Trova il corso associato alla materia
        string codiceCorso;
        for (const auto& record : esami) {
            if (pulisciStringa(record[2]) == codiceMateria) {
                codiceCorso = pulisciStringa(record[0]);
                break;
            }
        }

        if (codiceCorso.empty()) {
            cout << "Errore: materia non associata a nessun corso.\n";
            return;
        }

        // Aggiungi lo studente
        studenti[matricola] = {matricola, cognome, nome};
        studenteCorsi[matricola].insert(codiceCorso);
        corsi[codiceCorso].studenti.insert(matricola);

        // Aggiungi il record agli esami
        vector<string> nuovoRecord = {
            codiceCorso,
            corsi[codiceCorso].nome,
            codiceMateria,
            materie[codiceMateria].descrizione,
            matricola,
            cognome,
            nome
        };
        esami.push_back(nuovoRecord);

        cout << "Studente aggiunto con successo.\n";
    }

    // 10. Salvataggio dati
    void salvaCSV(const string& nomeFile) {
        ofstream file(nomeFile);
        if (!file.is_open()) {
            cerr << "Errore nel salvataggio del file" << endl;
            return;
        }

        file << "codice_corso,descrizione_corso,codice_materia,descrizione_materia,matricola_studente,cognome_studente,nome_studente\n";

        for (const auto& record : esami) {
            for (size_t i = 0; i < record.size(); ++i) {
                file << record[i];
                if (i != record.size() - 1) file << ",";
            }
            file << "\n";
        }
        file.close();
        cout << "Dati salvati con successo in " << nomeFile << endl;
    }

    // Menu principale
    void mostraMenu() {
        cout << "\n=== MENU PRINCIPALE ===\n";
        cout << "1. Cerca corsi per matricola\n";
        cout << "2. Cerca corsi per cognome\n";
        cout << "3. Lista studenti per corso\n";
        cout << "4. Stampa esami di un corso\n";
        cout << "5. Numero studenti per corso\n";
        cout << "6. Numero materie per corso\n";
        cout << "7. Ricerca materie\n";
        cout << "8. Inserisci nuovo studente\n";
        cout << "9. Salva dati\n";
        cout << "0. Esci\n";
        cout << "Scelta: ";
    }

    void esegui() {
        int scelta;
        do {
            mostraMenu();
            cin >> scelta;

            // Pulisci il buffer di input dopo aver letto la scelta
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch(scelta) {
                case 1: ricercaPerMatricola(); break;
                case 2: ricercaPerCognome(); break;
                case 3: studentiPerCorso(); break;
                case 4: stampaEsamiCorso(); break;
                case 5: stampaNumeroStudenti(); break;
                case 6: stampaNumeroMaterie(); break;
                case 7: ricercaMaterie(); break;
                case 8: inserisciStudente(); break;
                case 9: salvaCSV("corsi_studenti_aggiornato.csv"); break;
                case 0: cout << "Uscita...\n"; break;
                default: cout << "Scelta non valida!\n";
            }
        } while(scelta != 0);
    }
};

int main() {
    GestioneCorsi gestione;
    gestione.leggiCSV("corsi_studenti.csv");
    gestione.esegui();
    return 0;
}
