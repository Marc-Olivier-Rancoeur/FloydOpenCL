#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>

#include <CL\\cl.h>

using namespace std;
using namespace std::chrono;

unsigned int* InitMatrice(const unsigned int& taille);
void AffichageMatrice(const unsigned int* matrice, const unsigned int& taille);
void FloydSeq(unsigned int* matrice, const unsigned int& taille);
string ChargeOCLProgramme(const string& nomFichier);

int main() {

	// G�n�ration de la matrice
	unsigned int taille;
	cin >> taille;
	if (taille < 2){
		cout << "La taille doit �tre sup�rieure ou �gale � 2." << endl;
		return -1;
	}
	unsigned int* matrice = InitMatrice(taille);



	// Initialisation OCL, d�claration et mise en forme des donn�es
	string programmeString = ChargeOCLProgramme("script.cl");
	cl_int status;
	cl_uint nomPlateformes = 0;
	cl_platform_id* plateformes = nullptr;
	cl_uint nomDevices = 0;
	cl_device_id* devices = nullptr;
	cl_context contexte;
	cl_command_queue cmdQueue;
	cl_mem matriceCL;
	size_t tailleDonnees = sizeof(unsigned int) * taille * taille;

	// Plateformes
	status = clGetPlatformIDs(0, nullptr, &nomPlateformes);
	plateformes = (cl_platform_id*)malloc(nomPlateformes * sizeof(cl_platform_id));
	status = clGetPlatformIDs(nomPlateformes, plateformes, NULL);
	// Devices
	status = clGetDeviceIDs(plateformes[0], CL_DEVICE_TYPE_ALL, 0, nullptr, &nomDevices);
	devices = (cl_device_id*)malloc(nomDevices * sizeof(cl_device_id));
	status = clGetDeviceIDs(plateformes[0], CL_DEVICE_TYPE_ALL, nomDevices, devices, nullptr);
	// Contexte
	contexte = clCreateContext(nullptr, nomDevices, devices, nullptr, nullptr, &status);
	// File d'attente
	cmdQueue = clCreateCommandQueue(contexte, devices[0], 0, &status);
	// Buffer
	matriceCL = clCreateBuffer(contexte, CL_MEM_READ_WRITE, tailleDonnees, nullptr, &status);
	// Ecriture buffer
	status = clEnqueueWriteBuffer(cmdQueue, matriceCL, CL_TRUE, 0, tailleDonnees, matrice, 0, nullptr, nullptr);


	// Cr�ation et compilation du programme
	size_t programmeStringTaille = programmeString.length();
	char* programmeStringChar = new char[programmeStringTaille + 1];
	strcpy(programmeStringChar, programmeString.c_str());
	cl_program programme;
	cl_kernel kernel;

	// Cr�ation du programme
	programme = clCreateProgramWithSource(contexte, 1, (const char**)&programmeStringChar, nullptr, &status);
	// Compilation
	status = clBuildProgram(programme, nomDevices, devices, nullptr, nullptr, nullptr);
	// Status return infos
	if (status) {
		size_t log_size;
		clGetProgramBuildInfo(programme, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		// Allocate memory for the log
		char* log = (char*)malloc(log_size);
		// Get the log
		clGetProgramBuildInfo(programme, devices[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		// Print the log
		printf("%s\n", log);
		printf("ERREUR A LA COMPILATION: %d\n", status);
		free(log);
		return -1;
	}
	// Cr�ation du kernel
	kernel = clCreateKernel(programme, "floyd", &status);
	// Passage des param�tres
	status = clSetKernelArg(kernel, 0, sizeof(cl_int), (void*)&taille);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&matriceCL);
	cout << "Initialisation termin�e" << endl;



	// D�but calculs
	size_t tailleTravailGeneral[2] = { taille, taille };

	auto debut = high_resolution_clock::now();
	// Appel
	for (unsigned int i = 0; i < taille; i++) {
		status = clSetKernelArg(kernel, 2, sizeof(int), (void*)&i);
		status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, nullptr, tailleTravailGeneral, nullptr, 0, nullptr, nullptr);
		clFlush(cmdQueue);
	}
	clFinish(cmdQueue);
	auto fin = high_resolution_clock::now();
	cout << "Calculs termin�s" << endl;



	// R�cup�ration de la matrice
	clEnqueueReadBuffer(cmdQueue, matriceCL, CL_TRUE, 0, tailleDonnees, matrice, 0, nullptr, nullptr);
	// Affichage final
	//AffichageMatrice(matrice, taille);
	duration<double> duree = fin - debut;
	cout << "temps d'ex�cution : " << duree.count() << endl;



	// D�sallocation de la m�moire
	clReleaseKernel(kernel);
	clReleaseProgram(programme);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(matriceCL);
	clReleaseContext(contexte);

	delete matrice;
	delete programmeStringChar;
	free(plateformes);
	free(devices);

	return 0;
}

unsigned int* InitMatrice(const unsigned int& taille) {
	unsigned int* nouvelleMatrice = new unsigned int[taille*taille];

	// Initialisation de la matrice
	unsigned int i;
	for (i = 0; i < (taille * taille) - 1; i++) { // Boucle d'initialisation des valeurs de la table
		nouvelleMatrice[i] = taille + 1;
	}
	nouvelleMatrice[taille * (taille - 1)] = 1; // D�finit le lien entre l'�l�ment n-1 et l'�l�ment 0
	nouvelleMatrice[(taille * taille) - 1] = 0;
	for (i = 0; i < taille - 1; i++) {
		nouvelleMatrice[(taille * i) + i] = 0;
		nouvelleMatrice[(taille * i) + i + 1] = 1;
	}

	return nouvelleMatrice;
}

void AffichageMatrice(const unsigned int* matrice, const unsigned int& taille) {
	for (unsigned int i = 0; i < taille; i++) {
		for (unsigned int j = 0; j < taille; j++) {
			cout << matrice[(taille * i) + j] << " ";
		}
		cout << endl;
	}
}

void FloydSeq(unsigned int* matrice, const unsigned int& taille) {
	for (unsigned int i = 0; i < taille; i++) {
		for (unsigned int j = 0; j < taille; j++) {
			for (unsigned int k = 0; k < taille; k++) {
				matrice[(taille * j) + k] = min(matrice[(taille * j) + k], matrice[(taille * j) + i] + matrice[(taille * i) + k]);
			}
		}
	}
}

string ChargeOCLProgramme(const string& nomFichier) {
	ifstream fichier(nomFichier);
	if(fichier){
		stringstream progStream;
		progStream << fichier.rdbuf();
		return progStream.str();
	}
}