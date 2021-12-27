#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<unistd.h>

void ordenar(int valores[],int left,int medium,int right)
{
  int n1=medium-left+1,n2=right-medium,i,j,k;
  int l[n1],r[n2];
  for(i=0;i<n1;i++)
  {
    l[i]=valores[left+i];
  }
  for(i=0;i<n2;i++)
  {
    r[i]=valores[medium+i+1];
  }
  i=0;
  j=0;
  k=left;
  while(i<n1 && j<n2)
  {
    if(l[i]<r[j])
    {
      valores[k++]=l[i++];
    }
    else
    {
      valores[k++]=r[j++];
    }
  }
  while(i<n1)
  {
    valores[k++]=l[i++];
  }
  while (j<n2)
  {
    valores[k++]=r[j++];
  }
}

void ordenar_mergesort(int valores[],int left,int right)
{
  int medium=(left+right)/2;
  int estado;
  int lc,rc;
  if(left>=right)
  {
    return;
  }
  lc=fork();
  if(lc<0)
  {
    printf("Error al crear proceso hijo\n");
    exit(1);
  }
  else if(lc==0)
  {
    ordenar_mergesort(valores,left,medium);
    exit(0);
  }
  else
  {
    rc=fork();
    if(rc<0)
    {
      printf("Error al crear proceso hijo con la porcion derecha\n");
      exit(1);
    }
    else if(rc==0)
    {
      ordenar_mergesort(valores,medium+1,right);
      exit(0);
    }
  }
  waitpid(lc,&estado,0);
  waitpid(rc,&estado,0);
  ordenar(valores,left,medium,right);
}

int main(int argc, char *argv[])
{
  if(argc <2){
	  printf("Por favor, debe ingresar dos argumentos \n 1- Indicador si va a ingresar una ruta del archivo. \n 2. La ruta del archivo o los números a ordenar separados por coma. \n\n NOTA: Los números a ingresar deben ser separados por coma. \n Por Ejemplo: \n 1 -   ./a.out -f archivo.txt \n 2 -   ./a.out -n 1,4,6,2,20,15 \n\n");
          exit(1);
          return 1;
  }
  key_t k;
  int numbers_quantity = 0;
  int array_numbers[10000];
  int i;  
  char *option;
  int id;
  int mat_id;
  int *shm_array;
  char delimito[] = ",";
  char texto[10000];
  if(argc != 3){
	  if(strcmp(argv[1],"-h") == 0 ){
          printf("Por favor, debe ingresar dos argumentos \n 1- Indicador si va a ingresar una ruta del archivo. \n 2. La ruta del archivo o los números a ordenar separados por coma. \n\n NOTA: Los números a ingresar deben ser separados por coma. \n Por Ejemplo: \n 1 -   ./a.out -f archivo.txt \n 2 -   ./a.out -n 1,4,6,2,20,15 \n\n");
	  exit(1);
	  return 1;
  }else{
	  printf("Por favor, debe ingresar dos argumentos \n 1- Indicador si va a ingresar una ruta del archivo. \n 2. La ruta del archivo o los números a ordenar separados por coma. \n\n NOTA: Los números a ingresar deben ser separados por coma. \n");
	  exit(1);
	  return 1;
	  }
  }
  if(strcmp(argv[1],"-f")==0){
	  printf("Se realizará el ordenamiento de un archivo.\n");
	  FILE *archivo = fopen(argv[2], "r");
	  if(archivo == NULL){
                  printf("Error abriendo el archivo. \n");
		  exit(1);
		  return 1;
	  }
	  char c;
	  int add = 0; 
          while(!feof(archivo)){
		  c= fgetc(archivo);
		  if(c!='\0' || c!='\n'){
			  texto[add] = c;
		  }
		  add++;
	  }
          fclose(archivo);
	  
	  }
  else if(strcmp(argv[1],"-n")==0){
	  strcpy(texto,argv[2]);
	  
  }
  else{
	  printf("El argumento dado no existe \n");
	  exit(1);
	  return 1;
  }
  strtok(texto, "\n");
          char *token = strtok(texto, delimito);
          if(token != NULL){
                  while(token != NULL){
                          array_numbers[numbers_quantity] = atoi(token);
                          numbers_quantity++;
                          token = strtok(NULL, delimito);
        }
    }

  k = IPC_PRIVATE;
  id = shmget(k,numbers_quantity,IPC_CREAT | 0666);
  if(id<0)
  {
    printf("Error al obtener el segmento de memoria compartida, que debería ser con permisos rwx.\n");
    exit(1);
    return 1;
  }
  shm_array = shmat(id,NULL,0);
  if(mat_id<0)
  {
    printf("Error al adjuntar el segmento de memoria /// Se debería asignar a la primera sección disponible seleccionada por el sistema.\n");
    exit(1);
    return 1;
  }
  for(i=0;i<numbers_quantity;i++)
  {
    shm_array[i]=array_numbers[i];
  }
  ordenar_mergesort(shm_array,0,numbers_quantity-1);
  printf("El orden de sus números es el siguiente: \n");
  for(i=0;i<numbers_quantity;i++)
  {
	  printf("%d,",shm_array[i]);
  }
  printf("\n\n");
  if (shmdt(shm_array) == -1)
  {
		printf("Error al separar del espacio de direcciones compartidas al proceso\n");
		exit(1);
		return 1;
	}
	if (shmctl(id, IPC_RMID, NULL) == -1)
  {
		printf("Error al quitar el segmento de direccion compartida \n");
		exit(1);
		return 1;
	}
  return 0;
}
