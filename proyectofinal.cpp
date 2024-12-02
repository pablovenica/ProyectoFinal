#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <chrono>
#include <map>
#include <ctime>
#include <iomanip>

#define NOMBRE_ARCHIVO "Base_Datos_COMA.csv"

using namespace std;
using namespace std::chrono;

struct Partido {
    string jornada;
    string fecha;
    string equipoLocal;
    int golesLocal;
    int golesVisitante;
    string equipoVisitante;
    string competicion;
};

class BaseDatos {
private:
    vector<Partido> partidos;
    unordered_map<string, int> golesTotales;
    unordered_map<string, size_t> indicePorJornada;
    unordered_map<string, map<string, int>> golesAFavor;
    unordered_map<string, map<string, int>> golesEnContra;
    unordered_map<string, map<string, pair<int, int>>> triunfosDerrotas;

public:
//Lee datos del archivo CSV.
    void cargarDesdeCSV() {
        ifstream archivo(NOMBRE_ARCHIVO);
        if (!archivo.is_open()) {
            cerr << "Error al abrir el archivo: " << NOMBRE_ARCHIVO << endl;
            return;
        }

        string linea;
        getline(archivo, linea);

        while (getline(archivo, linea)) {
            stringstream ss(linea);
            Partido partido;
            string token;

            try {
                getline(ss, partido.jornada, ',');
                getline(ss, partido.fecha, ',');
                getline(ss, partido.equipoLocal, ',');

                getline(ss, token, ','); partido.golesLocal = stoi(token);
                getline(ss, token, ','); partido.golesVisitante = stoi(token);

                getline(ss, partido.equipoVisitante, ',');
                getline(ss, partido.competicion, ',');

                partidos.push_back(partido);
                golesTotales[partido.equipoLocal] += partido.golesLocal;
                golesTotales[partido.equipoVisitante] += partido.golesVisitante;
                indicePorJornada[partido.jornada] = partidos.size() - 1;

                golesAFavor[partido.equipoLocal][partido.competicion] += partido.golesLocal;
                golesEnContra[partido.equipoVisitante][partido.competicion] += partido.golesLocal;
                golesAFavor[partido.equipoVisitante][partido.competicion] += partido.golesVisitante;
                golesEnContra[partido.equipoLocal][partido.competicion] += partido.golesVisitante;

                if (partido.golesLocal > partido.golesVisitante) {
                    triunfosDerrotas[partido.equipoLocal][partido.competicion].first++;
                    triunfosDerrotas[partido.equipoVisitante][partido.competicion].second++;
                } else if (partido.golesLocal < partido.golesVisitante) {
                    triunfosDerrotas[partido.equipoVisitante][partido.competicion].first++;
                    triunfosDerrotas[partido.equipoLocal][partido.competicion].second++;
                }
            } catch (invalid_argument& e) {
                cerr << "Error al convertir goles a entero en la linea: " << linea << endl;
            } catch (out_of_range& e) {
                cerr << "Numero fuera de rango en la linea: " << linea << endl;
            }
        }
        archivo.close();
        cout << "Datos cargados desde " << NOMBRE_ARCHIVO << endl;
    }

//Muestra los goles totales de cada equipo ordenados alfabéticamente.
    void mostrarResultadosPrincipales() {
        vector<pair<string, int>> equiposGoles(golesTotales.begin(), golesTotales.end());

        sort(equiposGoles.begin(), equiposGoles.end());

        cout << "Goles Totales por Equipo (ordenados alfabéticamente): " << endl;
        for (const auto& equipo : equiposGoles) {
            cout << equipo.first << ": " << equipo.second << " goles " << endl;
        }
    }

//Permite al usuario añadir un nuevo partido con datos proporcionados manualmente.
    void agregarPartido() {
        Partido nuevoPartido;
        cout << "Ingrese la jornada: ";
        cin.ignore();
        getline(cin, nuevoPartido.jornada);
        cout << "Ingrese la fecha: ";
        getline(cin, nuevoPartido.fecha);
        cout << "Ingrese el equipo local: ";
        getline(cin, nuevoPartido.equipoLocal);
        cout << "Ingrese los goles del equipo local: ";
        cin >> nuevoPartido.golesLocal;
        cout << "Ingrese los goles del equipo visitante: ";
        cin >> nuevoPartido.golesVisitante;
        cin.ignore();
        cout << "Ingrese el equipo visitante: ";
        getline(cin, nuevoPartido.equipoVisitante);
        cout << "Ingrese la competicion: ";
        getline(cin, nuevoPartido.competicion);

        partidos.push_back(nuevoPartido);
        golesTotales[nuevoPartido.equipoLocal] += nuevoPartido.golesLocal;
        golesTotales[nuevoPartido.equipoVisitante] += nuevoPartido.golesVisitante;
        indicePorJornada[nuevoPartido.jornada] = partidos.size() - 1;

        cout << "Partido agregado exitosamente." << endl;
    }

//Elimina un partido específico basado en su jornada.
    void eliminarPartido(const string& jornada) {
        auto it = find_if(partidos.begin(), partidos.end(), [&jornada](const Partido& p) {
            return p.jornada == jornada;
        });

        if (it != partidos.end()) {
            partidos.erase(it);
            cout << "Partido eliminado exitosamente." << endl;
        } else {
            cout << "No se encontro un partido con la jornada especificada." << endl;
        }
    }

//Modifica los datos de un partido existente.
    void modificarPartido(const string& jornada) {
        auto it = find_if(partidos.begin(), partidos.end(), [&jornada](Partido& p) {
            return p.jornada == jornada;
        });

        if (it != partidos.end()) {
            cout << "Modificando partido de la jornada " << it->jornada << endl;
            cout << "Ingrese la nueva fecha (actual: " << it->fecha << "): ";
            getline(cin, it->fecha);
            cout << "Ingrese el equipo local (actual: " << it->equipoLocal << "): ";
            getline(cin, it->equipoLocal);
            cout << "Ingrese los goles del equipo local (actual: " << it->golesLocal << "): ";
            cin >> it->golesLocal;
            cout << "Ingrese los goles del equipo visitante (actual: " << it->golesVisitante << "): ";
            cin >> it->golesVisitante;
            cin.ignore();
            cout << "Ingrese el equipo visitante (actual: " << it->equipoVisitante << "): ";
            getline(cin, it->equipoVisitante);
            cout << "Ingrese la competicion (actual: " << it->competicion << "): ";
            getline(cin, it->competicion);

            cout << "Partido modificado exitosamente." << endl;
        } else {
            cout << "No se encontro un partido con la jornada especificada." << endl;
        }
    }

//Escribe todos los partidos almacenados en un archivo CSV.
    void guardarEnCSV() {
        ofstream archivo(NOMBRE_ARCHIVO);
        if (!archivo.is_open()) {
            cerr << "Error al abrir el archivo para guardar: " << NOMBRE_ARCHIVO << endl;
            return;
        }

        archivo << "Jornada,Fecha,Equipo Local,Goles Local,Goles Visitante,Equipo Visitante,Competicion\n";

        for (const auto& partido : partidos) {
            archivo << partido.jornada << ',' << partido.fecha << ',' << partido.equipoLocal << ','
                    << partido.golesLocal << ',' << partido.golesVisitante << ',' << partido.equipoVisitante << ','
                    << partido.competicion << '\n';
        }

        archivo.close();
        cout << "Datos guardados en " << NOMBRE_ARCHIVO << endl;
    }

//Encuentra y muestra los 5 partidos con más goles por competición.
void mostrarTop5PartidosConMasGoles() {
    unordered_map<string, vector<Partido>> partidosPorCompeticion;

    for (const auto& partido : partidos) {
        partidosPorCompeticion[partido.competicion].push_back(partido);
    }

    cout << "Top 5 partidos con mayor cantidad de goles por competicion:" << endl;

    for (auto& competicionPartidos : partidosPorCompeticion) {
        auto& competicion = competicionPartidos.first;
        auto& listaPartidos = competicionPartidos.second;

        sort(listaPartidos.begin(), listaPartidos.end(), [](const Partido& a, const Partido& b) {
            int golesA = a.golesLocal + a.golesVisitante;
            int golesB = b.golesLocal + b.golesVisitante;
            if (golesA != golesB) return golesA > golesB;
            return a.fecha > b.fecha;
        });

        cout << "Competicion: " << competicion << endl;
        for (int i = 0; i < min(5, (int)listaPartidos.size()); i++) {
            cout << "Jornada: " << listaPartidos[i].jornada
                 << ", Fecha: " << listaPartidos[i].fecha
                 << ", Equipos: " << listaPartidos[i].equipoLocal << " vs " << listaPartidos[i].equipoVisitante
                 << ", Goles Totales: " << (listaPartidos[i].golesLocal + listaPartidos[i].golesVisitante) << endl;
        }
        cout << endl;
    }
}

//Muestra goles a favor y en contra de un equipo por competición.
void mostrarGolesTotalesEquipoPorCompeticion(const string& equipo) {
    cout << "Goles totales para el equipo: " << equipo << endl;

    if (golesAFavor.find(equipo) == golesAFavor.end()) {
        cout << "No se encontraron datos para el equipo especificado." << endl;
        return;
    }

    for (const auto& competicionGoles : golesAFavor[equipo]) {
        const string& competicion = competicionGoles.first;
        int golesAFavorEquipo = competicionGoles.second;

        // Verificamos si existe la clave en golesEnContra antes de acceder
        int golesEnContraEquipo = golesEnContra[equipo].count(competicion) > 0 ? golesEnContra[equipo][competicion] : 0;

        cout << "Competicion: " << competicion << ", Goles a favor: " << golesAFavorEquipo
             << ", Goles en contra: " << golesEnContraEquipo << endl;
    }
}


//Calcula el promedio de goles a favor y en contra para un equipo.
void mostrarPromedioGolesEquipoPorCompeticion(const string& equipo) {
    cout << "Promedio de goles para el equipo: " << equipo << endl;

    if (golesAFavor.find(equipo) == golesAFavor.end()) {
        cout << "No se encontraron datos para el equipo especificado." << endl;
        return;
    }

    for (const auto& competicionGoles : golesAFavor[equipo]) {
        const string& competicion = competicionGoles.first;
        int golesAFavorEquipo = competicionGoles.second;

        int golesEnContraEquipo = golesEnContra[equipo].count(competicion) ? golesEnContra[equipo][competicion] : 0;
        int partidosCompeticion = count_if(partidos.begin(), partidos.end(), [&](const Partido& partido) {
            return (partido.equipoLocal == equipo || partido.equipoVisitante == equipo) && partido.competicion == competicion;
        });

        if (partidosCompeticion > 0) {
            cout << "Competicion: " << competicion
                 << ", Promedio de goles a favor: " << static_cast<float>(golesAFavorEquipo) / partidosCompeticion
                 << ", Promedio de goles en contra: " << static_cast<float>(golesEnContraEquipo) / partidosCompeticion << endl;
        }
    }
}

//Muestra el total de triunfos y derrotas de un equipo por competición.
void mostrarTriunfosDerrotasPorEquipo(const string& equipo) {
    cout << "Resultados de triunfos y derrotas para el equipo: " << equipo << endl;

    if (triunfosDerrotas.find(equipo) == triunfosDerrotas.end()) {
        cout << "No se encontraron datos para el equipo especificado." << endl;
        return;
    }

    for (const auto& competicionResultados : triunfosDerrotas[equipo]) {
        const auto& competicion = competicionResultados.first;
        const auto& resultados = competicionResultados.second;
        cout << "Competicion: " << competicion
             << ", Triunfos: " << resultados.first
             << ", Derrotas: " << resultados.second << endl;
    }
}

//Identifica la fecha con más y menos goles de un equipo.
void mostrarFechaConMasYMenosGoles(const string& equipo) {
    unordered_map<string, pair<string, string>> fechasGoles;

    for (const auto& partido : partidos) {
        if (partido.equipoLocal == equipo || partido.equipoVisitante == equipo) {
            int golesTotales = partido.golesLocal + partido.golesVisitante;
            const string& fechaActual = partido.fecha;
            const string& competicion = partido.competicion;

            if (fechasGoles.count(competicion) == 0) {
                fechasGoles[competicion] = {fechaActual, fechaActual};
            } else {
                if (golesTotales > (golesAFavor[equipo][competicion] + golesEnContra[equipo][competicion])) {
                    fechasGoles[competicion].first = fechaActual;
                }
                if (golesTotales < (golesAFavor[equipo][competicion] + golesEnContra[equipo][competicion])) {
                    fechasGoles[competicion].second = fechaActual;
                }
            }
        }
    }

    cout << "Fechas con mas y menos goles para el equipo: " << equipo << endl;
    for (const auto& competicionFechas : fechasGoles) {
        const auto& competicion = competicionFechas.first;
        const auto& fechas = competicionFechas.second;
        cout << "Competicion: " << competicion
             << ", Fecha con mas goles: " << fechas.first
             << ", Fecha con menos goles: " << fechas.second << endl;
    }
}

//Encuentra la competición con mayor cantidad de goles.
void mostrarCompeticionConMasGoles() {
    unordered_map<string, int> golesPorCompeticion;

    for (const auto& partido : partidos) {
        golesPorCompeticion[partido.competicion] += partido.golesLocal + partido.golesVisitante;
    }

    auto maxCompeticion = max_element(golesPorCompeticion.begin(), golesPorCompeticion.end(),
                                      [](const auto& a, const auto& b) { return a.second < b.second; });

    if (maxCompeticion != golesPorCompeticion.end()) {
        cout << "Competicion con mas goles convertidos: " << maxCompeticion->first
             << " con " << maxCompeticion->second << " goles." << endl;
    } else {
        cout << "No se encontraron competiciones." << endl;
    }
}

//Muestra los equipos con más y menos goles por competición.
void mostrarEquipoConMasYMenosGolesPorCompeticion() {
    for (const auto& equipoGoles : golesAFavor) {
        const string& equipo = equipoGoles.first;
        cout << "Equipo: " << equipo << endl;

        for (const auto& competicionGoles : equipoGoles.second) {
            const string& competicion = competicionGoles.first;
            int golesAFavorEquipo = competicionGoles.second;
            int golesEnContraEquipo = golesEnContra[equipo].count(competicion) ? golesEnContra[equipo][competicion] : 0;

            cout << "Competicion: " << competicion
                 << ", Goles a favor: " << golesAFavorEquipo
                 << ", Goles en contra: " << golesEnContraEquipo << endl;
        }
    }
}


//Lista los resultados de un equipo en una competición específica.
void mostrarResultadosEquipoCompeticion(const string& equipo, const string& competicion) {
    cout << "Resultados de " << equipo << " en la competicion " << competicion << ":" << endl;

    for (const auto& partido : partidos) {
        if ((partido.equipoLocal == equipo || partido.equipoVisitante == equipo) && partido.competicion == competicion) {
            string resultado = (partido.equipoLocal == equipo) ?
                to_string(partido.golesLocal) + " a " + to_string(partido.golesVisitante) :
                to_string(partido.golesVisitante) + " a " + to_string(partido.golesLocal);

            string descripcion = (partido.golesLocal > partido.golesVisitante) ?
                "gano a " + partido.equipoVisitante :
                (partido.golesLocal < partido.golesVisitante) ? "perdio con " + partido.equipoVisitante : "empato con " + partido.equipoVisitante;

            cout << equipo << " " << partido.fecha << " " << resultado << " " << descripcion << "." << endl;
        }
    }
}

//Filtra resultados de un equipo entre dos fechas dadas.
tm convertirFecha(const string& fecha) {
    tm t = {};
    stringstream ss(fecha);
    ss >> get_time(&t, "%d/%m/%Y"); // Formato D/M/YYYY
    return t;
}

void mostrarResultadosEquipoFechas(const string& equipo, const string& fechaInicio, const string& fechaFin) {
    cout << "Resultados de " << equipo << " entre " << fechaInicio << " y " << fechaFin << ":" << endl;

    tm inicio = convertirFecha(fechaInicio);
    tm fin = convertirFecha(fechaFin);

    for (const auto& partido : partidos) {
        tm fechaPartido = convertirFecha(partido.fecha);

        // Comparación de fechas
        if ((partido.equipoLocal == equipo || partido.equipoVisitante == equipo) &&
            difftime(mktime(&fechaPartido), mktime(&inicio)) >= 0 &&
            difftime(mktime(&fin), mktime(&fechaPartido)) >= 0) {

            string resultado = (partido.equipoLocal == equipo) ?
                to_string(partido.golesLocal) + " a " + to_string(partido.golesVisitante) :
                to_string(partido.golesVisitante) + " a " + to_string(partido.golesLocal);

            string descripcion = (partido.golesLocal > partido.golesVisitante) ?
                "gano a " + partido.equipoVisitante :
                (partido.golesLocal < partido.golesVisitante) ? "perdio con " + partido.equipoVisitante : "empato con " + partido.equipoVisitante;

            cout << equipo << " " << partido.fecha << " " << resultado << " " << descripcion << "." << endl;
        }
    }
}

//Compara estadísticas generales entre dos equipos.
void compararRendimientoGeneral(const string& equipo1, const string& equipo2) {
    cout << "Comparacion de rendimiento entre " << equipo1 << " y " << equipo2 << ":" << endl;

    for (const auto& competicionGoles : golesAFavor) {
        const auto& equipo = competicionGoles.first;
        if (equipo == equipo1 || equipo == equipo2) {
            for (const auto& competicion : competicionGoles.second) {
                int golesAF = golesAFavor[equipo][competicion.first];
                int golesContra = golesEnContra[equipo][competicion.first];

                cout << equipo << " Goles a favor: " << golesAF
                    << " Goles en contra: " << golesContra
                    << " en " << competicion.first << endl;
            }
        }
    }
}

//Muestra estadísticas de enfrentamientos directos entre dos equipos.
void compararRendimientoParticular(const string& equipo1, const string& equipo2) {
    int partidosJugados = 0, empates = 0, victoriasEquipo1 = 0, victoriasEquipo2 = 0;

    for (const auto& partido : partidos) {
        if ((partido.equipoLocal == equipo1 && partido.equipoVisitante == equipo2) ||
            (partido.equipoLocal == equipo2 && partido.equipoVisitante == equipo1)) {

            partidosJugados++;
            if (partido.golesLocal == partido.golesVisitante) empates++;
            else if (partido.equipoLocal == equipo1 && partido.golesLocal > partido.golesVisitante) victoriasEquipo1++;
            else if (partido.equipoVisitante == equipo1 && partido.golesVisitante > partido.golesLocal) victoriasEquipo1++;
            else victoriasEquipo2++;
        }
    }

    cout << "Resultados de enfrentamientos entre " << equipo1 << " y " << equipo2 << ":" << endl;
    cout << "Partidos jugados: " << partidosJugados << endl;
    cout << "Empates: " << empates << endl;
    cout << "Victorias de " << equipo1 << ": " << victoriasEquipo1 << endl;
    cout << "Victorias de " << equipo2 << ": " << victoriasEquipo2 << endl;
}

//Filtra equipos cuyo promedio de goles sea mayor o menor a un umbral.
void filtrarEquiposPorUmbralPromedioGoles(int umbral, bool porEncima) {
    unordered_map<string, unordered_map<string, float>> promedioGolesPorCompeticion;

    for (const auto& partido : partidos) {
        int golesTotalLocal = golesAFavor[partido.equipoLocal][partido.competicion];
        int golesTotalVisitante = golesAFavor[partido.equipoVisitante][partido.competicion];

        int partidosLocal = count_if(partidos.begin(), partidos.end(), [&](const Partido& p) {
            return p.equipoLocal == partido.equipoLocal && p.competicion == partido.competicion;
        });
        int partidosVisitante = count_if(partidos.begin(), partidos.end(), [&](const Partido& p) {
            return p.equipoVisitante == partido.equipoVisitante && p.competicion == partido.competicion;
        });

        promedioGolesPorCompeticion[partido.equipoLocal][partido.competicion] = static_cast<float>(golesTotalLocal) / partidosLocal;
        promedioGolesPorCompeticion[partido.equipoVisitante][partido.competicion] = static_cast<float>(golesTotalVisitante) / partidosVisitante;
    }

    cout << "Equipos con promedio de goles " << (porEncima ? "por encima de " : "por debajo de ") << umbral << " por partido:" << endl;
    for (const auto& equipoPromedio : promedioGolesPorCompeticion) {
        const string& equipo = equipoPromedio.first;
        for (const auto& competicionPromedio : equipoPromedio.second) {
            float promedioGoles = competicionPromedio.second;
            if ((porEncima && promedioGoles >= umbral) || (!porEncima && promedioGoles <= umbral)) {
                cout << equipo << " " << promedioGoles << " goles promedio por partido en " << competicionPromedio.first << endl;
            }
        }
    }
}
};

