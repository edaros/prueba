#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>

#define tamano 27

int main(int argc, char *argv[]){

	
	char *nameFile;
	//char *numberProcess;
	int cantProcess=0;
	int j = 1;
	int status;
	int lTotal=0;
	char *parametros[10];
	//char PROGRAMA[]= "contarPalabras";
	//char *ARCHIVO= "temporal";
	FILE *file;
	
	// guardamos el archivo, lo separamos y leemos el numero de procesos 		// entrados por el usuario.
	nameFile = strtok(argv[1], ",");
	cantProcess = atoi (strtok(NULL, ","));

	// creamos un arreglo para los procesos 

	pid_t process[cantProcess];

	// manejo de memoria compartida
	//int cantMemoria= sizeof (int)*cantProcess;
	key_t Clave;
	int Id_Memoria;
	int *Memoria;
	
	//clave para el segmento compartido 
	Clave = 4321;
	
	file= fopen (nameFile, "r");
	if (file == NULL)
	{
		printf("Error en el  archivo de texto");
		return 0;
	}

	char inicio[10];	
	char final[10];	// Es la cantidad de caracteres  donde cada proceso va a finalizar su lectura de caracteres
	//char MemTam[10];
	char numberProcess[10];
	int tam; 
	(file,0,SEEK_END); 
	tam = ftell(file); 
	
	//calcular trabajo
	int cantcharacters = (tam/cantProcess);	
	
	
	for(j= 0;j < cantProcess; j++) //crear la cantidad de procesos especificada por el usuario
	{	
		process[j] = fork();
		if(process[j]==0)
		{
			int start = j*cantcharacters;	//donde inicia el conteo de caracteres
			sprintf(inicio,"%d", start);	//convertir en cadena los parametros a pasar
			sprintf(final,"%d", (j+1)*(cantcharacters));	//convertir en cadena los parametros a pasar
			//sprintf(MemTam,"%d", cantMemoria);
			sprintf(numberProcess, "%d", j);
			parametros[0] = "./contarPalabras",
			parametros[1] = nameFile;
			parametros[2] = inicio;
			parametros[3] = final; 
			parametros[4]= numberProcess;
			//parametros[5] = MemTam;
			parametros[6] =  NULL;

			execvp("./contarPalabras", parametros);	
			exit(1);
		}
	}
	
	//for(j = 0; j < cantProcess; j++)
	//{
	//	wait(NULL);
	//}
	j=0;
	while(j<=cantProcess)
	{

	wait(&status);	
	j++;	

	}
	//
	//	Creamos la memoria con la clave reci√©n conseguida. Para ello llamamos
	//	a la funci√≥n shmget pas√°ndole la clave, el tama√±o de memoria que
	//	queremos reservar 
	 
	Id_Memoria = shmget (Clave, tamano, IPC_CREAT | 0777 );
	if (Id_Memoria == -1)
	{
		perror("No consigo Id para memoria compartida");
		exit (1);
	}

	//
	//	Una vez creada la memoria, hacemos que uno de nuestros punteros
	//	apunte a la zona de memoria reci√©n creada. 
	//
	Memoria = (int *)shmat (Id_Memoria, (char *)0, 0);
	if (Memoria == NULL)
	{
		perror("No consigo memoria compartida");
		exit (1);
	}

	j=0;
	while(j < atoi(cantProcess)){
		
		lTotal = lTotal + Memoria[j];
		printf("Proceso %d: %d\n", j, Memoria[j]);
		fprintf(file, "Proceso %d: %d\n", j, Memoria[j]);
		j++;
		
	}
	fprintf(file, "TOTAL: %d", lTotal);

	shmdt ((char *)Memoria);
	shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);
	
}