// Función del menú
void mostrarMenu() {
    cout << "Menu: " << endl;
    cout << "1. Mostrar resultados principales" << endl;
    cout << "2. Agregar partido" << endl;
    cout << "3. Eliminar partido" << endl;
    cout << "4. Modificar partido" << endl;
    cout << "5. Guardar cambios en archivo CSV" << endl;
    cout << "6. Mostrar Top 5 partidos con mas goles" << endl;
    cout << "7. Mostrar goles totales por equipo y competicion" << endl;
    cout << "8. Mostrar promedio de goles por equipo y competicion" << endl;
    cout << "9. Mostrar triunfos y derrotas por equipo y competicion" << endl;
    cout << "10. Mostrar fecha con mas y menos goles por equipo y competicion" << endl;
    cout << "11. Mostrar competición con mas goles" << endl;
    cout << "12. Mostrar equipo con mas y menos goles por competicion" << endl;
    cout << "13. Consultar todos los resultados de un equipo en una competicion especifica" << endl;
    cout << "14. Consultar resultados de un equipo entre dos fechas" << endl;
    cout << "15. Comparacion de rendimiento general entre dos equipos" << endl;
    cout << "16. Comparacion de rendimiento particular entre dos equipos" << endl;
    cout << "17. Filtrar equipos por umbral de goles promedio por partido" << endl;
    cout << "0. Salir" << endl;
}

int main() {
    BaseDatos bd;

    auto start = high_resolution_clock::now();
    bd.cargarDesdeCSV();
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Tiempo en cargar datos desde CSV: " << duration.count() << " ms" << endl;

    int opcion;
    string equipo, equipo2, competicion, fechaInicio, fechaFin;
    int umbral;
    bool porEncima;

    do {
        mostrarMenu();
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore(); // Limpiar el buffer para leer correctamente las entradas de texto.

        switch (opcion) {
            case 1:
                bd.mostrarResultadosPrincipales();
                break;
            case 2:
                bd.agregarPartido();
                break;
            case 3: {
                string jornada;
                cout << "Ingrese la jornada del partido a eliminar: ";
                getline(cin, jornada);
                bd.eliminarPartido(jornada);
                break;
            }
            case 4: {
                string jornada;
                cout << "Ingrese la jornada del partido a modificar: ";
                getline(cin, jornada);
                bd.modificarPartido(jornada);
                break;
            }
            case 5:
                bd.guardarEnCSV();
                break;
            case 6:
                bd.mostrarTop5PartidosConMasGoles();
                break;
            case 7:
                cout << "Ingrese el nombre del equipo: ";
                getline(cin, equipo);
                bd.mostrarGolesTotalesEquipoPorCompeticion(equipo);
                break;
            case 8:
                cout << "Ingrese el nombre del equipo: ";
                getline(cin, equipo);
                bd.mostrarPromedioGolesEquipoPorCompeticion(equipo);
                break;
            case 9:
                cout << "Ingrese el nombre del equipo: ";
                getline(cin, equipo);
                bd.mostrarTriunfosDerrotasPorEquipo(equipo);
                break;
            case 10:
                cout << "Ingrese el nombre del equipo: ";
                getline(cin, equipo);
                bd.mostrarFechaConMasYMenosGoles(equipo);
                break;
            case 11:
                bd.mostrarCompeticionConMasGoles();
                break;
            case 12:
                bd.mostrarEquipoConMasYMenosGolesPorCompeticion();
                break;
            case 13:
                cout << "Ingrese el nombre del equipo: ";
                getline(cin, equipo);
                cout << "Ingrese el nombre de la competición: ";
                getline(cin, competicion);
                bd.mostrarResultadosEquipoCompeticion(equipo, competicion);
                break;
            case 14:
                cout << "Ingrese el nombre del equipo: ";
                getline(cin, equipo);

                cout << "Ingrese la fecha de inicio (formato D/M/YYYY): ";
                getline(cin, fechaInicio);

                cout << "Ingrese la fecha de fin (formato D/M/YYYY): ";
                getline(cin, fechaFin);

                bd.mostrarResultadosEquipoFechas(equipo, fechaInicio, fechaFin);
                break;
            case 15:
                cout << "Ingrese el nombre del primer equipo: ";
                getline(cin, equipo);
                cout << "Ingrese el nombre del segundo equipo: ";
                getline(cin, equipo2);
                bd.compararRendimientoGeneral(equipo, equipo2);
                break;
            case 16:
                cout << "Ingrese el nombre del primer equipo: ";
                getline(cin, equipo);
                cout << "Ingrese el nombre del segundo equipo: ";
                getline(cin, equipo2);
                bd.compararRendimientoParticular(equipo, equipo2);
                break;
            case 17:
                cout << "Ingrese el umbral de goles promedio: ";
                cin >> umbral;
                cout << "¿Desea filtrar por encima del umbral? (1 para Si, 0 para No): ";
                cin >> porEncima;
                bd.filtrarEquiposPorUmbralPromedioGoles(umbral, porEncima);
                break;
            case 0:
                cout << "Salir" << endl;
                break;
            default:
                cout << "Opcion invalida. Intente nuevamente." << endl;
        }
    } while (opcion != 0);

    return 0;
}
